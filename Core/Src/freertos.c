/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
volatile uint32_t countSensor = 0;
volatile uint32_t countDisplay = 0;
volatile uint32_t countDiagnostico = 0;

/* USER CODE END Variables */
/* Definitions for TaskSensor */
osThreadId_t TaskSensorHandle;
const osThreadAttr_t TaskSensor_attributes = {
  .name = "TaskSensor",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskDisplay */
osThreadId_t TaskDisplayHandle;
const osThreadAttr_t TaskDisplay_attributes = {
  .name = "TaskDisplay",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskDiagnostico */
osThreadId_t TaskDiagnosticoHandle;
const osThreadAttr_t TaskDiagnostico_attributes = {
  .name = "TaskDiagnostico",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void TaskSensor_fun(void *argument);
void TaskDisplay_fun(void *argument);
void TaskDiagnostico_fun(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of TaskSensor */
  TaskSensorHandle = osThreadNew(TaskSensor_fun, NULL, &TaskSensor_attributes);

  /* creation of TaskDisplay */
  TaskDisplayHandle = osThreadNew(TaskDisplay_fun, NULL, &TaskDisplay_attributes);

  /* creation of TaskDiagnostico */
  TaskDiagnosticoHandle = osThreadNew(TaskDiagnostico_fun, NULL, &TaskDiagnostico_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_TaskSensor_fun */
/**
  * @brief  Function implementing the TaskSensor thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_TaskSensor_fun */
void TaskSensor_fun(void *argument)
{
  /* USER CODE BEGIN TaskSensor_fun */
  /* Infinite loop */
  for(;;)
  {
	char mensagem[80];

	for(;;)
	{
		countSensor++;
		int tamanho = snprintf(
			mensagem,
			sizeof(mensagem),
			"[Task Sensor] Execucao: %lu | Prioridade: %d\r\n",
			(unsigned long)countSensor,
			(int)osThreadGetPriority(TaskSensorHandle)
		);

		HAL_UART_Transmit(&huart1, (uint8_t*)mensagem, (uint16_t)tamanho, HAL_MAX_DELAY);
	}
  }
  /* USER CODE END TaskSensor_fun */
}

/* USER CODE BEGIN Header_TaskDisplay_fun */
/**
* @brief Function implementing the TaskDisplay thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TaskDisplay_fun */
void TaskDisplay_fun(void *argument)
{
  /* USER CODE BEGIN TaskDisplay_fun */
  /* Infinite loop */
  for(;;)
  {
	char mensagem[80];

	for(;;)
	{
		countDisplay++;
		int tamanho = snprintf(
			mensagem,
			sizeof(mensagem),
			"[Task Display] Execucao: %lu | Prioridade: %d\r\n",
			(unsigned long)countDisplay,
			(int)osThreadGetPriority(TaskDisplayHandle)
	);

	HAL_UART_Transmit(&huart1, (uint8_t*)mensagem, (uint16_t)tamanho, HAL_MAX_DELAY);
	}
  }
  /* USER CODE END TaskDisplay_fun */
}

/* USER CODE BEGIN Header_TaskDiagnostico_fun */
/**
* @brief Function implementing the TaskDiagnostico thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TaskDiagnostico_fun */
void TaskDiagnostico_fun(void *argument)
{
  /* USER CODE BEGIN TaskDiagnostico_fun */
  /* Infinite loop */
  for(;;)
  {
	char mensagem[80];

	for(;;)
	{
		countDiagnostico++;
		int tamanho = snprintf(
			mensagem,
			sizeof(mensagem),
			"[Task Diagnostico] Execucao: %lu | Prioridade: %d\r\n",
			(unsigned long)countDiagnostico,
			(int)osThreadGetPriority(TaskDiagnosticoHandle)
	);

	HAL_UART_Transmit(&huart1, (uint8_t*)mensagem, (uint16_t)tamanho, HAL_MAX_DELAY);
	}
  }
  /* USER CODE END TaskDiagnostico_fun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

