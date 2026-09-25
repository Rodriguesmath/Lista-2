/**
  ******************************************************************************
  * @file    task_producao.c
  * @brief   Atividade 8 - Implementação das tarefas de Produção:
  *          TaskSensor (Produtora) e TaskProcessamento (Consumidora Crítica).
  ******************************************************************************
  */

#include "app_industrial.h"
#include "industrial_tasks.h"
#include <stdio.h>

/* Tabela de espessuras simuladas (décimos de mm: 50 = 5.0 mm, 46/55 = defeituosas) */
static const uint32_t c_padraoEspessura[] = {
  50, 49, 51, 50, 46, 50, 52, 45, 46, 45 /* As peças 8, 9, 10 forçam 3 falhas consecutivas para alarme */
};
#define NUM_AMOSTRAS_SIMULADAS (sizeof(c_padraoEspessura) / sizeof(c_padraoEspessura[0]))

/* =============================================================================
 * TAREFA 1: SENSOR DE PRESENÇA DE PEÇA NA ESTEIRA (Produtora)
 * Prioridade: Normal | Periodicidade: 1000 ms
 * ============================================================================= */
void TaskSensorFun(void *argument)
{
  uint32_t contadorId = 1;
  char buf[120];

  /* Aguarda estabilização do boot */
  osDelay(600);

  for (;;)
  {
    /* Se a linha estiver em parada de emergência/alarme, o sensor aguarda */
    if (g_estatisticas.statusLinha == STATUS_LINHA_PARADA_ALARME)
    {
      osDelay(500);
      continue;
    }

    /* Simula leitura da peça física pelo sensor */
    uint32_t espessura = c_padraoEspessura[(contadorId - 1) % NUM_AMOSTRAS_SIMULADAS];

    g_pecaAtual.id = contadorId;
    g_pecaAtual.espessura_decimo_mm = espessura;
    g_pecaAtual.status = PECA_PENDENTE;
    g_pecaAtual.tickEntrada = osKernelGetTickCount();

    /* Atualiza total detectado */
    osMutexAcquire(mutexDadosHandle, osWaitForever);
    g_estatisticas.totalDetectadas++;
    osMutexRelease(mutexDadosHandle);

    snprintf(buf, sizeof(buf),
             "[SENSOR] Peca #%lu detectada na esteira | Espessura: %lu.%lu mm\r\n",
             (unsigned long)contadorId,
             (unsigned long)(espessura / 10),
             (unsigned long)(espessura % 10));
    App_UART_Print(buf);

    /* Sincronização: Sinaliza à TaskProcessamento que há uma nova peça */
    osSemaphoreRelease(semPecaProntaHandle);

    contadorId++;

    /* Intervalo de avanço mecânico da esteira até a próxima peça (1000 ms) */
    osDelay(1000);
  }
}

/* =============================================================================
 * TAREFA 2: PROCESSAMENTO E INSPEÇÃO DE QUALIDADE (Consumidora Crítica)
 * Prioridade: Alta (osPriorityHigh) | Sincronização: osSemaphoreAcquire
 * ============================================================================= */
void TaskProcessamentoFun(void *argument)
{
  char buf[150];

  for (;;)
  {
    /* Permanece bloqueada no semáforo até que o sensor detecte uma peça */
    osSemaphoreAcquire(semPecaProntaHandle, osWaitForever);

    uint32_t id = g_pecaAtual.id;
    uint32_t esp = g_pecaAtual.espessura_decimo_mm;

    /* Simula o tempo de processamento físico / inspeção ótica da peça (80 ms) */
    osDelay(80);

    /* Critério de Inspeção: Aprovada entre 4.8 mm e 5.2 mm */
    StatusPeca_t statusInspecao;
    if (esp >= ESPESSURA_MIN_APROVADA && esp <= ESPESSURA_MAX_APROVADA) {
      statusInspecao = PECA_APROVADA;
    } else {
      statusInspecao = PECA_REJEITADA;
    }
    g_pecaAtual.status = statusInspecao;

    /* Atualização atômica das estatísticas de produção */
    osMutexAcquire(mutexDadosHandle, osWaitForever);
    g_estatisticas.totalProcessadas++;

    if (statusInspecao == PECA_APROVADA) {
      g_estatisticas.totalAprovadas++;
      g_estatisticas.falhasConsecutivas = 0;
    } else {
      g_estatisticas.totalRejeitadas++;
      g_estatisticas.falhasConsecutivas++;
    }

    /* Se atingir o limite de peças consecutivas com defeito, dispara alarme */
    if (g_estatisticas.falhasConsecutivas >= LIMITE_FALHAS_ALARME) {
      g_estatisticas.statusLinha = STATUS_LINHA_PARADA_ALARME;
      osSemaphoreRelease(semAlarmeHandle);
    }
    osMutexRelease(mutexDadosHandle);

    /* Notificação serial do processamento */
    snprintf(buf, sizeof(buf),
             "[PROCESSO] Peca #%lu inspecionada: %s (%lu.%lu mm)\r\n",
             (unsigned long)id,
             (statusInspecao == PECA_APROVADA ? "APROVADA [OK]" : "REJEITADA [DEFEITO]"),
             (unsigned long)(esp / 10),
             (unsigned long)(esp % 10));
    App_UART_Print(buf);
  }
}
