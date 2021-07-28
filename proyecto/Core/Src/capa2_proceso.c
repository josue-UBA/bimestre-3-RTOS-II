/*=============================================================================
 * Date: 2021/07/08
 * Version: v1.0
 *===========================================================================*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "crc8.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "sapi.h"
#include "capa2_proceso.h"
#include "main.h"
#include "qmpool.h"
#include "crc8.h"

static _sObject object2;

static _sFrame frameCapa2;

extern QueueHandle_t queue_c2_to_c3;

extern QueueHandle_t queue_c3_to_c2;

extern QueueHandle_t queue_c2_to_c1;

void
tarea_procesoMsg_c2(void *taskParmPtr);

extern QMPool poolMem_200, poolMem_100, poolMem_50, poolMem_25;

static void
procesoFrame_c2();

void capa2_OnRx(void *noUsado) {

	if (object2.bufferIn == NULL) {
		object2.bufferIn = (tPool) QMPool_get(&poolMem_200, 0);
		if (object2.bufferIn == NULL) {
			uint8_t caracter = uartRxRead(object2.usedUart); //leo el caracter para desocupar el buffer
			object2.state = END;
		} else {
			object2.bufferIn[object2.indexWriteRx] = uartRxRead(object2.usedUart); //REcibo el caracter por única vez aquí
		}
	} else {
		object2.bufferIn[object2.indexWriteRx] = uartRxRead(object2.usedUart); //REcibo el caracter aquí si no es NULL el buffer
	}

	if (object2.bufferIn != NULL) {
		procesoFrame_c2();
	}

}

void init_capa2_proceso(uartMap_t uart) {

	BaseType_t res;

	/* Inicializar la UART_USB junto con las interrupciones de Tx y Rx */
	uartConfig(uart, 115200);

	// Seteo un callback al evento de recepcion y habilito su interrupcion
	uartCallbackSet(uart, UART_RECEIVE, capa2_OnRx, NULL);

	// Habilito todas las interrupciones de UART_USB
	uartInterrupt(uart, true);

	object2.bufferIn = (tPool) QMPool_get(&poolMem_200, 0);
	object2.indexWriteRx = 0;
	object2.state = AWAITING;
	object2.usedUart = uart;

	/* CREAR TAREAS DE LA CAPA 2*/

	res = xTaskCreate(tarea_procesoMsg_c2,     // Funcion de la tarea a ejecutar
			(const char*) "tarea_procesoMsg_c2", // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE * 2, 		// Cantidad de stack de la tarea
			0,                          			// Parametros de tarea
			tskIDLE_PRIORITY + 1,         			// Prioridad de la tarea
			0                         // Puntero a la tarea creada en el sistema
			);

	// Gestion de errores
	configASSERT(res == pdPASS);

}

void tarea_procesoMsg_c2(void *taskParmPtr) {

	// ---------- CONFIGURACIONES ------------------------------
	_sFrame msg;
	tPool auxmsg;
	uint8_t crc, pcrc[3], index;

	// ---------- REPETIR POR SIEMPRE --------------------------

	while (TRUE) {
		if ((xQueueReceive(queue_c3_to_c2, &msg, portMAX_DELAY)) == pdTRUE) {
			/* Pido memoria auxiliar para armar el paquete */
			auxmsg = (tPool) QMPool_get(msg.pool, 0);
			if (auxmsg != NULL) {
				/* agrego la secuancia en la memoria Auxiliar para poder calcular el CRC */
				for (uint8_t i = 0; msg.mensaje[i]; i++) {
					if (i < SECUENCIA) {
						auxmsg[i] = msg.secuencia[i];
						auxmsg[i + SECUENCIA] = msg.mensaje[i];
					} else {
						auxmsg[i + SECUENCIA] = msg.mensaje[i];
					}
				}
				/* Calculo CRC y con sprintf lo convierto en digitos ascii hexadeciamales*/
				crc = crc8_calc(0, auxmsg, (strlen(auxmsg)));
				sprintf(pcrc, "%X%X", (crc >> 4), (crc & 0x0F));
				/*vuelvo a armar el mensaje para encolarlo, agrego al final el CRC y el EOM*/
				index = 0;
				msg.mensaje[index] = SOM;
				while (auxmsg[index] != '\0') {
					msg.mensaje[index + 1] = auxmsg[index++];
				}
				msg.mensaje[index++] = pcrc[0];
				msg.mensaje[index++] = pcrc[1];
				msg.mensaje[index] = EOM;
				QMPool_put(msg.pool, auxmsg); //libero la memoria auxiliar
				xQueueSend(queue_c2_to_c1, &msg, 0); //Encolo el mensaje para capa 1
			} else {
				printf("Error al pedir Memoria en Capa 2 tarea_procesoMsg_c2 \n ");
			}

		}

		vTaskDelay(1); //espero 1 ms luego de enviar el FRAME y antes de leer de nuevo la cola.
	}
}

static void procesoFrame_c2() {

	uint8_t crc, aux[3], pcrc[3], valSec;

	switch (object2.state) {
	case AWAITING:
		if (object2.bufferIn[object2.indexWriteRx] == SOM) {
			object2.state = START;
			object2.indexWriteRx++;
		} else {
			object2.indexWriteRx = 0;
		}
		break;
	case START:
		if (object2.bufferIn[object2.indexWriteRx] == EOM) {
			object2.state = END;
			frameCapa2.mensaje = NULL;
			/* Pido memoria de acuardo al tamaño del FRAME*/
			if (object2.indexWriteRx < PACKET_SIZE_25) {
				frameCapa2.mensaje = (tPool) QMPool_get(&poolMem_25, 0);
				frameCapa2.pool = &poolMem_25;
			} else {
				if (object2.indexWriteRx < PACKET_SIZE_50) {
					frameCapa2.mensaje = (tPool) QMPool_get(&poolMem_50, 0);
					frameCapa2.pool = &poolMem_50;
				} else {
					if (object2.indexWriteRx < PACKET_SIZE_100) {
						frameCapa2.mensaje = (tPool) QMPool_get(&poolMem_100, 0);
						frameCapa2.pool = &poolMem_100;
					} else {
						frameCapa2.mensaje = (tPool) QMPool_get(&poolMem_200, 0);
						frameCapa2.pool = &poolMem_200;
					}
				}
			}
			/* Si conseguí memoria realizo las comprobaciones y encolo para capa 3*/
			if (frameCapa2.mensaje != NULL) {
				for (uint8_t i = 0; i < object2.indexWriteRx - CRC8; i++)
					frameCapa2.mensaje[i] = object2.bufferIn[i + 1];
				crc = crc8_calc(0, frameCapa2.mensaje, (object2.indexWriteRx - CRC8));
				sprintf(aux, "%c%c", (object2.bufferIn[object2.indexWriteRx - 2]), (object2.bufferIn[object2.indexWriteRx - 1]));
				sprintf(pcrc, "%X%X", (crc >> 4), (crc & 0x0F));
				/* Si el CRC coincide sigo adelante con el manejo del FRAME */
				if ((strncmp(pcrc, aux, 2)) == 0) {
					/* valido que la secuencia sea numerica, digitos del 0 al 9 y de paso la cargo en el _sFrame*/
					valSec = TRUE;
					for (uint8_t a = 0; a < SECUENCIA; a++) {
						if (frameCapa2.mensaje[a] < '0' || frameCapa2.mensaje[a] > '9') {
							valSec = FALSE;
						}
						frameCapa2.secuencia[a] = frameCapa2.mensaje[a];
					}
					/* Si la secuencia es valida, Encolo para capa3 y libero la memoria de recepción*/
					if (valSec) {
						/* quito la secuencia del mensaje antes de encolarlo para capa3*/
						for (uint8_t i = 0; i <= object2.indexWriteRx - OVERLOAD; i++) {
							if (i == (object2.indexWriteRx - OVERLOAD))
								frameCapa2.mensaje[i] = '\0';
							else
								frameCapa2.mensaje[i] = frameCapa2.mensaje[i + SECUENCIA];
						}
						xQueueSend(queue_c2_to_c3, &frameCapa2, 0);
						QMPool_put(&poolMem_200, object2.bufferIn);
						object2.bufferIn = NULL;
					} else {
						/* Si la secuencia no es valida, libero la memoria que acabo de pedir y reinicio el buffer de recepción y la FSM*/
						QMPool_put(frameCapa2.pool, frameCapa2.mensaje);
						object2.indexWriteRx = 0;
						object2.state = AWAITING;
					}
				} else {
					/* Si el CRC es no coincide, reinicio el buffer y la FSM*/
					object2.indexWriteRx = 0;
					object2.state = AWAITING;
				}
			}
		} else {
			if (object2.indexWriteRx >= (MAXLENGFRAME - 1)) {
				object2.indexWriteRx = 0;
				object2.state = AWAITING;
			} else {
				if (object2.bufferIn[object2.indexWriteRx] == SOM) {
					object2.bufferIn[0] = SOM;
					object2.indexWriteRx = 1;
				} else {
					object2.indexWriteRx++;
				}

			}
		}
		break;
	case END:
		object2.state = AWAITING;
		object2.indexWriteRx = 0; //reinicio el indice para recibir el nuevo frame.

		break;
	default:
		object2.state = AWAITING;

	}

}

