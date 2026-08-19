/******************************************************************************
 * File Name:   PmbusHw.c
 *
 * Description: This code example demonstrates the implementation of PMBus protocol
 * using the Infineon's MCUs.
 *
 * Related Document:
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

#if defined(APP_KIT_PSC3M5_CC1)

/*******************************************************************************
*       INCLUDES
*******************************************************************************/
#include "mtb_pmbus.h"
//#include "cy_pdl.h"
#include "cybsp.h"

/*******************************************************************************
*     PRIVATE MACROS
*******************************************************************************/
#define PMBUS_I2C_IRQ_PRIORITY (3U)

/*******************************************************************************
*    PRIVATE TYPEDEFS
*******************************************************************************/


/*******************************************************************************
*    PRIVATE FUNCTION PROTOTYPES
*******************************************************************************/
static void I2cIsr(void);
static void HwResourceEnableCallback(mtb_pmbus_hw_resources_ctrl_action_t);
static void HwIsrEnable(void);
static void HwIsrDisable(void);

/*******************************************************************************
*    STATIC VARIABLES
*******************************************************************************/
static cy_stc_scb_i2c_context_t i2c_pdl_context;

/* PMBus timeout timer HAL object */
static mtb_pmbus_stc_config_hal_t pmbus_hal_cfg =
{
    .hw_ptr          = PMBUS_I2C_HW,
    .pdl_i2c_context = &i2c_pdl_context,
};

/*******************************************************************************
*    EXPORTED VARIABLES
*******************************************************************************/
mtb_pmbus_stc_t pmbus_inst;

mtb_pmbus_stc_config_hw_t pmbus_hw_cfg =
{
    .hal_config                = &pmbus_hal_cfg,
    .hw_resource_ctrl_callback = HwResourceEnableCallback,
    .enable_hw_irq_callback    = HwIsrEnable,
    .disable_hw_irq_callback   = HwIsrDisable,
};

/*******************************************************************************
*    PRIVATE FUNCTIONS
*******************************************************************************/
/************************************************************
* Function Name: I2cIsr
*************************************************************
* Summary: Instance agnostic wrapper for the PMBus middleware
*          function
*
* Parameters: void
* Returns: void
************************************************************/
static void I2cIsr(void)
{
    mtb_pmbus_i2c_isr(&pmbus_inst);
}


/************************************************************
* Function Name: HwResourceEnableCallback
*************************************************************
* Summary: This function enables or disables the PMBus HW
*          resources in accordance with the input parameter.
*
* Parameters: mtb_pmbus_hw_resources_ctrl_action_t
* Returns: void
************************************************************/
static void HwResourceEnableCallback(mtb_pmbus_hw_resources_ctrl_action_t action)
{
    if (action == MTB_PMBUS_HW_RESOURCES_ENABLE)
    {
        Cy_SCB_I2C_Enable(PMBUS_I2C_HW);
    }
    else if (action == MTB_PMBUS_HW_RESOURCES_DISABLE)
    {
        Cy_SCB_I2C_Disable(PMBUS_I2C_HW, &i2c_pdl_context);
    }
}


/************************************************************
* Function Name: HwIsrEnable
*************************************************************
* Summary: This function enables the I2C interrupt requests
*          through the NVIC module.
*
* Parameters: void
* Returns: void
************************************************************/
static void HwIsrEnable(void)
{
    NVIC_EnableIRQ((IRQn_Type)PMBUS_I2C_IRQ);
}


/************************************************************
* Function Name: HwIsrDisable
*************************************************************
* Summary: This function disables the I2C interrupt requests
*          through the NVIC module.
*
* Parameters: void
* Returns: void
************************************************************/
static void HwIsrDisable(void)
{
    NVIC_DisableIRQ((IRQn_Type)PMBUS_I2C_IRQ);
}


/*******************************************************************************
*    EXPORTED FUNCTIONS
*******************************************************************************/
/************************************************************
* Function Name: PMBUS_HwInit
*************************************************************
* Summary: Function initializes the PMBus middleware related
*          HW resources.
*
* Parameters: void
* Returns: void
************************************************************/
void PMBUS_HW_Init(void)
{
    cy_en_scb_i2c_status_t pdl_i2c_status;
    cy_en_sysint_status_t  pld_interrupt_status;

    pdl_i2c_status = Cy_SCB_I2C_Init(PMBUS_I2C_HW, &PMBUS_I2C_config, &i2c_pdl_context);
    if (CY_SCB_I2C_SUCCESS != pdl_i2c_status)
    {
        MTB_PMBUS_LOG_ERR("Error during I2C PDL initialization. Status: %X", pdl_i2c_status);
    }
    else
    {
        cy_stc_sysint_t i2c_isr_cfg =
        {
            .intrSrc      = PMBUS_I2C_IRQ,
            .intrPriority = PMBUS_I2C_IRQ_PRIORITY
        };

        pld_interrupt_status = Cy_SysInt_Init(&i2c_isr_cfg, I2cIsr);
        if (CY_SYSINT_SUCCESS != pld_interrupt_status)
        {
            MTB_PMBUS_LOG_ERR("Error during I2C Interrupt initialization. Status: %X", pld_interrupt_status);
        }
        else
        {
            MTB_PMBUS_LOG_INF("I2C transport is initialized");
        }
    }
}


#endif //APP_KIT_PSC3M5_CC1
