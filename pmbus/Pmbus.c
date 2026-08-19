/******************************************************************************
 * File Name:   Pmbus.c
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

/*******************************************************************************
*       INCLUDES
*******************************************************************************/
#include "Pmbus.h"
#include "CtrlVars.h"
#include "Params.h"
#include "Controller.h"
#include "mtb_oscilloscope_data.h"
#include "mtb_pmbus.h"
#include "mtb_oscilloscope.h"
#include "mtb_oscilloscope_cfg.h"
//#include "cybsp.h"
//#include "cy_retarget_io.h"

/*******************************************************************************
*     PRIVATE MACROS
*******************************************************************************/
#define PWRDEV_CONF_PAGES       (1U) /** TODO: The Page number must be changed to 2 when PFC + LLC will be implemented */
#define PWRDEV_ACT_PAGE         (1L) // The active page is where the LLC-specific control parameters are allocated.

#define OPERATION_CODE              (0x01U)
#define ON_OFF_CONFIG_CODE          (0x02U)
#define CLEAR_FAULTS_CODE           (0x03U)
#define RESTORE_DEFAULT_ALL_CODE    (0x12U)
#define STORE_USER_ALL_CODE         (0x15U)
#define VOUT_MODE_CODE              (0x20U)
#define VOUT_COMMAND_CODE           (0x21U)
#define VOUT_MAX_CODE               (0x24U)
#define VOUT_TRANSITION_RATE_CODE   (0x27U)
#define VOUT_MIN_CODE               (0x2BU)
#define MAX_DUTY_CODE               (0x32U)
#define FREQUENCY_SWITCH_CODE       (0x33U)
#define VOUT_OV_FAULT_LIMIT_CODE    (0x40U)
#define VIN_OV_FAULT_LIMIT_CODE     (0x55U)
#define VIN_UV_FAULT_LIMIT_CODE     (0x59U)
#define IIN_OC_FAULT_LIMIT_CODE     (0x5BU)
#define STATUS_BYTE_CODE            (0x78U)
#define STATUS_WORD_CODE            (0x79U)
#define STATUS_INPUT_CODE           (0x7CU)
#define READ_VIN_CODE               (0x88U)
#define READ_VOUT_CODE              (0x8BU)
#define READ_IOUT_CODE              (0x8CU)
#define READ_DUTY_CYCLE_CODE        (0x94U)
#define READ_FREQUENCY_CODE         (0x95U)
#define MFR_ID_CODE                 (0x99U)
#define MFR_MODEL_CODE              (0x9AU)
#define MFR_REVISION_CODE           (0x9BU)
#define IC_DEVICE_ID_CODE           (0xADU)
#define IC_DEVICE_REV_CODE          (0xAEU)
#define SCOPE_TRIG_INDEX_CODE       (0xEDU)
#define SCOPE_TRIG_HOLD_OFF_CODE    (0xEEU)
#define SCOPE_DATA_RDY_CODE         (0xEFU)
#define READ_SCOPE_VAR_CNT_CODE     (0xF0U)
#define SCOPE_VAR_LIST_CODE         (0xF1U)
#define SCOPE_VAR_MAP_CODE          (0xF2U)
#define SCOPE_MODE_CODE             (0xF3U)
#define SCOPE_CH_CTRL_CODE          (0xF4U)
#define SCOPE_TRIG_CH_CODE          (0xF5U)
#define SCOPE_TRIG_SLOPE_CODE       (0xF6U)
#define SCOPE_TRIG_POS_CODE         (0xF7U)
#define SCOPE_TRIG_LEVEL_CODE       (0xF8U)
#define SCOPE_SAMP_CLK_DIV_CODE     (0xF9U)
#define READ_SCOPE_SAMP_CLK_CODE    (0xFAU)
#define READ_SCOPE_SPCH_CNT_CODE    (0xFBU)
#define SCOPE_CH_SAMP_BUF_CODE      (0xFCU)

#define OPERATION_SIZE              (1U)
#define ON_OFF_CONFIG_SIZE          (1U)
#define CLEAR_FAULTS_SIZE           (0U)
#define RESTORE_DEFAULT_ALL_SIZE    (0U)
#define STORE_USER_ALL_SIZE         (0U)
#define VOUT_MODE_SIZE              (1U)
#define VOUT_COMMAND_SIZE           (2U)
#define VOUT_MAX_SIZE               (2U)
#define VOUT_TRANSITION_RATE_SIZE   (2U)
#define VOUT_MIN_SIZE               (2U)
#define MAX_DUTY_SIZE               (2U)
#define FREQUENCY_SWITCH_SIZE       (2U)
#define VOUT_OV_FAULT_LIMIT_SIZE    (2U)
#define VIN_OV_FAULT_LIMIT_SIZE     (2U)
#define VIN_UV_FAULT_LIMIT_SIZE     (2U)
#define IIN_OC_FAULT_LIMIT_SIZE     (2U)
#define STATUS_BYTE_SIZE            (1U)
#define STATUS_WORD_SIZE            (2U)
#define STATUS_INPUT_SIZE           (1U)
#define READ_VIN_SIZE               (2U)
#define READ_VOUT_SIZE              (2U)
#define READ_IOUT_SIZE              (2U)
#define READ_DUTY_CYCLE_SIZE        (2U)
#define READ_FREQUENCY_SIZE         (2U)
#define MFR_ID_SIZE                 (3U)
#define MFR_MODEL_SIZE              (3U)
#define MFR_REVISION_SIZE           (2U)
#define IC_DEVICE_ID_SIZE           (2U)
#define IC_DEVICE_REV_SIZE          (2U)
#define SCOPE_TRIG_INDEX_SIZE       (4U)
#define SCOPE_TRIG_HOLD_OFF_SIZE    (4U)
#define SCOPE_DATA_RDY_SIZE         (1U)
#define READ_SCOPE_VAR_CNT_SIZE     (1U)
#define SCOPE_VAR_LIST_SIZE         (254U)
#define SCOPE_VAR_MAP_SIZE          (2U)
#define SCOPE_MODE_SIZE             (1U)
#define SCOPE_CH_CTRL_SIZE          (2U)
#define SCOPE_TRIG_CH_SIZE          (1U)
#define SCOPE_TRIG_SLOPE_SIZE       (1U)
#define SCOPE_TRIG_POS_SIZE         (4U)
#define SCOPE_TRIG_LEVEL_SIZE       (5U)
#define SCOPE_SAMP_CLK_DIV_SIZE     (2U)
#define READ_SCOPE_SAMP_CLK_SIZE    (4U)
#define READ_SCOPE_SPCH_CNT_SIZE    (2U)
#define SCOPE_CH_SAMP_BUF_SIZE      (254U)

#define PMBUS_CMD_TABLE_SIZE    (45U)

#define IC_DEVICE_FAMILYID_LO ((kit_info.chip_id >> 16U) & 0x0FFUL)
#define IC_DEVICE_FAMILYID_HI ((kit_info.chip_id >> 24U) & 0x0FFUL)
#define IC_DEVICE_REVISIONID  (kit_info.chip_id & 0x0FFUL)


#define VOUT_MODE_PARAM_MSK (0x1FU)

#define OPERATION_ON    (1U)
#define OPERATION_OFF   (0U)

#define OPERATION_ON_OFF_MSK            (0x80U)
#define OPERATION_POWER_DOWN_MSK        (0x40U)
#define OPERATION_VOUT_SOURCE_MSK       (0x30U)
#define OPERATION_VOUT_LIMIT_FAULT_MSK  (0x0CU)
#define OPERATION_AVSBUS_PMBUS_SW_MSK   (0x02U)

#define OPERATION_ON_OFF_POS            (7U)
#define OPERATION_POWER_DOWN_POS        (6U)
#define OPERATION_VOUT_SOURCE_POS       (4U)
#define OPERATION_VOUT_LIMIT_FAULT_POS  (2U)
#define OPERATION_AVSBUS_PMBUS_SW_POS   (1U)

#define IMMEDIATE_OFF      (1U)
#define DELAYED_OFF        (0U)
#define START_DISABLE      (1U)
#define START_ENABLE       (0U)
#define SOURCE_ON          (1U)
#define SOURCE_OFF         (0U)

#define PIN_OFF_ACTION_POS (0U)
#define PIN_START_SRC_POS  (2U)
#define BUS_START_SRC_POS  (3U)
#define START_AT_PWRON_POS (4U)

#define PIN_OFF_ACTION_MSK     (0x01U)
#define PIN_START_SRC_MSK      (0x04U)
#define BUS_START_SRC_MSK      (0x08U)
#define START_AT_PWRON_MSK     (0x10U)

/*******************************************************************************
*    PRIVATE TYPEDEFS
*******************************************************************************/
typedef union
{
    uint8_t  buffer[2];
    uint16_t value;
} VAL16_DATA_t;

typedef union
{
    uint8_t  buffer[4];
    uint32_t value;
} VAL32_DATA_t;

typedef union
{
    struct
    {
        uint8_t reserved            : 1;
        uint8_t avsbus_pmbus_sw     : 1;
        uint8_t vout_limit_fault    : 2;
        uint8_t vout_source         : 2;
        uint8_t power_down          : 1;
        uint8_t on_off              : 1;
    } bf;
    uint8_t value;
} OPERATION_DATA_t;

typedef union
{
    struct
    {
        uint8_t pin_off_action      : 1;
        uint8_t pin_polarity        : 1;
        uint8_t pin_as_start_src    : 1;
        uint8_t bus_as_start_src    : 1;
        uint8_t start_at_pwr_on     : 1;
        uint8_t reserved            : 3;
    } bf;
    uint8_t value;
} ON_OFF_CONFIG_DATA_t;

typedef union
{
    struct
    {
        uint8_t none_of_above   : 1;
        uint8_t cml_event       : 1;
        uint8_t temperature_f_w : 1;
        uint8_t vin_uv_fault    : 1;
        uint8_t iout_oc_fault   : 1;
        uint8_t vout_ov_fault   : 1;
        uint8_t unit_is_off     : 1;
        uint8_t unit_was_busy   : 1;
    } bf;
    uint8_t value;
} STATUS_LBYTE_DATA_t;

typedef union
{
    struct
    {
        uint8_t unknown_f_w     : 1;
        uint8_t other           : 1;
        uint8_t fan_f_w         : 1;
        uint8_t power_good      : 1;
        uint8_t mfr_specific    : 1;
        uint8_t input_f_w       : 1;
        uint8_t iout_pout_f_w   : 1;
        uint8_t vout_f_w        : 1;
    } bf;
    uint8_t value;
} STATUS_HBYTE_DATA_t;

typedef union
{
    struct
    {
        STATUS_LBYTE_DATA_t lo_byte;
        STATUS_HBYTE_DATA_t hi_byte;
    };

    uint8_t  buffer[2];
    uint16_t value;
} STATUS_WORD_DATA_t;

typedef union
{
    struct
    {
        uint8_t pin_op_warning  : 1;
        uint8_t iin_oc_warning  : 1;
        uint8_t iin_oc_fault    : 1;
        uint8_t vin_low_off     : 1;
        uint8_t vin_uv_fault    : 1;
        uint8_t vin_uv_warning  : 1;
        uint8_t vin_ov_warning  : 1;
        uint8_t vin_ov_fault    : 1;
    } bf;
    uint8_t value;
} STATUS_INPUT_DATA_t;

#pragma pack(push,1)
typedef union
{
    struct
    {
        uint32_t     level;
        uint8_t      num;
    };

    uint8_t  buffer[5];
} CH_LEVEL_CFG_t;
#pragma pack(pop)

typedef union
{
    struct
    {
        uint8_t var_idx;
        uint8_t ch_num;
    };

    uint8_t  buffer[2];
} VAR_MAP_REG_t;

typedef union
{
    struct
    {
        uint8_t ch_num;
        uint8_t ch_cmd;
    };

    uint8_t  buffer[2];
} CH_CTRL_REG_t;

typedef union
{
    struct
    {
        uint8_t ch_num;
        uint8_t block_num;
    };

    uint8_t  buffer[2];
} CH_SAMP_BUF_REG_t;

/*******************************************************************************
*    PRIVATE FUNCTION PROTOTYPES
*******************************************************************************/
static void GeneralEventCallback(mtb_pmbus_events_t event);
static bool OperationCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ClearFaultsCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool OnOffConfigCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool RestoreDefaultAllCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool StoreUserAllCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool VoutModeCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool VoutCommandCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool VoutMaxCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool VoutTransitionRateCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool VoutMinCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool MaxDutyCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool FrequencySwitchCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool VoutOvFaultLimitCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool VinOvFaultLimitCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool VinUvFaultLimitCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool IinOcFaultLimitCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool StatusByteCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool StatusWordCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool StatusInputCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ReadVinCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ReadVoutCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ReadIoutCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ReadDutyCycleCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ReadFrequencyCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool IcDeviceIdCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool IcDeviceRevCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeTrigIndexCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeTrigHoldOffCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeDataRdyCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ReadScopeVarCntCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeVarListCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeVarMapCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeModeCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeChCtrlCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeTrigChCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeTrigSlopeCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeTrigPosCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeTrigLevelCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeSampClkDivCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ReadScopeSampClkCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ReadScopeSpchCntCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);
static bool ScopeChSampBufCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte);


/*******************************************************************************
*    STATIC VARIABLES
*******************************************************************************/
// static cy_stc_scb_uart_context_t    DEBUG_UART_context;
// static volatile uint8_t user_led_toggled_cnt = 0U;

static   int8_t vout_exp          = -8;
static  uint8_t operation_reg     = 0U;
static uint16_t unit_status_word  = 0U;
static  uint8_t status_input_reg  = 0U;
static  uint8_t on_off_config_reg = (IMMEDIATE_OFF << PIN_OFF_ACTION_POS) | (START_ENABLE << START_AT_PWRON_POS) | (SOURCE_OFF << PIN_START_SRC_POS) | (SOURCE_OFF << BUS_START_SRC_POS);

static uint8_t status_byte_buffer           [STATUS_BYTE_SIZE]  = { 0U };
static uint8_t status_word_buffer           [STATUS_WORD_SIZE]  = { 0U };
static uint8_t status_input_buffer          [STATUS_INPUT_SIZE] = { 0U };

static uint8_t operation_buffer             [PWRDEV_CONF_PAGES][OPERATION_SIZE]            = { 0U };
static uint8_t on_off_config_buffer         [PWRDEV_CONF_PAGES][OPERATION_SIZE]            = { 0U };
static uint8_t vout_mode_buffer             [PWRDEV_CONF_PAGES][VOUT_MODE_SIZE]            = { 0U };
static uint8_t vout_command_buffer          [PWRDEV_CONF_PAGES][VOUT_COMMAND_SIZE]         = { 0U };
static uint8_t vout_max_buffer              [PWRDEV_CONF_PAGES][VOUT_MAX_SIZE]             = { 0U };
static uint8_t vout_transition_rate_buffer  [PWRDEV_CONF_PAGES][VOUT_TRANSITION_RATE_SIZE] = { 0U };
static uint8_t vout_min_buffer              [PWRDEV_CONF_PAGES][VOUT_MIN_SIZE]             = { 0U };
static uint8_t max_duty_buffer              [PWRDEV_CONF_PAGES][MAX_DUTY_SIZE]             = { 0U };
static uint8_t frequency_switch_buffer      [PWRDEV_CONF_PAGES][FREQUENCY_SWITCH_SIZE]     = { 0U };
static uint8_t vout_ov_fault_limit_buffer   [PWRDEV_CONF_PAGES][VOUT_OV_FAULT_LIMIT_SIZE]  = { 0U };
static uint8_t vin_ov_fault_limit_buffer    [PWRDEV_CONF_PAGES][VIN_OV_FAULT_LIMIT_SIZE]   = { 0U };
static uint8_t vin_uv_fault_limit_buffer    [PWRDEV_CONF_PAGES][VIN_UV_FAULT_LIMIT_SIZE]   = { 0U };
static uint8_t iin_oc_fault_limit_buffer    [PWRDEV_CONF_PAGES][IIN_OC_FAULT_LIMIT_SIZE]   = { 0U };
static uint8_t read_vin_buffer              [PWRDEV_CONF_PAGES][READ_VIN_SIZE]             = { 0U };
static uint8_t read_vout_buffer             [PWRDEV_CONF_PAGES][READ_VOUT_SIZE]            = { 0U };
static uint8_t read_iout_buffer             [PWRDEV_CONF_PAGES][READ_IOUT_SIZE]            = { 0U };
static uint8_t read_duty_cycle_buffer       [PWRDEV_CONF_PAGES][READ_DUTY_CYCLE_SIZE]      = { 0U };
static uint8_t read_frequency_buffer        [PWRDEV_CONF_PAGES][READ_FREQUENCY_SIZE]       = { 0U };

static uint8_t mfr_id_buffer                [MFR_ID_SIZE + 1] = { MFR_ID_SIZE, 'I', 'F', 'X' }; // INFINEON
static uint8_t mfr_model_buffer             [MFR_MODEL_SIZE + 1] = { MFR_MODEL_SIZE, 'L', 'L', 'C' }; // LLC
static uint8_t mfr_revision_buffer          [MFR_REVISION_SIZE + 1] = { MFR_REVISION_SIZE, 0x01, 0x00 }; // REV 1.0

static uint8_t unpaged_shared_buffer        [MTB_PMBUS_CMD_MAX_SIZE];

// PMBus command table
static mtb_pmbus_stc_config_cmd_t cmd_table[PMBUS_CMD_TABLE_SIZE] =
{
    /* OPERATION_CODE: Read/Write without Page support */
    {
        .cmd_code   = OPERATION_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = operation_buffer,
        .callback   = OperationCallback,
        .data_size  = OPERATION_SIZE,
    },
    /* ON_OFF_CONFIG_CODE: Read/Write with Page support */
    {
        .cmd_code   = ON_OFF_CONFIG_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = on_off_config_buffer,
        .callback   = OnOffConfigCallback,
        .data_size  = ON_OFF_CONFIG_SIZE,
    },
    /* CLEAR_FAULTS_CODE: Write with Page support */
    {
        .cmd_code   = CLEAR_FAULTS_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = NULL,
        .callback   = ClearFaultsCallback,
        .data_size  = CLEAR_FAULTS_SIZE,
    },
    /* RESTORE_DEFAULT_ALL_CODE: Write without Page support */
    {
        .cmd_code   = RESTORE_DEFAULT_ALL_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = NULL,
        .callback   = RestoreDefaultAllCallback,
        .data_size  = RESTORE_DEFAULT_ALL_SIZE,
    },
    /* STORE_USER_ALL_CODE: Write without Page support */
    {
        .cmd_code   = STORE_USER_ALL_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = NULL,
        .callback   = StoreUserAllCallback,
        .data_size  = STORE_USER_ALL_SIZE,
    },
    /* VOUT_MODE_CODE: Read with Page support */
    {
        .cmd_code   = VOUT_MODE_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = vout_mode_buffer,
        .callback   = VoutModeCallback,
        .data_size  = VOUT_MODE_SIZE,
    },
    /* VOUT_COMMAND_CODE: Read/Write with Page support */
    {
        .cmd_code   = VOUT_COMMAND_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = vout_command_buffer,
        .callback   = VoutCommandCallback,
        .data_size  = VOUT_COMMAND_SIZE,
    },
    /* VOUT_MAX_CODE: Read/Write with Page support */
    {
        .cmd_code   = VOUT_MAX_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = vout_max_buffer,
        .callback   = VoutMaxCallback,
        .data_size  = VOUT_MAX_SIZE,
    },
    /* VOUT_TRANSITION_RATE_CODE: Read with Page support */
    {
        .cmd_code   = VOUT_TRANSITION_RATE_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = vout_transition_rate_buffer,
        .callback   = VoutTransitionRateCallback,
        .data_size  = VOUT_TRANSITION_RATE_SIZE,
    },
    /* VOUT_MIN_CODE: Read/Write with Page support */
    {
        .cmd_code   = VOUT_MIN_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = vout_min_buffer,
        .callback   = VoutMinCallback,
        .data_size  = VOUT_MIN_SIZE,
    },
    /* MAX_DUTY_CODE: Read with Page support */
    {
        .cmd_code   = MAX_DUTY_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = max_duty_buffer,
        .callback   = MaxDutyCallback,
        .data_size  = MAX_DUTY_SIZE,
    },
    /* FREQUENCY_SWITCH_CODE: Read with Page support */
    {
        .cmd_code   = FREQUENCY_SWITCH_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = frequency_switch_buffer,
        .callback   = FrequencySwitchCallback,
        .data_size  = FREQUENCY_SWITCH_SIZE,
    },
    /* VOUT_OV_FAULT_LIMIT_CODE: Read/Write with Page support */
    {
        .cmd_code   = VOUT_OV_FAULT_LIMIT_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = vout_ov_fault_limit_buffer,
        .callback   = VoutOvFaultLimitCallback,
        .data_size  = VOUT_OV_FAULT_LIMIT_SIZE,
    },
    /* VIN_OV_FAULT_LIMIT_CODE: Read/Write with Page support */
    {
        .cmd_code   = VIN_OV_FAULT_LIMIT_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = vin_ov_fault_limit_buffer,
        .callback   = VinOvFaultLimitCallback,
        .data_size  = VIN_OV_FAULT_LIMIT_SIZE,
    },
    /* VIN_UV_FAULT_LIMIT_CODE: Read/Write with Page support */
    {
        .cmd_code   = VIN_UV_FAULT_LIMIT_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = vin_uv_fault_limit_buffer,
        .callback   = VinUvFaultLimitCallback,
        .data_size  = VIN_UV_FAULT_LIMIT_SIZE,
    },
    /* IIN_OC_FAULT_LIMIT_CODE: Read/Write with Page support */
    {
        .cmd_code   = IIN_OC_FAULT_LIMIT_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = iin_oc_fault_limit_buffer,
        .callback   = IinOcFaultLimitCallback,
        .data_size  = IIN_OC_FAULT_LIMIT_SIZE,
    },
    /* STATUS_BYTE_CODE: Read without Page support */
    {
        .cmd_code   = STATUS_BYTE_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = status_byte_buffer,
        .callback   = StatusByteCallback,
        .data_size  = STATUS_BYTE_SIZE,
    },
    /* STATUS_WORD_CODE: Read without Page support */
    {
        .cmd_code   = STATUS_WORD_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = status_word_buffer,
        .callback   = StatusWordCallback,
        .data_size  = STATUS_WORD_SIZE,
    },
    /* STATUS_INPUT_CODE: Read without Page support */
    {
        .cmd_code   = STATUS_INPUT_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = status_input_buffer,
        .callback   = StatusInputCallback,
        .data_size  = STATUS_INPUT_SIZE,
    },
    /* READ_VIIN_CODE: Read with Page support */
    {
        .cmd_code   = READ_VIN_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = read_vin_buffer,
        .callback   = ReadVinCallback,
        .data_size  = READ_VIN_SIZE,
    },
    /* READ_VOUT_CODE: Read with Page support */
    {
        .cmd_code   = READ_VOUT_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = read_vout_buffer,
        .callback   = ReadVoutCallback,
        .data_size  = READ_VOUT_SIZE,
    },
    /* READ_IOUT_CODE: Read with Page support */
    {
        .cmd_code   = READ_IOUT_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = read_iout_buffer,
        .callback   = ReadIoutCallback,
        .data_size  = READ_IOUT_SIZE,
    },
    /* READ_DUTY_CYCLE_CODE: Read with Page support */
    {
        .cmd_code   = READ_DUTY_CYCLE_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = read_duty_cycle_buffer,
        .callback   = ReadDutyCycleCallback,
        .data_size  = READ_DUTY_CYCLE_SIZE,
    },
    /* READ_FREQUENCY_CODE: Read with Page support */
    {
        .cmd_code   = READ_FREQUENCY_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_PAGE | MTB_PMBUS_CMD_CAP_FORMAT_LIN_11_16,
        .data_buf   = read_frequency_buffer,
        .callback   = ReadFrequencyCallback,
        .data_size  = READ_FREQUENCY_SIZE,
    },
    /* MFR_ID: Block Read without Page support */
    {
        .cmd_code   = MFR_ID_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = mfr_id_buffer,
        .callback   = NULL,
        .data_size  = MFR_ID_SIZE,
    },
    /* MFR_MODEL: Block Read without Page support */
    {
        .cmd_code   = MFR_MODEL_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = mfr_model_buffer,
        .callback   = NULL,
        .data_size  = MFR_MODEL_SIZE,
    },
    /* MFR_REVISION: Block Read without Page support */
    {
        .cmd_code   = MFR_REVISION_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = mfr_revision_buffer,
        .callback   = NULL,
        .data_size  = MFR_REVISION_SIZE,
    },

    /* IC_DEVICE_ID: Block Read without Page support */
    {
        .cmd_code   = IC_DEVICE_ID_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = IcDeviceIdCallback,
        .data_size  = IC_DEVICE_ID_SIZE,
    },
    /* IC_DEVICE_REV: Block Read without Page support */
    {
        .cmd_code   = IC_DEVICE_REV_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = IcDeviceRevCallback,
        .data_size  = IC_DEVICE_REV_SIZE,
    },
    /* SCOPE_TRIG_INDEX_CODE: Block Read without Page support */
    {
        .cmd_code   = SCOPE_TRIG_INDEX_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeTrigIndexCallback,
        .data_size  = SCOPE_TRIG_INDEX_SIZE,
    },
    /* SCOPE_TRIG_HOLD_OFF_CODE: Block Read/Write without Page support */
    {
        .cmd_code   = SCOPE_TRIG_HOLD_OFF_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeTrigHoldOffCallback,
        .data_size  = SCOPE_TRIG_HOLD_OFF_SIZE,
    },
    /* SCOPE_DATA_RDY_CODE: Block Read/Write without Page support */
    {
        .cmd_code   = SCOPE_DATA_RDY_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeDataRdyCallback,
        .data_size  = SCOPE_DATA_RDY_SIZE,
    },
    /* READ_SCOPE_VAR_CNT_CODE: Block Read without Page support */
    {
        .cmd_code   = READ_SCOPE_VAR_CNT_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ReadScopeVarCntCallback,
        .data_size  = READ_SCOPE_VAR_CNT_SIZE,
    },
    /* SCOPE_VAR_LIST_CODE: Block Read/Write without Page support */
    {
        .cmd_code   = SCOPE_VAR_LIST_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeVarListCallback,
        .data_size  = SCOPE_VAR_LIST_SIZE,
    },
    /* SCOPE_VAR_MAP_CODE: Block Write without Page support */
    {
        .cmd_code   = SCOPE_VAR_MAP_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeVarMapCallback,
        .data_size  = SCOPE_VAR_MAP_SIZE,
    },
    /* SCOPE_MODE_CODE: Block Read/Write without Page support */
    {
        .cmd_code   = SCOPE_MODE_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeModeCallback,
        .data_size  = SCOPE_MODE_SIZE,
    },
    /* SCOPE_CH_CTRL_CODE: Block Write without Page support */
    {
        .cmd_code   = SCOPE_CH_CTRL_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeChCtrlCallback,
        .data_size  = SCOPE_CH_CTRL_SIZE,
    },
    /* SCOPE_TRIG_CH_CODE: Block Write without Page support */
    {
        .cmd_code   = SCOPE_TRIG_CH_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeTrigChCallback,
        .data_size  = SCOPE_TRIG_CH_SIZE,
    },
    /* SCOPE_TRIG_SLOPE_CODE: Block Write without Page support */
    {
        .cmd_code   = SCOPE_TRIG_SLOPE_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeTrigSlopeCallback,
        .data_size  = SCOPE_TRIG_SLOPE_SIZE,
    },
    /* SCOPE_TRIG_POS_CODE: Block Write without Page support */
    {
        .cmd_code   = SCOPE_TRIG_POS_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeTrigPosCallback,
        .data_size  = SCOPE_TRIG_POS_SIZE,
    },
    /* SCOPE_TRIG_LEVEL_CODE: Block Write without Page support */
    {
        .cmd_code   = SCOPE_TRIG_LEVEL_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeTrigLevelCallback,
        .data_size  = SCOPE_TRIG_LEVEL_SIZE,
    },
    /* SCOPE_SAMP_CLK_DIV_CODE: Block Write without Page support */
    {
        .cmd_code   = SCOPE_SAMP_CLK_DIV_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_WR | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeSampClkDivCallback,
        .data_size  = SCOPE_SAMP_CLK_DIV_SIZE,
    },
    /* READ_SCOPE_SAMP_CLK_CODE: Block Read without Page support */
    {
        .cmd_code   = READ_SCOPE_SAMP_CLK_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ReadScopeSampClkCallback,
        .data_size  = READ_SCOPE_SAMP_CLK_SIZE,
    },
    /* READ_SCOPE_SPCH_CNT_CODE: Block Read without Page support */
    {
        .cmd_code   = READ_SCOPE_SPCH_CNT_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_RD | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ReadScopeSpchCntCallback,
        .data_size  = READ_SCOPE_SPCH_CNT_SIZE,
    },
    /* SCOPE_CH_SAMP_BUF_CODE: Block Read/Write without Page support */
    {
        .cmd_code   = SCOPE_CH_SAMP_BUF_CODE,
        .cmd_cap    = MTB_PMBUS_CMD_CAP_DIR_PROCESS_CALL | MTB_PMBUS_CMD_CAP_BLOCK | MTB_PMBUS_CMD_CAP_FORMAT_NO_NUM,
        .data_buf   = unpaged_shared_buffer,
        .callback   = ScopeChSampBufCallback,
        .data_size  = SCOPE_CH_SAMP_BUF_SIZE,
    },
};

static mtb_pmbus_stc_config_t pmbus_cfg =
{
    /* Pointer to hardware configuration structure */
    .hw_config            = &pmbus_hw_cfg,
    /* PMBus device address (7-bit) */
    .address              = PMBUS_DEVICE_ADDRESS,
    /* Disable PEC support for minimal test */
    .enable_pec           = true,
    /* Enable SMBALERT signal */
    //.enable_smbalert = false,
    /* Enable PMBus features */
    .enable_pmbus         = true,
    /* Enable pages for PMBus test */
    .num_pages            = PMBUS_TOTAL_NUM_PAGES,
    /* Enable phases for PMBus test*/
    .num_phases           = 0U,
    /* Zone Write and Zone Read protocols are disabled for minimal test */
    .enable_zone          = false,
    /* No implemented PMBus commands */
    .impl_cmd_mask        = MTB_PMBUS_IMPL_CMD_PAGE_EN | MTB_PMBUS_IMPL_CMD_REVISION_EN | MTB_PMBUS_IMPL_CMD_CAPABILITY_EN,
    /* Pointer to simple test command table */
    .cmd_table            = cmd_table,
    /* Number of commands in test table */
    .cmd_num              = PMBUS_CMD_TABLE_SIZE,
    /* General call address disabled for minimal test */
    .enable_gen_call_addr = false,
    /* General event callback for minimal PMBus test */
    .gen_callback         = GeneralEventCallback,
    /* No error event callback for minimal PMBus test */
    .errors_callback      = NULL,
    /* PMBus revision */
    .revision             = MTB_PMBUS_REVISION_1_4,
    /* PMBus speed (100 kHz) */
    .speed                = MTB_PMBUS_SPEED_100,
    /* Numeric data format not used */
    .enable_ieee_format   = false,
};

/*******************************************************************************
*    EXPORTED VARIABLES
*******************************************************************************/


/*******************************************************************************
*    PRIVATE FUNCTIONS
*******************************************************************************/
/************************************************************
* Function Name: PMBUS_Start
*************************************************************
* Summary: This function initializes i2c HW and starts PMBus
*
* Parameters: void
* Returns: void
************************************************************/
void PMBUS_Start(void)
{
    mtb_pmbus_status_t pmbus_status;

    PMBUS_HW_Init();

    pmbus_status = mtb_pmbus_init(&pmbus_inst, &pmbus_cfg);

    if (MTB_PMBUS_STATUS_SUCCESS != pmbus_status)
    {
        /* Handle the pmbus error status */
        MTB_PMBUS_LOG_DBG("PMBus initialization failed!");
    }
    else
    {
        pmbus_status = mtb_pmbus_enable(&pmbus_inst);
        if (MTB_PMBUS_STATUS_SUCCESS != pmbus_status)
        {
            /* Handle the pmbus error status */
        }
    }
}


/************************************************************
* Function Name: GeneralEventCallback
*************************************************************
* Summary: General event callback function.
*
* Parameters: mtb_pmbus_events_t event
* Return: void
************************************************************/
void GeneralEventCallback(mtb_pmbus_events_t event)
{
    if (event == MTB_PMBUS_QUICK_CMD_WR_EVENT)
    {
        printf("Gen: MTB_PMBUS_QUICK_CMD_WR_EVENT\n\r");
        /* Toggle User LED on quick write command */
//        Cy_GPIO_Inv(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
//        user_led_toggled_cnt++;
        printf("User LED toggled\n\r");
    }
}


/************************************************************
* Function Name: OperationCallback
*************************************************************
* Summary: The OPERATION callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool OperationCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    bool callback_status = true;
    ON_OFF_CONFIG_DATA_t* on_off_config_reg_p;
    on_off_config_reg_p = (ON_OFF_CONFIG_DATA_t*)&on_off_config_reg;

    /* Avoid compiler warnings */
    (void)phase;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, OPERATION_CODE, false); //The write operation allowed
        }
        else
        {
            //The active page is the only page that is allowed in the current project for writing the OPERATION command
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, OPERATION_CODE, true);
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_BYTE)
    {
        if ((SOURCE_ON != on_off_config_reg_p->bf.bus_as_start_src) && (OPERATION_OFF != (byte & OPERATION_ON_OFF_MSK)))
        {
            callback_status = false; //if the START operation via the bus command is not allowed, then the ON operation is ignored
        }
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            operation_reg = (vars.en) ? ((uint8_t)OPERATION_ON << OPERATION_ON_OFF_POS) : ((uint8_t)OPERATION_OFF);
            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, OPERATION_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, &operation_reg, OPERATION_SIZE);
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            OPERATION_DATA_t operation_buf;

            (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, OPERATION_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, &operation_buf.value, OPERATION_SIZE);

            if ((OPERATION_ON == operation_buf.bf.on_off) && (SOURCE_ON == on_off_config_reg_p->bf.bus_as_start_src))
            {
                mtb_pmbus_cmd_enable_disable_isr(&pmbus_inst, RESTORE_DEFAULT_ALL_CODE, false);
                operation_reg = (uint8_t)OPERATION_ON << OPERATION_ON_OFF_POS;
                vars.en = true;
            }
            else
            {
                mtb_pmbus_cmd_enable_disable_isr(&pmbus_inst, RESTORE_DEFAULT_ALL_CODE, true);
                operation_reg = (uint8_t)OPERATION_OFF;
                vars.en = false;
            }
        }
    }

    return callback_status;
}


/************************************************************
* Function Name: OnOffConfigCallback
*************************************************************
* Summary: The ON_OFF_CONFIG callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool OnOffConfigCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    ON_OFF_CONFIG_DATA_t* on_off_config_reg_p;
    on_off_config_reg_p = (ON_OFF_CONFIG_DATA_t*)&on_off_config_reg;

    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, ON_OFF_CONFIG_CODE, false); //The write operation allowed
        }
        else
        {
            //The active page is the only page that is allowed in the current project for writing the ON_OFF_CONFIG command
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, ON_OFF_CONFIG_CODE, true);
        }
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            on_off_config_reg &= ~START_AT_PWRON_MSK;
            on_off_config_reg |= (Dis == params.ctrl.auto_start) ? ((uint8_t)START_DISABLE << START_AT_PWRON_POS) : (uint8_t)START_ENABLE;

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, ON_OFF_CONFIG_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, &on_off_config_reg, ON_OFF_CONFIG_SIZE);
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            ON_OFF_CONFIG_DATA_t on_off_config_buf;

            (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, ON_OFF_CONFIG_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, &on_off_config_buf.value, ON_OFF_CONFIG_SIZE);
            //The on_off_config_reg update with the start_at_pwr_on bit-field occurs during the MTB_PMBUS_CMD_READ_REQ
            params.ctrl.auto_start = (START_ENABLE == on_off_config_buf.bf.start_at_pwr_on) ? En : Dis;

            if (SOURCE_ON == on_off_config_buf.bf.bus_as_start_src)
            {
                on_off_config_reg_p->bf.bus_as_start_src = SOURCE_ON;
                params.sys.cmd.source = External;
            }
            else // SOURCE_OFF == on_off_config_buf.bf.bus_as_start_src
            {
                on_off_config_reg_p->bf.bus_as_start_src = SOURCE_OFF;
                params.sys.cmd.source = Internal;
            }
        }
    }

    return true;
}


/************************************************************
* Function Name: ClearFaultsCallback
*************************************************************
* Summary: The CLEAR_FAULTS callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ClearFaultsCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            uint16_t timeout_us = (uint16_t)(1.0e6f * params.sys.faults.max_clr_tries * params.sys.samp.ts1); // [us]

            vars.clr_faults = true;
            while ((!sm.vars.fault.clr_done) && (0U != timeout_us))
            {
                timeout_us--;
                Cy_SysLib_DelayUs(1U);
            }
            vars.clr_faults = false;
        }
    }

    return true;
}


/************************************************************
* Function Name: RestoreDefaultAllCallback
*************************************************************
* Summary: The RESTORE_DEFAULT_ALL callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool RestoreDefaultAllCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        if (vars.en)
        {
            mtb_pmbus_cmd_enable_disable_isr(&pmbus_inst, RESTORE_DEFAULT_ALL_CODE, false);
        }
    }
    else if ((event == MTB_PMBUS_CMD_WRITE_DONE) && (!vars.en))
    {
        #define REQ_COUNT (3U)
        FCN_EXE_LABEL_t req_sequence[REQ_COUNT] = { Init_Manual_Params, Auto_Calc_Params, Reset_Modules };

        for (uint16_t i=0; i < REQ_COUNT; i++)
        {
            uint16_t timeout_us = (uint16_t)(2.0e6f * params.sys.samp.ts1); // (2.0 * ts1) [us]
            uint32_t req_mask = 1UL << (uint16_t)req_sequence[i];
            fcn_exe_handler.req |= req_mask;
            while (((fcn_exe_handler.req & req_mask) != (fcn_exe_handler.ack & req_mask)) && (0U != timeout_us))
            {
                timeout_us--;
                Cy_SysLib_DelayUs(1U);
            }
            fcn_exe_handler.req &= ~req_mask;
        }
#undef REQ_COUNT
    }

    return true;
}


/************************************************************
* Function Name: StoreUserAllCallback
*************************************************************
* Summary: The STORE_USER_ALL callback function. It includes the
*          'Reset_Modules' feature, and must be invoked following
*          the completion of all configurations.
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool StoreUserAllCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        #define REQ_COUNT (2U)
        FCN_EXE_LABEL_t req_sequence[REQ_COUNT] = { Flash_Params, Reset_Modules };

        for (uint16_t i=0; i < REQ_COUNT; i++)
        {
            uint16_t timeout_us = (uint16_t)(2.0e6f * params.sys.samp.ts1); // (2.0 * ts1) [us]
            uint32_t req_mask = 1UL << (uint16_t)req_sequence[i];
            fcn_exe_handler.req |= req_mask;
            while (((fcn_exe_handler.req & req_mask) != (fcn_exe_handler.ack & req_mask)) && (0U != timeout_us))
            {
                timeout_us--;
                Cy_SysLib_DelayUs(1U);
            }
            fcn_exe_handler.req &= ~req_mask;
        }
#undef REQ_COUNT
    }

    return true;
}


/************************************************************
* Function Name: VoutModeCallback
*************************************************************
* Summary: The VOUT_MODE callback function.
*          Only read operations are supported.
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool VoutModeCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (1L == page)
        {
            uint8_t vout_mode;
            vout_mode = (uint8_t)vout_exp & (uint8_t)VOUT_MODE_PARAM_MSK;

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, VOUT_MODE_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, &vout_mode, VOUT_MODE_SIZE);
        }
    }

    return true;
}


/************************************************************
* Function Name: VoutCommandCallback
*************************************************************
* Summary: The VOUT_COMMAND callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool VoutCommandCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, VOUT_COMMAND_CODE, false); //The write operation allowed
        }
        else
        {
            //The active page is the only page that is allowed in the current project for writing the VOUT_COMMAND command
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, VOUT_COMMAND_CODE, true);
        }
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vout_cmd;
            vout_cmd.value = mtb_pmbus_float_to_lin16(LLC_NOM_VOUT * vars.cmd_ext, vout_exp); /* [V] */

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, VOUT_COMMAND_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vout_cmd.buffer, VOUT_COMMAND_SIZE);
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vout_cmd;

            (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, VOUT_COMMAND_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vout_cmd.buffer, VOUT_COMMAND_SIZE);
            vars.cmd_ext = 1.0f/LLC_NOM_VOUT * mtb_pmbus_lin16_to_float(vout_cmd.value, vout_exp); /* [V] */
        }
    }

    return true;
}


/************************************************************
* Function Name: VoutMaxCallback
*************************************************************
* Summary: The VOUT_MAX callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool VoutMaxCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, VOUT_MAX_CODE, false); //The write operation allowed
        }
        else
        {
            //The active page is the only page that is allowed in the current project for writing the VOUT_MAX command
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, VOUT_MAX_CODE, true);
        }
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vout_max;

            vout_max.value = mtb_pmbus_float_to_lin16(params.sys.cmd.vout.lim.max, vout_exp); /* [V] */

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, VOUT_MAX_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vout_max.buffer, VOUT_MAX_SIZE);
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vout_max;

            (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, VOUT_MAX_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vout_max.buffer, VOUT_MAX_SIZE);
            params.sys.cmd.vout.lim.max = mtb_pmbus_lin16_to_float(vout_max.value, vout_exp);
        }
    }

    return true;
}


/************************************************************
* Function Name: VoutTransitionRateCallback
*************************************************************
* Summary: The VOUT_TRANSITION_RATE callback function
*          Only read operations are supported.
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool VoutTransitionRateCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vout_rate;
            vout_rate.value = mtb_pmbus_float_to_lin11(params.sys.rate_lim.vout_cmd * 1e-3f); /* [mV/usec]  value_mV_usec = value_V_sec / 1000 */

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, VOUT_TRANSITION_RATE_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vout_rate.buffer, VOUT_TRANSITION_RATE_SIZE);
        }
    }

    return true;
}


/************************************************************
* Function Name: VoutMinCallback
*************************************************************
* Summary: The VOUT_MIN callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool VoutMinCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, VOUT_MIN_CODE, false); //The write operation allowed
        }
        else
        {
            //The active page is the only page that is allowed in the current project for writing the VOUT_MIN command
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, VOUT_MIN_CODE, true);
        }
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vout_min;
            vout_min.value = mtb_pmbus_float_to_lin16(params.sys.cmd.vout.lim.min, vout_exp); /* [V] */

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, VOUT_MIN_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vout_min.buffer, VOUT_MIN_SIZE);
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vout_min;

            (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, VOUT_MIN_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vout_min.buffer, VOUT_MIN_SIZE);
            params.sys.cmd.vout.lim.min = mtb_pmbus_lin16_to_float(vout_min.value, vout_exp);
        }
    }

    return true;
}


/************************************************************
* Function Name: MaxDutyCallback
*************************************************************
* Summary: The MAX_DUTY callback function
*          Only read operations are supported.
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool MaxDutyCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (1L == page)
        {
            VAL16_DATA_t max_duty;
            max_duty.value = mtb_pmbus_float_to_lin11(100.0f); /* [%] The value is hard-coded and can't be changed */

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, MAX_DUTY_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, max_duty.buffer, MAX_DUTY_SIZE);
        }
    }

    return true;
}


/************************************************************
* Function Name: FrequencySwitchCallback
*************************************************************
* Summary: The FREQUENCY_SWITCH callback function
*          Only read operations are supported.
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool FrequencySwitchCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (1L == page)
        {
            VAL16_DATA_t sw_frequency;
            sw_frequency.value = mtb_pmbus_float_to_lin11(1.0E-3f / vars.Tsw_cmd_int); // [kHz]

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, FREQUENCY_SWITCH_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, sw_frequency.buffer, FREQUENCY_SWITCH_SIZE);
        }
    }

    return true;
}


/************************************************************
* Function Name: VoutOvFaultLimitCallback
*************************************************************
* Summary: The VOUT_OV_FAULT_LIMIT callback function
*          Read/Write operations are supported.
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
static bool VoutOvFaultLimitCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, VOUT_OV_FAULT_LIMIT_CODE, false); //The write operation allowed
        }
        else
        {
            //The active page is the only page that is allowed in the current project for writing the VOUT_OV_FAULT_LIMIT command
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, VOUT_OV_FAULT_LIMIT_CODE, true);
        }
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vout_ov;
            vout_ov.value = mtb_pmbus_float_to_lin16(params.sys.faults.sw.vout.thresh.max, vout_exp); /* [V] */

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, VOUT_OV_FAULT_LIMIT_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vout_ov.buffer, VOUT_OV_FAULT_LIMIT_SIZE);
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vout_ov;

            (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, VOUT_OV_FAULT_LIMIT_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vout_ov.buffer, VOUT_OV_FAULT_LIMIT_SIZE);
            params.sys.faults.sw.vout.thresh.max = mtb_pmbus_lin16_to_float(vout_ov.value, vout_exp);
        }
    }

    return true;
}


/************************************************************
* Function Name: VinOvFaultLimitCallback
*************************************************************
* Summary: The VIN_OV_FAULT_LIMIT callback function
*          Read/Write operations are supported.
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
static bool VinOvFaultLimitCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, VIN_OV_FAULT_LIMIT_CODE, false); //The write operation allowed
        }
        else
        {
            //The active page is the only page that is allowed in the current project for writing the VIN_OV_FAULT_LIMIT command
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, VIN_OV_FAULT_LIMIT_CODE, true);
        }
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vin_ov;
            vin_ov.value = mtb_pmbus_float_to_lin11(params.sys.faults.sw.vin.thresh.max); /* [V] */

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, VIN_OV_FAULT_LIMIT_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vin_ov.buffer, VIN_OV_FAULT_LIMIT_SIZE);
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vin_ov;

            (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, VIN_OV_FAULT_LIMIT_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vin_ov.buffer, VIN_OV_FAULT_LIMIT_SIZE);
            params.sys.faults.sw.vin.thresh.max = mtb_pmbus_lin11_to_float(vin_ov.value);
        }
    }

    return true;
}


/************************************************************
* Function Name: VinUvFaultLimitCallback
*************************************************************
* Summary: The VIN_UV_FAULT_LIMIT callback function
*          Read/Write operations are supported.
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
static bool VinUvFaultLimitCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, VIN_UV_FAULT_LIMIT_CODE, false); //The write operation allowed
        }
        else
        {
            //The active page is the only page that is allowed in the current project for writing the VIN_UV_FAULT_LIMIT command
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, VIN_UV_FAULT_LIMIT_CODE, true);
        }
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vin_uv;
            vin_uv.value = mtb_pmbus_float_to_lin11(params.sys.faults.sw.vin.thresh.min); /* [V] */

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, VIN_UV_FAULT_LIMIT_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vin_uv.buffer, VIN_UV_FAULT_LIMIT_SIZE);
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t vin_uv;

            (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, VIN_UV_FAULT_LIMIT_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vin_uv.buffer, VIN_UV_FAULT_LIMIT_SIZE);
            params.sys.faults.sw.vin.thresh.min = mtb_pmbus_lin11_to_float(vin_uv.value);
        }
    }

    return true;
}


/************************************************************
* Function Name: IinOcFaultLimitCallback
*************************************************************
* Summary: The IIN_OC_FAULT_LIMIT callback function
*          Read/Write operations are supported.
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
static bool IinOcFaultLimitCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, IIN_OC_FAULT_LIMIT_CODE, false); //The write operation allowed
        }
        else
        {
            //The active page is the only page that is allowed in the current project for writing the IIN_OC_FAULT_LIMIT command
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, IIN_OC_FAULT_LIMIT_CODE, true);
        }
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t ir_oc;
            ir_oc.value = mtb_pmbus_float_to_lin11(params.sys.faults.hw.ires.thresh.max); /* [A] */

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, IIN_OC_FAULT_LIMIT_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, ir_oc.buffer, IIN_OC_FAULT_LIMIT_SIZE);
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        if (PWRDEV_ACT_PAGE == page)
        {
            VAL16_DATA_t ir_oc;

            (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, IIN_OC_FAULT_LIMIT_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, ir_oc.buffer, IIN_OC_FAULT_LIMIT_SIZE);
            params.sys.faults.hw.ires.thresh.max = mtb_pmbus_lin11_to_float(ir_oc.value);
        }
    }

    return true;
}


/************************************************************
* Function Name: StatusByteCallback
*************************************************************
* Summary: The STATUS_BYTE callback function
*          Only read operations are supported.
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
static bool StatusByteCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    STATUS_WORD_DATA_t* unit_status_word_p;
    unit_status_word_p = (STATUS_WORD_DATA_t*)&unit_status_word;

    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        unit_status_word_p->lo_byte.bf.unit_was_busy   = 0U; /** TODO: Implement busy status processing after MVP */
        unit_status_word_p->lo_byte.bf.vout_ov_fault   = faults.flags_latched.sw.ov_vout | faults.flags_latched.hw.ov_vout;
        unit_status_word_p->lo_byte.bf.iout_oc_fault   = faults.flags_latched.sw.oc_iout | faults.flags_latched.hw.oc_iout;
        unit_status_word_p->lo_byte.bf.vin_uv_fault    = faults.flags_latched.sw.uv_vin;
        unit_status_word_p->lo_byte.bf.temperature_f_w = faults.flags_latched.sw.ot;
        unit_status_word_p->lo_byte.bf.cml_event       = 0U; /** TODO: Implement CML event processing after MVP */
        unit_status_word_p->lo_byte.bf.none_of_above   = (faults.flags_latched.sw.em_stop | faults.flags_latched.hw.oc_ires |
                                                          faults.flags_latched.sw.ov_vin  | faults.flags_latched.hw.ov_vin |
                                                          faults.flags_latched.sw.params);

        (void)mtb_pmbus_cmd_update_data_isr(&pmbus_inst, STATUS_BYTE_CODE, unit_status_word_p->buffer, STATUS_BYTE_SIZE);
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        /** TODO: Implement the clear bits request processing after MVP */
    }

    return true;
}


/************************************************************
* Function Name: StatusWordCallback
*************************************************************
* Summary: The STATUS_WORD callback function
*          Only read operations are supported.
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
static bool StatusWordCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    STATUS_WORD_DATA_t* unit_status_word_p;
    unit_status_word_p = (STATUS_WORD_DATA_t*)&unit_status_word;

    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        unit_status_word_p->lo_byte.bf.unit_was_busy   = 0U; /** TODO: Implement busy status processing after MVP */
        unit_status_word_p->lo_byte.bf.vout_ov_fault   = faults.flags_latched.sw.ov_vout | faults.flags_latched.hw.ov_vout;
        unit_status_word_p->lo_byte.bf.iout_oc_fault   = faults.flags_latched.sw.oc_iout | faults.flags_latched.hw.oc_iout;
        unit_status_word_p->lo_byte.bf.vin_uv_fault    = faults.flags_latched.sw.uv_vin;
        unit_status_word_p->lo_byte.bf.temperature_f_w = faults.flags_latched.sw.ot;
        unit_status_word_p->lo_byte.bf.cml_event       = 0U; /** TODO: Implement CML event processing after MVP */
        unit_status_word_p->lo_byte.bf.none_of_above   = faults.flags_latched.sw.em_stop | faults.flags_latched.hw.oc_ires |
                                                         faults.flags_latched.sw.ov_vin  | faults.flags_latched.hw.ov_vin |
                                                         faults.flags_latched.sw.params;

        unit_status_word_p->hi_byte.bf.input_f_w       = faults.flags_latched.sw.uv_vin  | faults.flags_latched.sw.ov_vin |
                                                         faults.flags_latched.hw.oc_ires | faults.flags_latched.hw.ov_vin;
        unit_status_word_p->hi_byte.bf.iout_pout_f_w   = unit_status_word_p->lo_byte.bf.iout_oc_fault; /** TODO: The control logic of this bit should be changed after the STATUS_IOUT-register will be supported */
        unit_status_word_p->hi_byte.bf.vout_f_w        = unit_status_word_p->lo_byte.bf.vout_ov_fault;

        (void)mtb_pmbus_cmd_update_data_isr(&pmbus_inst, STATUS_WORD_CODE, unit_status_word_p->buffer, STATUS_WORD_SIZE);
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        /** TODO: Implement the clear bits request processing after MVP */
    }

    return true;
}


/************************************************************
* Function Name: StatusInputCallback
*************************************************************
* Summary: The STATUS_INPUT callback function
*          Only read operations are supported.
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
static bool StatusInputCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    STATUS_INPUT_DATA_t* status_input_reg_p;
    status_input_reg_p = (STATUS_INPUT_DATA_t*)&status_input_reg;

    /* Avoid compiler warnings */
    (void)byte;
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        status_input_reg_p->bf.iin_oc_fault    = faults.flags_latched.hw.oc_ires;
        status_input_reg_p->bf.vin_uv_fault    = faults.flags_latched.sw.uv_vin;
        status_input_reg_p->bf.vin_ov_fault    = faults.flags_latched.sw.ov_vin | faults.flags_latched.hw.ov_vin;

        (void)mtb_pmbus_cmd_update_data_isr(&pmbus_inst, STATUS_INPUT_CODE, &status_input_reg, STATUS_INPUT_SIZE);
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        /** TODO: Implement the clear bits request processing after MVP */
    }

    return true;
}


/************************************************************
* Function Name: ReadVinCallback
*************************************************************
* Summary: The READ_VIN callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ReadVinCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (1L == page)
        {
            VAL16_DATA_t vin_fb;
            vin_fb.value = mtb_pmbus_float_to_lin11(vars.vin_fb); /* [V] */

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, READ_VIN_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vin_fb.buffer, READ_VIN_SIZE);
        }
    }

    return true;
}


/************************************************************
* Function Name: ReadVoutCallback
*************************************************************
* Summary: The READ_VOUT callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ReadVoutCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (1L == page)
        {
            VAL16_DATA_t vout_fb;
            vout_fb.value = mtb_pmbus_float_to_lin16(vars.vout_fb, vout_exp); /* [V] */

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, READ_VOUT_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, vout_fb.buffer, READ_VOUT_SIZE);
        }
    }

    return true;
}


/************************************************************
* Function Name: ReadIoutCallback
*************************************************************
* Summary: The READ_IOUT callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ReadIoutCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (1L == page)
        {
            VAL16_DATA_t iout_fb;
            iout_fb.value = mtb_pmbus_float_to_lin11(vars.iout_fb); /* [A] */

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, READ_IOUT_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, iout_fb.buffer, READ_IOUT_SIZE);
        }
    }

    return true;
}


/************************************************************
* Function Name: ReadDutyCycleCallback
*************************************************************
* Summary: The READ_DUTY_CYCLE callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ReadDutyCycleCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (1L == page)
        {
            VAL16_DATA_t duty_cycle;
            duty_cycle.value = mtb_pmbus_float_to_lin11(NORM_TO_PERC(vars.d_cmd_int)); // [%]

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, READ_DUTY_CYCLE_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, duty_cycle.buffer, READ_DUTY_CYCLE_SIZE);
        }
    }

    return true;
}


/************************************************************
* Function Name: ReadFrequencyCallback
*************************************************************
* Summary: The READ_FREQUENCY callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ReadFrequencyCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        if (1L == page)
        {
            VAL16_DATA_t sw_frequency;
            sw_frequency.value = mtb_pmbus_float_to_lin11(1.0E-3f / vars.Tsw_cmd_int); // [kHz]

            (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, READ_FREQUENCY_CODE, page, MTB_PMBUS_NO_PAGE_PHASE, sw_frequency.buffer, READ_FREQUENCY_SIZE);
        }
    }

    return true;
}


/************************************************************
* Function Name: IcDeviceIdCallback
*************************************************************
* Summary: The IC_DEVICE_ID callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool IcDeviceIdCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;
    (void)page;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        unpaged_shared_buffer[0] = IC_DEVICE_ID_SIZE;
        unpaged_shared_buffer[1] = (uint8_t)IC_DEVICE_FAMILYID_LO;
        unpaged_shared_buffer[2] = (uint8_t)IC_DEVICE_FAMILYID_HI;
    }

    return true;
}


/************************************************************
* Function Name: IcDeviceRevCallback
*************************************************************
* Summary: The IC_DEVICE_REV callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool IcDeviceRevCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;
    (void)page;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        unpaged_shared_buffer[0] = IC_DEVICE_REV_SIZE;
        unpaged_shared_buffer[1] = (uint8_t)IC_DEVICE_REVISIONID;
        unpaged_shared_buffer[2] = (uint8_t)0U;
    }

    return true;
}


/************************************************************
* Function Name: ScopeTrigIndexCallback
*************************************************************
* Summary: The SCOPE_TRIG_INDEX callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ScopeTrigIndexCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;
    (void)page;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        VAL32_DATA_t index = (VAL32_DATA_t)mtb_oscilloscope_get_trig_ix();
        (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, SCOPE_TRIG_INDEX_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, index.buffer, SCOPE_TRIG_INDEX_SIZE);
    }

    return true;
}


/************************************************************
* Function Name: ScopeTrigHoldOffCallback
*************************************************************
* Summary: The SCOPE_TRIG_HOLD_OFF callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ScopeTrigHoldOffCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;
    (void)page;

    if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        VAL32_DATA_t count;
        (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, SCOPE_TRIG_HOLD_OFF_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, count.buffer, SCOPE_TRIG_HOLD_OFF_SIZE);

        mtb_oscilloscope_set_trig_hold_off(count.value);
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        VAL32_DATA_t count = (VAL32_DATA_t)mtb_oscilloscope_get_trig_hold_off();
        (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, SCOPE_TRIG_HOLD_OFF_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, count.buffer, SCOPE_TRIG_HOLD_OFF_SIZE);
    }

    return true;
}


/************************************************************
* Function Name: ScopeDataRdyCallback
*************************************************************
* Summary: The SCOPE_DATA_RDY callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ScopeDataRdyCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;
    (void)page;

    if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        uint8_t data;
        (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, SCOPE_DATA_RDY_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, &data, SCOPE_DATA_RDY_SIZE);
        if (0U == data)
        {
            mtb_oscilloscope_clr_data_rdy(); // Initiate the probe_scope data collecting
        }
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        uint8_t ready = mtb_oscilloscope_get_data_rdy();
        (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, SCOPE_DATA_RDY_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, &ready, SCOPE_DATA_RDY_SIZE);
    }

    return true;
}


/************************************************************
* Function Name: ReadScopeVarCntCallback
*************************************************************
* Summary: The READ_SCOPE_VAR_CNT callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ReadScopeVarCntCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;
    (void)page;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        uint8_t count = mtb_oscilloscope_data_get_var_count();
        (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, READ_SCOPE_VAR_CNT_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, &count, READ_SCOPE_VAR_CNT_SIZE);
    }

    return true;
}


/************************************************************
* Function Name: ScopeVarListCallback
*************************************************************
* Summary: The SCOPE_VAR_LIST callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ScopeVarListCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    static uint8_t block_num = 0;
    bool callback_status = true;
    #define VARS_IN_BLOCK (((uint8_t)SCOPE_VAR_LIST_SIZE - (uint8_t)sizeof(block_num)) / (uint8_t)MTB_OSCILLOSCOPE_DATA_VAR_NAME_LEN)

    /* Avoid compiler warnings */
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_WRITE_BYTE)
    {
        uint8_t var_cnt = mtb_oscilloscope_data_get_var_count();
        uint8_t block_cnt = (var_cnt + (uint8_t)(VARS_IN_BLOCK - 1U)) / (uint8_t)VARS_IN_BLOCK; // Integer division with rounding up (ceil)
        if ((byte >= block_cnt) || (NULL == mtb_oscilloscope_data_registry_ptr))
        {
            block_num = 0;
            callback_status = false;
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, SCOPE_VAR_LIST_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, &block_num, sizeof(block_num));
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        uint8_t src_base = block_num * (uint8_t)VARS_IN_BLOCK;
        uint8_t src_var_cnt = mtb_oscilloscope_data_registry_ptr->var_count;

        unpaged_shared_buffer[0] = (uint8_t)(MTB_OSCILLOSCOPE_DATA_VAR_NAME_LEN * VARS_IN_BLOCK + sizeof(block_num));
        unpaged_shared_buffer[1] = block_num;
        for (uint8_t v = 0; v < (uint8_t)VARS_IN_BLOCK; v++)
        {
            const SCOPEDATA_VAR_t* src_var_p = &mtb_oscilloscope_data_registry_ptr->var_list[src_base + v];
            uint8_t dst_idx = (uint8_t)MTB_OSCILLOSCOPE_DATA_VAR_NAME_LEN * v;

            for (uint8_t n = 0; n < (uint8_t)MTB_OSCILLOSCOPE_DATA_VAR_NAME_LEN; n++)
            {
                uint8_t src_var_idx = src_base + v;
                if (src_var_idx < src_var_cnt)
                {
                    unpaged_shared_buffer[2 + dst_idx + n] = src_var_p->name[n];
                }
                else
                {
                    unpaged_shared_buffer[2 + dst_idx + n] = (uint8_t)'\0';
                }
            }
        }
    }

    return callback_status;
#undef VARS_IN_BLOCK
}


/************************************************************
* Function Name: ScopeVarMapCallback
*************************************************************
* Summary: The SCOPE_VAR_MAP callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ScopeVarMapCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    #define VAR_IDX (0U)
    #define CH_NUM  (1U)
    static uint8_t byte_cnt = 0;
    bool callback_status = true;

    /* Avoid compiler warnings */
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        byte_cnt = 0;
    }
    else if (event == MTB_PMBUS_CMD_WRITE_BYTE)
    {
        uint8_t var_cnt = mtb_oscilloscope_data_get_var_count();
        if (((VAR_IDX == byte_cnt) && (byte >= var_cnt)) ||
            ((CH_NUM == byte_cnt) && ((MTB_OSCILLOSCOPE_MAX_CH < byte) || (0U == byte))))
        {
            callback_status = false;
        }
        else
        {
            byte_cnt++;
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        VAR_MAP_REG_t map_reg;
        (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, SCOPE_VAR_MAP_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, map_reg.buffer, SCOPE_VAR_MAP_SIZE);

        const SCOPEDATA_VAR_t* var_rec_p = mtb_oscilloscope_data_get_var_rec(map_reg.var_idx);
        mtb_oscilloscope_chan_cfg(map_reg.ch_num, var_rec_p->data_addr, var_rec_p->data_type);
    }

    return callback_status;
#undef VAR_IDX
#undef CH_NUM
}


/************************************************************
* Function Name: ScopeModeCallback
*************************************************************
* Summary: The SCOPE_MODE callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
static bool ScopeModeCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    bool callback_status = true;

    /* Avoid compiler warnings */
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_WRITE_BYTE)
    {
        if (MTB_OSCILLOSCOPE_MODE_TRIG < byte)
        {
            callback_status = false;
        }
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        uint8_t mode = mtb_oscilloscope_get_mode();
        (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, SCOPE_MODE_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, &mode, SCOPE_MODE_SIZE);
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        uint8_t mode;
        (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, SCOPE_MODE_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, &mode, SCOPE_MODE_SIZE);

        mtb_oscilloscope_set_mode(mode);
    }

    return callback_status;
}


/************************************************************
* Function Name: ScopeChCtrlCallback
*************************************************************
* Summary: The SCOPE_CH_CTRL callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ScopeChCtrlCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    #define CH_NUM (0U)
    #define CH_CMD (1U)
    static uint8_t byte_cnt = 0;
    bool callback_status = true;

    /* Avoid compiler warnings */
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        byte_cnt = 0;
    }
    else if (event == MTB_PMBUS_CMD_WRITE_BYTE)
    {
        if (((CH_NUM == byte_cnt) && ((MTB_OSCILLOSCOPE_MAX_CH < byte) || (0U == byte))) ||
            ((CH_CMD == byte_cnt) && (1U < byte)))
        {
            callback_status = false;
        }
        else
        {
            byte_cnt++;
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        CH_CTRL_REG_t ctrl_reg;
        (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, SCOPE_CH_CTRL_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, ctrl_reg.buffer, SCOPE_CH_CTRL_SIZE);

        mtb_oscilloscope_chan_ctrl(ctrl_reg.ch_num, ctrl_reg.ch_cmd);
    }

    return callback_status;
#undef CH_NUM
#undef CH_CMD
}


/************************************************************
* Function Name: ScopeTrigChCallback
*************************************************************
* Summary: The SCOPE_TRIG_CH callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ScopeTrigChCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    bool callback_status = true;

    /* Avoid compiler warnings */
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_WRITE_BYTE)
    {
        if ((MTB_OSCILLOSCOPE_MAX_CH < byte) || (0U == byte))
        {
            callback_status = false;
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        uint8_t ch_num;
        (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, SCOPE_TRIG_CH_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, &ch_num, SCOPE_TRIG_CH_SIZE);

        mtb_oscilloscope_set_trig_chan(ch_num);
    }

    return callback_status;
}


/************************************************************
* Function Name: ScopeTrigSlopeCallback
*************************************************************
* Summary: The SCOPE_TRIG_SLOPE callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ScopeTrigSlopeCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    bool callback_status = true;

    /* Avoid compiler warnings */
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_WRITE_BYTE)
    {
        if (MTB_OSCILLOSCOPE_TRIG_POS < byte)
        {
            callback_status = false;
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        uint8_t slope;
        (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, SCOPE_TRIG_SLOPE_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, &slope, SCOPE_TRIG_SLOPE_SIZE);

        mtb_oscilloscope_set_trig_slope(slope);
    }

    return callback_status;
}


/************************************************************
* Function Name: ScopeTrigPosCallback
*************************************************************
* Summary: The SCOPE_TRIG_POS callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ScopeTrigPosCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;
    (void)page;

    if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        VAL32_DATA_t pos;

        (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, SCOPE_TRIG_POS_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, pos.buffer, SCOPE_TRIG_POS_SIZE);

        mtb_oscilloscope_set_trig_pos(pos.value);
    }

    return true;
}


/************************************************************
* Function Name: ScopeTrigLevelCallback
*************************************************************
* Summary: The SCOPE_TRIG_LEVEL callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ScopeTrigLevelCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    #define CH_NUM (4U)
    static uint8_t byte_cnt = 0;
    bool callback_status = true;

    /* Avoid compiler warnings */
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        byte_cnt = 0;
    }
    else if (event == MTB_PMBUS_CMD_WRITE_BYTE)
    {
        if ((CH_NUM == byte_cnt) && ((MTB_OSCILLOSCOPE_MAX_CH < byte) || (0U == byte)))
        {
            callback_status = false;
        }
        else
        {
            byte_cnt++;
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        CH_LEVEL_CFG_t ch;

        (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, SCOPE_TRIG_LEVEL_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, (uint8_t*)&ch, SCOPE_TRIG_LEVEL_SIZE);

        mtb_oscilloscope_set_trig_level(ch.num, ch.level);
    }

    return callback_status;
#undef CH_NUM
}


/************************************************************
* Function Name: ScopeSampClkDivCallback
*************************************************************
* Summary: The SCOPE_SAMP_CLK_DIV callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ScopeSampClkDivCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;
    (void)page;

    if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        VAL16_DATA_t div;
        (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, SCOPE_SAMP_CLK_DIV_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, div.buffer, SCOPE_SAMP_CLK_DIV_SIZE);

        mtb_oscilloscope_set_samp_clk_div(div.value);
    }

    return true;
}


/************************************************************
* Function Name: ReadScopeSampClkCallback
*************************************************************
* Summary: The READ_SCOPE_SAMP_CLK callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ReadScopeSampClkCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;
    (void)page;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        VAL32_DATA_t clk;
        clk.value = mtb_oscilloscope_get_samp_clk();

        (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, READ_SCOPE_SAMP_CLK_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, clk.buffer, READ_SCOPE_SAMP_CLK_SIZE);
    }

    return true;
}


/************************************************************
* Function Name: ReadScopeSpchCntCallback
*************************************************************
* Summary: The READ_SCOPE_SPCH_CNT callback function
*          Provides the number of samples per channel SPCH (the buffer size)
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ReadScopeSpchCntCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    /* Avoid compiler warnings */
    (void)phase;
    (void)byte;
    (void)page;

    if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        VAL16_DATA_t size;
        size.value = (uint16_t)MTB_OSCILLOSCOPE_MAX_SAMPLES;

        (void)mtb_pmbus_cmd_update_data_ext_isr(&pmbus_inst, READ_SCOPE_SPCH_CNT_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, size.buffer, READ_SCOPE_SPCH_CNT_SIZE);
    }

    return true;
}


/************************************************************
* Function Name: ScopeChSampBufCallback
*************************************************************
* Summary: The SCOPE_CH_SAMP_BUF callback function
*
* Parameters: mtb_pmbus_cmd_events_t event, int32_t page,
*             int32_t phase, uint8_t byte
* Return: bool
************************************************************/
bool ScopeChSampBufCallback(mtb_pmbus_cmd_events_t event, int32_t page, int32_t phase, uint8_t byte)
{
    #define CH_NUM    (0U)
    #define BLOCK_NUM (1U)
    #define SAMP_BYTES_IN_BLOCK (SCOPE_CH_SAMP_BUF_SIZE - (uint16_t)sizeof(CH_SAMP_BUF_REG_t))
    static CH_SAMP_BUF_REG_t cmd_reg;
    static uint8_t  byte_cnt  = 0;
    static uint16_t block_cnt = 0U;
    bool callback_status = true;
    uint16_t src_bytes_cnt = MTB_OSCILLOSCOPE_MAX_SAMPLES * (uint16_t)sizeof(float32_t);

    /* Avoid compiler warnings */
    (void)phase;
    (void)page;

    if (event == MTB_PMBUS_CMD_MATCH)
    {
        byte_cnt  = 0;
        block_cnt = 0U;

        bool data_ready = (1UL == mtb_oscilloscope_get_data_rdy()) ? true : false;
        if (!data_ready)
        {
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, SCOPE_CH_SAMP_BUF_CODE, true);
        }
        else
        {
            mtb_pmbus_cmd_wr_protect_isr(&pmbus_inst, SCOPE_CH_SAMP_BUF_CODE, false);
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_BYTE)
    {
        block_cnt = (src_bytes_cnt + (SAMP_BYTES_IN_BLOCK - 1U)) / SAMP_BYTES_IN_BLOCK; // Integer division with rounding up (ceil)
        block_cnt = (255U <= block_cnt) ? 255U : block_cnt;

        if (((CH_NUM == byte_cnt) && ((MTB_OSCILLOSCOPE_MAX_CH < byte) || (0U == byte))) ||
            ((BLOCK_NUM == byte_cnt) && (byte >= (uint8_t)block_cnt)))
        {
            cmd_reg.ch_num = 1;
            cmd_reg.block_num = 0;
            callback_status = false;
        }
        else
        {
            byte_cnt++;
        }
    }
    else if (event == MTB_PMBUS_CMD_WRITE_DONE)
    {
        (void)mtb_pmbus_cmd_read_data_ext_isr(&pmbus_inst, SCOPE_CH_SAMP_BUF_CODE, MTB_PMBUS_NO_PAGE_PHASE, MTB_PMBUS_NO_PAGE_PHASE, cmd_reg.buffer, sizeof(CH_SAMP_BUF_REG_t));
    }
    else if (event == MTB_PMBUS_CMD_READ_REQ)
    {
        uint8_t* src_addr_p = mtb_oscilloscope_get_samp_buf_addr(cmd_reg.ch_num);

        if (NULL != src_addr_p)
        {
            uint16_t src_base = (uint16_t)cmd_reg.block_num * SAMP_BYTES_IN_BLOCK;
            uint8_t* src_base_addr_p = (uint8_t*)(src_addr_p + src_base);

            unpaged_shared_buffer[0] = (uint8_t)(SCOPE_CH_SAMP_BUF_SIZE);
            unpaged_shared_buffer[1] = cmd_reg.ch_num;
            unpaged_shared_buffer[2] = cmd_reg.block_num;
            for (uint8_t s = 0; s < (uint8_t)SAMP_BYTES_IN_BLOCK; s++)
            {
                if ((src_base + s) < src_bytes_cnt)
                {
                    unpaged_shared_buffer[3 + s] = *(src_base_addr_p + s);
                }
                else
                {
                    unpaged_shared_buffer[3 + s] = 0xFFU;
                }
            }
        }
        else
        {
            callback_status = false;
        }
    }

    return callback_status;
#undef SAMP_BYTES_IN_BLOCK
#undef BLOCK_NUM
#undef CH_NUM
}


/*******************************************************************************
*    EXPORTED FUNCTIONS
*******************************************************************************/
