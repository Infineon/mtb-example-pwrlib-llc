/***************************************************************************//**
* \\file mtb_pmbus_conf.h
* \\brief Auto-generated compile time options configuration file.
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

#ifndef MTB_PMBUS_CONF_H
#define MTB_PMBUS_CONF_H

#include "mtb_pmbus_log_level.h"

/* Compile time options generated from the PMBus Configurator */
#define MTB_PMBUS_LOG_LEVEL                (MTB_PMBUS_LOG_LEVEL_OFF)
#define MTB_PMBUS_CUSTOM_LOG               (0U)
#define MTB_PMBUS_SUPPORT_EXT_CMD          (1U)
#define MTB_PMBUS_SUPPORT_PEC              (1U)
#define MTB_PMBUS_PAGES_NUM                (16U)
#define MTB_PMBUS_PHASES_NUM               (16U)
#define MTB_PMBUS_ENABLE_CONFIG_CHECK      (1U)
#define MTB_PMBUS_CONFIG_FULL_CHECK        (1U)
#define MTB_PMBUS_ENABLE_TIMEOUT           (0U)
#define MTB_PMBUS_ZONE_MAX_NUM             (127U)
#define MTB_PMBUS_ZONE_SEL_ADV             (1U)
#define MTB_PMBUS_IMPL_CMD_REVISION        (1U)
#define MTB_PMBUS_IMPL_CMD_CAPABILITY      (1U)
#define MTB_PMBUS_IMPL_CMD_QUERY           (1U)
#define MTB_PMBUS_IMPL_CMD_PAGE            (1U)
#define MTB_PMBUS_IMPL_CMD_PHASE           (1U)
#define MTB_PMBUS_IMPL_CMD_ZONE_CONFIG     (1U)
#define MTB_PMBUS_IMPL_CMD_ZONE_ACTIVE     (1U)
#define MTB_PMBUS_IMPL_CMD_PAGE_PLUS_WRITE (1U)
#define MTB_PMBUS_IMPL_CMD_PAGE_PLUS_READ  (1U)
#define MTB_PMBUS_IMPL_CMD_P2_PLUS_WRITE   (1U)
#define MTB_PMBUS_IMPL_CMD_P2_PLUS_READ    (1U)

#endif /* MTB_PMBUS_CONF_H */
