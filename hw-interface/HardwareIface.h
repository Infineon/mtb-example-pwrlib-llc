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

#pragma once

#include "cybsp.h"
#include "stdio.h"
#include "HardwareConfig.h"
#include "MCU.h"

/** \cond */
typedef struct
{
    MCU_t mcu;
} HW_t;
/** \endcond */

extern HW_t hw;

/**
 * \defgroup group_llc_init LLC Initialization & Startup
 * \{
 * The LLC reference code example implements a three-step initialization
 * sequence in the main() function before entering the main loop:
 *
 * \code{.c}
 * int main(void)
 * {
 *     // ...
 *     HW_IFACE_Init();       // Step 1: Assign hardware-specific functions to the library interface
 *     APP_PARAMS_Init();     // Step 2: Register parameter initialization callbacks
 *     STATE_MACHINE_Init();  // Step 3: Initialize state machine and enter Init state
 *     // ...
 * }
 * \endcode
 *
 * See \ref HW_IFACE_Init, \ref APP_PARAMS_Init and the **mtb-pwrlib-llc** library
 * STATE_MACHINE_Init-function documentation for details.
 * \}
 */

/** \addtogroup group_llc_init
 * \{
 */

/**
 ***********************************************************************
 * \brief   Initialize the hardware abstraction interface. Assigns
 *          hardware-specific MCU functions to the library's function
 *          pointer interface (hw_fcn). This includes peripheral
 *          init/start/stop, gate driver control, fault reset,
 *          synchronous rectifier enable, critical section management,
 *          and flash read/write operations.
 *
 * \note    Must be called before APP_PARAMS_Init() and
 *          STATE_MACHINE_Init().
 **********************************************************************/
void HW_IFACE_Init();

/** \} */
