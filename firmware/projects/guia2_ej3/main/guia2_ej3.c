/*! @mainpage guia2_ej3
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 01/10/2025 | Document creation		                         |
 *
 * @author Gastón Díaz (diazgastonj@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "timer_mcu.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_SWITCH_READ_PERIOD 100
#define CONFIG_MEASUREMENT_PERIOD 1000
#define CONFIG_BLINK_PERIOD_LED_3 500

#define CONFIG_MEASUREMENT_PERIOD_US 1000 * 1000
/*==================[internal data definition]===============================*/
TaskHandle_t Medir_task_handle = NULL;
TaskHandle_t EncenderLEDs_task_handle = NULL;
TaskHandle_t ControlLCD_task_handle = NULL;

uint8_t _tecla;
bool _medicionActivada = true;
bool _hold = false;
uint16_t _distanciaCentimetros = 0;
/*==================[internal functions declaration]=========================*/
static void Medir(void *pvParameter)
{
    while (true)
    {

        if (_medicionActivada)
            _distanciaCentimetros = HcSr04ReadDistanceInCentimeters();

       // vTaskDelay(CONFIG_MEASUREMENT_PERIOD / portTICK_PERIOD_MS);
	   ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

static void EncenderLEDs(void *pvParameter)
{
    while (true)
    {
        if (_medicionActivada)
        {
            if (_distanciaCentimetros < 10)
            {
                LedOff(LED_1);
                LedOff(LED_2);
                LedOff(LED_3);
            }
            else if (_distanciaCentimetros < 20)
            {
                LedOn(LED_1);
                LedOff(LED_2);
                LedOff(LED_3);
            }
            else if (_distanciaCentimetros < 30)
            {
                LedOn(LED_1);
                LedOn(LED_2);
                LedOff(LED_3);
            }
            else
            {
                LedOn(LED_1);
                LedOn(LED_2);
                LedOn(LED_3);
            }
        }
        else
        {
            LedOff(LED_1);
            LedOff(LED_2);
            LedOff(LED_3);
        }

        //vTaskDelay(CONFIG_MEASUREMENT_PERIOD / portTICK_PERIOD_MS);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}
static void ControlLCD(void *pvParameter)
{
    while (true)
    {
        if (!_medicionActivada)
        {
            LcdItsE0803Off();
        }
        else if (!_hold)
        {
            LcdItsE0803Write(_distanciaCentimetros);
        }
        //vTaskDelay(CONFIG_MEASUREMENT_PERIOD / portTICK_PERIOD_MS);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}
void Tecla1()
{
	_medicionActivada = !_medicionActivada;
}
void Tecla2()
{
	_hold = !_hold;
}
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(Medir_task_handle, pdFALSE);   
	vTaskNotifyGiveFromISR(EncenderLEDs_task_handle, pdFALSE);   
	vTaskNotifyGiveFromISR(ControlLCD_task_handle, pdFALSE);   
}
/*==================[external functions definition]==========================*/
void app_main(void)
{
	    timer_config_t timer_medicion = {
        .timer = TIMER_A,
        .period = CONFIG_MEASUREMENT_PERIOD_US,
        .func_p = FuncTimerA,
        .param_p = NULL
    };

    LedsInit();
    HcSr04Init(GPIO_3, GPIO_2);
    SwitchesInit();
	SwitchActivInt(SWITCH_1, Tecla1, NULL);
	SwitchActivInt(SWITCH_2, Tecla2, NULL);
    LcdItsE0803Init();

	TimerInit(&timer_medicion);
	
    xTaskCreate(&Medir, "Medir", 512, NULL, 5, &Medir_task_handle);
    xTaskCreate(&EncenderLEDs, "EncenderLEDs", 512, NULL, 5, &EncenderLEDs_task_handle);
    xTaskCreate(&ControlLCD, "ControlLCD", 512, NULL, 5, &ControlLCD_task_handle);


	TimerStart(timer_medicion.timer);
}

/*==================[end of file]============================================*/