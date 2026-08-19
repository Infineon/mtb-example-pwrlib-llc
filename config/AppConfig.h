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

/**
 * \defgroup group_llc_cfg LLC Configuration
 * \{
 * \section param_types Parameter Types
 *
 * \verbatim
 * PARAMETER TYPES
 * |
 * |-- Hardware Parameters
 * |   |-- Can be changed at pre-compile time but not at runtime (since HW doesn't change at runtime).
 * |   |-- They are defined as pre-compile macros.
 * |   '-- Macro definitions are in HardwareConfig.h.
 * |
 * '-- Software Parameters
 *     |-- Can be changed at both pre-compile time and runtime (by GUI).
 *     |-- They are defined as memory structures that can be changed at runtime.
 *     '-- There are two types of SW parameters.
 *         |-- Manual parameters
 *         |   |-- Entered manually by user, e.g. command output voltage.
 *         |   |-- Library has a default set of manual parameters.
 *         |   '-- They are often overwritten by the application in AppConfig.c.
 *         '-- Auto-calculated parameters
 *             |-- Auto-calculated by the library algorithms based on manual parameters e.g. controller kp & ki.
 *             |-- Can be overwritten by application if needed for advanced users (AppConfig.c).
 *             '-- Basic users do not need to set them in the application and can let the library algorithms auto-calculate them.
 *
 * Sequence: Manual Parameters (App) -> Auto-calculations (Lib) -> Overwrites (App)
 * \endverbatim
 *
 * \defgroup group_llc_hw_cfg LLC hardware setup
 * \defgroup group_llc_hw_sr Synchronous rectifier timing
 * \defgroup group_llc_hw_dt Dead time LUT modulation
 * \}
 **/

#pragma once

/** \addtogroup group_llc_init
 *  \{
 */

/**
 ***********************************************************************
 * \brief   Initialize pointers to parameter initialization callbacks.
 *          Registers three function pointers in the logical sequence
 *          of the parameter initialization predefined by the library:
 *          -# \ref APP_PARAMS_InitManual
 *          -# PARAMS_InitAutoCalc (see library documentation for details)
 *          -# \ref APP_PARAMS_InitOverWrite
 *
 * \note    Must be called after \ref HW_IFACE_Init and before
 *          STATE_MACHINE_Init (see library documentation for details).
 **********************************************************************/
void APP_PARAMS_Init(void);

/** \} group_llc_init */

/** \addtogroup group_llc_cfg_sw
 *  \{
 */

/**
 ***********************************************************************
 * \brief   Provide application-specific manual parameters. This
 *          function replaces the library's PARAMS_InitManual and is
 *          the sole source of manual parameters (nominal ratings,
 *          circuit parameters, operating ranges, and control settings).
 *
 * \note    The values set here are used as inputs for the subsequent
 *          PARAMS_InitAutoCalc step.
 **********************************************************************/
void APP_PARAMS_InitManual(void);

/**
 ***********************************************************************
 * \brief   Overwrite library auto-calculated parameters. Use this
 *          function to replace specific values computed by
 *          PARAMS_InitAutoCalc (e.g. controller gains, timing
 *          thresholds) with application-specific values.
 **********************************************************************/
void APP_PARAMS_InitOverWrite(void);
/** \} */
