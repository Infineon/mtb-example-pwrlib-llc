/******************************************************************************
 * File Name:   main.c
 *
 * Description: This code example demonstrates the implementation of LLC power
 * converter using the Infineon's MCU.
 *
 * Related Document: See README.md
 *
 *******************************************************************************
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

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cybsp.h"
#include "stdio.h"
#include "cy_retarget_io.h"
#include "HardwareIface.h"
#include "AppConfig.h"
#include "Controller.h"
#include "Pmbus.h"

/*******************************************************************************
 * Macros
 ********************************************************************************/
#define LOGGING_DELAY_MS       200    /* Logging delay */

/*******************************************************************************
 * Function prototypes
 ********************************************************************************/
static void RetargetIoInit(void);
static void RetargetIoRun(void);

/*******************************************************************************
 * Global variables
 ********************************************************************************/

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    result = cybsp_init();                 // Initialize the device and board peripherals.
    CY_ASSERT(result == CY_RSLT_SUCCESS);  // Board init failed. Stop program execution.

    RetargetIoInit();                      // Initialize retarget-io to use the debug UART port
    PMBUS_Start();                         // Initialize PMBus

    // LLC initialization sequence: HW_IFACE -> APP_PARAMS -> STATE_MACHINE
    HW_IFACE_Init();
    APP_PARAMS_Init();
    STATE_MACHINE_Init();

    (void)(result);
    for (;;)
    {
        RetargetIoRun();
        Cy_SysLib_Delay(LOGGING_DELAY_MS);
        Cy_GPIO_Inv(HRT_BT_LED_PORT, HRT_BT_LED_NUM); // Heart beat
    }
}


/*******************************************************************************
* Function Name: RetargetIoInit
********************************************************************************
* Summary:
* This function initializes retarget-io to use the debug UART port
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
static void RetargetIoInit(void)
{
    cy_rslt_t result;

    static cy_stc_scb_uart_context_t  DEBUG_UART_context;  // Debug UART context
    static mtb_hal_uart_t             DEBUG_UART_hal_obj;  // Debug UART HAL object

    result = (cy_rslt_t)Cy_SCB_UART_Init(DEBUG_UART_HW, &DEBUG_UART_config, &DEBUG_UART_context);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    Cy_SCB_UART_Enable(DEBUG_UART_HW);

    result = mtb_hal_uart_setup(&DEBUG_UART_hal_obj, &DEBUG_UART_hal_config, &DEBUG_UART_context, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    result = cy_retarget_io_init(&DEBUG_UART_hal_obj);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    // \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen
    printf("\x1b[2J\x1b[;H");
    printf("\r\n--------------------------------------------------------------------------------------------"
           "\r\n"
           "\r\n      Power Suite FB LLC converter reference code example on KIT_PSC3_LLC1.                 "
           "\r\n"
           "\r\n Output Voltage Regulation starts when input voltage is provided at the DC input connector. "
           "\r\n Input voltage can be provided:                                                             "
           "\r\n"
           "\r\n   1. from any 40V DC power source (power capabilities should meed the LLC needs)           "
           "\r\n   2. from KIT_PSC3_PFC1 kit (PFC output voltage/power settings should meed the LLC needs)  "
           "\r\n"
           "\r\n Control regulates output voltage, USER_LED on both LLC and CC1 card will glow.             "
           "\r\n SW_FAULT led will glow when converter detects a fault.                                     "
           "\r\n--------------------------------------------------------------------------------------------"
           "\r\n\n");
}


/*******************************************************************************
* Function Name: RetargetIoRun
********************************************************************************
* Summary:
* This function prints the active state with output voltage and load
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
static void RetargetIoRun(void)
{
    switch (sm.current)
    {
        case Volt_Ctrl_State:
        {
            printf("\rVOUT = %+6.2f V;  IOUT = %+6.2f A", vars.vout_fb, vars.iout_fb);
            break;
        }

        case Init_State:
        {
            printf("\rLLC Not Started                                              ");
            break;
        }

        case Fault_State:
        {
            printf("\rLLC Fault                                                    ");
            break;
        }

        default:
        {
            break;
        }
    }
}
