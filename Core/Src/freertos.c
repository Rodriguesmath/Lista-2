/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Atividade 6 - Condição de Corrida e Mutex no FreeRTOS
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
/* -----------------------------------------------------------------------------
 * Chave de configuração para os experimentos da Atividade 6:
 *
 * 0 = EXPERIMENTO 1 (SEM MUTEX):
 *     Demonstração de Condição de Corrida (Race Condition).
 *     O valor final do contadorGlobal será menor que 200.000 devido à
 *     preempção no ciclo não atômico LER -> MODIFICAR -> ESCREVER.
 *
 * 1 = EXPERIMENTO 2 (COM MUTEX):
 *     Proteção com osMutexAcquire / osMutexRelease.
 *     O valor final do contadorGlobal atingirá com precisão exatos 200.000.
 * ----------------------------------------------------------------------------- */
#define USAR_MUTEX 0

#define ITERACOES_POR_TASK 100000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* Variável compartilhada entre as tarefas (recurso crítico concorrente) */
volatile uint32_t contadorGlobal = 0;

/* Flags para coordenação e sincronização entre Monitor e Incrementadores */
volatile uint8_t iniciarExperimento = 0;
volatile uint8_t task1Finalizada = 0;
volatile uint8_t task2Finalizada = 0;

/* Mutex para proteção de contadorGlobal */
osMutexId_t contadorMutexHandle;
const osMutexAttr_t contadorMutex_attributes = {
  .name = "contadorMutex"
};

/* Handles e atributos das 3 tarefas da Atividade 6 */
osThreadId_t TaskIncrementador1Handle;
osThreadId_t TaskIncrementador2Handle;
osThreadId_t TaskMonitorHandle;

const osThreadAttr_t TaskIncrementador1_attributes = {
  .name = "TaskInc1",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t TaskIncrementador2_attributes = {
  .name = "TaskInc2",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t TaskMonitor_attributes = {
  .name = "TaskMonitor",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void TaskIncrementador1Fun(void *argument);
void TaskIncrementador2Fun(void *argument);
void TaskMonitorFun(void *argument);
/* USER CODE END FunctionPrototypes */

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
  /* Criação do Mutex de proteção do contadorGlobal */
  contadorMutexHandle = osMutexNew(&contadorMutex_attributes);
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
  /* USER CODE BEGIN RTOS_THREADS */
  TaskIncrementador1Handle = osThreadNew(TaskIncrementador1Fun, NULL, &TaskIncrementador1_attributes);
  TaskIncrementador2Handle = osThreadNew(TaskIncrementador2Fun, NULL, &TaskIncrementador2_attributes);
  TaskMonitorHandle        = osThreadNew(TaskMonitorFun, NULL, &TaskMonitor_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_TaskIncrementador1Fun */
/**
  * @brief  Tarefa Incrementadora 1: executa 100.000 iterações em contadorGlobal.
  * @param  argument: Não utilizado
  * @retval None
  */
/* USER CODE END Header_TaskIncrementador1Fun */
void TaskIncrementador1Fun(void *argument)
{
  /* USER CODE BEGIN TaskIncrementador1Fun */
  for(;;)
  {
    /* Aguarda a ordem de largada da tarefa Monitor */
    while (!iniciarExperimento) osDelay(5);

    /* Executa o laço de 100.000 incrementos */
    for (int i = 0; i < ITERACOES_POR_TASK; i++)
    {
#if (USAR_MUTEX == 1)
      osMutexAcquire(contadorMutexHandle, osWaitForever);
      contadorGlobal++;
      osMutexRelease(contadorMutexHandle);
#else
      contadorGlobal++;
#endif
    }

    /* Sinaliza que completou as 100.000 iterações */
    task1Finalizada = 1;

    /* Aguarda a tarefa Monitor registrar o resultado antes da próxima rodada */
    while (iniciarExperimento) osDelay(5);
  }
  /* USER CODE END TaskIncrementador1Fun */
}

/* USER CODE BEGIN Header_TaskIncrementador2Fun */
/**
  * @brief  Tarefa Incrementadora 2: executa 100.000 iterações em contadorGlobal.
  * @param  argument: Não utilizado
  * @retval None
  */
/* USER CODE END Header_TaskIncrementador2Fun */
void TaskIncrementador2Fun(void *argument)
{
  /* USER CODE BEGIN TaskIncrementador2Fun */
  for(;;)
  {
    /* Aguarda a ordem de largada da tarefa Monitor */
    while (!iniciarExperimento) osDelay(5);

    /* Executa o laço de 100.000 incrementos */
    for (int i = 0; i < ITERACOES_POR_TASK; i++)
    {
#if (USAR_MUTEX == 1)
      osMutexAcquire(contadorMutexHandle, osWaitForever);
      contadorGlobal++;
      osMutexRelease(contadorMutexHandle);
#else
      contadorGlobal++;
#endif
    }

    /* Sinaliza que completou as 100.000 iterações */
    task2Finalizada = 1;

    /* Aguarda a tarefa Monitor registrar o resultado antes da próxima rodada */
    while (iniciarExperimento) osDelay(5);
  }
  /* USER CODE END TaskIncrementador2Fun */
}

/* USER CODE BEGIN Header_TaskMonitorFun */
/**
  * @brief  Tarefa Monitora: coordena as rodadas, calcula tempos e exibe resultados via UART.
  * @param  argument: Não utilizado
  * @retval None
  */
/* USER CODE END Header_TaskMonitorFun */
void TaskMonitorFun(void *argument)
{
  /* USER CODE BEGIN TaskMonitorFun */
  char msg[150];
  uint32_t rodada = 0;

  /* Aguarda estabilização da porta serial no boot */
  osDelay(500);

  for(;;)
  {
    rodada++;
    contadorGlobal = 0;
    task1Finalizada = 0;
    task2Finalizada = 0;

    snprintf(msg, sizeof(msg), "\r\n==================================================\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);

    snprintf(msg, sizeof(msg), "[RODADA %lu] ATIVIDADE 6 - %s\r\n",
             (unsigned long)rodada,
             (USAR_MUTEX ? "COM MUTEX (Protegido)" : "SEM MUTEX (Condicao de Corrida)"));
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);

    snprintf(msg, sizeof(msg), "Disparando Task 1 e Task 2 (2x 100.000 incrementos)...\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);

    uint32_t tInicio = osKernelGetTickCount();
    iniciarExperimento = 1;

    /* Aguarda ambas as tarefas concluírem as 100.000 iterações */
    while (!task1Finalizada || !task2Finalizada)
    {
      osDelay(5);
    }
    uint32_t tFim = osKernelGetTickCount();
    iniciarExperimento = 0;

    /* Exibe os resultados e diagnósticos */
    snprintf(msg, sizeof(msg), "Resultado Final: contadorGlobal = %lu\r\n", (unsigned long)contadorGlobal);
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);

    snprintf(msg, sizeof(msg), "Valor Esperado : 200000\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);

    if (contadorGlobal == 200000)
    {
      snprintf(msg, sizeof(msg), "Status         : SUCESSO (Sem perdas! Tempo decorrido: %lu ms)\r\n",
               (unsigned long)(tFim - tInicio));
    }
    else
    {
      long perdas = 200000 - (long)contadorGlobal;
      snprintf(msg, sizeof(msg), "Status         : FALHA! Condicao de Corrida! (Perda: %ld contagens)\r\n", perdas);
    }
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);

    snprintf(msg, sizeof(msg), "==================================================\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);

    /* Pausa de 3 segundos entre rodadas para permitir análise dos dados */
    osDelay(3000);
  }
  /* USER CODE END TaskMonitorFun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
