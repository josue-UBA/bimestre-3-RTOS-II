/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
	estado_inicio, estado_parentesis, estado_almacena,
} estado;
struct Books {
   uint8_t paq[0];
   char  author[50];
   char  subject[100];
   int   book_id;
} book;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define TAM_PAQUETE 20
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* Definitions for capa_1 */
osThreadId_t capa_1Handle;
const osThreadAttr_t capa_1_attributes = { .name = "capa_1", .stack_size = 128
		* 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for capa_2 */
osThreadId_t capa_2Handle;
const osThreadAttr_t capa_2_attributes = { .name = "capa_2", .stack_size = 128
		* 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for capa_3 */
osThreadId_t capa_3Handle;
const osThreadAttr_t capa_3_attributes = { .name = "capa_3", .stack_size = 128
		* 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for cola_2_3 */
osMessageQueueId_t cola_2_3Handle;
const osMessageQueueAttr_t cola_2_3_attributes = { .name = "cola_2_3" };
/* Definitions for cola_3_2 */
osMessageQueueId_t cola_3_2Handle;
const osMessageQueueAttr_t cola_3_2_attributes = { .name = "cola_3_2" };
/* Definitions for cola_2_1 */
osMessageQueueId_t cola_2_1Handle;
const osMessageQueueAttr_t cola_2_1_attributes = { .name = "cola_2_1" };
/* USER CODE BEGIN PV */
uint8_t dataT[14] = "hola\n\r";
uint8_t dataR[1] = "";
uint8_t paquete[200] = "";
uint8_t paquete_vacio[200] = "";
estado estado_actual = estado_inicio;
int contador = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void funcion_capa_1(void *argument);
void funcion_capa_2(void *argument);
void funcion_capa_3(void *argument);

/* USER CODE BEGIN PFP */
void maquina_de_estados();
int valida_seq();
int valida_c();
int valida_crc();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Init scheduler */
	osKernelInitialize();

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* Create the queue(s) */
	/* creation of cola_2_3 */
	cola_2_3Handle = osMessageQueueNew(16, sizeof(uint16_t),
			&cola_2_3_attributes);

	/* creation of cola_3_2 */
	cola_3_2Handle = osMessageQueueNew(16, sizeof(uint16_t),
			&cola_3_2_attributes);

	/* creation of cola_2_1 */
	cola_2_1Handle = osMessageQueueNew(16, sizeof(uint16_t),
			&cola_2_1_attributes);

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* creation of capa_1 */
	capa_1Handle = osThreadNew(funcion_capa_1, NULL, &capa_1_attributes);

	/* creation of capa_2 */
	capa_2Handle = osThreadNew(funcion_capa_2, NULL, &capa_2_attributes);

	/* creation of capa_3 */
	capa_3Handle = osThreadNew(funcion_capa_3, NULL, &capa_3_attributes);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	/* USER CODE END RTOS_EVENTS */

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
int valida_seq() {
	/* retorna 0 si es que hay caraceteres. Retorna 1 si todo esta bien. */
	if (paquete[0] < '0' || '9' < paquete[0])
		return 0;
	if (paquete[1] < '0' || '9' < paquete[1])
		return 0;
	if (paquete[2] < '0' || '9' < paquete[2])
		return 0;
	if (paquete[3] < '0' || '9' < paquete[3])
		return 0;
	return 1; /* si no son numeros */
}
int valida_crc() {
	/* retorna 0 si es que hay caraceteres. Retorna 1 si todo esta bien. */
	if (paquete[contador - 1] < '0' || '9' < paquete[contador - 1])
		return 0;
	if (paquete[contador - 2] < '0' || '9' < paquete[contador - 2])
		return 0;
	else
		return 1; /* si no son numeros */
}
int valida_c() {
	if (paquete[4] == 'P' || paquete[4] == 'S' || paquete[4] == 'C')/* pascal */
		return 1;
	return 0; /* si no son numeros */
}
void maquina_de_estados() {
	switch (estado_actual) {
	case estado_inicio: {
		estado_actual = estado_parentesis;
		printf("estado inicio %c\n\r", dataR[0]);
		break;
	}
	case estado_parentesis: {
		if (dataR[0] == '(') {
			/* entonces cambia de estado */
			estado_actual = estado_almacena;
			printf("estado_parentesis -> estado_almacena\n\r");
		} else {
			/* sigue buscando el caracter '(' */
			estado_actual = estado_parentesis;
			printf("estado_parentesis -> estado_parentesis\n\r");
		}
	}
		break;
	case estado_almacena: {
		if (dataR[0] == ')') { /* se cierra la cadena. Se pasa al estado_parentesis */
			/* modifica estado */
			estado_actual = estado_parentesis;
			/* para debug */
			printf(
					"estado_almacena -> estado_parentesis. El paquete es: %s\n\r",
					paquete);
			/* valida SEQ */
			printf("validando seq: %d\n\r", valida_seq());
			/* valida SEQ */
			printf("validando c: %d\n\r", valida_c());
			/* valida CRC */
			printf("validando CRC: %d\n\r", valida_crc());
			/* limpia arreglo */
			for (int i = 0; i < contador; i++) {
				/* el valor 0 es nulo. No poner espacio porque espacio es un valor tambien */
				paquete[i] = 0;
			}
			/*reinicia contador */
			contador = 0;
		} else if (TAM_PAQUETE <= contador) { /* 10 es el tamaño maximo permitido */
			/* modifica estado */
			estado_actual = estado_parentesis;
			/* limpia arreglo */
			for (int i = 0; i < contador; i++) {
				/* el valor 0 es nulo. No poner espacio porque espacio es un valor tambien */
				paquete[i] = 0;
			}
			/*reinicia contador */
			contador = 0;
			/* para debug */
			printf(
					"estado_almacena -> estado_parentesis / es muy grande el paquete es: %s\n\r",
					paquete);
		} else {
			/* sigue guardandado caracter */
			contador++;
			strcat(paquete, dataR);
			/* modifica estado */
			estado_actual = estado_almacena;
			printf("estado_parentesis -> estado_almacena\n\r");
		}
	}
		break;
	default: {
		printf("default\n\r");
	}
	}
	printf("contador: %d\n\r", contador);
}
int __io_putchar(int ch) {
	uint8_t c[1];
	c[0] = ch & 0x00FF;
	HAL_UART_Transmit(&huart2, &*c, 1, 10);
	return ch;
}

int _write(int file, char *ptr, int len) {
	int DataIdx;
	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		__io_putchar(*ptr++);
	}
	return len;
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_funcion_capa_1 */
/**
 * @brief  Function implementing the capa_1 thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_funcion_capa_1 */
void funcion_capa_1(void *argument) {
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	for (;;) {
		osDelay(1);
	}
	/* USER CODE END 5 */
}

/* USER CODE BEGIN Header_funcion_capa_2 */
/**
 * @brief Function implementing the capa_2 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_funcion_capa_2 */

void funcion_capa_2(void *argument) {
	/* USER CODE BEGIN funcion_capa_2 */
	printf("funcion capa 2\n\r");
	/* Infinite loop */
	for (;;) {
		HAL_UART_Receive(&huart2, dataR, 1, HAL_MAX_DELAY);
		maquina_de_estados();
		osDelay(1);
	}
	/* USER CODE END funcion_capa_2 */
}

/* USER CODE BEGIN Header_funcion_capa_3 */
/**
 * @brief Function implementing the capa_3 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_funcion_capa_3 */
void funcion_capa_3(void *argument) {
	/* USER CODE BEGIN funcion_capa_3 */
	/* Infinite loop */
	for (;;) {
		osDelay(1);
	}
	/* USER CODE END funcion_capa_3 */
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM1) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
