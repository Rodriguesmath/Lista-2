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
/* 1. Enum com os estados da Máquina de Estados Finita (FSM) de cada veículo */
typedef enum {
  ESTADO_AGUARDANDO_VAGA = 0,
  ESTADO_ESTACIONADO,
  ESTADO_PERMANECENDO,
  ESTADO_SAINDO,
  ESTADO_VAGA_LIBERADA
} EstadoCarro_t;

/* 2. Struct que encapsula os atributos e o estado atual de cada veículo */
typedef struct {
  const char *nome;
  uint32_t tempoEstacionadoMs;
  uint32_t tempoRuaMs;
  EstadoCarro_t estado;
} Carro_t;

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

static inline const char* GetEstadoDescricao(EstadoCarro_t estado)
{
  switch (estado)
  {
    case ESTADO_AGUARDANDO_VAGA: return "Tentando entrar... Aguardando vaga.";
    case ESTADO_ESTACIONADO:     return "Entrada autorizada! Estacionou na vaga.";
    case ESTADO_PERMANECENDO:    return "Permanecendo no estacionamento...";
    case ESTADO_SAINDO:          return "Saindo do estacionamento...";
    case ESTADO_VAGA_LIBERADA:   return "Vaga liberada com sucesso!";
    default:                     return "";
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
void AtualizarStatusCarro(Carro_t *carro, EstadoCarro_t novoEstado);
void Carro1Task(void *argument);
void Carro2Task(void *argument);
void Carro3Task(void *argument);
void Carro4Task(void *argument);
void Carro5Task(void *argument);
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
  Carro1Handle = osThreadNew(Carro1Task, NULL, &Carro1_attributes);
  Carro2Handle = osThreadNew(Carro2Task, NULL, &Carro2_attributes);
  Carro3Handle = osThreadNew(Carro3Task, NULL, &Carro3_attributes);
  Carro4Handle = osThreadNew(Carro4Task, NULL, &Carro4_attributes);
  Carro5Handle = osThreadNew(Carro5Task, NULL, &Carro5_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_Carro1Task */
/**
  * @brief  Tarefa do Carro 1: Simula ciclo de vida com tempos de 3000ms (vaga) e 2000ms (rua).
  * @param  argument: Não utilizado
  * @retval None
  */
/* USER CODE END Header_Carro1Task */
void Carro1Task(void *argument)
{
  /* USER CODE BEGIN Carro1Task */
  static Carro_t carro1 = {
    .nome = "Carro 1",
    .tempoEstacionadoMs = 3000,
    .tempoRuaMs = 2000,
    .estado = ESTADO_AGUARDANDO_VAGA
  };

  for(;;)
  {
    /* 1. Tentativa de Entrada */
    AtualizarStatusCarro(&carro1, ESTADO_AGUARDANDO_VAGA);

    /* 2. Aquisição do Recurso: Bloqueia se as vagas estiverem esgotadas */
    osSemaphoreAcquire(vagasHandle, osWaitForever);
    AtualizarStatusCarro(&carro1, ESTADO_ESTACIONADO);

    /* 3. Permanência no Estacionamento */
    osDelay(carro1.tempoEstacionadoMs / 2);
    AtualizarStatusCarro(&carro1, ESTADO_PERMANECENDO);
    osDelay(carro1.tempoEstacionadoMs / 2);

    /* 4. Saída do Estacionamento */
    AtualizarStatusCarro(&carro1, ESTADO_SAINDO);

    /* 5. Liberação da Vaga */
    AtualizarStatusCarro(&carro1, ESTADO_VAGA_LIBERADA);
    osSemaphoreRelease(vagasHandle);

    /* Tempo transitando na rua */
    osDelay(carro1.tempoRuaMs);
  }
  /* USER CODE END Carro1Task */
}

/* USER CODE BEGIN Header_Carro2Task */
/**
  * @brief  Tarefa do Carro 2: Simula ciclo de vida com tempos de 2000ms (vaga) e 2500ms (rua).
  * @param  argument: Não utilizado
  * @retval None
  */
/* USER CODE END Header_Carro2Task */
void Carro2Task(void *argument)
{
  /* USER CODE BEGIN Carro2Task */
  static Carro_t carro2 = {
    .nome = "Carro 2",
    .tempoEstacionadoMs = 2000,
    .tempoRuaMs = 2500,
    .estado = ESTADO_AGUARDANDO_VAGA
  };

  for(;;)
  {
    /* 1. Tentativa de Entrada */
    AtualizarStatusCarro(&carro2, ESTADO_AGUARDANDO_VAGA);

    /* 2. Aquisição do Recurso: Bloqueia se as vagas estiverem esgotadas */
    osSemaphoreAcquire(vagasHandle, osWaitForever);
    AtualizarStatusCarro(&carro2, ESTADO_ESTACIONADO);

    /* 3. Permanência no Estacionamento */
    osDelay(carro2.tempoEstacionadoMs / 2);
    AtualizarStatusCarro(&carro2, ESTADO_PERMANECENDO);
    osDelay(carro2.tempoEstacionadoMs / 2);

    /* 4. Saída do Estacionamento */
    AtualizarStatusCarro(&carro2, ESTADO_SAINDO);

    /* 5. Liberação da Vaga */
    AtualizarStatusCarro(&carro2, ESTADO_VAGA_LIBERADA);
    osSemaphoreRelease(vagasHandle);

    /* Tempo transitando na rua */
    osDelay(carro2.tempoRuaMs);
  }
  /* USER CODE END Carro2Task */
}

/* USER CODE BEGIN Header_Carro3Task */
/**
  * @brief  Tarefa do Carro 3: Simula ciclo de vida com tempos de 4000ms (vaga) e 2000ms (rua).
  * @param  argument: Não utilizado
  * @retval None
  */
/* USER CODE END Header_Carro3Task */
void Carro3Task(void *argument)
{
  /* USER CODE BEGIN Carro3Task */
  static Carro_t carro3 = {
    .nome = "Carro 3",
    .tempoEstacionadoMs = 4000,
    .tempoRuaMs = 2000,
    .estado = ESTADO_AGUARDANDO_VAGA
  };

  for(;;)
  {
    /* 1. Tentativa de Entrada */
    AtualizarStatusCarro(&carro3, ESTADO_AGUARDANDO_VAGA);

    /* 2. Aquisição do Recurso: Bloqueia se as vagas estiverem esgotadas */
    osSemaphoreAcquire(vagasHandle, osWaitForever);
    AtualizarStatusCarro(&carro3, ESTADO_ESTACIONADO);

    /* 3. Permanência no Estacionamento */
    osDelay(carro3.tempoEstacionadoMs / 2);
    AtualizarStatusCarro(&carro3, ESTADO_PERMANECENDO);
    osDelay(carro3.tempoEstacionadoMs / 2);

    /* 4. Saída do Estacionamento */
    AtualizarStatusCarro(&carro3, ESTADO_SAINDO);

    /* 5. Liberação da Vaga */
    AtualizarStatusCarro(&carro3, ESTADO_VAGA_LIBERADA);
    osSemaphoreRelease(vagasHandle);

    /* Tempo transitando na rua */
    osDelay(carro3.tempoRuaMs);
  }
  /* USER CODE END Carro3Task */
}

/* USER CODE BEGIN Header_Carro4Task */
/**
  * @brief  Tarefa do Carro 4: Simula ciclo de vida com tempos de 2500ms (vaga) e 3000ms (rua).
  * @param  argument: Não utilizado
  * @retval None
  */
/* USER CODE END Header_Carro4Task */
void Carro4Task(void *argument)
{
  /* USER CODE BEGIN Carro4Task */
  static Carro_t carro4 = {
    .nome = "Carro 4",
    .tempoEstacionadoMs = 2500,
    .tempoRuaMs = 3000,
    .estado = ESTADO_AGUARDANDO_VAGA
  };

  for(;;)
  {
    /* 1. Tentativa de Entrada */
    AtualizarStatusCarro(&carro4, ESTADO_AGUARDANDO_VAGA);

    /* 2. Aquisição do Recurso: Bloqueia se as vagas estiverem esgotadas */
    osSemaphoreAcquire(vagasHandle, osWaitForever);
    AtualizarStatusCarro(&carro4, ESTADO_ESTACIONADO);

    /* 3. Permanência no Estacionamento */
    osDelay(carro4.tempoEstacionadoMs / 2);
    AtualizarStatusCarro(&carro4, ESTADO_PERMANECENDO);
    osDelay(carro4.tempoEstacionadoMs / 2);

    /* 4. Saída do Estacionamento */
    AtualizarStatusCarro(&carro4, ESTADO_SAINDO);

    /* 5. Liberação da Vaga */
    AtualizarStatusCarro(&carro4, ESTADO_VAGA_LIBERADA);
    osSemaphoreRelease(vagasHandle);

    /* Tempo transitando na rua */
    osDelay(carro4.tempoRuaMs);
  }
  /* USER CODE END Carro4Task */
}

/* USER CODE BEGIN Header_Carro5Task */
/**
  * @brief  Tarefa do Carro 5: Simula ciclo de vida com tempos de 3500ms (vaga) e 2000ms (rua).
  * @param  argument: Não utilizado
  * @retval None
  */
/* USER CODE END Header_Carro5Task */
void Carro5Task(void *argument)
{
  /* USER CODE BEGIN Carro5Task */
  static Carro_t carro5 = {
    .nome = "Carro 5",
    .tempoEstacionadoMs = 3500,
    .tempoRuaMs = 2000,
    .estado = ESTADO_AGUARDANDO_VAGA
  };

  for(;;)
  {
    /* 1. Tentativa de Entrada */
    AtualizarStatusCarro(&carro5, ESTADO_AGUARDANDO_VAGA);

    /* 2. Aquisição do Recurso: Bloqueia se as vagas estiverem esgotadas */
    osSemaphoreAcquire(vagasHandle, osWaitForever);
    AtualizarStatusCarro(&carro5, ESTADO_ESTACIONADO);

    /* 3. Permanência no Estacionamento */
    osDelay(carro5.tempoEstacionadoMs / 2);
    AtualizarStatusCarro(&carro5, ESTADO_PERMANECENDO);
    osDelay(carro5.tempoEstacionadoMs / 2);

    /* 4. Saída do Estacionamento */
    AtualizarStatusCarro(&carro5, ESTADO_SAINDO);

    /* 5. Liberação da Vaga */
    AtualizarStatusCarro(&carro5, ESTADO_VAGA_LIBERADA);
    osSemaphoreRelease(vagasHandle);

    /* Tempo transitando na rua */
    osDelay(carro5.tempoRuaMs);
  }
  /* USER CODE END Carro5Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
  * @brief  Atualiza o estado do veículo e transmite o log correspondente via UART.
  * @param  carro: Ponteiro para a struct Carro_t
  * @param  novoEstado: Novo estado da máquina de estados do carro
  * @retval None
  */
void AtualizarStatusCarro(Carro_t *carro, EstadoCarro_t novoEstado)
{
  carro->estado = novoEstado;
  char msg[128];
  while (huart1.gState != HAL_UART_STATE_READY) osDelay(1);
  snprintf(msg, sizeof(msg), "[%lu] [%s] [Prioridade: %s] %s\r\n",
           (unsigned long)++ordemExecucao,
           carro->nome,
           GetPriorityName(osThreadGetPriority(osThreadGetId())),
           GetEstadoDescricao(carro->estado));
  HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 100);
}
/* USER CODE END Application */


