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
 *<!-- The LLC PMBus documentation structure -->
 * \defgroup group_llc_pmbus_general Power System Management Protocol (PMBus)
 * \{
 *
 * \}
 *<!-- End of the LLC PMBus documentation structure -->
 *
 * \addtogroup group_llc_pmbus_general
 * \{
 *
 * \section section_pmbus_overview PMBus Implementation Overview
 * \par
 * This implementation provides a comprehensive PMBus interface for LLC Resonant Power Converter
 * applications using the Infineon MTB PMBus middleware. The implementation enables communication
 * and control of power management parameters through the industry-standard PMBus protocol.
 * \par
 * The PMBus implementation is built on top of the **MTB PMBus middleware**, which provides:
 * - Full PMBus v1.4 protocol compliance
 * - I2C/SMBus hardware abstraction
 * - Command processing framework
 * - Automatic packet error checking (PEC)
 * - Multi-page and multi-phase support
 * \par
 * For detailed information about the MTB PMBus middleware, refer to the
 * [PMBus Middleware Library documentation](https://github.com/Infineon/mtb-pmbus#pmbus-middleware-library)
 *
 * \section section_pmbus_commands Supported PMBus Commands
 * \par
 * The following table lists all PMBus commands supported by this implementation:
 * \par
 * | Command Code | Command Name | Read | Write | Paged | Data Format | Description |
 * |--------------|--------------|------|-------|-------|-------------|-------------|
 * | 0x01 | \ref subsection_operation "OPERATION" | ✓ | ✓ | ✓ | Raw | Power supply on/off control |
 * | 0x02 | \ref subsection_on_off_config "ON_OFF_CONFIG" | ✓ | ✓ | ✓ | Raw | On/off configuration settings |
 * | 0x03 | CLEAR_FAULTS | ✗ | ✓ | ✓ | Raw | Clear all fault conditions |
 * | 0x12 | \ref subsection_restore_default_all "RESTORE_DEFAULT_ALL" | ✗ | ✓ | ✗ | Raw | Restore factory default settings |
 * | 0x15 | \ref subsection_store_user_all "STORE_USER_ALL" | ✗ | ✓ | ✗ | Raw | Store current settings to NVM |
 * | 0x20 | \ref subsection_vout_mode "VOUT_MODE" | ✓ | ✗ | ✓ | Raw | Output voltage data format |
 * | 0x21 | VOUT_COMMAND | ✓ | ✓ | ✓ | ULINEAR16 | Output voltage setting |
 * | 0x24 | VOUT_MAX | ✓ | ✓ | ✓ | ULINEAR16 | Maximum output voltage limit |
 * | 0x27 | VOUT_TRANSITION_RATE | ✓ | ✗ | ✓ | LINEAR11 | Output voltage transition rate |
 * | 0x2B | VOUT_MIN | ✓ | ✓ | ✓ | ULINEAR16 | Minimum output voltage limit |
 * | 0x32 | MAX_DUTY | ✓ | ✗ | ✓ | LINEAR11 | Maximum duty cycle |
 * | 0x33 | FREQUENCY_SWITCH | ✓ | ✗ | ✓ | LINEAR11 | Switching frequency |
 * | 0x40 | VOUT_OV_FAULT_LIMIT | ✓ | ✓ | ✓ | ULINEAR16 | Output overvoltage fault limit |
 * | 0x55 | VIN_OV_FAULT_LIMIT | ✓ | ✓ | ✓ | LINEAR11 | Input overvoltage fault limit |
 * | 0x59 | VIN_UV_FAULT_LIMIT | ✓ | ✓ | ✓ | LINEAR11 | Input undervoltage fault limit |
 * | 0x5B | IIN_OC_FAULT_LIMIT | ✓ | ✓ | ✓ | LINEAR11 | Input overcurrent fault limit |
 * | 0x78 | \ref subsection_status_byte "STATUS_BYTE" | ✓ | ✗ | ✗ | Raw | Summary status byte |
 * | 0x79 | \ref subsection_status_word "STATUS_WORD" | ✓ | ✗ | ✗ | Raw | Summary status word |
 * | 0x7C | \ref subsection_status_input "STATUS_INPUT" | ✓ | ✗ | ✗ | Raw | Input status register |
 * | 0x88 | READ_VIN | ✓ | ✗ | ✓ | LINEAR11 | Input voltage measurement |
 * | 0x8B | READ_VOUT | ✓ | ✗ | ✓ | ULINEAR16 | Output voltage measurement |
 * | 0x8C | READ_IOUT | ✓ | ✗ | ✓ | LINEAR11 | Output current measurement |
 * | 0x94 | READ_DUTY_CYCLE | ✓ | ✗ | ✓ | LINEAR11 | Current duty cycle |
 * | 0x95 | READ_FREQUENCY | ✓ | ✗ | ✓ | LINEAR11 | Current switching frequency |
 * | 0x99 | \ref subsection_mfr_id "MFR_ID" | ✓ | ✗ | ✗ | Block | Manufacturer identification |
 * | 0x9A | \ref subsection_mfr_model "MFR_MODEL" | ✓ | ✗ | ✗ | Block | Manufacturer model |
 * | 0x9B | MFR_REVISION | ✓ | ✗ | ✗ | Block | Manufacturer revision |
 * | 0xAD | \ref subsection_ic_device_id "IC_DEVICE_ID" | ✓ | ✗ | ✗ | Block | IC device identification |
 * | 0xAE | \ref subsection_ic_device_rev "IC_DEVICE_REV" | ✓ | ✗ | ✗ | Block | IC device revision |
 * | 0xED | \ref subsection_scope_trig_index "SCOPE_TRIG_INDEX" | ✓ | ✗ | ✗ | Block | Scope trigger index position |
 * | 0xEE | \ref subsection_scope_trig_hold_off "SCOPE_TRIG_HOLD_OFF" | ✓ | ✓ | ✗ | Block | Scope trigger hold-off count |
 * | 0xEF | \ref subsection_scope_data_rdy "SCOPE_DATA_RDY" | ✓ | ✓ | ✗ | Block | Scope data ready status |
 * | 0xF0 | \ref subsection_read_scope_var_cnt "READ_SCOPE_VAR_CNT" | ✓ | ✗ | ✗ | Block | Read scope variable count |
 * | 0xF1 | \ref subsection_scope_var_list "SCOPE_VAR_LIST" | Process | Call | ✗ | Block | Scope variable list |
 * | 0xF2 | \ref subsection_scope_var_map "SCOPE_VAR_MAP" | ✗ | ✓ | ✗ | Block | Scope variable mapping |
 * | 0xF3 | \ref subsection_scope_mode "SCOPE_MODE" | ✓ | ✓ | ✗ | Block | Scope operating mode |
 * | 0xF4 | \ref subsection_scope_ch_ctrl "SCOPE_CH_CTRL" | ✗ | ✓ | ✗ | Block | Scope channel control |
 * | 0xF5 | \ref subsection_scope_trig_ch "SCOPE_TRIG_CH" | ✗ | ✓ | ✗ | Block | Scope trigger channel |
 * | 0xF6 | \ref subsection_scope_trig_slope "SCOPE_TRIG_SLOPE" | ✗ | ✓ | ✗ | Block | Scope trigger slope |
 * | 0xF7 | \ref subsection_scope_trig_pos "SCOPE_TRIG_POS" | ✗ | ✓ | ✗ | Block | Scope trigger position |
 * | 0xF8 | \ref subsection_scope_trig_level "SCOPE_TRIG_LEVEL" | ✗ | ✓ | ✗ | Block | Scope trigger level |
 * | 0xF9 | \ref subsection_scope_samp_clk_div "SCOPE_SAMP_CLK_DIV" | ✗ | ✓ | ✗ | Block | Scope sampling clock divider |
 * | 0xFA | \ref subsection_read_scope_samp_clk "READ_SCOPE_SAMP_CLK" | ✓ | ✗ | ✗ | Block | Read scope sampling clock |
 * | 0xFB | \ref subsection_read_scope_spch_cnt "READ_SCOPE_SPCH_CNT" | ✓ | ✗ | ✗ | Block | Read scope samples per channel count |
 * | 0xFC | \ref subsection_scope_ch_samp_buf "SCOPE_CH_SAMP_BUF" | Process | Call | ✗ | Block | Scope channel sample buffer |
 *
 * \note In the current implementation, **page 1** is the only active page for all paged commands.
 * Read operations targeting other pages complete without error but return no meaningful data.
 * Write operations targeting other pages are rejected with a NACK response.
 *
 * \section section_pmbus_command_details Command Descriptions
 * \par
 * Block-format commands comply with the PMBus specification: the first byte of every
 * block transfer is the byte count field, followed by payload bytes. The \c mtb-pmbus middleware
 * handles this byte count automatically at the API level — inserting it on write and parsing
 * it on read — without application involvement. Therefore, all Block-format tables below
 * describe **payload data bytes only**, with Byte 0 being the first payload byte.
 * \par
 * The only exception applies to commands whose transfer buffer is populated manually by the
 * application (\ref subsection_ic_device_id "IC_DEVICE_ID", \ref subsection_ic_device_rev "IC_DEVICE_REV",
 * \ref subsection_scope_var_list "SCOPE_VAR_LIST", \ref subsection_scope_ch_samp_buf "SCOPE_CH_SAMP_BUF"):
 * in this case the payload byte count is explicitly placed at `buffer[0]`,
 * and the format tables for those commands include that field.
 *
 * \subsection subsection_mfr_id MFR_ID (0x99)
 * \par
 * Returns the manufacturer identification string. This implementation returns "IFX"
 * (3 bytes) identifying Infineon as the manufacturer. The command uses block read
 * format with automatic byte count prefix.
 * \par
 * **Data transmission:** The data is sent in reverse sequence over the PMBus interface.
 * - ASCII string: "IFX"
 * - Hexadecimal equivalent: 0x49, 0x46, 0x58 ('I' + 'F' + 'X')
 * - Transmitted sequence:   0x58, 0x46, 0x49 ('X' + 'F' + 'I')
 *
 * \subsection subsection_mfr_model MFR_MODEL (0x9A)
 * \par
 * Returns the manufacturer model string. This implementation returns "LLC"
 * (3 bytes) identifying this as an LLC Resonant Power Converter device. The command uses
 * block read format with automatic byte count prefix.
 * \par
 * **Data transmission:** The data is sent in reverse sequence over the PMBus interface.
 * - ASCII string: "LLC"
 * - Hexadecimal equivalent: 0x4C, 0x4C, 0x43 ('L' + 'L' + 'C')
 * - Transmitted sequence:   0x43, 0x4C, 0x4C ('C' + 'L' + 'L')
 *
 * \subsection subsection_operation OPERATION (0x01)
 * \par
 * Controls the power supply on/off state and operational parameters. The command
 * supports both read and write operations with page support.
 * \par
 * **Supported bit fields (processed by implementation):**
 * - **Bit 7: On/Off control** (1 = On, 0 = Off) - **Fully supported and processed**
 * \par
 * **Defined but unsupported bit fields (writes will be ignored):**
 * - Bit 6: Power down control
 * - Bits 5-4: Output voltage source control
 * - Bits 3-2: Output voltage limit fault response
 * - Bit 1: AVSBus/PMBus switch control
 * - Bit 0: Reserved
 * \par
 * The implementation includes safety checks to ensure the device can only be turned on
 * if the bus control source is enabled in the ON_OFF_CONFIG register. Writing to
 * unsupported bit fields will be accepted but ignored - only the On/Off control bit
 * (bit 7) is actually processed and affects device operation.
 *
 * \note When reading this command, only the On/Off status (bit 7) reflects the
 * actual device state. All other bits will return as 0.
 *
 * \subsection subsection_on_off_config ON_OFF_CONFIG (0x02)
 * \par
 * Configures the power supply startup and shutdown behavior according to PMBus
 * specification. The command supports both read and write operations with page support.
 * \par
 * **Configurable bit fields (processed by implementation):**
 * - **Bit 3: Bus as start source enable** (1 = external bus control, 0 = internal control) - **Fully supported and processed**
 * \par
 * **Application logic controlled bit fields (read only, writes will be ignored):**
 * - **Bit 0: CONTROL pin off action** = 1 (turn off as fast as possible)
 * - Bit 1: CONTROL pin polarity = 0 (is not used)
 * - **Bit 2: CONTROL pin as start source** = 0 (unit ignores CONTROL pin)
 * - **Bit 4: Start at power-on behavior** = 0 (unit powers up when power is present)
 * - Bits 7-5: Reserved = 0 (reserved for future use)
 * \par
 * **Current implementation behavior:**
 * When bit 3 = 1 (bus start source enabled), the device allows external PMBus
 * command control via OPERATION command and sets internal source to "External".
 * When bit 3 = 0 (bus start source disabled), the device operates under internal
 * control only and ignores OPERATION commands from PMBus.
 *
 * \note This implementation does not support a physical CONTROL pin. Only the
 * bus start source control (bit 3) is configurable. All other bits are set to
 * application-specific values and cannot be changed via PMBus commands.
 *
 * \subsection subsection_vout_mode VOUT_MODE (0x20)
 * \par
 * Specifies the data format used for output voltage commands and telemetry.
 * This is a read-only, paged command that returns the voltage data format currently
 * used by the application.
 * \par
 * **PMBus VOUT_MODE bit field definition:**
 * - **Bits [7:5]**: Mode = 000 (ULINEAR16 data format)
 * - **Bits [4:0]**: Parameter = 11000 (-8 in two's complement)
 * \par
 * This supported format provides voltage resolution of 1/256 = 0.00390625 V
 * and supports voltage range from 0V to approximately 255.996V.
 * \par
 * **Implementation details:**
 * - Only read operations are supported - write attempts are ignored
 * - Format cannot be changed by the user via PMBus commands
 * - All voltage-related commands (VOUT_COMMAND, VOUT_MAX, VOUT_MIN, READ_VOUT, etc.)
 *   use this ULINEAR16 format with exponent -8
 * - Format is consistent across all pages
 *
 * \note The voltage data format is fixed at compile time and optimized for this
 * LLC application's voltage range and resolution requirements.
 *
 * \subsection subsection_restore_default_all RESTORE_DEFAULT_ALL (0x12)
 * \par
 * Restores all configurable parameters to their factory default values. This is a
 * write-only, unpaged command that operates globally across all pages. The command
 * executes a sequence of operations to reinitialize the device:
 * - Initializes manual parameters to factory defaults
 * - Performs automatic parameter calculations
 * - Resets all control modules
 * \par
 * The command includes safety mechanisms to prevent execution when the device is
 * enabled (ON state). If attempted while the device is running, the command will
 * be automatically disabled to prevent operational disruption.
 *
 * \subsection subsection_store_user_all STORE_USER_ALL (0x15)
 * \par
 * Stores all current user-configurable parameters to non-volatile memory (NVM).
 * This is a write-only, unpaged command that preserves the current device
 * configuration for restoration after power cycles. The command executes:
 * - Flash memory storage of current parameters
 * - Module reset to apply stored configuration
 *
 * \note The STORE_USER_ALL command must be called after all power device configurations are finished.
 * This ensures that the complete configuration is being properly processed, recalculated, and saved to NVM and becomes the new power-on default state. <br>
 * Failure to call this command after configuration changes will result in loss of settings and unexpected behavior upon device restart.
 *
 * \subsection subsection_status_byte STATUS_BYTE (0x78)
 * \par
 * Returns a one-byte summary of the most critical fault and status conditions of the
 * device, as defined by the PMBus specification. This is a read-only, unpaged command.
 * Refer to the PMBus v1.4 specification for the standard bit-field definitions.
 * \par
 * **Supported bit fields (processed by implementation):**
 * - **Bit 5: VOUT_OV_FAULT** - ***output overvoltage*** fault
 * - **Bit 4: IOUT_OC_FAULT** - ***output overcurrent*** fault
 * - **Bit 3: VIN_UV_FAULT** - ***input undervoltage*** fault
 * - **Bit 2: TEMPERATURE** - ***over-temperature*** fault/warning
 * - **Bit 0: NONE OF THE ABOVE** - asserted when any fault not covered by the bits above is active
 *   (***emergency stop***, ***input hardware overcurrent***, ***input overvoltage***, ***parameters fault***, ***output overvoltage***)
 * \par
 * **Defined but unsupported bit fields (always read as 0):**
 * - Bit 7: BUSY
 * - Bit 6: OFF
 * - Bit 1: CML (Communication, Memory, Logic)
 * \par
 * \note Write access (clear-bits request) is not implemented. Writes are accepted but produce no effect on the latched fault flags.
 *
 * \subsection subsection_status_word STATUS_WORD (0x79)
 * \par
 * Returns the two-byte status summary register. The low byte is identical to
 * \ref subsection_status_byte "STATUS_BYTE (0x78)". The high byte adds upper-level
 * fault summary bits per the PMBus v1.4 specification. This is a read-only, unpaged command.
 * \par
 * **Supported high-byte bit fields (processed by implementation):**
 * - **Bit 15: VOUT** - asserted on ***output overvoltage*** fault
 * - **Bit 14: IOUT/POUT** - asserted on ***output overcurrent*** fault
 *   (currently mirrors the low-byte IOUT_OC_FAULT; will be re-sourced once a dedicated STATUS_IOUT register is added)
 * - **Bit 13: INPUT** - asserted on any input-side fault: ***input undervoltage***, ***input overvoltage***, or ***input hardware overcurrent***
 * \par
 * **Defined but unsupported high-byte bit fields (always read as 0):**
 * - Bit 12: MFR_SPECIFIC
 * - Bit 11: POWER_GOOD#
 * - Bit 10: FANS
 * - Bit  9: OTHER
 * - Bit  8: UNKNOWN
 * \par
 * The low byte is populated identically to \ref subsection_status_byte "STATUS_BYTE";
 * see that section for low-byte details.
 *
 * \note Write access (clear-bits request) is not implemented.
 *
 * \subsection subsection_status_input STATUS_INPUT (0x7C)
 * \par
 * Returns the input-side fault and warning status register. This is a read-only,
 * unpaged command. Refer to the PMBus v1.4 specification for the standard bit-field
 * definitions.
 * \par
 * **Supported bit fields (processed by implementation):**
 * - **Bit 7: VIN_OV_FAULT** - ***input overvoltage*** fault
 * - **Bit 4: VIN_UV_FAULT** - ***input undervoltage*** fault
 * - **Bit 2: IIN_OC_FAULT** - ***input hardware overcurrent*** fault (mapped from the resonant-current HW OC)
 * \par
 * **Defined but unsupported bit fields (always read as 0):**
 * - Bit 6: VIN_OV_WARNING
 * - Bit 5: VIN_UV_WARNING
 * - Bit 3: UNIT OFF FOR LOW VIN
 * - Bit 1: IIN_OC_WARNING
 * - Bit 0: PIN_OP_WARNING
 * \par
 * \note Write access (clear-bits request) is not implemented.
 *
 * \subsection subsection_ic_device_id IC_DEVICE_ID (0xAD)
 * \par
 * Returns the MCU family identification in a 2-byte block. The value is
 * obtained at runtime from the target MCU's chip identification register.
 * \par
 * **Response Format:**
 * | Byte | Description             |
 * |------|-------------------------|
 * | 0    | Byte count in block = 2 |
 * | 1    | Family ID, low byte     |
 * | 2    | Family ID, high byte    |
 * \par
 * **Known family ID values:**
 * - **0x0118** - PSC3x5 MCU family
 * - **0x0119** - PSC3x8 MCU family
 *
 * \note During the GUI handshake this command is temporarily disabled by `PMBUS_Start()`.
 *
 * \subsection subsection_ic_device_rev IC_DEVICE_REV (0xAE)
 * \par
 * Returns the MCU silicon revision in a 2-byte block. The value is obtained
 * at runtime from the target MCU's chip identification register.
 * \par
 * **Response Format:**
 * | Byte | Description             |
 * |------|-------------------------|
 * | 0    | Byte count in block = 2 |
 * | 1    | Revision ID             |
 * | 2    | Reserved = 0x00         |
 *
 * \note During the GUI handshake this command is temporarily disabled by `PMBUS_Start()`.
 *
 * \section section_pmbus_manufacturer_commands Manufacturer Specific Command Descriptions
 * \par
 * This implementation provides a comprehensive set of manufacturer-specific commands for advanced
 * oscilloscope and data acquisition functionality. These commands enable real-time monitoring,
 * triggering, and data capture capabilities for power system analysis and debugging.
 *
 * \subsection subsection_scope_trig_index SCOPE_TRIG_INDEX (0xED)
 * \par
 * Returns the current trigger index position within the sample buffer. This is a
 * read-only, unpaged command that provides the exact sample position where the
 * trigger event was detected during the last data capture.
 * \par
 * **Response Format:**
 * | Byte | Description | Format |
 * |------|-------------|--------|
 * | 0-3  | Trigger index position | 32-bit unsigned integer |
 * \par
 * **Trigger Index Position:** The trigger index represents the sample position
 * (0-based) where the trigger condition was satisfied. This information is essential
 * for understanding the exact timing relationship between the trigger event and the
 * captured data samples.
 * \par
 * The trigger index value is updated each time a successful trigger occurs during
 * scope operation. Use this command after \ref subsection_scope_data_rdy "SCOPE_DATA_RDY (0xEF)" confirms
 * data availability to determine the precise trigger location within the captured samples.
 *
 * \subsection subsection_scope_trig_hold_off SCOPE_TRIG_HOLD_OFF (0xEE)
 * \par
 * Controls the trigger hold-off period to prevent false triggering on noise or
 * unwanted signal transitions. This is a read/write, unpaged command that sets
 * the minimum number of samples to wait after a trigger before allowing another
 * trigger event.
 * \par
 * **Request Format (Write):**
 * | Byte | Description | Format | Value Range |
 * |------|-------------|--------|-------------|
 * | 0-3  | Hold-off sample count | 32-bit unsigned integer | 0 to \ref subsection_read_scope_spch_cnt "SPCH" - 1 |
 * \par
 * **Response Format (Read):**
 * | Byte | Description | Format |
 * |------|-------------|--------|
 * | 0-3  | Current hold-off count | 32-bit unsigned integer |
 * \par
 * **Hold-off Sample Count:** Specifies the number of samples the scope will
 * wait after detecting a trigger before it becomes ready to trigger again.
 * This prevents multiple triggers on the same signal event and helps ensure
 * clean trigger behavior in noisy environments.
 *
 * \subsection subsection_scope_data_rdy SCOPE_DATA_RDY (0xEF)
 * \par
 * Controls and monitors the data ready flag status. This unpaged command supports both
 * read and write operations to initiate data collection and check availability status.
 * \par
 * **Write Operation:**
 * - **0x00**: Clears the data ready flag and initiates data collection
 * - Other values: Ignored
 * \par
 * **Read Operation:**
 * - **0x00**: No data ready - oscilloscope is idle or data is still being captured
 * - **0x01**: Data ready - captured samples are available for reading
 * \par
 * **Usage Workflow:**
 * 1. Write 0x00 to SCOPE_DATA_RDY to initiate oscilloscope data collection
 * 2. Poll by reading SCOPE_DATA_RDY until it returns 0x01
 * 3. Retrieve sample data using SCOPE_CH_SAMP_BUF command
 * 4. Repeat process for additional captures as needed
 *
 * \subsection subsection_read_scope_var_cnt READ_SCOPE_VAR_CNT (0xF0)
 * \par
 * Returns the total number of variables available for scope monitoring. This is a
 * read-only, unpaged command that provides the count of system variables that can
 * be mapped to scope channels for data capture.
 * \par
 * The returned count represents all variables defined in the ScopeData registry table,
 * which includes power system parameters such as:
 * - **Feedback signals:** Vin_fb, Vout_fb, Iout_fb, Temp_fb
 * - **Raw sensor data:** Vin_raw, Vout_raw, Iout_raw, Temp_raw
 * - **Filtered sensor values:** Vin_flt, Vout_flt, Iout_flt, Temp_flt
 * - **Command signals:** Cmd_ext, Cmd_int, Cmd_final, Vo_cmd_ext, Vo_cmd_int
 * - **Switching parameters:** Tsw_cmd_ext, Tsw_cmd_vlt, Tsw_cmd_int
 * - **Duty cycle controls:** d_cmd_ext, d_cmd_volt, d_cmd_int
 * - **Timing controls:** Td_on_sr, Td_off_sr
 * - **Controller outputs:** pi_vout_out
 * \par
 * All variables are pre-configured as 32-bit floating point values and are
 * available for real-time monitoring through the scope functionality.
 * \par
 * **Customization Support:**<br>
 * Application developers can customize the ScopeData registry table by modifying
 * the variable list in ScopeData.c to include their own application-specific
 * variables for scope monitoring. Each variable entry in the registry table
 * requires:
 * - **Variable name:** Fixed 12-character string identifier
 * - **Data address:** Pointer to the variable's memory location
 * - **Data type:** Probe scope data type (e.g., PROBE_SCOPE_FP32)
 *
 * \note The registry supports up to 255 variables total, allowing extensive customization
 * for specific application monitoring needs. After modifying the registry table,
 * the returned count will automatically reflect the updated number of available
 * variables.
 *
 * \subsection subsection_scope_var_list SCOPE_VAR_LIST (0xF1)
 * \par
 * Retrieves the list of names of scope variables using **Block Write-Block Read Process Call Protocol**.
 * This command requires writing a block number, then returns the corresponding
 * variable names from the ScopeData registry table.
 * \par
 * **Protocol Details:**
 * - **Write Phase**: Send block number (1 byte, 0-based index)
 * - **Read Phase**: Receive variable name list in block format
 * - **Block size**: 253 bytes
 * \par
 * **Request Format:**
 * | Byte | Description | Value Range |
 * |------|-------------|-------------|
 * | 0 | Block number | 0 - 255 |
 * \par
 * **Response Format:**
 * The response contains a block of concatenated variable names, where each variable
 * name is exactly 12 bytes (SCOPEDATA_VAR_NAME_LEN). Variable names are null-terminated
 * strings, with each byte representing the hexadecimal character code of the ASCII character.<br>
 * Names are separated by null (\\0) characters. If the variable name is shorter than 12 characters, the remaining bytes are padded with space (0x20) characters.
 * \par
 * | Bytes | Description |
 * |-------|-------------|
 * | 0 | Byte count in block |
 * | 1 | Block number (echo) |
 * | 2-13 | Variable name 1 (12 bytes) |
 * | 14-25 | Variable name 2 (12 bytes) |
 * | ... | ... |
 * | 241-253 | Variable name N (12 bytes)|
 * \par
 * **Block Byte Count Calculation:**
 * - **Variables per block** = (Block_Size - Header_Size) / Variable_Name_Length <br>
 * - **Variables per block** = (253 - 1) / 12 = 21 <br>
 * - **Total blocks** = ceil ( \ref subsection_read_scope_var_cnt "Variables_Count" / Variables_Per_Block ) <br>
 * \par
 * **Usage Example:**
 * - **Request block 0x00**: Returns variables 0-20 (names)
 * - **Request block 0x01**: Returns variables 21-41 (names)
 * - **If total variables** = 25, then 2 blocks are needed (0 and 1)
 *
 * \subsection subsection_scope_var_map SCOPE_VAR_MAP (0xF2)
 * \par
 * Configures the mapping between system variables and scope channels. This is a
 * write-only, unpaged command that assigns a specific variable from the ScopeData
 * registry table to a scope channel for data capture.
 * \par
 * **Request Format:**
 * | Byte | Field | Description | Value Range |
 * |------|-------|-------------|-------------|
 * | 0 | Variable Index | Index of variable in complete \ref subsection_scope_var_list "Variable_List" | 0 to ( \ref subsection_read_scope_var_cnt "Variables_Count" - 1) |
 * | 1 | Channel Number | Target scope channel number | 1 to 4 |
 * \par
 * **Variable Index:** Use \ref subsection_scope_var_list "SCOPE_VAR_LIST (0xF1)" command to retrieve
 * the complete list of available variables and their corresponding indices. The variable index
 * corresponds to the position of the variable in the ScopeData registry table (0-based).
 * \par
 * **Channel Number:** Specifies which scope channel (1-4) will monitor the selected
 * variable. Each channel can be mapped to a different variable, allowing simultaneous
 * monitoring of multiple system parameters.
 * \par
 * **Usage Example:**
 * - **Write [0x0105]** (Byte 0 = 0x05, Byte 1 = 0x01): Map variable at index 5 to channel 1
 * - **Write [0x030C]** (Byte 0 = 0x0C, Byte 1 = 0x03): Map variable at index 12 to channel 3
 * \par
 * After successful mapping, the specified channel will capture data from the assigned
 * variable during scope operations. Use \ref subsection_scope_ch_ctrl "SCOPE_CH_CTRL (0xF4)" to enable/disable individual channels.
 *
 * \subsection subsection_scope_mode SCOPE_MODE (0xF3)
 * \par
 * Controls the scope operating mode and capture behavior. This is a
 * read/write, unpaged command that controls the operational state.
 * \par
 * **Supported Modes:**
 * - **0x00**: Idle mode - scope is stopped
 * - **0x01**: Single shot mode - capture once when triggered
 * - **0x02**: Continuous mode - continuous capture and trigger
 * - **0x03**: Auto mode - automatic triggering without external trigger condition
 * \par
 * The scope mode determines how data capture is initiated and whether it repeats
 * after completing a capture cycle.
 *
 * \note Data capturing is possible only when mode is not Idle (0x00), even if
 * individual channels are enabled via \ref subsection_scope_ch_ctrl "SCOPE_CH_CTRL (0xF4)" command. The scope must be in an
 * active mode (Single shot, Continuous, or Auto) to perform data acquisition.<br>
 * It is recommended use this command as last step in scope configuration sequence.
 *
 * \subsection subsection_scope_ch_ctrl SCOPE_CH_CTRL (0xF4)
 * \par
 * Controls individual scope channel enable/disable states. This is a
 * write-only, unpaged command that enables or disables specific scope channels
 * for data capture operations.
 * \par
 * **Request Format:**
 * | Byte | Field | Description | Value Range |
 * |------|-------|-------------|-------------|
 * | 0 | Channel Number | Target scope channel number | 1 to 4 |
 * | 1 | Channel Command | Channel state control | 0 / 1|
 * \par
 * **Channel Number:** Specifies which scope channel to control. The channel
 * must be a valid channel number within the supported range.
 * \par
 * **Channel Command:** Controls the channel state:
 * - **0x00**: Disable the specified channel - channel will not capture data
 * - **0x01**: Enable the specified channel - channel will capture data during scope operations
 * \par
 * **Usage Example:**
 * - **Write [0x0101]** (Byte 0 = 0x01, Byte 1 = 0x01): Enable channel 1 for data capture
 * - **Write [0x0003]** (Byte 0 = 0x03, Byte 1 = 0x00): Disable channel 3 from data capture
 * \par
 * Only enabled channels will capture data during scope operations, allowing
 * selective monitoring of specific variables while reducing data volume and
 * improving performance. Each channel can be individually controlled without
 * affecting other channels.
 * \par
 * \note This command must be used after mapping variables to
 * channels via \ref subsection_scope_var_map "SCOPE_VAR_MAP (0xF2)" command.<br>
 * Enabling a channel without a mapped variable will not produce meaningful data.
 *
 * \subsection subsection_scope_trig_ch SCOPE_TRIG_CH (0xF5)
 * \par
 * Selects which scope channel will be used as the trigger source for data capture.
 * This is a write-only, unpaged command that configures the trigger source channel.
 * \par
 * **Request Format:**
 * | Byte | Field | Description | Value Range |
 * |------|-------|-------------|-------------|
 * | 0 | Channel Number | Trigger source channel number | 1 to 4 |
 * \par
 * **Channel Number:** Specifies which scope channel will be used as the trigger
 * source. The trigger channel must be one of the enabled scope channels configured
 * via \ref subsection_scope_ch_ctrl "SCOPE_CH_CTRL (0xF4)" command.
 * \par
 * When the selected channel's signal meets the configured trigger conditions
 * ( \ref subsection_scope_trig_slope "slope", \ref subsection_scope_trig_level "level"), data capture will be initiated for all enabled channels.<br>
 * The trigger channel must have a mapped variable and be enabled for proper triggering operation.
 *
 * \subsection subsection_scope_trig_slope SCOPE_TRIG_SLOPE (0xF6)
 * \par
 * Configures the trigger slope condition for the selected trigger channel.
 * This command is write-only and unpaged.
 * \par
 * **Request Format:**
 * | Byte | Description | Value Range |
 * |------|-------------|---------|
 * | 0    | Slope setting | 0 / 1 |
 * \par
 * **Slope setting:** Determines the direction of signal transition that will
 * initiate the trigger condition when the signal crosses the trigger level.

 * \par
 * **Supported Slope Settings:**
 * - **0x00**: Negative slope - trigger on falling edge
 * - **0x01**: Positive slope - trigger on rising edge
 *
 * \subsection subsection_scope_trig_pos SCOPE_TRIG_POS (0xF7)
 * \par
 * Sets the trigger position within the capture buffer by specifying the number of
 * samples to be captured before the trigger event. This command is write-only and unpaged.
 * \par
 * **Request Format:**
 * | Byte | Description | Format | Value Range |
 * |------|-------------|---------|-------------|
 * | 0-3  | Pre-trigger sample count | 32-bit unsigned integer | 0 to \ref subsection_read_scope_spch_cnt "SPCH" |
 * **SPCH** "Samples Per Channel" - the total number of samples that can be captured per channel.
 * \par
 * **Pre-trigger Sample Count:** This position divides data buffer into pre-triggered and post-triggered parts, allowing
 * investigation of signal behavior before the trigger event. Pre-trigger data enables
 * analysis of signal conditions leading up to the trigger event, which is essential
 * for debugging and understanding system behavior.
 * \par
 * **Position Values:**
 * - **0x00000000**: Trigger at start of buffer (all 1000 samples captured after trigger)
 * - **0x000001F4**: Trigger at middle of buffer (500 pre-trigger + 500 post-trigger samples)
 * - **0x000003E7**: Trigger at end of buffer (999 pre-trigger + 1 post-trigger sample)
 *
 * \subsection subsection_scope_trig_level SCOPE_TRIG_LEVEL (0xF8)
 * \par
 * Configures the trigger level threshold for the selected trigger channel.
 * When the trigger channel signal crosses this level with the configured
 * \ref subsection_scope_trig_slope "slope" condition, triggering occurs.
 * This command is write-only, unpaged, and uses block data format.
 * \par
 * **Request Format:**
 * | Byte | Field | Description | Value Range |
 * |------|-------|-------------|-------------|
 * | 0-3  | Trigger Level | 4-byte value | Variable dependent |
 * | 4    | Channel Number | Target trigger channel | 1 to 4 |
 * \par
 * **Trigger Level:** The 4-byte field carries the trigger threshold value in the same
 * data type as the variable mapped to the trigger channel via \ref subsection_scope_var_map "SCOPE_VAR_MAP (0xF2)".
 * Any numeric data type with a size of 4 bytes or less, including floating-point types, can be transferred.
 * The bytes are placed in **little-endian order** (LSB first).
 * \par
 * **Channel Number:** Specifies which channel's trigger level to configure.
 * Any available channel can be configured independently of the current trigger channel selection.
 *
 * \note Only the trigger level of the channel selected via
 * \ref subsection_scope_trig_ch "SCOPE_TRIG_CH (0xF5)" will be used as the active trigger threshold.
 * \par
 * **Usage Example:**
 * - **Write [0x9A99193F01]** (Bytes 0-3 = `0x9A`, `0x99`, `0x19`, `0x3F`, Byte 4 = 0x01):
 *   Set trigger level to 0.6f (IEEE 754: `0x3F19999A`) on channel 1
 * - **Write [0x0000003F03]** (Bytes 0-3 = `0x00`, `0x00`, `0x00`, `0x3F`, Byte 4 = 0x03):
 *   Set trigger level to 0.5f (IEEE 754: `0x3F000000`) on channel 3
 *
 * \subsection subsection_scope_samp_clk_div SCOPE_SAMP_CLK_DIV (0xF9)
 * \par
 * Sets the sampling clock divider to control the data capture rate. This command
 * is write-only and unpaged.
 * \par
 * **Request Format:**
 * | Byte | Description | Format | Value Range |
 * |------|-------------|---------|-------------|
 * | 0-1  | Clock divider value | 16-bit unsigned integer | 1 to 65535 |
 * \par
 * **Divider Calculation:** Divider value = \ref subsection_read_scope_samp_clk "Base clock frequency" / Desired sampling rate
 * \par
 * Higher divider values result in slower sampling rates but longer capture duration
 * for the same buffer size. Lower divider values provide higher time resolution
 * but shorter total capture time.
 *
 * \subsection subsection_read_scope_samp_clk READ_SCOPE_SAMP_CLK (0xFA)
 * \par
 * Returns the base sampling clock frequency being used for data capture.
 * This is a read-only, unpaged command that provides the base clock frequency.
 * \par
 * **Response Format:**
 * | Byte | Description | Format |
 * |------|-------------|---------|
 * | 0-3  | Base clock frequency | 32-bit unsigned integer (Hz) |
 * \par
 * This information is useful for determining the time base of captured samples
 * and calculating actual timing relationships in the captured data. The actual
 * sampling rate is calculated as: Base clock frequency / \ref subsection_scope_samp_clk_div "Divider value".
 *
 * \subsection subsection_read_scope_spch_cnt READ_SCOPE_SPCH_CNT (0xFB)
 * \par
 * Returns the maximum number of samples per channel (SPCH) that can be captured
 * by the scope buffer. This is a read-only, unpaged command that provides the
 * static buffer size configuration.
 * \par
 * **Response Format:**
 * | Byte | Description | Format |
 * |------|-------------|--------|
 * | 0-1  | SPCH count  | 16-bit unsigned integer |
 * \par
 * **SPCH**: "Samples Per Channel" represents the buffer size used for data capture per scope channel. This is a fixed value determined
 * by the ProbeScope library configuration and does not change during operation.
 * \par
 * This value is essential for:
 * - Calculating valid ranges for \ref subsection_scope_trig_pos "SCOPE_TRIG_POS (0xF7)" command (0 to SPCH-1)
 * - Understanding total capture buffer capacity
 * - Calculating the block count needed to read all channel samples via \ref subsection_scope_ch_samp_buf "SCOPE_CH_SAMP_BUF (0xFC)" command
 *
 * \subsection subsection_scope_ch_samp_buf SCOPE_CH_SAMP_BUF (0xFC)
 * \par
 * Retrieves captured sample data from scope channel buffers using **Block Write-Block Read Process Call Protocol**.
 * This command requires writing channel number and block number, then returns the corresponding
 * sample data from the specified channel's capture buffer.
 * \par
 * **Protocol Details:**
 * - **Write Phase**: Send channel number and block number
 * - **Read Phase**: Receive sample data block with header information
 * - **Block size**: 254 bytes
 * \par
 * **Request Format:**
 * | Byte | Description | Value Range |
 * |------|-------------|-------------|
 * | 0 | Channel number | 1 to 4 |
 * | 1 | Block number | 0 to (**Total blocks per channel** - 1) |
 * \par
 * **Response Format:**
 * The response contains a block of sample data with header information. Each sample
 * is represented as a 32-bit floating point value (4 bytes) in the variable's native
 * format and units.
 * \par
 * | Bytes | Description |
 * |-------|-------------|
 * | 0 | Byte count in block |
 * | 1 | Channel number (echo) |
 * | 2 | Block number (echo) |
 * | 3-6 | Sample 1 (4 bytes) |
 * | 7-10 | Sample 2 (4 bytes) |
 * | ... | ... |
 * |250-254| Sample N (4 bytes) |
 * \par
 * **Block Count Calculation:**
 * - **Sample data size per channel** = \ref subsection_read_scope_spch_cnt "SPCH" × sizeof(float32_t) <br>
 * - **Sample data size per channel** = 1000 × 4 = 4000 bytes <br>
 * - **Sample bytes per block** = Block_Size - Header_Size = 254 - 2 = 252 bytes <br>
 * - **Total blocks per channel** = ceil( Sample_Data_Size / Sample_Bytes_Per_Block ) <br>
 * - **Total blocks per channel** = ceil( 4000 / 252 ) = 16 blocks <br>
 * \par
 * **Usage Example:**
 * - **Write [0x0001]** (Byte 0 = 0x01, Byte 1 = 0x00): Request channel 1, block 0 (samples 0-62, first 252 bytes)
 * - **Write [0x0101]** (Byte 0 = 0x01, Byte 1 = 0x01): Request channel 1, block 1 (samples 63-125, next 252 bytes)
 * - **Write [0x0F01]** (Byte 0 = 0x01, Byte 1 = 0x0F): Request channel 1, block 15 (final 148 bytes + padding)
 * \par
 * **Data Retrieval Process:**
 * 1. Read \ref subsection_read_scope_spch_cnt "READ_SCOPE_SPCH_CNT (0xFB)" to determine actual sample count
 * 2. Calculate **Total blocks per channel** count using the formula above
 * 3. Ensure \ref subsection_scope_data_rdy "SCOPE_DATA_RDY (0xEF)" indicates data is available
 * 4. Retrieve sample data by requesting each block sequentially
 * 5. Repeat action 4 for each enabled channel as needed
 * 6. Reset data ready flag via \ref subsection_scope_data_rdy "SCOPE_DATA_RDY (0xEF)" to initiate new capture
 * 7. Repeat actions from 3 to 6 for subsequent data captures
 * \par
 * \note If data is not ready, the PMBus middleware will automatically send a NACK response to this command, preventing invalid data access.<br>
 * It is recommended to verify data readiness before attempting to read sample buffers via \ref subsection_scope_data_rdy "SCOPE_DATA_RDY (0xEF)" request.
 *
 * \}
 */


#ifndef PMBUS_H_
#define PMBUS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*  INCLUDES
*******************************************************************************/
#include "mtb_pmbus.h"

/*******************************************************************************
*  MACROS & DEFINES
*******************************************************************************/
#define PMBUS_DEVICE_ADDRESS    (88U)
#define PMBUS_TOTAL_NUM_PAGES   (2U)

/*******************************************************************************
*  TYPEDEFS & STRUCTS
*******************************************************************************/


/*******************************************************************************
*  EXPORTED VARIABLES
*******************************************************************************/
extern mtb_pmbus_stc_config_hw_t pmbus_hw_cfg;
extern mtb_pmbus_stc_t pmbus_inst;

/*******************************************************************************
*  EXPORTED FUNCTIONS
*******************************************************************************/
void PMBUS_HW_Init(void);
void PMBUS_Start(void);


#ifdef __cplusplus
}
#endif

#endif /* PMBUS_H_ */
