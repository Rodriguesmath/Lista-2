/**
  ******************************************************************************
  * @file    industrial_tasks.h
  * @brief   Atividade 8 - Protótipos e configurações das tarefas do sistema.
  ******************************************************************************
  */

#ifndef INC_INDUSTRIAL_TASKS_H_
#define INC_INDUSTRIAL_TASKS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"

/* Handles das Threads */
extern osThreadId_t TaskSensorHandle;
extern osThreadId_t TaskProcessamentoHandle;
extern osThreadId_t TaskSupervisaoHandle;
extern osThreadId_t TaskLogHandle;
extern osThreadId_t TaskAlarmeHandle;

/* Funções de execução das tarefas */
void TaskSensorFun(void *argument);
void TaskProcessamentoFun(void *argument);
void TaskSupervisaoFun(void *argument);
void TaskLogFun(void *argument);
void TaskAlarmeFun(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* INC_INDUSTRIAL_TASKS_H_ */
