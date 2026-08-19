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

/**
 * \addtogroup group_llc_cfg
 * \{
 *   \defgroup group_llc_cfg_hw Hardware Parameters
 *   \{
 *    \defgroup group_llc_cfg_hw_brd External Board Parameters
 *    \defgroup group_llc_cfg_hw_mod Internal Modulator Parameters
 *   \}
 *   \defgroup group_llc_cfg_sw Software parameters
 * \}
 *
 * \addtogroup group_llc_hw_cfg
 * \{
 * The code example provides the next hardware setup to drive the LLC plant:
 * \image html HW01.png
 *
 * The burst modulator (BRST) TCPWM parameters can be updated on-the-fly, see **mtb-pwrlib-llc** library documentation
 *
 * All primary driver (DRVx) and synchronous rectifier (SR) TCPWMs are configured as up-down (center-aligned)
 * dead-time PWMs, are being started and updated synchronously.
 *
 * The primary driver PWM DRV0 is in symmetrical mode and the DRV1 is in asymmetrical - this provides a
 * possibility to adjust the primary driver bridge timings to compensate probable HW asymmetry between bridge arms,
 * see \ref LLC_TMG_DRV1_H and \ref LLC_TMG_DRV1_L.
 * \image html HW02.png
 *
 * The ADC triggering time, primary driver ON-time, synchronous rectifier ON-time and phase shift values
 * can be updated on-the-fly, see **mtb-pwrlib-llc** library documentation
 *
 * The modulator output signals are killed by analog input thresholds:
 * - Resonant current threshold - to avoid the resonant circuit overcurrent, especially at low voltages at start
 * - Output voltage threshold - to avoid output overvoltage, especially during load transitions
 *
 * The thresholds can be updated on-the-fly, see the \ref group_llc_hw_prot_thld_func.
 *
 * There is also a PWM generating blanking pulses for the protection CSG comparators to prevent false protection
 * triggering due to the possible power switching noise on the Ires and Iout sensing signals.
 * The blanking pulse duration is configurable using \ref LLC_TMG_BLNK.
 *
 *   \defgroup group_llc_hw_prot_thld_func Modulator HW protection threshold functions
 *   \{
 * These functions serve to prevent the system from failures by killing the modulator output signals when the measured values
 * exceed the defined thresholds. The thresholds can be updated on-the-fly.
 *   \}
 *   \defgroup group_llc_hw_prot_latch Modulator HW protection latching
 * \}
 **/

#pragma once

#include "cybsp.h"
#include "stdio.h"
#include "cy_pdl.h"


#define KIT_ID              (0x00A0U)    /**< For GUI's recognition of HW */

/** \addtogroup group_llc_cfg_sw
 *  \{
 */
#define PARAMS_LOAD_FLASH   (false)     /**< default: true
                                         * - true : Parameters are loaded from flash first. If they are valid, they are used directly, if not, they are loaded from code.
                                         * - false: Parameters are always loaded from code.
                                         */
/** \} */

/** \addtogroup group_llc_cfg_hw_mod
 *  \{
 */
#define NDRM (2U)           /**< Number of primary driver PWMs (2 for full-bridge driver with timing adjustment possibility) */
#define NSRM (1U)           /**< Number of synchronous rectifier PWMs (1 for center-tapped full-wave synchronous rectifier) */
#define NPWM (NDRM + NSRM)  /**< The total number of PWMs (driver + rectifier) */
#define NDRP (4U)           /**< Number of driver PWM output pins */
#define NSRP (2U)           /**< Number of synchronous rectifier PWM output pins */

#define LLC_TMG_DRV1_H (0.0f) /**< [s] The primary driver PWM1 direct line (which is connected to high switch of the second half bridge) rising edge (which is controlled by compare0 + dead time) timing adjustment
                               *        relatively to the DRV0 direct line (which is connected to low switch of the first half bridge) rising edge, in seconds (positive means delay, negative - advancing)
                               */
#define LLC_TMG_DRV1_L (0.0f) /**< [s] The primary driver PWM1 complementary line (which is connected to low switch of the second half bridge) rising edge (which is controlled by compare1 + dead time) timing adjustment
                               *        relatively to the DRV0 complementary line (which is connected to high switch of the first half bridge) rising edge, in seconds (positive means delay, negative - advancing)
                               */
#define LLC_TMG_BLNK   (100.0E-9f) /**< [s] The CSG blanking pulse duration, starting from the primary driver DRV0 compare 0 match event, in seconds */
/** \} */

/**
 * \addtogroup group_llc_hw_prot_latch
 * \{
 * This feature allows to integrate the Modulator HW protection with middlware fault processing.
 * * If disabled - the modulator HW protection works as usual - it kills PWMs signals immediately when the protected parameter overcomes the threshold set by \ref group_llc_hw_prot_thld_func.
 * * If enabled - additionally to the basic (described above) behavior, it also being processed by the middleware FaultProtect subsystem - the HW protection status is being processed by the
 * mtb-pwrlib-llc middleware FaultProtect sub-system.
 *
 * For the KIT_PSC3_LLC1 this is not necessary (disabled by default) because the kit has on-board HW protections.
 * However, for another HW, which e.g. does not have on-board protections, it may be useful to make the MCU internal HW protection triggers processed by the mtb-pwrlib-llc middleware.
 *
 * \note By default (latching is disabled) the nature of the modulator HW protection feature is primarily exactly protection from faults, a kind of opposite to the fault latching.
 * Since it immediately kills PWMs, the protection output triggers are usually dithering (when the threshold overcome is detected in one cycle, it probably will not be detected during next cycle,
 * because PWMs were killed and the observed parameter is descreased), which can be filtered out by debouncer (the usual FaultProtect input),
 * so it may look like the HW protection prevents itself from fault latching.
 * This is exactly why in this Reference Code Example the debouncing is bypassed for Ires, Iout and Vout HW protection channels - to make it useful when latching is enabled.
 * However by default the modulator HW protection thresholds are tuned to protect from HW faults on the KIT_PSC3_LLC1 kit, so they are much lower that on-board protection thresholds.
 * And to work as latched HW fault detection, the thresholds should be adjusted (approximately to the level of KIT_PSC3_LLC1 on-board protection), to avoid undesired false faults.
 **/
#define LLC_PROT_VIN_LATCH  (false)  /**< [bool] Input voltage HW protection latching */
#define LLC_PROT_IRES_LATCH (false)  /**< [bool] Resonant current HW protection latching */
#define LLC_PROT_VOUT_LATCH (false)  /**< [bool] Output voltage HW protection latching */
#define LLC_PROT_IOUT_LATCH (false)  /**< [bool] Output current HW protection latching */
/** \} */

// HW protection interrupt masks, used for HW protection latching feature, aligned with HW design.
#define LLC_PROT_VIN_INTR_MSK  (CY_HPPASS_INTR_SAR_LIMIT_GROUP_1) // Vin channel is in the sequencer group 1
#define LLC_PROT_IRES_INTR_MSK (CY_HPPASS_INTR_CSG_1_CMP)         // Ires is detected by CSG slice 1
#define LLC_PROT_VOUT_INTR_MSK (CY_HPPASS_INTR_SAR_LIMIT_GROUP_0) // Vout channel is in the sequencer group 0
#define LLC_PROT_IOUT_INTR_MSK (CY_HPPASS_INTR_CSG_3_CMP)         // Iout is detected by CSG slice 3

// HPPASS-specific definitions
#define ADC_VREF        (CY_CFG_PWR_VDDA_MV*1.0E-3f)    // volts
#define ADC_RES         (12U)     // bits
#define DAC_RES         (10U)     // bits

/** \addtogroup group_llc_cfg_hw_brd
 *  \{
 */
#define LLC_NOM_VIN         (40.0f)             /**< Nominal input voltage [V] */
#define LLC_NOM_VOUT        (5.0f)              /**< Nominal output voltage [V] */
#define LLC_NOM_POUT        (50.0f)             /**< Nominal output power [W] */
#define LLC_NOM_IRES        (3.3f)              /**< Nominal resonant current [A] */

#define LLC_CIRCUIT_Lr      (2.6E-6f)           /**< Resonant inductance [H] */
#define LLC_CIRCUIT_Cr      (36.0E-9f)          /**< Resonant capacitance [F] */
#define LLC_CIRCUIT_Lm      (12.5E-6f)          /**< Magnetizing inductance [H] */
#define LLC_CIRCUIT_N       (7.0f)              /**< Transformer's turn ratio */
#define LLC_CIRCUIT_C       (1690.0E-6f)        /**< Output capacitance [F] */

#define ACTIVE_TEMP_SENSOR  (false)             /**< Indicates presence of active temperature sensor (as opposed to passive NTC) */
#define ACTIVE_FAN_CONTROL  (false)             /**< Indicates presence of active fan control */

#define ADC_SCALE_VIN       (2.0f*3.3f/(2.0f*3.3f+100.0f))  /**< Input voltage scale [V/V] = [kOhm/kOhm] */
#define ADC_SCALE_VOUT      (3.3f/(3.3f+4.0f*3.3f))         /**< Output voltage scale [V/V] = [kOhm/kOhm] */
#define ADC_SCALE_IRES      (291.3E-3f)                     /**< Resonant current scale [V/A], R526 || R527 = 29.13 Ohms */
#define ADC_OFFSET_IRES     (434.2E-3f)                     /**< Resonant current offset [V], R530 / R531 || R532 provides 434.2 mV */
#define ADC_SCALE_IOUT      (-32.0E-3f)                     /**< Output current scale [V/A], 32 mV/A * correction coefficient, the Iout signal has reversed polarity */
#define ADC_OFFSET_IOUT     (1.645f)                        /**< Output current offset [V], VDD/2 resistive divider + correction offset */
/** \} */

// Internal definitions
#define LLC_DRV0_GRP (TCPWM_GRP_CNT_GET_GRP(LLC_DRV0_NUM))
#define LLC_DRV1_GRP (TCPWM_GRP_CNT_GET_GRP(LLC_DRV1_NUM))
#define LLC_BRST_GRP (TCPWM_GRP_CNT_GET_GRP(LLC_BRST_NUM))
#define LLC_SR_GRP   (TCPWM_GRP_CNT_GET_GRP(LLC_SR_NUM))

extern TCPWM_Type*                      LLC_PWM_HW[NPWM];
extern uint32_t                         LLC_PWM_GRP[NPWM];
extern uint32_t                         LLC_PWM_NUM[NPWM];
extern const cy_stc_tcpwm_pwm_config_t* LLC_PWM_CFG[NPWM];
extern GPIO_PRT_Type*                   LLC_DR_PORT[NDRP];
extern GPIO_PRT_Type*                   LLC_SR_PORT[NSRP];
extern uint32_t                         LLC_DR_PIN[NDRP];
extern uint32_t                         LLC_SR_PIN[NSRP];
extern en_hsiom_sel_t                   LLC_DR_HSIOM[NDRP];
extern en_hsiom_sel_t                   LLC_SR_HSIOM[NSRP];


/**
 * \addtogroup group_llc_hw_prot_thld_func
 * \{
 */

/**
 ***********************************************************************
 * \brief   Set the input voltage protection threshold using the ADC
 *          Digital Comparator module.
 *
 * \param[in] v_in  The Vin threshold in Volts.
 *                   Range: 0...(Vref * Vin gain).
 *                   For example, if Vref is 3.3V and Vin gain is
 *                   0.0619 (divider 6.6:106.7), the maximum Vin
 *                   threshold is 52.4 V.
 **********************************************************************/
void MCU_SetVinProtThld(float32_t v_in);

/**
 ***********************************************************************
 * \brief   Set the output voltage protection threshold using the ADC
 *          Limit Detection module.
 *
 * \param[in] v_out The Vout threshold in Volts.
 *                   Range: 0...(Vref * Vout gain).
 *                   For example, if Vref is 3.3V and Vout gain is
 *                   0.2 (divider 1:5), the maximum Vout threshold
 *                   is 16.5 V.
 *
 * \note    These are theoretical calculations based on ADC Vref and
 *          voltage sensing gain; actual power board capabilities may
 *          differ.
 **********************************************************************/
void MCU_SetVoutProtThld(float32_t v_out);

/**
 ***********************************************************************
 * \brief   Set the resonant current protection threshold using the
 *          CSG comparator.
 *
 * \param[in] i_res The Ires threshold in Amperes.
 *                   Range: 0...(Vref * Ires gain - Ires offset).
 *                   For example, if Vref is 3.3V, Ires gain is
 *                   3.43 A/V and Ires offset is 1.49 A, the maximum
 *                   Ires threshold is 9.83 A.
 **********************************************************************/
void MCU_SetIresProtThld(float32_t i_res);

/**
 ***********************************************************************
 * \brief   Set the output current protection threshold using the
 *          CSG comparator.
 *
 * \param[in] i_out The Iout threshold in Amperes.
 *                   Range: 0...(Vref * Iout gain - Iout offset).
 *                   For example, if Vref is 3.3V, Iout gain is
 *                   31.3 A/V and Iout offset is 51.6 A, the maximum
 *                   Iout threshold is 51.6 A.
 *
 * \note    These are theoretical calculations based on DAC Vref and
 *          current sensing gain/offset; actual power board
 *          capabilities may differ.
 **********************************************************************/
void MCU_SetIoutProtThld(float32_t i_out);
/** \} */
