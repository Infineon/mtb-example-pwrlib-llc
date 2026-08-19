/*******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#include "HardwareIface.h"
#include "stdlib.h"

#if (!ACTIVE_TEMP_SENSOR)
const TEMP_SENS_LUT_t Temp_Sens_LUT =
{
    .step     = 1.0f / (TEMP_SENS_LUT_WIDTH + 1.0f), // [%], normalized voltage wrt Vcc
    .step_inv = (TEMP_SENS_LUT_WIDTH + 1.0f),       // [1/%], inverse normalized voltage
    .val      ={ 176.3f, 135.8f, 113.9f, 98.77f, 86.96f, 77.08f, 68.4f, 60.48f, 53.03f, 45.8f, 38.58f, 31.15f, 23.2f, 14.24f, 3.279f, -12.59f } // [degree C]
};
#endif

inline void MCU_ReadADCResults(void)
{
    hw.mcu.adc_results[ADC_VIN]      = *LLC_VIN_RSLT_PTR;
    hw.mcu.adc_results[ADC_VOUT]     = *LLC_VOUT_RSLT_PTR;
    hw.mcu.adc_results[ADC_IOUT]     = *LLC_IOUT_FLT_RSLT_PTR;
    hw.mcu.adc_results[ADC_TEMP]     = *LLC_NTC_RSLT_PTR;
    hw.mcu.adc_results[ADC_POT]      = *LLC_POT_RSLT_PTR;
}


// Local function to set dead times for both high and low switches symmetrically
static inline void SetBothDT(TCPWM_Type const* base, uint32_t cnt_num, uint32_t dead_time)
{
    Cy_TCPWM_PWM_SetDTBuff(base, cnt_num, dead_time | (uint32_t)(dead_time << TCPWM_GRP_CNT_DT_BUFF_DT_LINE_COMPL_OUT_Pos));
}


#define LLC_MOD_UPDATE_TIMEOUT (20UL) // This value depends on switching frequency range (especially the low limit) and while cycle execution timing.

RAMFUNC_BEGIN
inline void MCU_ModUpd(void)
{
    int32_t loc_hlf_pd = (int32_t)(hw.mcu.clk.tcpwm_hi_res * vars.Tsw_cmd_int + 0.5f) >> 1U; // half period (rounding)
    int32_t loc_qrt_pd = loc_hlf_pd >> 1U; // quarter period
    // Note: Td_on and Td_off are clamped such that
    // 1- Maximum phase shifts will be +90 & -90 degrees
    // 2- {Td_on >= Td_off} to avoid {H=1 , L=1} as the REC PWM output
    int32_t loc_cc_shft = SAT(-loc_qrt_pd, loc_qrt_pd, (int32_t)(hw.mcu.clk.tcpwm_hi_res * vars.Td_off_sr + 0.5f));
    int32_t loc_dt_drv = (int32_t)(hw.mcu.clk.tcpwm_hi_res * vars.T_dt + 0.5f);
    int32_t loc_dt_rec = hw.mcu.rec.dead_time + SAT(0, loc_hlf_pd, (int32_t)(hw.mcu.clk.tcpwm_hi_res * (vars.Td_on_sr - vars.Td_off_sr) + 0.5f));
    uint32_t loc_timeout = LLC_MOD_UPDATE_TIMEOUT;

    Cy_TCPWM_ClearInterrupt(LLC_DRV0_HW, LLC_DRV0_NUM, CY_TCPWM_INT_ON_TC);
    while ((0UL == (CY_TCPWM_INT_ON_TC & Cy_TCPWM_GetInterruptStatus(LLC_DRV0_HW, LLC_DRV0_NUM))) && (0UL != loc_timeout)) // wait for subsequent TC (twice a switching period)
    {
        loc_timeout--;
    }

    if ((0UL != loc_timeout) && (0L != loc_hlf_pd))
    {
        Cy_TCPWM_PWM_SetPeriod1(LLC_DRV0_HW, LLC_DRV0_NUM, loc_hlf_pd);
        Cy_TCPWM_PWM_SetPeriod1(LLC_DRV1_HW, LLC_DRV1_NUM, loc_hlf_pd);
        Cy_TCPWM_PWM_SetPeriod1(LLC_SR_HW, LLC_SR_NUM, loc_hlf_pd);
        Cy_TCPWM_Block_SetCC0BufVal(LLC_DRV0_HW, LLC_DRV0_NUM, loc_qrt_pd);
        Cy_TCPWM_Block_SetCC0BufVal(LLC_DRV1_HW, LLC_DRV1_NUM, loc_qrt_pd + hw.mcu.tmg.drv_h);
        Cy_TCPWM_Block_SetCC1BufVal(LLC_DRV1_HW, LLC_DRV1_NUM, loc_qrt_pd - hw.mcu.tmg.drv_l);
        Cy_TCPWM_Block_SetCC0BufVal(LLC_SR_HW, LLC_SR_NUM, loc_qrt_pd + loc_cc_shft);
        Cy_TCPWM_Block_SetCC1BufVal(LLC_SR_HW, LLC_SR_NUM, loc_qrt_pd - loc_cc_shft);
        SetBothDT(LLC_DRV0_HW, LLC_DRV0_NUM, loc_dt_drv);
        SetBothDT(LLC_DRV1_HW, LLC_DRV1_NUM, loc_dt_drv);
        SetBothDT(LLC_SR_HW, LLC_SR_NUM, loc_dt_rec); // update rectifier dead time (rising edge)
    }

    if (vars.d_cmd_int < hw.mcu.brst.tick_frac) // Avoid the short 3-clock-cycles (~12ns) pulse, protection from short pulses
    {
        Cy_TCPWM_PWM_SetCompare0BufVal(LLC_BRST_HW, LLC_BRST_NUM, 0UL);
        Cy_TCPWM_PWM_SetCompare1BufVal(LLC_BRST_HW, LLC_BRST_NUM, 0UL);
    }
    else
    {
        float32_t loc_brst = (float32_t)hw.mcu.brst.period * vars.d_cmd_int;
        // The burst PWM always kills one cycle more than its actual duty-cycle, this is a specific of TCPWM + TrigMux HW,
        // therefore we have non-linear two-tick jump at the last duty cycle step (e.g. at the 98.4->98.5 rounding transition the real duty cycle becomes 100%, not at 99.5 as would be expected, at period 100),
        // however due to the non-linear parabolic burst modulation characteristic, this is not critical.
        Cy_TCPWM_PWM_SetCompare0BufVal(LLC_BRST_HW, LLC_BRST_NUM, (uint32)(loc_brst + 1.75f)); // Update burst PWM first half-pulse (0.5 rounding + 0.25 asymmetry for resolution + 1 because of kill behavior)
        // To achieve the true physical period * 2 resolution (e.g. resolution 100 at physical period 50) the compare0 and compare1 intermediate values (before convert to integer) have to differ (be asymmetric) by 0.5
        Cy_TCPWM_PWM_SetCompare1BufVal(LLC_BRST_HW, LLC_BRST_NUM, (uint32)(loc_brst + 0.25f)); // Update burst PWM second half-pulse (0.5 rounding - 0.25 asymmetry for resolution)
        // The kill-specific 1 is intentionally done asymmetrically (not distributed by 0.5 to both) to avoid the burst duty cycle 1 cases during update twice an up-down period, protection from short pulses,
        // however this asymmetry shifts the mentioned 'last duty cycle step' by one tick 98.5 -> 99.5 at period 100,
        // and again, due to the non-linear parabolic burst modulation characteristic, this is not critical.
    }
}


RAMFUNC_END

void MCU_SetVinProtThld(float32_t v_in)
{
    uint32_t loc_lim = (uint32_t)(v_in / hw.mcu.adc_scale.vin);
    loc_lim = MIN(loc_lim, (uint32_t)((1UL << (ADC_RES + (uint32_t)LLC_VIN_config.avg)) - 1UL)); // ADC averaging is accumulative (not auto right-shifting)
    HPPASS_SAR_LIMIT_HIGH(HPPASS_BASE, LLC_VIN_OV_IDX) = loc_lim;
}


void MCU_SetVoutProtThld(float32_t v_out)
{
    uint32_t loc_lim = (uint32_t)(v_out / hw.mcu.adc_scale.vout);
    loc_lim = MIN(loc_lim, (uint32_t)((1UL << (ADC_RES + (uint32_t)LLC_VOUT_config.avg)) - 1UL)); // ADC averaging is accumulative (not auto right-shifting)
    HPPASS_SAR_LIMIT_HIGH(HPPASS_BASE, LLC_VOUT_OV_IDX) = loc_lim;
}


void MCU_SetIresProtThld(float32_t i_res)
{
    uint16_t loc_dac = (uint16_t)((i_res + ADC_OFFSET_IRES / ADC_SCALE_IRES) * (1U << DAC_RES) * ADC_SCALE_IRES / ADC_VREF);
    loc_dac = MIN(loc_dac, CY_HPPASS_CSG_DAC_VAL_MAX);
    Cy_HPPASS_DAC_SetValue(LLC_IRES_OC_SLICE_IDX, loc_dac);
    Cy_HPPASS_SetFwTrigger(1U << LLC_IRES_DAC_UPD_IDX);
}


void MCU_SetIoutProtThld(float32_t i_out)
{
    uint16_t loc_dac = (uint16_t)((i_out + ADC_OFFSET_IOUT / ADC_SCALE_IOUT) * (1U << DAC_RES) * ADC_SCALE_IOUT / ADC_VREF);
    loc_dac = MIN(loc_dac, CY_HPPASS_CSG_DAC_VAL_MAX);
    Cy_HPPASS_DAC_SetValue(LLC_IOUT_OC_SLICE_IDX, loc_dac);
    Cy_HPPASS_SetFwTrigger(1U << LLC_IOUT_DAC_UPD_IDX);
}


TCPWM_Type*  LLC_PWM_HW[NPWM]      = { LLC_DRV0_HW, LLC_DRV1_HW, LLC_SR_HW };
uint32_t     LLC_PWM_GRP[NPWM]     = { LLC_DRV0_GRP, LLC_DRV1_GRP, LLC_SR_GRP };
uint32_t     LLC_PWM_NUM[NPWM]     = { LLC_DRV0_NUM, LLC_DRV1_NUM, LLC_SR_NUM };
const cy_stc_tcpwm_pwm_config_t* LLC_PWM_CFG[NPWM] = { &LLC_DRV0_config, &LLC_DRV1_config, &LLC_SR_config };

GPIO_PRT_Type* LLC_DR_PORT[NDRP] = { LLC_DRV0_H_PORT, LLC_DRV0_L_PORT, LLC_DRV1_H_PORT, LLC_DRV1_L_PORT };
GPIO_PRT_Type* LLC_SR_PORT[NSRP] = { LLC_SR_A_PORT, LLC_SR_B_PORT };

uint32_t       LLC_DR_PIN[NDRP]  = { LLC_DRV0_H_PIN, LLC_DRV0_L_PIN, LLC_DRV1_H_PIN, LLC_DRV1_L_PIN };
uint32_t       LLC_SR_PIN[NSRP]  = { LLC_SR_A_PIN, LLC_SR_B_PIN };

en_hsiom_sel_t LLC_DR_HSIOM[NDRP] = { LLC_DRV0_H_HSIOM, LLC_DRV0_L_HSIOM, LLC_DRV1_H_HSIOM, LLC_DRV1_L_HSIOM };
en_hsiom_sel_t LLC_SR_HSIOM[NSRP] = { LLC_SR_A_HSIOM, LLC_SR_B_HSIOM };
