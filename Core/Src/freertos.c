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
typedef struct {
  const char *nome;
  uint32_t tempoEstacionadoMs;
  uint32_t tempoRuaMs;
} CarroInfo_t;

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
/* Experimento da Atividade 4: 3 -> 2 -> 1 vaga */
#define TOTAL_VAGAS 3
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint32_t ordemExecucao = 0;

/* Semáforo Contador para gerenciar as vagas do estacionamento */
osSemaphoreId_t vagasHandle;
const osSemaphoreAttr_t vagas_attributes = {
  .name = "vagasSem"
};

/* Configuração dos 5 veículos com tempos dinâmicos para simulação realista */
static const CarroInfo_t carrosInfo[5] = {
  { "Carro 1", 3000, 2000 },
  { "Carro 2", 2000, 2500 },
  { "Carro 3", 4000, 2000 },
  { "Carro 4", 2500, 3000 },
  { "Carro 5", 3500, 2000 },
};

/* Handles das 5 tarefas */
osThreadId_t Carro1Handle;
osThreadId_t Carro2Handle;
osThreadId_t Carro3Handle;
osThreadId_t Carro4Handle;
osThreadId_t Carro5Handle;

const osThreadAttr_t Carro1_attributes = { .name = "Carro1", .stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal };
const osThreadAttr_t Carro2_attributes = { .name = "Carro2", .stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal };
const osThreadAttr_t Carro3_attributes = { .name = "Carro3", .stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal };
const osThreadAttr_t Carro4_attributes = { .name = "Carro4", .stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal };
const osThreadAttr_t Carro5_attributes = { .name = "Carro5", .stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal };

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void TaskCarroFun(void *argument);
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
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* Semáforo Contador: TOTAL_VAGAS máximas, TOTAL_VAGAS inicialmente disponíveis */
  vagasHandle = osSemaphoreNew(TOTAL_VAGAS, TOTAL_VAGAS, &vagas_attributes);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* USER CODE BEGIN RTOS_THREADS */
  Carro1Handle = osThreadNew(TaskCarroFun, (void *)&carrosInfo[0], &Carro1_attributes);
  Carro2Handle = osThreadNew(TaskCarroFun, (void *)&carrosInfo[1], &Carro2_attributes);
  Carro3Handle = osThreadNew(TaskCarroFun, (void *)&carrosInfo[2], &Carro3_attributes);
  Carro4Handle = osThreadNew(TaskCarroFun, (void *)&carrosInfo[3], &Carro4_attributes);
  Carro5Handle = osThreadNew(TaskCarroFun, (void *)&carrosInfo[4], &Carro5_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_TaskCarroFun */
/**
  * @brief  Função genérica que implementa o ciclo de vida de cada carro.
  * @param  argument: Ponteiro para CarroInfo_t
  * @retval None
  */
/* USER CODE END Header_TaskCarroFun */
void TaskCarroFun(void *argument)
{
  /* USER CODE BEGIN TaskCarroFun */
  CarroInfo_t *carro = (CarroInfo_t *)argument;
  char msg[128];

  /* Infinite loop */
  for(;;)
  {
    /* -------------------------------------------------------------------------
     * 1. Tentativa de Entrada: Carro chega e solicita vaga
     * ------------------------------------------------------------------------- */
    while (huart1.gState != HAL_UART_STATE_READY) osDelay(1);
    snprintf(msg, sizeof(msg), "[%lu] [%s] [Prioridade: %s] Tentando entrar... Aguardando vaga.\r\n",
             (unsigned long)++ordemExecucao,
             carro->nome,
             GetPriorityName(osThreadGetPriority(osThreadGetId())));
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);

    /* -------------------------------------------------------------------------
     * 2. Aquisição do Recurso: Bloqueia no semáforo se não houver vagas livres
     * ------------------------------------------------------------------------- */
    osSemaphoreAcquire(vagasHandle, osWaitForever);
    while (huart1.gState != HAL_UART_STATE_READY) osDelay(1);
    snprintf(msg, sizeof(msg), "[%lu] [%s] [Prioridade: %s] Entrada autorizada! Estacionou na vaga.\r\n",
             (unsigned long)++ordemExecucao,
             carro->nome,
             GetPriorityName(osThreadGetPriority(osThreadGetId())));
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);

    /* -------------------------------------------------------------------------
     * 3. Permanência: Veículo ocupa a vaga pelo tempo determinado
     * ------------------------------------------------------------------------- */
    osDelay(carro->tempoEstacionadoMs / 2);
    while (huart1.gState != HAL_UART_STATE_READY) osDelay(1);
    snprintf(msg, sizeof(msg), "[%lu] [%s] [Prioridade: %s] Permanecendo no estacionamento...\r\n",
             (unsigned long)++ordemExecucao,
             carro->nome,
             GetPriorityName(osThreadGetPriority(osThreadGetId())));
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);
    osDelay(carro->tempoEstacionadoMs / 2);

    /* -------------------------------------------------------------------------
     * 4. Saída do Estacionamento: Notifica que está deixando a vaga
     * ------------------------------------------------------------------------- */
    while (huart1.gState != HAL_UART_STATE_READY) osDelay(1);
    snprintf(msg, sizeof(msg), "[%lu] [%s] [Prioridade: %s] Saindo do estacionamento...\r\n",
             (unsigned long)++ordemExecucao,
             carro->nome,
             GetPriorityName(osThreadGetPriority(osThreadGetId())));
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);

    /* -------------------------------------------------------------------------
     * 5. Liberação da Vaga: Devolve o token ao semáforo contador
     * ------------------------------------------------------------------------- */
    while (huart1.gState != HAL_UART_STATE_READY) osDelay(1);
    snprintf(msg, sizeof(msg), "[%lu] [%s] [Prioridade: %s] Vaga liberada com sucesso!\r\n",
             (unsigned long)++ordemExecucao,
             carro->nome,
             GetPriorityName(osThreadGetPriority(osThreadGetId())));
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);

    /* Libera o token do semáforo, desbloqueando a próxima tarefa em espera */
    osSemaphoreRelease(vagasHandle);

    /* Tempo transitando na rua antes da próxima tentativa */
    osDelay(carro->tempoRuaMs);
  }
  /* USER CODE END TaskCarroFun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */


