/*! @mainpage examen
 *
 * @section genDesc General Description
 *
 * Proyecto de evaluación final de materia Electrónica Programable fecha 02 Dic 2025
 * 
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	SPEAKER+ 	| 	GPIO_3		|
 * | 	SPEAKER GND	| 	GND		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 02/12/2025 | Document creation		                         |
 *
 * @author Díaz, Gastón Jair (diazgastonj@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "uart_mcu.h"
#include "hc_sr04.h"
#include "gpio_mcu.h"
#include "timer_mcu.h"
/*==================[macros and definitions]=================================*/

#define CONFIG_MEASUREMENT_CYCLE_MS 500 ///<  Tiempo de perído de medición distancia en milisegundos
#define CONFIG_ACELEROMETER_MEASUREMENT_CYCLE_US 1000 * 10 ///<  Tiempo de perído de medición acelerómetro en microsegundos
#define CONFIG_CICLO_ALARMA_ESTADO_PRECAUCION_MS 1000 ///<  Período de alarma en estado de precaución en milisegundos
#define CONFIG_CICLO_ALARMA_ESTADO_PELIGRO_MS 500 ///<  Período de alarma en estado de peligro en milisegundos
#define DISTANCIA_PRECAUCION 500  ///<  Distancia en cm para precaución
#define DISTANCIA_PELIGRO 300  ///<  Distancia en cm para peligro
#define ESTADO_PRECACION 2 ///<  Variable que define el estado de precaución
#define ESTADO_PELIGRO 1   ///<  Variable que define el estado de peligro
#define ESTADO_NORMAL 0   ///<  Variable que define el estado normal

#define LED_ROJO LED_3    ///<  Definición led rojo
#define LED_AMARILLO LED_2///<  Definición led amarillo
#define LED_VERDE LED_1   ///<  Definición led verde
#define GPIO_ALARMA GPIO_3 ///<  Definición pin de alarma sonora
/*==================[internal data definition]===============================*/

uint16_t _distancia;
uint8_t _estadoActual = ESTADO_NORMAL;

TaskHandle_t medir_distancia_task_handle = NULL;
TaskHandle_t alarma_sonora_task_handle = NULL;

/*==================[internal functions declaration]=========================*/
/** @fn bool AcelerometerInit()
 * @brief Función que inicializa los pines de selección del driver
 * @param[in] gSelect1
 * @return 1 (true) if no error
 */
bool AcelerometerInit();
/** @fn float ReadXValue()
 * @brief Función que lee el pin x del driver y devuelve el valor convertido de analógico a digital en unidades de gravedad.
 * @param[in] No hay parámetros
 * @return Aceleración en el eje x
 */
float ReadXValue(){
	return 0.3f; //mockup
}
/** @fn float ReadYValue()
 * @brief Función que lee el pin y del driver y devuelve el valor convertido de analógico a digital en unidades de gravedad.
 * @param[in] No hay parámetros
 * @return Aceleración en el eje y
 */
float ReadYValue(){
	return 0.5f;//mockup
}
/** @fn float ReadZValue()
 * @brief Función que lee el pin z del driver y devuelve el valor convertido de analógico a digital en unidades de gravedad.
 * @param[in] No hay parámetros
 * @return Aceleración en el eje z
 */
float ReadZValue(){
	return 0.9f;//mockup
}
/** @fn void Controlar_leds()
 * @brief Función que controla el estado de los leds según la distancia medida.
 * @param[in] No hay parámetros
 */
void Controlar_leds()
{
	if (_estadoActual == ESTADO_PELIGRO) 
	{
		LedOn(LED_ROJO);
		LedOn(LED_AMARILLO);
		LedOn(LED_VERDE);

	}
	else if (_estadoActual == ESTADO_PRECACION)
	{
		LedOff(LED_ROJO);
		LedOn(LED_AMARILLO);
		LedOn(LED_VERDE);
	}
	else if (_estadoActual == ESTADO_NORMAL)	
	{
		LedOn(LED_VERDE);
		LedOff(LED_AMARILLO);
		LedOff(LED_ROJO);
	}
}	
/** @fn void Enviar_advertencia()
 * @brief Función que envía advertencia dependiendo del estado (peligro o precaucion).
 * @param[in] No hay parámetros
 */
void Enviar_advertencia()
{

	if (_estadoActual == ESTADO_PELIGRO)
		UartSendString(UART_CONNECTOR, "Peligro, vehículo cerca\r\n");
	else if (_estadoActual == ESTADO_PRECACION)
		UartSendString(UART_CONNECTOR, "Precaución, vehículo cercano\r\n");


}
/** @fn void RegularEstadp()
 * @brief Función que envía advertencia si cambia el estado y este lo amerita.
 * @param[in] No hay parámetros
 */
void RegularEstado()
{
	if (_distancia < DISTANCIA_PELIGRO) 
	{
		if(_estadoActual != ESTADO_PELIGRO)
			Enviar_advertencia(); // enviar advertencia solo si cambia el estado
		_estadoActual = ESTADO_PELIGRO;

	}
	else if (_distancia <= DISTANCIA_PRECAUCION)
	{
		if(_estadoActual != ESTADO_PRECACION)
			Enviar_advertencia(); 
		_estadoActual = ESTADO_PRECACION;
	}
	else	
	{
		_estadoActual = ESTADO_NORMAL;
	}

}
/** @fn void Medir_distancia()
 * @brief Función que mide la distancia utilizando el sensor ultrasónico.
 * @param[in] No hay parámetros
 */
void Medir_distancia()
{
	while (true)
	{
		_distancia = HcSr04ReadDistanceInCentimeters(); // Realiza la medición de distancia
		RegularEstado();
		Controlar_leds();
		vTaskDelay(pdMS_TO_TICKS(CONFIG_MEASUREMENT_CYCLE_MS));
	}
}
/** @fn void Medir_acelerometro()
 * @brief Función que mide el valor del acelerómetro y detecta caídas.
 * @param[in] No hay parámetros
 */
void Medir_acelerometro()
{
	//0.3v/G -> 4G = 1.2v
	float x = fabsf(ReadXValue());
	float y = fabsf(ReadYValue());
	float z = fabs(ReadZValue()); //tomo valores absolutos para detectar sin importar la dirección

	if (x + y + z > 1.2f)
	{
		UartSendString(UART_CONNECTOR, "Caída detectada\r\n");
	}
}
/** @fn void Alarma_sonora()
 * @brief Función que controla la alarma sonora según el estado actual.
 * @param[in] No hay parámetros
 */
void Alarma_sonora()
{
	while (true)
	{
		if (_estadoActual == ESTADO_PELIGRO) 
		{
			GPIOOn(GPIO_ALARMA);
			vTaskDelay(pdMS_TO_TICKS(CONFIG_CICLO_ALARMA_ESTADO_PELIGRO_MS));
			GPIOOff(GPIO_ALARMA);	
			vTaskDelay(pdMS_TO_TICKS(CONFIG_CICLO_ALARMA_ESTADO_PELIGRO_MS));
		}
		else if (_estadoActual == ESTADO_PRECACION)
		{ 
			GPIOOn(GPIO_ALARMA);
			vTaskDelay(pdMS_TO_TICKS(CONFIG_CICLO_ALARMA_ESTADO_PRECAUCION_MS));
			GPIOOff(GPIO_ALARMA);	
			vTaskDelay(pdMS_TO_TICKS(CONFIG_CICLO_ALARMA_ESTADO_PRECAUCION_MS));
		}		
		else
		{
			GPIOOff(GPIO_ALARMA); //failsafe por si acaso	
			vTaskDelay(pdMS_TO_TICKS(100)); //espera un tiempo para no saturar el CPU
		}	
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){

	serial_config_t UARTMensaje;
	/*
	Baud rate calculado -> Texto más largo 32 caracteres, 2 veces por segundo (tasa medición) -> 64 caracteres por segundo
	 10 bits por cada uno (start, data, stop) -> 640 bits por segundo
	 Se elige el valor comercial inmediato mayor -> 1200 bps	
	*/
	UARTMensaje.baud_rate = 1200; 
	UARTMensaje.port = UART_CONNECTOR;
	HcSr04Init(GPIO_3/*echo*/, GPIO_2/*trigger*/);
	LedsInit();              
	UartInit(&UARTMensaje);   
	timer_config_t timer_senial = {
        .timer = TIMER_A,
        .period = CONFIG_ACELEROMETER_MEASUREMENT_CYCLE_US,
        .func_p = Medir_acelerometro,
        .param_p = NULL
    };
    TimerInit(&timer_senial);

	xTaskCreate(Medir_distancia, "MedirDistancia", 2048, NULL, 5, &medir_distancia_task_handle);
	xTaskCreate(Alarma_sonora, "AlarmSonora", 512, NULL, 5, &alarma_sonora_task_handle);


}
/*==================[end of file]============================================*/