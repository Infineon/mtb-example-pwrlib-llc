/*******************************************************************************
* File Name:   mtb_oscilloscope_data_cfg.c
*
* Description: Application-specific configuration of the oscilloscope variable
*              list. Customize the contents of the mtb_oscilloscope_data_list[]
*              array below to expose the variables monitored by the
*              oscilloscope. The mtb_oscilloscope_data_registry is built from
*              this list at compile time and consumed by the oscilloscope
*              data module.
*
********************************************************************************
* \copyright
* (c) 2026, Infineon Technologies AG or an affiliate of
* Infineon Technologies AG.
*
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/*******************************************************************************
*       INCLUDES
*******************************************************************************/
#include "mtb_oscilloscope_data.h"
#include "mtb_oscilloscope.h"
#include "CtrlVars.h"
#include "Params.h"
#include "SensorIface.h"


/*******************************************************************************
*       EXPORTED VARIABLES
*******************************************************************************/
// Application-specific list of variables to be exposed via the oscilloscope.
/* coverity[misra_c_2012_rule_5_9_violation] Caused by testing tools implementation specific, not a code defect */
static const SCOPEDATA_VAR_t mtb_oscilloscope_data_list[] =
{
    { "Vin_fb     ", (uint8_t*)&vars.vin_fb,            MTB_OSCILLOSCOPE_FP32 },
    { "Vout_fb    ", (uint8_t*)&vars.vout_fb,           MTB_OSCILLOSCOPE_FP32 },
    { "Iout_fb    ", (uint8_t*)&vars.iout_fb,           MTB_OSCILLOSCOPE_FP32 },
    { "Temp_fb    ", (uint8_t*)&vars.temp_fb,           MTB_OSCILLOSCOPE_FP32 },

    { "Vin_raw    ", (uint8_t*)&sensor_iface.vin.raw,   MTB_OSCILLOSCOPE_FP32 },
    { "Vin_flt    ", (uint8_t*)&sensor_iface.vin.filt,  MTB_OSCILLOSCOPE_FP32 },
    { "Vout_raw   ", (uint8_t*)&sensor_iface.vout.raw,  MTB_OSCILLOSCOPE_FP32 },
    { "Vout_flt   ", (uint8_t*)&sensor_iface.vout.filt, MTB_OSCILLOSCOPE_FP32 },
    { "Iout_raw   ", (uint8_t*)&sensor_iface.iout.raw,  MTB_OSCILLOSCOPE_FP32 },
    { "Iout_flt   ", (uint8_t*)&sensor_iface.iout.filt, MTB_OSCILLOSCOPE_FP32 },
    { "Cmd_raw    ", (uint8_t*)&sensor_iface.cmd.raw,   MTB_OSCILLOSCOPE_FP32 },
    { "Cmd_flt    ", (uint8_t*)&sensor_iface.cmd.filt,  MTB_OSCILLOSCOPE_FP32 },
    { "Temp_raw   ", (uint8_t*)&sensor_iface.temp.raw,  MTB_OSCILLOSCOPE_FP32 },
    { "Temp_flt   ", (uint8_t*)&sensor_iface.temp.filt, MTB_OSCILLOSCOPE_FP32 },

    { "Cmd_ext    ", (uint8_t*)&vars.cmd_ext,           MTB_OSCILLOSCOPE_FP32 },
    { "Cmd_int    ", (uint8_t*)&vars.cmd_int,           MTB_OSCILLOSCOPE_FP32 },
    { "Cmd_final  ", (uint8_t*)&vars.cmd_final,         MTB_OSCILLOSCOPE_FP32 },
    { "Vo_cmd_ext ", (uint8_t*)&vars.vout_cmd_ext,      MTB_OSCILLOSCOPE_FP32 },
    { "Vo_cmd_int ", (uint8_t*)&vars.vout_cmd_int,      MTB_OSCILLOSCOPE_FP32 },
    { "Tsw_cmd_ext", (uint8_t*)&vars.Tsw_cmd_ext,       MTB_OSCILLOSCOPE_FP32 },
    { "Tsw_cmd_vlt", (uint8_t*)&vars.Tsw_cmd_volt,      MTB_OSCILLOSCOPE_FP32 },
    { "Tsw_cmd_int", (uint8_t*)&vars.Tsw_cmd_int,       MTB_OSCILLOSCOPE_FP32 },
    { "Td_on_sr   ", (uint8_t*)&vars.Td_on_sr,          MTB_OSCILLOSCOPE_FP32 },
    { "Td_off_sr  ", (uint8_t*)&vars.Td_off_sr,         MTB_OSCILLOSCOPE_FP32 },
    { "d_cmd_ext  ", (uint8_t*)&vars.d_cmd_ext,         MTB_OSCILLOSCOPE_FP32 },
    { "d_cmd_volt ", (uint8_t*)&vars.d_cmd_volt,        MTB_OSCILLOSCOPE_FP32 },
    { "d_cmd_int  ", (uint8_t*)&vars.d_cmd_int,         MTB_OSCILLOSCOPE_FP32 },

//    { "pi_vout_out", (uint8_t*)&ctrl.pi_vout.output,    MTB_OSCILLOSCOPE_FP32 },
    // Add more variables as needed
};


// Variable registry consumed by the oscilloscope data module.
// Initialized at compile time from the mtb_oscilloscope_data_list[] above.
const SCOPEDATA_t mtb_oscilloscope_data_registry =
{
    .var_count = sizeof(mtb_oscilloscope_data_list) / sizeof(SCOPEDATA_VAR_t),
    .var_list  = mtb_oscilloscope_data_list
};
