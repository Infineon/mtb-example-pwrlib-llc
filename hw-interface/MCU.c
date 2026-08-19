/*******************************************************************************
* (c) 2026, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#include "HardwareIface.h"
#include "Controller.h"
#include "cy_gpio.h"
#include "mtb_oscilloscope.h"
#include "mtb_oscilloscope_data.h"


void MCU_Init()
{
    MCU_InitChipInfo();
    MCU_InitADCs();
    MCU_InitInterrupts();
    MCU_InitTimers();
    MCU_InitProtections();
    mtb_oscilloscope_init((uint32_t)params.sys.samp.fs0);
    mtb_oscilloscope_data_init();
}


void MCU_InitChipInfo()
{
    kit_info.chip_id = Cy_SysLib_GetDevice();
    kit_info.chip_id <<= 16;
    kit_info.chip_id |= Cy_SysLib_GetDeviceRevision();
}


void MCU_InitADCs()
{
    // ADC conversion coefficients .............................................
    hw.mcu.adc_scale.vin  = ADC_VREF / ADC_SCALE_VIN  / (1UL << ADC_RES) / (1UL << (uint32_t)LLC_VIN_config.avg)  * (LLC_VIN_config.diff ? 2UL : 1UL); // [V/ticks]
    hw.mcu.adc_scale.vout = ADC_VREF / ADC_SCALE_VOUT / (1UL << ADC_RES) / (1UL << (uint32_t)LLC_VOUT_config.avg) * (LLC_VOUT_config.diff ? 2UL : 1UL); // [V/ticks]
    hw.mcu.adc_scale.iout = ADC_VREF / ADC_SCALE_IOUT / (1UL << ADC_RES) / (1UL << (uint32_t)LLC_IOUT_config.avg) * (LLC_IOUT_config.diff ? 2UL : 1UL); // [A/ticks]
    hw.mcu.adc_scale.pot  =                      1.0f / (1UL << ADC_RES) / (1UL << (uint32_t)LLC_POT_config.avg);      // [1/ticks]
    #if (!ACTIVE_TEMP_SENSOR) // passive NTC
    hw.mcu.adc_scale.temp =                      1.0f / (1UL << ADC_RES) / (1UL << (uint32_t)LLC_NTC_config.avg);      // [1/ticks], normalized voltage wrt Vcc
    #endif

    // Configure ADC modules ...................................................
    (void)Cy_HPPASS_AC_Start(0U, 1000U);
}


void MCU_InitInterrupts()
{
    // Interrupt callbacks and priorities (higher value = lower urgency) .......
    // Fast control loop:
    cy_stc_sysint_t LLC_ISR0_cfg = { .intrSrc = LLC_ISR0_IRQ, .intrPriority = 0 };
    Cy_SysInt_Init(&LLC_ISR0_cfg, MCU_RunISR0);

    // Slow control loop:
    cy_stc_sysint_t LLC_ISR1_cfg = { .intrSrc = LLC_ISR1_IRQ, .intrPriority = 1 };
    Cy_SysInt_Init(&LLC_ISR1_cfg, MCU_RunISR1);

    // NVIC connections
    hw.mcu.interrupt.nvic_isr0 = LLC_ISR0_cfg.intrSrc;
    hw.mcu.interrupt.nvic_isr1 = LLC_ISR1_cfg.intrSrc;
}


void MCU_InitTimers()
{
    hw.mcu.clk.tcpwm = Cy_SysClk_PeriPclkGetFrequency((en_clk_dst_t)CLK_TCPWM_GRP_NUM, CY_SYSCLK_DIV_8_BIT, CLK_TCPWM_NUM); // [Hz]
    hw.mcu.clk.tcpwm_hi_res = hw.mcu.clk.tcpwm * (LLC_DRV0_config.hrpwm_enable ? 64.0f : 1.0f);

    hw.mcu.tmg.drv_h = hw.mcu.clk.tcpwm_hi_res * LLC_TMG_DRV1_H;
    hw.mcu.tmg.drv_l = hw.mcu.clk.tcpwm_hi_res * LLC_TMG_DRV1_L;

    hw.mcu.drv.dead_time = LLC_DRV0_config.deadTimeClocks;
    hw.mcu.rec.dead_time = LLC_SR_config.deadTimeClocks;
    hw.mcu.brst.period = (uint32_t)(params.llc.range.fsw.max / params.ctrl.burst.f); // It counts each half-period, and this is compensated by the UPDN mode
    hw.mcu.brst.tick_frac = 0.25f / (float32_t)hw.mcu.brst.period; // 0.25 because we need a half of tick (rounding) and the actual period of up-down PWM is twice physical period
    hw.mcu.isr0.period = (uint32_t)(hw.mcu.clk.tcpwm / params.sys.samp.fs0);
    hw.mcu.isr1.period = params.sys.samp.fs0_fs1_ratio;

    hw.mcu.isr0_exe.sec_per_tick = (1.0f / hw.mcu.clk.tcpwm);   // [sec/ticks]
    hw.mcu.isr0_exe.inv_max_time = params.sys.samp.fs0;         // [1/sec]
    hw.mcu.isr1_exe.sec_per_tick = (1.0f / hw.mcu.clk.tcpwm);   // [sec/ticks]
    hw.mcu.isr1_exe.inv_max_time = params.sys.samp.fs1;         // [1/sec]

    Cy_TCPWM_Counter_Init(LLC_ISR0_HW, LLC_ISR0_NUM, &LLC_ISR0_config);
    Cy_TCPWM_Counter_SetPeriod(LLC_ISR0_HW, LLC_ISR0_NUM, hw.mcu.isr0.period - 1U);

    Cy_TCPWM_Counter_Init(LLC_ISR1_HW, LLC_ISR1_NUM, &LLC_ISR1_config);
    Cy_TCPWM_Counter_SetPeriod(LLC_ISR1_HW, LLC_ISR1_NUM, hw.mcu.isr1.period - 1U);
    TCPWM_GRP_CNT_TR_OUT_SEL(LLC_ISR1_HW, TCPWM_GRP_CNT_GET_GRP(LLC_ISR1_NUM), LLC_ISR1_NUM) =
        (_VAL2FLD(TCPWM_GRP_CNT_V2_TR_OUT_SEL_OUT0, CY_TCPWM_CNT_TRIGGER_ON_DISABLED) |
         _VAL2FLD(TCPWM_GRP_CNT_V2_TR_OUT_SEL_OUT1, CY_TCPWM_CNT_TRIGGER_ON_DISABLED)); // workaround, disable both trigger output (used for simultaneous start of DRV and REC)

    Cy_TCPWM_PWM_Init(LLC_BRST_HW, LLC_BRST_NUM, &LLC_BRST_config);
    Cy_TCPWM_PWM_SetPeriod0(LLC_BRST_HW, LLC_BRST_NUM, hw.mcu.brst.period);
    Cy_TCPWM_InputTriggerSetup(LLC_BRST_HW, LLC_BRST_NUM, CY_TCPWM_INPUT_TR_CAPTURE0, CY_TCPWM_INPUT_LEVEL, CY_TCPWM_INPUT_1); // workaround, auto-update at TC, DRIVERS-23378
    TCPWM_GRP_CNT_CTRL(LLC_BRST_HW, LLC_BRST_GRP, LLC_BRST_NUM) |= _VAL2FLD(TCPWM_GRP_CNT_V2_CTRL_UP_DOWN_MODE, CY_TCPWM_PWM_ASYMMETRIC_ALIGN); // workaround, custom TC generation twice a period, DRIVERS-24906

    #ifdef LLC_BLNK_ENABLED
    Cy_TCPWM_PWM_Init(LLC_BLNK_HW, LLC_BLNK_NUM, &LLC_BLNK_config);
    Cy_TCPWM_PWM_SetPeriod0(LLC_BLNK_HW, LLC_BLNK_NUM, (uint32_t)(hw.mcu.clk.tcpwm * LLC_TMG_BLNK));
    Cy_TCPWM_PWM_SetCompare0(LLC_BLNK_HW, LLC_BLNK_NUM, (uint32_t)(hw.mcu.clk.tcpwm * LLC_TMG_BLNK));
    #endif

    for (uint8_t idx = 0U; idx < NPWM; ++idx)
    {
        Cy_TCPWM_PWM_Init(LLC_PWM_HW[idx], LLC_PWM_NUM[idx], LLC_PWM_CFG[idx]);
        Cy_TCPWM_InputTriggerSetup(LLC_PWM_HW[idx], LLC_PWM_NUM[idx], CY_TCPWM_INPUT_TR_CAPTURE0, CY_TCPWM_INPUT_LEVEL, CY_TCPWM_INPUT_1); // workaround, auto-update at TC, DRIVERS-23378
        TCPWM_GRP_CNT_CTRL(LLC_PWM_HW[idx], LLC_PWM_GRP[idx], LLC_PWM_NUM[idx]) |= _VAL2FLD(TCPWM_GRP_CNT_V2_CTRL_UP_DOWN_MODE, CY_TCPWM_PWM_ASYMMETRIC_ALIGN); // workaround, custom TC generation twice a period, DRIVERS-24906
    }

    Cy_TCPWM_Counter_Init(EXE_TIMER_HW, EXE_TIMER_NUM, &EXE_TIMER_config); // execution timer
}


void MCU_InitProtections()
{
    // Initialize protection parameters
    MCU_SetVinProtThld(params.sys.faults.hw.vin.thresh.max);
    MCU_SetVoutProtThld(params.sys.faults.hw.vout.thresh.max);
    MCU_SetIresProtThld(params.sys.faults.hw.ires.thresh.max);
    MCU_SetIoutProtThld(params.sys.faults.hw.iout.thresh.max);
}


// local definitions, used for HW protection latching feature only
#if (LLC_PROT_VIN_LATCH && LLC_PROT_VOUT_LATCH)
#define LLC_PROT_LIM_INTR_MSK (LLC_PROT_VIN_INTR_MSK | LLC_PROT_VOUT_INTR_MSK)
#elif (LLC_PROT_VIN_LATCH)
#define LLC_PROT_LIM_INTR_MSK (LLC_PROT_VIN_INTR_MSK)
#elif (LLC_PROT_VOUT_LATCH)
#define LLC_PROT_LIM_INTR_MSK (LLC_PROT_VOUT_INTR_MSK)
#endif

#if (LLC_PROT_IRES_LATCH && LLC_PROT_IOUT_LATCH)
#define LLC_PROT_CMP_INTR_MSK (LLC_PROT_IRES_INTR_MSK | LLC_PROT_IOUT_INTR_MSK)
#elif (LLC_PROT_IRES_LATCH)
#define LLC_PROT_CMP_INTR_MSK (LLC_PROT_IRES_INTR_MSK)
#elif (LLC_PROT_IOUT_LATCH)
#define LLC_PROT_CMP_INTR_MSK (LLC_PROT_IOUT_INTR_MSK)
#endif

RAMFUNC_BEGIN
void MCU_RunISR0()
{
    #ifdef TEST_ISR0_ENABLED
    Cy_GPIO_Set(TEST_ISR0_PORT, TEST_ISR0_PIN);
    #endif
    MCU_StartTimeCap(&hw.mcu.isr0_exe);

    Cy_TCPWM_ClearInterrupt(LLC_ISR0_HW, LLC_ISR0_NUM, LLC_ISR0_config.interruptSources);

    MCU_ReadADCResults();

    sensor_iface.vin.raw = hw.mcu.adc_scale.vin * (int16_t)(hw.mcu.adc_results[ADC_VIN]);
    sensor_iface.vout.raw = hw.mcu.adc_scale.vout * (int16_t)(hw.mcu.adc_results[ADC_VOUT]);
    sensor_iface.iout.raw = hw.mcu.adc_scale.iout * (int16_t)(hw.mcu.adc_results[ADC_IOUT]) - (ADC_OFFSET_IOUT/ADC_SCALE_IOUT);

    // Read the status of HW faults from CSG and ADC limits
    #if (LLC_PROT_VIN_LATCH | LLC_PROT_VOUT_LATCH)
    {
        uint32_t loc_sar_lim_intr = Cy_HPPASS_SAR_Limit_GetInterruptStatus();
        #if (LLC_PROT_VIN_LATCH)
        sensor_iface.digital.ov_vin = (bool)(LLC_PROT_VIN_INTR_MSK & loc_sar_lim_intr);
        #endif
        #if (LLC_PROT_VOUT_LATCH)
        sensor_iface.digital.ov_vout = (bool)(LLC_PROT_VOUT_INTR_MSK & loc_sar_lim_intr);
        #endif
        Cy_HPPASS_SAR_Limit_ClearInterrupt(LLC_PROT_LIM_INTR_MSK & loc_sar_lim_intr);
    }
    #endif // LLC_PROT_VIN_LATCH | LLC_PROT_VOUT_LATCH

    #if (LLC_PROT_IRES_LATCH | LLC_PROT_IOUT_LATCH)
    {
        uint32_t loc_csg_cmp_intr = Cy_HPPASS_Comp_GetInterruptStatus();
        #if (LLC_PROT_IRES_LATCH)
        sensor_iface.digital.oc_ires = (bool)(LLC_PROT_IRES_INTR_MSK & loc_csg_cmp_intr);
        #endif
        #if (LLC_PROT_IOUT_LATCH)
        sensor_iface.digital.oc_iout = (bool)(LLC_PROT_IOUT_INTR_MSK & loc_csg_cmp_intr);
        #endif
        Cy_HPPASS_Comp_ClearInterrupt(LLC_PROT_CMP_INTR_MSK & loc_csg_cmp_intr);
    }
    #endif // LLC_PROT_IRES_LATCH | LLC_PROT_IOUT_LATCH

    STATE_MACHINE_RunISR0();

    mtb_oscilloscope_sampling();

    MCU_ModUpd();

    MCU_StopTimeCap(&hw.mcu.isr0_exe);
    #ifdef TEST_ISR0_ENABLED
    Cy_GPIO_Clr(TEST_ISR0_PORT, TEST_ISR0_PIN);
    #endif
}


RAMFUNC_END

void MCU_RunISR1()
{
    #ifdef TEST_ISR1_ENABLED
    Cy_GPIO_Set(TEST_ISR1_PORT, TEST_ISR1_PIN);
    #endif
    MCU_StartTimeCap(&hw.mcu.isr1_exe);

    Cy_TCPWM_ClearInterrupt(LLC_ISR1_HW, LLC_ISR1_NUM, LLC_ISR1_config.interruptSources);

    sensor_iface.cmd.raw = hw.mcu.adc_scale.pot * (uint16_t)hw.mcu.adc_results[ADC_POT];
    sensor_iface.temp.raw = MCU_TempSensorCalc(hw.mcu.adc_scale.temp * (uint16_t)hw.mcu.adc_results[ADC_TEMP]);

    sensor_iface.digital.user_btn = !Cy_GPIO_Read(N_LLC_USER_BTN_PORT, N_LLC_USER_BTN_PIN);

    STATE_MACHINE_RunISR1();

    Cy_GPIO_Write(LLC_FAULT_LED_PORT, LLC_FAULT_LED_PIN, (bool)(faults.flags_latched.all));
    Cy_GPIO_Write(LLC_ACT_LED_PORT, LLC_ACT_LED_PIN, (sm.current == Freq_Ctrl_State) || (sm.current == Volt_Ctrl_State) || (sm.current == Duty_Ctrl_State)); // only active control states

    MCU_StopTimeCap(&hw.mcu.isr1_exe);
    MCU_ProcessTimeCapISR1(&hw.mcu.isr0_exe);
    MCU_ProcessTimeCapISR1(&hw.mcu.isr1_exe);
    #ifdef TEST_ISR1_ENABLED
    Cy_GPIO_Clr(TEST_ISR1_PORT, TEST_ISR1_PIN);
    #endif
}


void MCU_StartPeripherals()
{
    MCU_EnterCriticalSection(); // No ISRs beyond this point

    NVIC_EnableIRQ(hw.mcu.interrupt.nvic_isr0);
    NVIC_EnableIRQ(hw.mcu.interrupt.nvic_isr1);

    Cy_HPPASS_DAC_Start(LLC_IRES_OC_SLICE_IDX, CY_HPPASS_DAC_FW);
    Cy_HPPASS_DAC_Start(LLC_IOUT_OC_SLICE_IDX, CY_HPPASS_DAC_FW);

    Cy_TCPWM_Counter_Enable(LLC_ISR0_HW, LLC_ISR0_NUM);
    Cy_TCPWM_Counter_Enable(LLC_ISR1_HW, LLC_ISR1_NUM);
    Cy_TCPWM_PWM_Enable(LLC_BRST_HW, LLC_BRST_NUM);
    #ifdef LLC_BLNK_ENABLED
    Cy_TCPWM_PWM_Enable(LLC_BLNK_HW, LLC_BLNK_NUM);
    #endif
    for (uint8_t idx = 0U; idx < NPWM; ++idx)
    {
        Cy_TCPWM_PWM_Enable(LLC_PWM_HW[idx], LLC_PWM_NUM[idx]);
    }
    Cy_TCPWM_Counter_Enable(EXE_TIMER_HW, EXE_TIMER_NUM);

    Cy_TCPWM_TriggerStart_Single(LLC_ISR0_HW, LLC_ISR0_NUM);
    Cy_TCPWM_TriggerStart_Single(LLC_ISR1_HW, LLC_ISR1_NUM);
    Cy_TCPWM_TriggerStart_Single(LLC_BRST_HW, LLC_BRST_NUM);
    #ifdef LLC_BLNK_ENABLED
    Cy_TCPWM_TriggerStart_Single(LLC_BLNK_HW, LLC_BLNK_NUM);
    #endif
    Cy_TrigMux_SwTrigger((uint32_t)LLC_ISR1_tr_out1_0_TRIGGER_IN, CY_TRIGGER_TWO_CYCLES); // synchronously start DRV(s) and SR(s)
    Cy_TCPWM_TriggerStart_Single(EXE_TIMER_HW, EXE_TIMER_NUM);

    MCU_ExitCriticalSection();
}


void MCU_StopPeripherals()
{
    MCU_EnterCriticalSection(); // No ISRs beyond this point

    Cy_TCPWM_Counter_Disable(EXE_TIMER_HW, EXE_TIMER_NUM);
    for (uint8_t idx = 0U; idx < NPWM; ++idx)
    {
        Cy_TCPWM_PWM_Disable(LLC_PWM_HW[idx], LLC_PWM_NUM[idx]);
    }
    Cy_TCPWM_PWM_Disable(LLC_BRST_HW, LLC_BRST_NUM);
    Cy_TCPWM_Counter_Disable(LLC_ISR1_HW, LLC_ISR1_NUM);
    Cy_TCPWM_Counter_Disable(LLC_ISR0_HW, LLC_ISR0_NUM);

    NVIC_DisableIRQ(hw.mcu.interrupt.nvic_isr1);
    NVIC_DisableIRQ(hw.mcu.interrupt.nvic_isr0);

    MCU_ExitCriticalSection();
}


void MCU_EnterCriticalSection()
{
    hw.mcu.interrupt.state = Cy_SysLib_EnterCriticalSection();
}


void MCU_ExitCriticalSection()
{
    Cy_SysLib_ExitCriticalSection(hw.mcu.interrupt.state);
}


void MCU_GateDriverEnterHighZ()
{
    Cy_TCPWM_PWM_Configure_LineSelect(LLC_BRST_HW, LLC_BRST_NUM, CY_TCPWM_OUTPUT_CONSTANT_1, CY_TCPWM_OUTPUT_CONSTANT_0); // Kill all PWMs lines synchronously

    for (uint8_t idx = 0U; idx < NDRP; ++idx)
    {
        Cy_GPIO_SetHSIOM(LLC_DR_PORT[idx], LLC_DR_PIN[idx], HSIOM_SEL_GPIO);
    }
}


void MCU_GateDriverExitHighZ()
{
    for (uint8_t idx = 0U; idx < NDRP; ++idx)
    {
        Cy_GPIO_SetHSIOM(LLC_DR_PORT[idx], LLC_DR_PIN[idx], LLC_DR_HSIOM[idx]);
    }

    Cy_TCPWM_PWM_Configure_LineSelect(LLC_BRST_HW, LLC_BRST_NUM, CY_TCPWM_OUTPUT_PWM_SIGNAL, CY_TCPWM_OUTPUT_CONSTANT_0); // Release PWMs synchronously - avoid asymmetric startup
}


void MCU_SyncRecEn(bool en)
{
    if (en)
    {
        for (uint8_t idx = 0U; idx < NSRP; ++idx)
        {
            Cy_GPIO_SetHSIOM(LLC_SR_PORT[idx], LLC_SR_PIN[idx], LLC_SR_HSIOM[idx]);
        }
    }
    else
    {
        for (uint8_t idx = 0U; idx < NSRP; ++idx)
        {
            Cy_GPIO_SetHSIOM(LLC_SR_PORT[idx], LLC_SR_PIN[idx], HSIOM_SEL_GPIO);
        }
    }
}


void MCU_FaultResetEngage()
{
    // There is no fault reset by MCU possibility on the KIT_PSC3_LLC1
}


void MCU_FaultResetRelease()
{
    // There is no fault reset by MCU possibility on the KIT_PSC3_LLC1
}


const uint8_t* Em_Eeprom_Storage = (uint8_t*)(CY_FLASH_BASE + CY_FLASH_SIZE - srss_0_eeprom_0_PHYSICAL_SIZE);

void MCU_FlashInit()
{
    // EEPROM Emulator
    hw.mcu.eeprom.config.eepromSize = srss_0_eeprom_0_SIZE,
    hw.mcu.eeprom.config.simpleMode = srss_0_eeprom_0_SIMPLEMODE,
    hw.mcu.eeprom.config.wearLevelingFactor = srss_0_eeprom_0_WEARLEVELING_FACTOR,
    hw.mcu.eeprom.config.redundantCopy = srss_0_eeprom_0_REDUNDANT_COPY,
    hw.mcu.eeprom.config.blockingWrite = srss_0_eeprom_0_BLOCKINGMODE,
    hw.mcu.eeprom.config.userFlashStartAddr = (uint32_t)&(Em_Eeprom_Storage[0U]),

    hw.mcu.eeprom.status = Cy_Em_EEPROM_Init(&hw.mcu.eeprom.config, &hw.mcu.eeprom.context);
    hw.mcu.eeprom.init_done = true;
}


bool MCU_FlashReadParams(PARAMS_ID_t id, PARAMS_t* ram_data)
{
    bool rslt = false;

    if (!hw.mcu.eeprom.init_done)
    {
        MCU_FlashInit();
    }

    if (CY_EM_EEPROM_SUCCESS == hw.mcu.eeprom.status)
    {
        hw.mcu.eeprom.status = Cy_Em_EEPROM_Read(0UL, ram_data, sizeof(PARAMS_t), &hw.mcu.eeprom.context);

        if (CY_EM_EEPROM_SUCCESS == hw.mcu.eeprom.status)
        {
            rslt = (ram_data->id.code == id.code) && (ram_data->id.build_config == id.build_config) && (ram_data->id.ver == id.ver);
        }
    }

    return rslt;
}


bool MCU_FlashWriteParams(PARAMS_t* ram_data)
{
    if (!hw.mcu.eeprom.init_done)
    {
        MCU_FlashInit();
    }

    if (CY_EM_EEPROM_SUCCESS == hw.mcu.eeprom.status)
    {
        hw.mcu.eeprom.status = Cy_Em_EEPROM_Write(0U, ram_data, sizeof(PARAMS_t), &hw.mcu.eeprom.context);
    }

    return (CY_EM_EEPROM_SUCCESS == hw.mcu.eeprom.status);
}


RAMFUNC_BEGIN
void MCU_StartTimeCap(MCU_TIME_CAP_t* time_cap)
{
    time_cap->start = (int32_t)(Cy_TCPWM_Counter_GetCounter(EXE_TIMER_HW, EXE_TIMER_NUM));
}


RAMFUNC_END

RAMFUNC_BEGIN
void MCU_StopTimeCap(MCU_TIME_CAP_t* time_cap)
{
    // Over flow and roll-over is OK as long as int32_t is used for 32bit timer/counters
    time_cap->stop = (int32_t)(Cy_TCPWM_Counter_GetCounter(EXE_TIMER_HW, EXE_TIMER_NUM));
    time_cap->duration_ticks = time_cap->stop - time_cap->start;
}


RAMFUNC_END

void MCU_ProcessTimeCapISR1(MCU_TIME_CAP_t* time_cap)
{
    time_cap->duration_sec = ((float)(time_cap->duration_ticks)) * (time_cap->sec_per_tick);
    time_cap->util = time_cap->duration_sec * time_cap->inv_max_time;
}


inline float32_t MCU_TempSensorCalc(float32_t adc_scaled)
{
    float32_t result;
    #if (ACTIVE_TEMP_SENSOR) // Active IC
    result = adc_scaled - (ADC_OFFSET_TEMP/ADC_SCALE_TEMP);
    #else // Passive NTC
    uint32_t index = SAT(1U, TEMP_SENS_LUT_WIDTH - 1U, (uint32_t)(adc_scaled * Temp_Sens_LUT.step_inv));
    float32_t input_index = Temp_Sens_LUT.step * index;
    result = Temp_Sens_LUT.val[index-1U] + (adc_scaled - input_index) * Temp_Sens_LUT.step_inv * (Temp_Sens_LUT.val[index] - Temp_Sens_LUT.val[index-1U]);
    #endif
    return result;
}
