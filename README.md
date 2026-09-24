# STM32F411 FreeRTOS & CMSIS-RTOS v2 - Lista de Práticas

Repositório contendo as implementações práticas de sistemas operacionais de tempo real com **FreeRTOS (API CMSIS-RTOS v2)** no microcontrolador **STM32F411CEU6 (WeAct BlackPill)** para o curso VIRTUS-CC.

## 🛠️ Plataforma e Ferramentas
- **Microcontrolador**: STM32F411CEU6 (ARM Cortex-M4 @ 100 MHz, 512 KB Flash, 128 KB RAM)
- **Base de Tempo HAL**: TIM1 (SysTick reservado exclusivamente para o escalonador FreeRTOS)
- **Comunicação Serial**: USART1 (PA9-TX / PA10-RX @ 115200 bps, 8N1)
- **Sistema Operacional**: FreeRTOS v10.3.1 via CMSIS-RTOS v2
- **Ambiente e Toolchain**: STM32CubeIDE / GNU Tools for STM32 (arm-none-eabi-gcc)

## 🌿 Estrutura de Branches por Questão
O repositório é organizado com uma branch dedicada para cada atividade prática:

* **`main`**: Projeto base e documentação de arquitetura.
* **`atividade-2`**: **Atendimento de uma Situação Crítica** (Escalonamento preemptivo, alternância temporal e prioridades: `osPriorityLow`, `osPriorityNormal`, `osPriorityHigh`).
* **`atividade-4`**: **Recursos Limitados** (Semáforo Contador: Simulação de estacionamento de 3 vagas disputadas concorrentemente por 5 tarefas de carros).
* **`atividade-6`**: **Condição de Corrida e Mutex** (Acesso concorrente a recurso compartilhado e proteção com `osMutex`).

## 💻 Como Compilar
Abra o projeto diretamente no **STM32CubeIDE** ou compile via linha de comando:
```bash
make -C Debug all -j4
```
