/*! @mainpage guia2_ej1
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
 * | 12/09/2025 | Document creation		                         |
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
/*==================[macros and definitions]=================================*/
#define CONFIG_SWITCH_READ_PERIOD 100
#define CONFIG_BLINK_PERIOD_LED_2 1500
#define CONFIG_BLINK_PERIOD_LED_3 500
/*==================[internal data definition]===============================*/
TaskHandle_t led1_task_handle = NULL;
TaskHandle_t led2_task_handle = NULL;
TaskHandle_t led3_task_handle = NULL;

uint8_t _tecla;
bool _medicionActivada = true;
bool _hold = false;
/*==================[internal functions declaration]=========================*/
static void LeerTeclas(void *pvParameter){
	SwitchesInit();
    while(true)    {
    	_tecla  = SwitchesRead();
    	switch(_tecla){
    		case SWITCH_1:
    			_medicionActivada = !_medicionActivada;
    		break;
    		case SWITCH_2:
    			_hold = !_hold;
    		break;
    	}
		vTaskDelay(CONFIG_SWITCH_READ_PERIOD / portTICK_PERIOD_MS);
	}
}

static void Led2Task(void *pvParameter){
    while(true){
        printf("LED_2 ON\n");
        LedOn(LED_2);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_2 / portTICK_PERIOD_MS);
        printf("LED_2 OFF\n");
        LedOff(LED_2);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_2 / portTICK_PERIOD_MS);
    }
}

static void Led3Task(void *pvParameter){
    while(true){
        printf("LED_3 ON\n");
        LedOn(LED_3);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_3 / portTICK_PERIOD_MS);
        printf("LED_3 OFF\n");
        LedOff(LED_3);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_3 / portTICK_PERIOD_MS);
    }
}
/*==================[external functions definition]==========================*/
void app_main(void){
    LedsInit();
    //xTaskCreate(&Led1Task, "LED_1", 512, NULL, 5, &led1_task_handle);
    //xTaskCreate(&Led2Task, "LED_2", 512, NULL, 5, &led2_task_handle);
    //xTaskCreate(&Led3Task, "LED_3", 512, NULL, 5, &led3_task_handle);
}

/*==================[end of file]============================================*/