/*! @mainpage guia1_ej3
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
 * | 10/09/2025 | Document creation		                         |
 *
 * @author Gastón Díaz (diazgastonj@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/*==================[macros and definitions]=================================*/
#define OFF 0
#define ON 1
#define TOGGLE 2
#define ESPERA 100
/*==================[internal data definition]===============================*/
struct leds
{
      uint8_t mode;       //ON, OFF, TOGGLE
	uint8_t n_led;        //indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de encendido/apagado
	uint16_t periodo;    //indica el tiempo de cada ciclo
} my_leds;
/*==================[internal functions declaration]=========================*/

void ledControlFunction(struct leds *str )
{
	
	if (str->mode == ON)
	{	
		/*se podría hacer de esta manera sin el if, igual con el off y toggle, 
			pero se lo dejo para seguir el diagrama de flujo*/
			//LedOn(str->n_led); 
		if (str->n_led == 1)			
			LedOn(1);
		if (str->n_led == 2)			
			LedOn(2);
		if (str->n_led == 3)			
			LedOn(3);
	}
	else if (str->mode == OFF)
	{	
		if (str->n_led == 1)			
			LedOff(1);
		if (str->n_led == 2)			
			LedOff(2);
		if (str->n_led == 3)			
			LedOff(3);
	}
	else if (str->mode == TOGGLE)	
	{	
		for (uint8_t i = 0; i < str->n_ciclos; i++)
		{
			if (str->n_led == 1)			
				LedToggle(1);
			if (str->n_led == 2)			
				LedToggle(2);
			if (str->n_led == 3)			
				LedToggle(3);
			
				//no vTaskDelay(periodo)???
			for (uint8_t j = 0; j < str->periodo/ESPERA; j++)
			{
				vTaskDelay(ESPERA/portTICK_PERIOD_MS);
			}
		}		
	}

	
	

}

/*==================[external functions definition]==========================*/
void app_main(void){
	
	LedsInit();
	struct leds config;

	config.mode = TOGGLE;
	config.n_led = 2;
	config.periodo = 500;
	config.n_ciclos = 15;

	ledControlFunction(&config);
}
/*==================[end of file]============================================*/