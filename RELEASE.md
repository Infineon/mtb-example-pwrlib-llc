# ModusToolbox&trade; Power Suite Full-Bridge LLC Reference Code Example 1.0.0 Release Notes

For a complete description refer to the [README.md](./README.md) and the [Reference Manual](https://infineon.github.io/mtb-example-pwrlib-llc/html/index.html)

## What's Included?

This asset release inludes the ModusToolbox&trade; Power Suite Full-Bridge LLC Reference Code Example

This is a pre-production preliminary asset release.
The documentation is not mature and comprehensive.
This is not recommended for production use, this is for demonstrational purposes only.

To view the full list of changes, refer to [Changelog section](https://infineon.github.io/mtb-example-pwrlib-llc/html/index.html#autotoc_md17).

## Defect Fixes

Preliminary release

## Known Issues and Limitations

- The default Device Configurator resources configuration has two information messages related to the configuration of pins 7.0 and 7.1.
  The KIT_PSC3_LLC1 does not have pull-up resistors on the mikroBUS I2C that is being used for the PMBus connection; therefore, the MCU's pull-ups are used.
  

## Supported Software and Tools

This version of the Full-Bridge LLC Reference Code Example was validated for the compatibility with the following Software and Tools:

| Software and Tools                                      | Version |
| :---                                                    | :----:  |
| ModusToolbox&trade; Software Environment                | 3.8.0   |
| ModusToolbox&trade; Power Suite GUI                     | 1.1.0   |
| TARGET_KIT_PSC3M5_CC1                                   | 3.2.0   |
| mtb-pwrlib-llc                                          | 1.0.0   |
| mtb-pdl-cat1                                            | 3.23.0  |
| mtb-hal-psc3                                            | 1.2.0   |
| mtb-pmbus                                               | 1.0.0   |
| mtb-oscilloscope                                        | 0.5.0   |
| emeeprom                                                | 2.60.0  |
| retarget-io                                             | 1.8.1   |
| GCC Compiler                                            | 14.2.1  |

## More information

For more information, refer to the following documents:

* [ModusToolbox&trade; Power Suite Full-Bridge LLC Reference Code Example README.md](./README.md)
* [ModusToolbox&trade; Power Suite Full-Bridge LLC Reference Code Example Reference Manual](https://infineon.github.io/mtb-example-pwrlib-llc/html/index.html)
* [ModusToolbox&trade; Power Suite LLC Middleware](https://github.com/Infineon/mtb-pwrlib-llc)
* [ModusToolbox&trade; Software Environment, Quick Start Guide, Documentation, and Videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/)
* [ModusToolbox&trade; Device Configurator Tool Guide](https://www.infineon.com/dgdl/Infineon-ModusToolbox_Device_Configurator_Guide_4-UserManual-v01_00-EN.pdf?fileId=8ac78c8c7d718a49017d99ab297631cb)
* [ModusToolbox&trade; Power Suite GUI](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxpowersuite) 
* [Infineon](http://www.infineon.com)

---
&copy; 2026, Infineon Technologies AG, or an affiliate of Infineon Technologies AG.
