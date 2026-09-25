/**
  ******************************************************************************
  * @file    app_industrial.h
  * @brief   Atividade 8 - Mini Sistema Industrial com FreeRTOS e CMSIS-RTOS v2.
  *          Definições de dados, objetos de sincronização e modos de teste.
  ******************************************************************************
  */

#ifndef INC_APP_INDUSTRIAL_H_
#define INC_APP_INDUSTRIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"
#include <stdint.h>
#include <stdbool.h>

/* =============================================================================
 * CONFIGURAÇÃO DOS MODOS DE EXPERIMENTO / DESAFIO DA ATIVIDADE 8
 * -----------------------------------------------------------------------------
 * 0 = MODO NORMAL / PROTEGIDO:
 *     - Semáforo binário sincroniza sensor e processamento.
 *     - Mutex UART protege a serial contra entrelaçamento de mensagens.
 *     - Mutex de Dados protege as variáveis de contagem de produção.
 *     - Sistema opera com 100% de estabilidade e determinismo.
 *
 * 1 = MODO DESAFIO 1 (Concorrência na UART sem Mutex):
 *     - Desativa o Mutex da UART.
 *     - TaskSupervisao e TaskLog disputam a USART1 simultaneamente.
 *     - O chaveamento por fatiamento de tempo corta mensagens pela metade,
 *       demonstrando corrupção de mensagens e falha de concorrência serial.
 *
 * 2 = MODO DESAFIO 2 (Inversão de Prioridade / Starvation):
 *     - TaskLog (Prioridade Baixa) retém o Mutex de Dados prolongadamente.
 *     - TaskSupervisao (Prioridade Média) monopoliza a CPU em busy-loop.
 *     - TaskProcessamento (Prioridade Alta) é bloqueada aguardando o recurso,
 *       demonstrando bloqueio indevido de tarefa crítica (Inversão de Prioridade).
 * ============================================================================= */
#define MODO_OPERACAO 0

/* Constantes operacionais da esteira industrial */
#define ESPESSURA_MIN_APROVADA   48   /* 4.8 mm */
#define ESPESSURA_MAX_APROVADA   52   /* 5.2 mm */
#define LIMITE_FALHAS_ALARME      3   /* 3 peças consecutivas com defeito disparam alarme */

/* Status da inspeção da peça */
typedef enum {
  PECA_PENDENTE = 0,
  PECA_APROVADA,
  PECA_REJEITADA
} StatusPeca_t;

/* Status operacional geral da linha de produção */
typedef enum {
  STATUS_LINHA_NORMAL = 0,
  STATUS_LINHA_ATENCAO,
  STATUS_LINHA_PARADA_ALARME
} StatusLinha_t;

/* Estrutura que representa a peça física na esteira */
typedef struct {
  uint32_t      id;
  uint32_t      espessura_decimo_mm; /* ex: 50 = 5.0 mm */
  StatusPeca_t  status;
  uint32_t      tickEntrada;
} Peca_t;

/* Estrutura de métricas compartilhadas da esteira */
typedef struct {
  uint32_t      totalDetectadas;
  uint32_t      totalProcessadas;
  uint32_t      totalAprovadas;
  uint32_t      totalRejeitadas;
  uint32_t      falhasConsecutivas;
  StatusLinha_t statusLinha;
} EstatisticasProducao_t;

/* Objetos de sincronização do RTOS (externos) */
extern osSemaphoreId_t semPecaProntaHandle;
extern osSemaphoreId_t semAlarmeHandle;
extern osMutexId_t     mutexUartHandle;
extern osMutexId_t     mutexDadosHandle;

/* Dados compartilhados do sistema (externos) */
extern volatile Peca_t                 g_pecaAtual;
extern volatile EstatisticasProducao_t g_estatisticas;

/* Funções de infraestrutura e aplicação */
void App_Industrial_Init(void);
void App_UART_Print(const char *msg);
void App_UART_PrintRaw(const char *msg);

#ifdef __cplusplus
}
#endif

#endif /* INC_APP_INDUSTRIAL_H_ */
