/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOSConfig.h"
#include "qmpool.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef char *tPool;

/**
 * Estructura _sFrame, para mandar por cola a las distintas capas
 */
typedef struct {
	unsigned char secuencia[4];
	char *mensaje;
	QMPool *pool;
} _sFrame;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
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

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
