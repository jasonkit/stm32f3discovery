#ifndef __UART_H__
#define __UART_H__

#include "cmsis_os.h"
#include "stm32f3_discovery.h"
#include "stm32f3xx_hal.h"

#include "jkit/ringbuf.h"

#define UART_NUM_DEVICE 5

enum UartReturnVal {
  UART_OK,
  UART_ERR_INVALID_UART_DEVICE_ID,
  UART_ERR_FAIL_TO_INITIALIZE,
  UART_ERR_DEVICE_NOT_YET_INITIALIZED,
  UART_ERR_TX_BUFFER_IS_FULL
};

struct UartContext {
  UART_HandleTypeDef handle;
  struct RingBuf txBuf;
  struct RingBuf rxBuf;
  osMutexId uartMutex;
  uint16_t rxChunkSize;
  uint16_t rxNextWr;
  uint8_t rxOverflow;
  void (*rxCallback)(void);
};

extern struct UartContext* gUartContexts[UART_NUM_DEVICE];

void uart_task_init();
enum UartReturnVal uart_context_init(uint8_t uid, struct UartContext* ctxt, uint32_t baudrate, uint16_t rxChunkSize, void (*rxCallback)(void));
enum UartReturnVal uart_send(uint8_t uid, void* pData, size_t size);

// Interrupt
#ifdef UART_USE_USART1
void USART1_IRQHandler(void);
#endif

#ifdef UART_USE_USART2
void USART2_IRQHandler(void);
#endif

#ifdef UART_USE_USART3
void USART3_IRQHandler(void);
#endif

#ifdef UART_USE_UART4
void UART4_IRQHandler(void);
#endif

#ifdef UART_USE_UART5
void UART5_IRQHandler(void);
#endif

#endif
