/*=============================================================================
 * Date: 2021/07/08
 * Version: v1.0
 *===========================================================================*/

#ifndef MAIN_H_
#define MAIN_H_

/*==================[inclusiones]============================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOSConfig.h"
#include "qmpool.h"

/*==================[definiciones y macros]==================================*/
#define RATE 					1000
#define PRINT_RATE_MS 			500
#define MAX_RATE 				pdMS_TO_TICKS(RATE)
#define PRINT_RATE 				pdMS_TO_TICKS(PRINT_RATE_MS)

#define MALLOC_ERROR 			"Malloc Failed!\n"
#define MSG_ERROR_QUE 			"Queue error.\r\n"
#define MSG_ERROR_MTX 			"Mutex error.\r\n"

#define N_QUEUE 				4

#define OVERLOAD 	7
#define CRC8		3
#define SECUENCIA	4

/**
 * Defines para los 4 Pool de memoria elegidos, de 200, 100, 50 y 25 bytes
 */
#define PACKET_SIZE_200  		200							    // Tamanio del paquete
#define POOL_TOTAL_BLOCKS_200	4								// Cuantos paquetes
#define PACKET_SIZE_100	  		100							    // Tamanio del paquete
#define POOL_TOTAL_BLOCKS_100 	8								// Cuantos paquetes
#define PACKET_SIZE_50  		50							    // Tamanio del paquete
#define POOL_TOTAL_BLOCKS_50 	16								// Cuantos paquetes
#define PACKET_SIZE_25  		25							    // Tamanio del paquete
#define POOL_TOTAL_BLOCKS_25 	32								// Cuantos paquetes
#define POOL_SIZE 				(POOL_TOTAL_BLOCKS_200*PACKET_SIZE_200) //Todos los pools tienen el mismo tamaño

/**MAxima longitud del Frame*/

#define MAXLENGFRAME	200

typedef char *tPool;

/**
 * Estructura _sFrame, para mandar por cola a las distintas capas
 */
typedef struct {
	unsigned char secuencia[4];
	char *mensaje;
	QMPool *pool;
} _sFrame;

/*==================[declaraciones de funciones externas]====================*/

#endif /* MAIN_H_ */
