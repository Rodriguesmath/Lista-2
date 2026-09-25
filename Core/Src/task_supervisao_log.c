/**
  ******************************************************************************
  * @file    task_supervisao_log.c
  * @brief   Atividade 8 - Implementação de TaskSupervisao e TaskLog.
  ******************************************************************************
  */

#include "app_industrial.h"
#include "industrial_tasks.h"
#include <stdio.h>

/* =============================================================================
 * TAREFA 3: SUPERVISÃO DA LINHA DE PRODUÇÃO (Supervisora)
 * Prioridade: Normal (osPriorityNormal) | Periodicidade: 2500 ms
 * ============================================================================= */
void TaskSupervisaoFun(void *argument)
{
  char buf[220];

  /* Aguarda primeiro lote de peças */
  osDelay(1800);

  for (;;)
  {
    /* Coleta consistente dos dados protegida por Mutex */
    osMutexAcquire(mutexDadosHandle, osWaitForever);
    uint32_t totalProc = g_estatisticas.totalProcessadas;
    uint32_t aprovadas = g_estatisticas.totalAprovadas;
    uint32_t rejeitadas = g_estatisticas.totalRejeitadas;
    StatusLinha_t status = g_estatisticas.statusLinha;
    osMutexRelease(mutexDadosHandle);

    uint32_t rendimento = 0;
    if (totalProc > 0) {
      rendimento = (aprovadas * 100) / totalProc;
    }

    const char *statusStr = "OPERACIONAL";
    if (status == STATUS_LINHA_PARADA_ALARME) {
      statusStr = "PARADA EM ALARME";
    } else if (g_estatisticas.falhasConsecutivas > 0) {
      statusStr = "ATENCAO (Falhas Detectadas)";
    }

    /* Formatação do painel de supervisão industrial */
    snprintf(buf, sizeof(buf),
             "\r\n--------------------------------------------------------------\r\n"
             "  [SUPERVISAO] Status: %s\r\n"
             "  Producao   : Total: %lu | Aprovadas: %lu | Rejeitadas: %lu\r\n"
             "  Qualidade  : Rendimento: %lu%% | Falhas Consecutivas: %lu\r\n"
             "--------------------------------------------------------------\r\n",
             statusStr,
             (unsigned long)totalProc,
             (unsigned long)aprovadas,
             (unsigned long)rejeitadas,
             (unsigned long)rendimento,
             (unsigned long)g_estatisticas.falhasConsecutivas);

    App_UART_Print(buf);

    osDelay(2500);
  }
}

/* =============================================================================
 * TAREFA 4: REGISTRO DE EVENTOS E HISTÓRICO (TaskLog)
 * Prioridade: Baixa (osPriorityLow) | Periodicidade: 1200 ms
 * ============================================================================= */
void TaskLogFun(void *argument)
{
  uint32_t idLog = 1;
  char buf[120];

  osDelay(800);

  for (;;)
  {
#if (MODO_OPERACAO == 2)
    /* MODO DESAFIO 2: Simulação de Inversão de Prioridade
     * A TaskLog (Prioridade Baixa) retém o Mutex de Dados por um tempo longo (400 ms)
     * enquanto a TaskProcessamento (Alta) necessita do recurso */
    osMutexAcquire(mutexDadosHandle, osWaitForever);
    snprintf(buf, sizeof(buf), "[LOG #%lu] Retendo recurso critico (simulando inversao)... (Tick: %lu)\r\n",
             (unsigned long)idLog, (unsigned long)osKernelGetTickCount());
    App_UART_Print(buf);
    
    /* Simula processamento demorado dentro da seção crítica */
    osDelay(400);
    
    osMutexRelease(mutexDadosHandle);
#else
    /* MODO NORMAL: Registro periódico leve */
    snprintf(buf, sizeof(buf),
             "[LOG #%lu] Tick: %lu ms | Esteira em operacao continua | Buffer OK\r\n",
             (unsigned long)idLog,
             (unsigned long)osKernelGetTickCount());
    App_UART_Print(buf);
#endif

    idLog++;
    osDelay(1200);
  }
}
