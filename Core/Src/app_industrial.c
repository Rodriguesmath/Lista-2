/**
  ******************************************************************************
  * @file    app_industrial.c
  * @brief   Atividade 8 - Implementação da infraestrutura, objetos de RTOS e UART.
  ******************************************************************************
  */

#include "app_industrial.h"
#include "industrial_tasks.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

/* =============================================================================
 * VARIÁVEIS GLOBAIS COMPARTILHADAS
 * ============================================================================= */
volatile Peca_t g_pecaAtual = {
  .id = 0,
  .espessura_decimo_mm = 50,
  .status = PECA_PENDENTE,
  .tickEntrada = 0
};

volatile EstatisticasProducao_t g_estatisticas = {
  .totalDetectadas = 0,
  .totalProcessadas = 0,
  .totalAprovadas = 0,
  .totalRejeitadas = 0,
  .falhasConsecutivas = 0,
  .statusLinha = STATUS_LINHA_NORMAL
};

/* =============================================================================
 * HANDLES E ATRIBUTOS DOS OBJETOS DE SINCRONIZAÇÃO
 * ============================================================================= */
osSemaphoreId_t semPecaProntaHandle;
const osSemaphoreAttr_t semPecaPronta_attributes = {
  .name = "semPecaPronta"
};

osSemaphoreId_t semAlarmeHandle;
const osSemaphoreAttr_t semAlarme_attributes = {
  .name = "semAlarme"
};

osMutexId_t mutexUartHandle;
const osMutexAttr_t mutexUart_attributes = {
  .name = "mutexUart"
};

osMutexId_t mutexDadosHandle;
const osMutexAttr_t mutexDados_attributes = {
  .name = "mutexDados"
};

/* =============================================================================
 * HANDLES E ATRIBUTOS DAS 5 TAREFAS DO MINI SISTEMA INDUSTRIAL
 * ============================================================================= */
osThreadId_t TaskSensorHandle;
osThreadId_t TaskProcessamentoHandle;
osThreadId_t TaskSupervisaoHandle;
osThreadId_t TaskLogHandle;
osThreadId_t TaskAlarmeHandle;

/* TaskAlarme: Prioridade Realtime (resposta imediata a emergências) */
const osThreadAttr_t TaskAlarme_attributes = {
  .name = "TaskAlarme",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};

/* TaskProcessamento: Prioridade High (processa peça assim que o sensor detecta) */
const osThreadAttr_t TaskProcessamento_attributes = {
  .name = "TaskProc",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};

/* TaskSensor: Prioridade Normal (cadência periódica da esteira) */
const osThreadAttr_t TaskSensor_attributes = {
  .name = "TaskSensor",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* TaskSupervisao: Prioridade Normal (telemetria e cálculo de indicadores) */
const osThreadAttr_t TaskSupervisao_attributes = {
  .name = "TaskSuperv",
  .stack_size = 384 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* TaskLog: Prioridade Low (registro histórico em segundo plano) */
const osThreadAttr_t TaskLog_attributes = {
  .name = "TaskLog",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* =============================================================================
 * FUNÇÃO DE TRANSMISSÃO UART SERIAL
 * ============================================================================= */
void App_UART_Print(const char *msg)
{
  if (msg == NULL) return;
  uint16_t len = (uint16_t)strlen(msg);

#if (MODO_OPERACAO == 1)
  /* MODO DESAFIO 1: Sem proteção por Mutex na UART
   * Mensagens longas são transmitidas com pequenos atrasos, sofrendo preempção
   * no meio do buffer e causando corrupção visual no CuteCom */
  for (uint16_t i = 0; i < len; i++) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&msg[i], 1, 10);
    /* Força pequenas esperas para aumentar probabilidade de troca de contexto no envio */
    for (volatile int delay = 0; delay < 1200; delay++);
  }
#else
  /* MODO NORMAL / PROTEGIDO: Exclusão mútua garantida por Mutex */
  if (mutexUartHandle != NULL) {
    osMutexAcquire(mutexUartHandle, osWaitForever);
  }
  HAL_UART_Transmit(&huart1, (uint8_t *)msg, len, 100);
  if (mutexUartHandle != NULL) {
    osMutexRelease(mutexUartHandle);
  }
#endif
}

void App_UART_PrintRaw(const char *msg)
{
  if (msg == NULL) return;
  HAL_UART_Transmit(&huart1, (uint8_t *)msg, (uint16_t)strlen(msg), 100);
}

/* =============================================================================
 * INICIALIZAÇÃO DA APLICAÇÃO INDUSTRIAL
 * ============================================================================= */
void App_Industrial_Init(void)
{
  char msg[200];

  /* 1. Criação dos Semáforos Binários */
  semPecaProntaHandle = osSemaphoreNew(1, 0, &semPecaPronta_attributes);
  semAlarmeHandle     = osSemaphoreNew(1, 0, &semAlarme_attributes);

  /* 2. Criação dos Mutexes */
  mutexUartHandle  = osMutexNew(&mutexUart_attributes);
  mutexDadosHandle = osMutexNew(&mutexDados_attributes);

  /* 3. Criação das 5 Tarefas */
  TaskAlarmeHandle        = osThreadNew(TaskAlarmeFun, NULL, &TaskAlarme_attributes);
  TaskProcessamentoHandle = osThreadNew(TaskProcessamentoFun, NULL, &TaskProcessamento_attributes);
  TaskSensorHandle        = osThreadNew(TaskSensorFun, NULL, &TaskSensor_attributes);
  TaskSupervisaoHandle    = osThreadNew(TaskSupervisaoFun, NULL, &TaskSupervisao_attributes);
  TaskLogHandle           = osThreadNew(TaskLogFun, NULL, &TaskLog_attributes);

  /* Banner Inicial */
  snprintf(msg, sizeof(msg),
           "\r\n==============================================================\r\n"
           "  ATIVIDADE 8 - MINI SISTEMA INDUSTRIAL DE ESTEIRA DE PECAS\r\n"
           "  FreeRTOS & CMSIS-RTOS v2 | STM32F411 BlackPill\r\n"
           "  Modo Operacional: %s\r\n"
           "==============================================================\r\n",
           (MODO_OPERACAO == 0 ? "0 - NORMAL / PROTEGIDO" :
            MODO_OPERACAO == 1 ? "1 - DESAFIO 1 (Sem Mutex UART - Colisao Serial)" :
                                 "2 - DESAFIO 2 (Inversao de Prioridade / Starvation)"));
  App_UART_PrintRaw(msg);
}
