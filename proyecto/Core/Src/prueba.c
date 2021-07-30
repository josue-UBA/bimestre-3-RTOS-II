/*
 * prueba.c
 *
 *  Created on: 30 jul. 2021
 *      Author: josue
 */

#include "main.h"
#include "prueba.h"
extern UART_HandleTypeDef huart2;

void prueba() {
	printf("esta es una prueba\n\r");

	uint8_t dataT[] = "hola\n\r";
	int tam=sizeof(dataT)/sizeof(dataT[0]);
	HAL_UART_Transmit(&huart2, dataT, tam, HAL_MAX_DELAY);
}
