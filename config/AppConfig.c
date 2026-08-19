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

#include "HardwareConfig.h"
#include "AppConfig.h"
#include "Params.h"

/**
 * \addtogroup group_llc_hw_sr
 * \{
 * The synchronous rectifier timing is controlled by two look-up tables for rising and falling edges separately, \ref Td_on_sr and \ref Td_off_sr
 * The LUTs are coded as linear arrays, however actually they are two dimensional:
 * * The horizontal X axis is a frequency dimension, usually defined by the modulator frequency range,
 *   see the PARAMS_InitAutoCalc middleware function for details.
 *   Each column numbers correspond to the frequency fsw.min + N * ((fsw.max - fsw.min) / (LUT_2D_WIDTH_X - 1)), where N - column number from 0 to LUT_2D_WIDTH_X - 1.
 *   So the frequency scale is linear.
 * * The vertical Y axis is a conductance (opposite to the load resistance) dimension, usually defined from zero (high-Z load) to nominal load resistance,
 *   see the PARAMS_InitAutoCalc middleware function for details.
 *   Each column numbers correspond to the conductance G.min + N * ((G.max - G.min) / (LUT_2D_WIDTH_Y - 1)), where N - column number from 0 to LUT_2D_WIDTH_Y - 1.
 *   So the conductance scale is linear.
 *
 * Timings are expressed naturally in seconds, e.g. 100.0E-9f means 100 ns.<br>
 * The positive value means delay, the negative - advancing (for both Ton and Toff).<br>
 * The technical limits of the SR timings is +- half of switching period, but the difference Ton - Toff cannot be less than roughly zero
 * ('roughly' because it depends also on the current primary driver dead time, and the SR minimal dead time is ~20ns).
 * Anyway, they are protected internally, i.e. if to set Ton to -200ns and Toff to +300ns - the actual SR pulses will not overlap each other,
 * instead it will work with minimal dead time ~20ns.<br>
 *
 * To perform fine syncronous rectifier timing tuning, the simplest way is to observe the secondary transformer coil voltage to secondary GND (across any SR switch) by oscilloscope:
 * * when the voltage goes under zero (roughly -0.7V) - this means the SR switch is opened and the current flows through the body diode.
 * * when the voltage is going to zero - this means the SR switch is closed and actively rectifying the output current.
 * \image html sr.png
 * D0..D3 signals are primary bridge driver, D4 and D5 are SR driver, and yellow/blue are both secondary transformer coil ends to GND_PS on KIT_PSC3_LLC1.<br>
 * The \ref Td_on_sr and \ref Td_off_sr can be precisely adjusted to achieve the minimal dead time / losses in whole the working range (frequency vs load) for the particular board.
 * \note Please make sure, the SR timing is modulated based on mesured output current, which is heavily filtered on the board and additionally within MCU and FW (to supress noises)
 * so during brief load transitions the SR modulation can possibly be delayed relatively to the actual output current phase, and without some timing marging the current back flow is possible.
 * Therefore it is recommended to leave some timing margins (leave SR switches opened) at the low output current areas (where losses aren't significant) when it is planned to work with dynamic loads.
 **/
static const float32_t Td_on_sr[LUT_2D_WIDTH_X*LUT_2D_WIDTH_Y] = /**< Synchronous rectifier rise/on time relative to the primary driver PWM DRV0 rising edge */
{
/** Fsw.min                                                                      Fsw.max  */
    400.0E-9f, 400.0E-9f, 400.0E-9f, 400.0E-9f, 500.0E-9f, 500.0E-9f, 500.0E-9f, 500.0E-9f, /** Load min, R = inf, G = 0 */
    400.0E-9f, 400.0E-9f, 400.0E-9f, 400.0E-9f, 500.0E-9f, 500.0E-9f, 500.0E-9f, 500.0E-9f,
    200.0E-9f, 200.0E-9f, 200.0E-9f, 200.0E-9f, 250.0E-9f, 250.0E-9f, 250.0E-9f, 250.0E-9f,
    200.0E-9f, 200.0E-9f, 200.0E-9f, 200.0E-9f, 250.0E-9f, 250.0E-9f, 250.0E-9f, 250.0E-9f,
    200.0E-9f, 200.0E-9f, 200.0E-9f, 200.0E-9f, 250.0E-9f, 250.0E-9f, 250.0E-9f, 250.0E-9f,
    200.0E-9f, 200.0E-9f, 200.0E-9f, 200.0E-9f, 250.0E-9f, 250.0E-9f, 250.0E-9f, 250.0E-9f,
    200.0E-9f, 200.0E-9f, 200.0E-9f, 200.0E-9f, 250.0E-9f, 250.0E-9f, 250.0E-9f, 250.0E-9f,
    200.0E-9f, 200.0E-9f, 200.0E-9f, 200.0E-9f, 250.0E-9f, 250.0E-9f, 250.0E-9f, 250.0E-9f  /** Load max, R = nom, G = 1/R */
};

static const float32_t Td_off_sr[LUT_2D_WIDTH_X*LUT_2D_WIDTH_Y] = /**< Synchronous rectifier fall/off time relative to the primary driver PWM DRV0 falling edge */
{
    0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f,
    0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f,
    0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f,
    0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f,
    0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f,
    0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f,
    0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f,
    0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f, 0.0E-9f
};
/** \}*/

/**
 * \addtogroup group_llc_hw_dt
 * \{
 * The primary bridge driver dead time can be (optionally) controlled by the \ref T_dt look-up table.<br>
 * The dead time (in LUT mode) is modulated based of switching frequency and conductance, similarly to the \ref group_llc_hw_sr.<br>
 * The dead time value is expressed naturally in seconds, e.g. 50.0E-9f means 50 ns.<br>
 * It is not recommended to set the dead time less than 20ns for the KIT_PSC3_LLC1 board.<br>
 * \note the params.llc.dt.method is set to DT_Perc (percentage) by default - this setting should be changed to DT_LUT to make the system work with T_dt table.
 *
 * To perform fine dead time tuning, the simplest way is to observe the primary bridge voltage (e.g. TP503 or TP513 to GND_PP on the KIT_PSC3_LLC1)
 * by the oscilloscope:
 * * The smooth slope without any creases with duration ~40ns and no significant ringing at the end means 'soft switching':
 * \image html soft.png
 * * The noticeable bending at the slope line, reduced duration, and trailing ringing means 'hard switching':
 * \image html hard.png
 * The T_dt can be precisely adjusted to achieve the minimal dead time with soft switching in whole the working range (frequency vs load) for the particular board.
 **/
static const float32_t T_dt[LUT_2D_WIDTH_X * LUT_2D_WIDTH_Y] = /**< Primary bridge driver dead time */
{
/** Fsw.min                                                               Fsw.max  */
    50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, /** Load min, R = inf, G = 0 */
    50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f,
    50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f,
    50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f,
    50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f,
    50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f,
    50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f,
    50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f, 50.0E-9f  /** Load max, R = nom, G = 1/R */
};
/** \}*/

void APP_PARAMS_Init(void)
{
    params_fcn.InitManual = APP_PARAMS_InitManual; // application
    params_fcn.InitAutoCalc = PARAMS_InitAutoCalc; // library
    params_fcn.InitOverWrite = APP_PARAMS_InitOverWrite; // application
}


void APP_PARAMS_InitManual(void)
{
    // LLC Parameters:
    // -> nominal ratings
    params.llc.nom.type = Full_Bridge; // [#]
    params.llc.nom.vin = LLC_NOM_VIN; // [V], dc
    params.llc.nom.vout = LLC_NOM_VOUT; // [V], dc
    params.llc.nom.pout = LLC_NOM_POUT; // [W], dc
    params.llc.nom.ires = LLC_NOM_IRES; // [A], ac peak
    // -> circuit parameters
    params.llc.circuit.Lr = LLC_CIRCUIT_Lr; // [H]
    params.llc.circuit.Cr = LLC_CIRCUIT_Cr; // [F]
    params.llc.circuit.Lm = LLC_CIRCUIT_Lm; // [H]
    params.llc.circuit.n = LLC_CIRCUIT_N; // [#]
    params.llc.circuit.C = LLC_CIRCUIT_C; // [F]
    // -> operating range parameters
    params.llc.range.fsw = (MINMAX_t) { 485.0E3f, 1.0E6f }; // [Hz]
    params.llc.range.vout = (MINMAX_t) { 3.75f, 5.60f }; // [V]
    // -> synchronous rectification parameters
    params.llc.sr.en = En; // [], disabled by default, LUTs must be initialized by HW tests before enabling
    // -> dead-time modulation parameters
    params.llc.dt.method = DT_Perc; // []
    params.llc.dt.perc = PERC_TO_NORM(5.0f); // [%]

    // System Parameters:
    // -> command
    params.sys.cmd.source = Internal;
    params.sys.cmd.vout.tgt = params.llc.nom.vout; // [V]
    // -> rate limiter
    params.sys.rate_lim.vout_cmd = params.sys.cmd.vout.tgt / 32E-3f; // [V/sec]
    // -> sampling
    params.sys.samp.fs0 = 50.0E3f; // [Hz]
    params.sys.samp.fs0_fs1_ratio = 5U; // []

    // Control Parameters:
    // --> general
    params.ctrl.id.mode = Voltage_Mode;   // []
    params.ctrl.auto_start = En;            // [] Enable auto-start by default
    params.ctrl.soft_start.f = 1.25E6f;      // [Hz]
    params.ctrl.soft_start.t = 1.0E-3f;     // [sec]
    params.ctrl.burst.f = 25.0E3f;          // [Hz]
    // --> output-voltage control
    params.ctrl.vout.bw = HZ_TO_RADSEC(500.0f); // [Ra/sec]
    // --> system identification
    params.ctrl.sys_id.en = true;
    // --> fan control
    params.ctrl.fan.en = ACTIVE_FAN_CONTROL;
    params.ctrl.fan.f_pwm = 25.0E3f; // [Hz]
    params.ctrl.fan.cpr = 2.0f; // [#]
}


void APP_PARAMS_InitOverWrite(void)
{
    params.sys.faults.hw.ires.thresh.max = params.llc.nom.ires * PERC_TO_NORM(230.0f); // [A], workaround to avoid HW protection false triggers at startup
    params.sys.faults.hw.iout.thresh.max = params.llc.nom.iout * PERC_TO_NORM(180.0f); // [A], workaround to avoid HW protection false triggers at startup
    params.sys.faults.hw.iout.thresh.max *= PI_OVER_TWO; // [A], pulsing (rectified unfiltered) peak (average * PI/2)
    params.sys.faults.sw.temp.thresh.max = 75.0f; // [Celsius], dc

    // Bypassing debouncers for the HW protection latching feature
    params.sys.faults.hw.ires.time = DISABLE_DBF_FS(params.sys.samp.fs0);
    params.sys.faults.hw.iout.time = DISABLE_DBF_FS(params.sys.samp.fs0);
    params.sys.faults.hw.vout.time = DISABLE_DBF_FS(params.sys.samp.fs0);

    LUT2DInit(&params.llc.sr.Td_on,
              (MINMAX_t) { params.llc.sr.Td_on.x_min, params.llc.sr.Td_on.x_max },
              (MINMAX_t) { params.llc.sr.Td_on.y_min, params.llc.sr.Td_on.y_max },
              Td_on_sr);
    LUT2DInit(&params.llc.sr.Td_off,
              (MINMAX_t) { params.llc.sr.Td_off.x_min, params.llc.sr.Td_off.x_max },
              (MINMAX_t) { params.llc.sr.Td_off.y_min, params.llc.sr.Td_off.y_max },
              Td_off_sr);
    LUT2DInit(&params.llc.dt.lut,
              (MINMAX_t) { params.llc.dt.lut.x_min, params.llc.dt.lut.x_max },
              (MINMAX_t) { params.llc.dt.lut.y_min, params.llc.dt.lut.y_max },
              T_dt);
    params.sys.faults.auto_clr_per = 10.0E-3f;
}
