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
#include "cy_em_eeprom.h"
#include "Controller.h"

enum
{   // ADC results
    ADC_VIN = 0U,         // Input voltage
    ADC_VOUT,             // Output voltage
    ADC_IOUT,             // Output current
    ADC_POT,              // Potentiometer
    ADC_TEMP,             // Temperature sensor

    ADC_MAX
};

typedef struct
{
    float32_t vin;      // [V/ticks]
    float32_t ires;     // [A/ticks]
    float32_t vres;     // [V/ticks]
    float32_t vout;     // [V/ticks]
    float32_t iout;     // [A/ticks]
    float32_t temp;     // [Celsius/ticks] for active IC, [1/ticks] for passive NTC
    float32_t pot;      // [%/ticks]
} MCU_ADC_SCALE_t;

typedef struct
{
    float32_t tcpwm;
    float32_t tcpwm_hi_res; // The high resolution virtual clock frequency to recalculation HRPWM period/compare/dead time values
} MCU_CLK_FRQ_t;  // [Hz]

typedef struct
{
    int32_t drv_h;  /**< [ticks] The primary driver PWM1 direct line timing rising edge timing adjustment, in counts, calculated from \ref LLC_TMG_DRV1_H */
    int32_t drv_l;  /**< [ticks] The primary driver PWM1 complementary line timing rising edge timing adjustment, in counts, calculated from \ref LLC_TMG_DRV1_L */
} MCU_TMG_t;  // [Hz]

typedef struct
{
    uint32_t period;       // [ticks]

    union
    {
        uint32_t dead_time;   // [ticks] dead time, for driver/SR PWMs
        float32_t tick_frac;  // [] the weight of a single tick in a full period as fraction of 1 (e.g. 1/period), for burst PWM short pulse protection
    };
} MCU_TCPWM_t;

typedef struct
{
    // Must use int32_t so overflow would be OK (using 32bit TCPWM)
    int32_t start;              // [ticks]
    int32_t stop;               // [ticks]
    int32_t duration_ticks;     // [ticks]

    float32_t duration_sec;         // [sec]
    float32_t util;                 // [%], utilization percentage

    float32_t sec_per_tick;         // [sec/tick], based on timer's frequency
    float32_t inv_max_time;         // [1/sec], inverse of max. time corresponding to 100% utilization
} MCU_TIME_CAP_t;

typedef struct
{
    IRQn_Type nvic_isr0;
    IRQn_Type nvic_isr1;
    uint32_t state;
} MCU_INT_t;  // interrupts

typedef struct
{
    bool init_done;
    cy_stc_eeprom_config_t config;
    cy_en_em_eeprom_status_t status;
    cy_stc_eeprom_context_t context;
} MCU_EEPROM_t;

typedef struct
{
    int32_t adc_results[ADC_MAX];
    MCU_ADC_SCALE_t adc_scale;
    MCU_CLK_FRQ_t clk;
    MCU_TCPWM_t drv;  // primary driver
    MCU_TCPWM_t rec;  // rectifier driver
    MCU_TMG_t tmg; // modulator timing correction
    MCU_TCPWM_t brst; // burst modulator
    MCU_TCPWM_t isr0; // fast isr timer
    MCU_TCPWM_t isr1; // slow isr timer
    MCU_TIME_CAP_t isr0_exe;
    MCU_TIME_CAP_t isr1_exe;
    MCU_INT_t interrupt;
    MCU_EEPROM_t eeprom;
} MCU_t;

// Initializations
void MCU_Init();
void MCU_InitChipInfo();
void MCU_InitADCs();
void MCU_InitInterrupts();
void MCU_InitTimers();
void MCU_InitProtections();

// Time-critical control interrupts
void MCU_RunISR0(); // Fast, highest priority 0
void MCU_RunISR1(); // Slow, second highest priority 1

// Start/stop LLC PWMs, ADCs, ISRs
void MCU_StartPeripherals();
void MCU_StopPeripherals();

// Enter/exit critical section
void MCU_EnterCriticalSection();
void MCU_ExitCriticalSection();

// Enter/exit high-z state (driver)
void MCU_GateDriverEnterHighZ();
void MCU_GateDriverExitHighZ();

// Enable/disable (synchronous rectification)
void MCU_SyncRecEn(bool en);

// Engage/release hardware fault reset
void MCU_FaultResetEngage();
void MCU_FaultResetRelease();

// Read/write flash
void MCU_FlashInit();
bool MCU_FlashReadParams(PARAMS_ID_t id, PARAMS_t* ram_data);
bool MCU_FlashWriteParams(PARAMS_t* ram_data);

// Handling execution time capture measurements
void MCU_StartTimeCap(MCU_TIME_CAP_t* time_cap);
void MCU_StopTimeCap(MCU_TIME_CAP_t* time_cap);
void MCU_ProcessTimeCapISR1(MCU_TIME_CAP_t* time_cap);

// Process temperature sensor results
#if (!ACTIVE_TEMP_SENSOR)
extern  const TEMP_SENS_LUT_t     Temp_Sens_LUT;
#endif
float32_t MCU_TempSensorCalc(float32_t adc_scaled);

void MCU_ReadADCResults(void); // Read ADC results
void MCU_ModUpd(void); // Modulator update
