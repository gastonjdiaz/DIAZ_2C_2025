/*! @mainpage Template
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
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
int8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number)
{
	for (uint8_t i = digits - 1; i >= 0 && data > 0; i--)
	{
		bcd_number[i] = data % 10;
		data = data / 10;
	}

	return 0;
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	uint32_t numero = 517;
	uint8_t digitos = 3;
	uint8_t arreglo[digitos];
	int8_t error = -1;

	error = convertToBcdArray(numero, digitos, arreglo);

	printf("%d ", error);
	for (size_t i = 0; i < digitos; i++)
	{
		printf("%d", arreglo[i]);
	}
	

}
/*==================[end of file]============================================*/