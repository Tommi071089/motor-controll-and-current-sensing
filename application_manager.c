/*
 * File:   application_manager.c
 * Author: M67252
 *
 * Created on December 29, 2022, 3:55 PM
 */
#include <xc.h>
#include"application_manager.h"

/*Variable declaration*/
uint16_t u16poti = 0;
uint16_t u16batAdc = 0;
uint16_t u16currAdc = 0;
double dblVolatge = 0;
double dblBatVoltage = 0;
double dblCurrent = 0;
double dblNewPoti = 0;
double dblAmpVoltage = 0;

uint16_t u16timer1Flag = 0;
uint16_t u16timing = 0;
bool bTtimeoutFlag = false;
double newADC;

/*timer1 Flag functions every 1s*/
void TMR1_CallBack(void) {
    printData();
}

/*battery Management function*/
void batteryMeasurement(void) {
    if (u16batAdc >= BATTERY_FULL) {
        LED5_SetHigh();
        LED4_SetHigh();
        LED3_SetHigh();
        LED2_SetHigh();
        LED1_SetHigh();

    } else if (u16batAdc <= BATTERY_FULL && u16batAdc > BATTERY_HALF_FULL) {
        LED5_SetLow();
        LED4_SetHigh();
        LED3_SetHigh();
        LED2_SetHigh();
        LED1_SetHigh();

    } else if (u16batAdc <= BATTERY_HALF_FULL && u16batAdc > BATTERY_ALMOST_EMPTY) {
        LED5_SetLow();
        LED4_SetLow();
        LED3_SetHigh();
        LED2_SetHigh();
        LED1_SetHigh();

    } else if (u16batAdc <= BATTERY_ALMOST_EMPTY && u16batAdc > BATTERY_EMPTY) {
        LED5_SetLow();
        LED4_SetLow();
        LED3_SetLow();
        LED2_SetHigh();
        LED1_SetHigh();

    } else if (u16batAdc <= BATTERY_EMPTY && u16batAdc > RECHARGE_BATTERY) {
        LED5_SetLow();
        LED4_SetLow();
        LED3_SetLow();
        LED2_SetLow();
        LED1_SetHigh();

    } else if (u16batAdc <= RECHARGE_BATTERY && u16batAdc > BATTERY_SHUTDOWN) {
        LED5_SetLow();
        LED4_SetLow();
        LED3_SetLow();
        LED2_SetLow();
        LED1_SetLow();
    }
}

double changeDutyCycle(double dutyCycle) {
    PG1DC = dutyCycle;
    PG1STATbits.UPDREQ = 1;
    return 1;
}

void app_task(void) {
    ADC1_SoftwareTriggerEnable();
    u16poti = ADCBUF23;
    newADC = (double) (ADC_TO_PWM_FACTOR * ADCBUF23);
    u16batAdc = ADCBUF22;
    u16currAdc = ADCBUF0;
    dblNewPoti = (double) u16poti;
    batteryMeasurement();
    changeDutyCycle(newADC);
}


void printData(void) {
    dblBatVoltage = (double) (u16batAdc * (MAX_VOLTAGE / ADC_MAX_COUNT)
            * VOLTAGE_DIVIDER_FACTOR);
    dblAmpVoltage = (double) (u16currAdc * MAX_VOLTAGE / ADC_MAX_COUNT);
    dblCurrent = (double) (dblAmpVoltage / SHUNT_RESISTOR) / (AMP_FACTOR);
    printf("ADC Percentage = %.2f%%,Battery Voltage = %.2f%c, "
            "Current = %.2f%c,AmpVoltage=%.2f%c\n\r",
            ADC_PERCENTAGE(dblNewPoti), dblBatVoltage, 'V',
            dblCurrent, 'A', dblAmpVoltage, 'V');


}
