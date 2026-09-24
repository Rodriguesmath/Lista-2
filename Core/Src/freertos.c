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
#include <stdio.h>
#include <string.h>
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
  PRIO_LOW    = osPriorityLow,
  PRIO_NORMAL = osPriorityNormal,
  PRIO_HIGH   = osPriorityHigh
} Prioridade_t;

static inline const char* GetPriorityName(osPriority_t prio)
{
  switch (prio)
  {
    case osPriorityLow:    return "LOW";
    case osPriorityNormal: return "NORMAL";
    case osPriorityHigh:   return "HIGH";
    default:               return "CUSTOM";
  }
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint32_t ordemExecucao = 0;
/* USER CODE END Variables */
/* Definitions for TaskInterface */
osThreadId_t TaskInterfaceHandle;
const osThreadAttr_t TaskInterface_attributes = {
  .name = "TaskInterface",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskProcesso */
osThreadId_t TaskProcessoHandle;
const osThreadAttr_t TaskProcesso_attributes = {
  .name = "TaskProcesso",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskEmergencia */
osThreadId_t TaskEmergenciaHandle;
const osThreadAttr_t TaskEmergencia_attributes = {
  .name = "TaskEmergencia",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh, /* Experimentos: osPriorityLow, osPriorityNormal ou osPriorityHigh */
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void TaskInterfaceFun(void *argument);
void TaskProcessoFun(void *argument);
void TaskEmergenciaFun(void *argument);

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
  /* creation of TaskInterface */
  TaskInterfaceHandle = osThreadNew(TaskInterfaceFun, NULL, &TaskInterface_attributes);

  /* creation of TaskProcesso */
  TaskProcessoHandle = osThreadNew(TaskProcessoFun, NULL, &TaskProcesso_attributes);

  /* creation of TaskEmergencia */
  TaskEmergenciaHandle = osThreadNew(TaskEmergenciaFun, NULL, &TaskEmergencia_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_TaskInterfaceFun */
/**
  * @brief  Function implementing the TaskInterface thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_TaskInterfaceFun */
void TaskInterfaceFun(void *argument)
{
  /* USER CODE BEGIN TaskInterfaceFun */
  char msg[80];
  /* Infinite loop */
  for(;;)
  {
    snprintf(msg, sizeof(msg), "[%lu] [INTERFACE] [Prioridade: %s] Atualizando tela\r\n",
             (unsigned long)++ordemExecucao,
             GetPriorityName(osThreadGetPriority(osThreadGetId())));
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);
    osDelay(1000);

    /* --- DESAFIO (Starvation) ---
     * Descomente as linhas abaixo para simular uso intensivo de CPU sem bloqueio:
     * for(volatile uint32_t i = 0; i < 20000000; i++);
     */
  }
  /* USER CODE END TaskInterfaceFun */
}

/* USER CODE BEGIN Header_TaskProcessoFun */
/**
* @brief Function implementing the TaskProcesso thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TaskProcessoFun */
void TaskProcessoFun(void *argument)
{
  /* USER CODE BEGIN TaskProcessoFun */
  char msg[80];
  /* Infinite loop */
  for(;;)
  {
    snprintf(msg, sizeof(msg), "[%lu] [PROCESSO] [Prioridade: %s] Executando ciclo\r\n",
             (unsigned long)++ordemExecucao,
             GetPriorityName(osThreadGetPriority(osThreadGetId())));
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);
    osDelay(1000);
  }
  /* USER CODE END TaskProcessoFun */
}

/* USER CODE BEGIN Header_TaskEmergenciaFun */
/**
* @brief Function implementing the TaskEmergencia thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TaskEmergenciaFun */
void TaskEmergenciaFun(void *argument)
{
  /* USER CODE BEGIN TaskEmergenciaFun */
  char msg[80];
  /* Infinite loop */
  for(;;)
  {
    snprintf(msg, sizeof(msg), "[%lu] [EMERGENCIA] [Prioridade: %s] Monitorando alarme\r\n",
             (unsigned long)++ordemExecucao,
             GetPriorityName(osThreadGetPriority(osThreadGetId())));
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);
    osDelay(1000);
  }
  /* USER CODE END TaskEmergenciaFun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

