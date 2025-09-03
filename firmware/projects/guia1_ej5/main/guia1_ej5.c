/*! @mainpage guia1_ej5
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
 * | 03/09/2025 | Document creation		                         |
 *
 * @author Gaston Diaz (diazgastonj@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>

#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;
/*==================[internal functions declaration]=========================*/
void mostrarDigito(uint8_t digito, gpioConf_t *arreglo)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		if (digito & 1 << i)
		{
			GPIOOn(arreglo[i].pin);
		}
		else
		{	
			GPIOOff(arreglo[i].pin);
		}
		
	}
}
/*==================[external functions definition]==========================*/
void app_main(void){

	gpioConf_t vector[4] = {
		{GPIO_20, GPIO_OUTPUT},
		{GPIO_21, GPIO_OUTPUT},
		{GPIO_22, GPIO_OUTPUT},
		{GPIO_23, GPIO_OUTPUT}
	};

	for (uint8_t i = 0; i < 4; i++)
	{
		GPIOInit(vector[i].pin, vector[i].dir);
	}

	mostrarDigito(7, vector);	
}
/*==================[end of file]============================================*/