/*! @mainpage guia2_ej4
 *
 * @section Proyecto creación de osciloscopio
 *
 * Originalmente pensado usando Serial Plotter pero cambiamos a
 * Serial Osciloscope.
 *
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
 * | 08/10/2025 | Document creation		                         |
 *
 * @author Gastón Díaz (diazgastonj@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "analog_io_mcu.h"
#include "uart_mcu.h"
#include "timer_mcu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*==================[macros and definitions]=================================*/
#define CONFIG_SAMPLING_PERIOD_US 2000 // 2 ms -> 500Hz
/*==================[internal data definition]===============================*/
TaskHandle_t GenerarDatosEnvio_task_handle = NULL;
/*==================[internal functions declaration]=========================*/
static void GenerarDatosEnvio(void *pvParameter)
{
	uint16_t valor;
	while (true)
	{
		AnalogInputReadSingle(CH1, &valor);
		// UartSendString(UART_PC, ">data:");
		UartSendString(UART_PC, (char *)UartItoa(valor, 10));
		UartSendString(UART_PC, "\r\n");

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}
}
void TimerFunc(void *parm)
{
	vTaskNotifyGiveFromISR(GenerarDatosEnvio_task_handle, pdFALSE);
}

/*==================[external functions definition]==========================*/
void app_main(void)
{

	timer_config_t timer_medicion = {
		.timer = TIMER_A,
		.period = CONFIG_SAMPLING_PERIOD_US,
		.func_p = TimerFunc,
		.param_p = NULL};

	serial_config_t my_uart = {
		.port = UART_PC,
		.baud_rate = 115200,
		.func_p = NULL,
		.param_p = NULL};
		
	analog_input_config_t analog_io_config = {
		.input = CH1,
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = 0};

	TimerInit(&timer_medicion);
	UartInit(&my_uart);
	AnalogInputInit(&analog_io_config);

	xTaskCreate(&GenerarDatosEnvio, "ControlLCD", 512, NULL, 5, &GenerarDatosEnvio_task_handle);

	TimerStart(timer_medicion.timer);
}
/*==================[end of file]============================================*/