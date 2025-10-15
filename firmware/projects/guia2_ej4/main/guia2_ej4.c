/*! @mainpage guia2_ej4
 *
 * @section Proyecto creación de osciloscopio
 *
 * Originalmente pensado usando Serial Plotter pero cambiamos a
 * Serial Osciloscope.
 * 
 * Grafica un ECG cargado en memoria a través del conversor de DA, que es enviado
 * al canal 0 el cual es conectado al canal 1 usado para leer mediante el conversor 
 * AD y graficado por puerto serie.
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
TaskHandle_t EnviarDatoECG_task_handle = NULL;

unsigned char ECG[] = {
17,17,17,17,17,17,17,17,17,17,17,18,18,18,17,17,17,17,17,17,17,18,18,18,18,18,18,18,17,17,16,16,16,16,17,17,18,18,18,17,17,17,17,
18,18,19,21,22,24,25,26,27,28,29,31,32,33,34,34,35,37,38,37,34,29,24,19,15,14,15,16,17,17,17,16,15,14,13,13,13,13,13,13,13,12,12,
10,6,2,3,15,43,88,145,199,237,252,242,211,167,117,70,35,16,14,22,32,38,37,32,27,24,24,26,27,28,28,27,28,28,30,31,31,31,32,33,34,36,
38,39,40,41,42,43,45,47,49,51,53,55,57,60,62,65,68,71,75,79,83,87,92,97,101,106,111,116,121,125,129,133,136,138,139,140,140,139,137,
133,129,123,117,109,101,92,84,77,70,64,58,52,47,42,39,36,34,31,30,28,27,26,25,25,25,25,25,25,25,25,24,24,24,24,25,25,25,25,25,25,25,
24,24,24,24,24,24,24,24,23,23,22,22,21,21,21,20,20,20,20,20,19,19,18,18,18,19,19,19,19,18,17,17,18,18,18,18,18,18,18,18,17,17,17,17,
17,17,17

} ;
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
static void EnviarDatoECG(void *pvParameter)
{
	while (true)
	{
		for (uint8_t i = 0; i < sizeof(ECG) ; i++)
		{
			AnalogOutputWrite(ECG[i]);
			ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		}
				
	}
}
void TimerFunc(void *parm)
{
	vTaskNotifyGiveFromISR(GenerarDatosEnvio_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(EnviarDatoECG_task_handle, pdFALSE);
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

	AnalogOutputInit();

	xTaskCreate(&GenerarDatosEnvio, "GenerarDatosEnvio", 512, NULL, 5, &GenerarDatosEnvio_task_handle);
	xTaskCreate(&EnviarDatoECG, "EnviarDatoECG", 512, NULL, 5, &EnviarDatoECG_task_handle);

	TimerStart(timer_medicion.timer);
}
/*==================[end of file]============================================*/