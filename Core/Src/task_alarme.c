/**
  ******************************************************************************
  * @file    task_alarme.c
  * @brief   Atividade 8 - Implementação de TaskAlarme (Prioridade Crítica/Realtime).
  ******************************************************************************
  */

#include "app_industrial.h"
#include "industrial_tasks.h"
#include <stdio.h>

/* =============================================================================
 * TAREFA 5: TRATAMENTO DE ALARMES E PARADA DE SEGURANÇA (TaskAlarme)
 * Prioridade: Realtime (osPriorityRealtime - Máxima Prioridade do Sistema)
 * Sincronização: osSemaphoreAcquire (Acorda instantaneamente em anomalias)
 * ============================================================================= */
void TaskAlarmeFun(void *argument)
{
  char buf[250];

  for (;;)
  {
    /* Permanece em estado de bloqueio absoluto até a ocorrência de uma emergência */
    osSemaphoreAcquire(semAlarmeHandle, osWaitForever);

    /* Como possui prioridade Realtime, preempte imediatamente qualquer outra tarefa */
    snprintf(buf, sizeof(buf),
             "\r\n**************************************************************\r\n"
             "  [ALARME CRITICO] PARADA DE EMERGENCIA ACIONADA!\r\n"
             "  Motivo : Limite de 3 pecas consecutivas com defeito atingido!\r\n"
             "  Acao   : Esteira paralisada por seguranca operacional!\r\n"
             "  Tick   : %lu ms\r\n"
             "**************************************************************\r\n",
             (unsigned long)osKernelGetTickCount());
    App_UART_Print(buf);

    /* Simula tempo de parada técnica para intervenção do operador (3000 ms) */
    osDelay(3000);

    /* Normalização e reset do sistema de segurança */
    osMutexAcquire(mutexDadosHandle, osWaitForever);
    g_estatisticas.falhasConsecutivas = 0;
    g_estatisticas.statusLinha = STATUS_LINHA_NORMAL;
    osMutexRelease(mutexDadosHandle);

    snprintf(buf, sizeof(buf),
             "[ALARME] Inspecao concluida. Esteira reiniciada com sucesso!\r\n\r\n");
    App_UART_Print(buf);
  }
}
