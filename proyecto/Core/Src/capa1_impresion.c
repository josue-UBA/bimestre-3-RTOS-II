/*=============================================================================
 * Date: 2021/07/08
 * Version: v1.0
 *===========================================================================*/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "sapi.h"
#include "capa1_impresion.h"
#include "main.h"

extern QueueHandle_t queue_c2_to_c1;

static uartMap_t currentUart;

void tarea_print_c1(void *taskParmPtr);

void init_capa1_impresion(uartMap_t uart) {

	BaseType_t res;
	currentUart = uart;

	// Creo tarea unica de impresion
	res = xTaskCreate(tarea_print_c1,          // Funcion de la tarea a ejecutar
			(const char*) "tarea_print_c1", // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE * 2, // Cantidad de stack de la tarea
			0,                          // Parametros de tarea
			tskIDLE_PRIORITY + 1,         // Prioridad de la tarea
			0                         // Puntero a la tarea creada en el sistema
			);

	// Gestion de errores
	configASSERT(res == pdPASS);

}

void tarea_print_c1(void *taskParmPtr) {
	// ---------- CONFIGURACIONES ------------------------------
	uint32_t index;
	_sFrame msg;

	// ---------- REPETIR POR SIEMPRE --------------------------

	while (TRUE) {
		if ((xQueueReceive(queue_c2_to_c1, &msg, portMAX_DELAY)) == pdTRUE) {
			index = 0;
			while (msg.mensaje[index] != '\0')
				uartTxWrite(currentUart, msg.mensaje[index]);
		}
		// Libero el bloque de memoria que ya fue trasmitido
		QMPool_put(msg.pool, msg.mensaje);
		msg.mensaje = NULL;
		vTaskDelay(1); //espero 1 ms luego de enviar el FRAME y antes de leer de nuevo la cola.
	}
}
