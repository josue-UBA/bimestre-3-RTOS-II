/*=============================================================================
 * Date: 2021/07/08
 * Version: v1.0
 *===========================================================================*/

#ifndef CAPA2_PROCESO_H_
#define CAPA2_PROCESO_H_
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "sapi.h"

#define SOM '('
#define EOM ')'

/**
 * Enumeración para la FSM que recibe y procesa los caracteres
 */
typedef enum {
	AWAITING, START, END
} _eFsmRx;

/**
 * Estructura para instanciar la capa 2, inicilizar la FSM y recibir los Caracteres.
 */
typedef struct {
	char *bufferIn;
	unsigned char indexWriteRx;
	_eFsmRx state;
	uartMap_t usedUart;
} _sObject;

void init_capa2_proceso(uartMap_t uart);

#endif /* CAPA2_PROCESO_H_ */
