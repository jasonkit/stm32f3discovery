#include "uart.h"

struct UartContext *gUartContexts[UART_NUM_DEVICE];
osThreadId uart_task_handle;

static void uart_task(void const *argument);
static USART_TypeDef *uart_instance_lookup(uint8_t uid);

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
  GPIO_TypeDef *uart_tx_gpio_port, *uart_rx_gpio_port;
  uint32_t uart_tx_pin, uart_rx_pin, uart_gpio_alt;
  IRQn_Type irqn;

  if (huart->Instance == USART1) {
    __GPIOA_CLK_ENABLE();
    __USART1_CLK_ENABLE();
    uart_tx_pin = GPIO_PIN_9;
    uart_rx_pin = GPIO_PIN_10;
    uart_gpio_alt = GPIO_AF7_USART1;
    uart_tx_gpio_port = GPIOA;
    uart_rx_gpio_port = GPIOA;
    irqn = USART1_IRQn;
  } else if (huart->Instance == USART2) {
    __GPIOA_CLK_ENABLE();
    __USART2_CLK_ENABLE();
    uart_tx_pin = GPIO_PIN_2;
    uart_rx_pin = GPIO_PIN_3;
    uart_gpio_alt = GPIO_AF7_USART2;
    uart_tx_gpio_port = GPIOA;
    uart_rx_gpio_port = GPIOA;
    irqn = USART2_IRQn;
  } else if (huart->Instance == USART3) {
    __GPIOB_CLK_ENABLE();
    __USART3_CLK_ENABLE();
    uart_tx_pin = GPIO_PIN_10;
    uart_rx_pin = GPIO_PIN_11;
    uart_gpio_alt = GPIO_AF7_USART3;
    uart_tx_gpio_port = GPIOB;
    uart_rx_gpio_port = GPIOB;
    irqn = USART3_IRQn;
  } else if (huart->Instance == UART4) {
    __GPIOC_CLK_ENABLE();
    __UART4_CLK_ENABLE();
    uart_tx_pin = GPIO_PIN_10;
    uart_rx_pin = GPIO_PIN_11;
    uart_gpio_alt = GPIO_AF5_UART4;
    uart_tx_gpio_port = GPIOC;
    uart_rx_gpio_port = GPIOC;
    irqn = UART4_IRQn;
  } else if (huart->Instance == UART5) {
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();
    __UART5_CLK_ENABLE();
    uart_tx_pin = GPIO_PIN_12;
    uart_rx_pin = GPIO_PIN_2;
    uart_gpio_alt = GPIO_AF5_UART5;
    uart_tx_gpio_port = GPIOC;
    uart_rx_gpio_port = GPIOD;
    irqn = UART5_IRQn;
  }

  /* setup gpio */
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  GPIO_InitStruct.Pin = uart_tx_pin;
  GPIO_InitStruct.Alternate = uart_gpio_alt;
  HAL_GPIO_Init(uart_tx_gpio_port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = uart_rx_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  HAL_GPIO_Init(uart_rx_gpio_port, &GPIO_InitStruct);

  /* setup interrupt */
  HAL_NVIC_SetPriority(irqn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1, 1);
  HAL_NVIC_EnableIRQ(irqn);
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART1) {
    __USART1_FORCE_RESET();
    __USART1_RELEASE_RESET();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);
  } else if (huart->Instance == USART2) {
    __USART2_FORCE_RESET();
    __USART2_RELEASE_RESET();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);
  } else if (huart->Instance == USART3) {
    __USART3_FORCE_RESET();
    __USART3_RELEASE_RESET();
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);
  } else if (huart->Instance == UART4) {
    __UART4_FORCE_RESET();
    __UART4_RELEASE_RESET();
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_11);
  } else if (huart->Instance == UART5) {
    __UART5_FORCE_RESET();
    __UART5_RELEASE_RESET();
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_12);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);
  }
}

void uart_task_init() {
  osThreadDef(UART_TASK, uart_task, osPriorityLow, 0, configMINIMAL_STACK_SIZE);
  uart_task_handle = osThreadCreate(osThread(UART_TASK), NULL);
}

static uint8_t uart_id_lookup(USART_TypeDef *instance) {
  if (instance == USART1) {
    return 1;
  } else if (instance == USART2) {
    return 2;
  } else if (instance == USART3) {
    return 3;
  } else if (instance == UART4) {
    return 4;
  } else if (instance == UART5) {
    return 5;
  } else {
    return 0;
  }
}

static USART_TypeDef *uart_instance_lookup(uint8_t uid) {
  switch (uid) {
    case 1:
      return USART1;
    case 2:
      return USART2;
    case 3:
      return USART3;
    case 4:
      return UART4;
    case 5:
      return UART5;
    default:
      return NULL;
  }
}

enum UartReturnVal uart_context_init(uint8_t uid, struct UartContext *ctxt, uint32_t baudrate) {
  USART_TypeDef *instance = uart_instance_lookup(uid);

  if (!instance) {
    return UART_ERR_INVALID_UART_DEVICE_ID;
  }

  ctxt->handle.Instance = instance;
  ctxt->handle.Init.BaudRate = baudrate;
  ctxt->handle.Init.WordLength = UART_WORDLENGTH_8B;
  ctxt->handle.Init.StopBits = UART_STOPBITS_1;
  ctxt->handle.Init.Parity = UART_PARITY_NONE;
  ctxt->handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  ctxt->handle.Init.Mode = UART_MODE_TX_RX;

  ctxt->uartMutex = osMutexCreate(NULL);
  ctxt->txBuf.mutex = osMutexCreate(NULL);
  ctxt->rxBuf.mutex = osMutexCreate(NULL);

  ctxt->txBuf.rd = ctxt->txBuf.wr = 0;
  ctxt->rxBuf.rd = ctxt->rxBuf.wr = 0;

  gUartContexts[uid - 1] = ctxt;

  if (HAL_UART_Init(&(ctxt->handle)) != HAL_OK) {
    return UART_ERR_FAIL_TO_INITIALIZE;
  }

  return UART_OK;
}

enum UartReturnVal uart_send(uint8_t uid, void *pData, size_t size) {
  if (uid < 1 || uid > UART_NUM_DEVICE) {
    return UART_ERR_INVALID_UART_DEVICE_ID;
  }

  struct UartContext *ctxt = gUartContexts[uid - 1];

  if (ctxt == NULL) {
    return UART_ERR_DEVICE_NOT_YET_INITIALIZED;
  }

  if (ringbuf_write(&(ctxt->txBuf), pData, size) != RINGBUF_OK) {
    return UART_ERR_TX_BUFFER_IS_FULL;
  }
  osSignalSet(uart_task_handle, 0x01);

  return UART_OK;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  uint8_t uid = uart_id_lookup(huart->Instance);
  osSignalSet(uart_task_handle, 0x01);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  uint8_t uid = uart_id_lookup(huart->Instance);
}

void HAL_UARTEx_WakeupCallback(UART_HandleTypeDef *huart) {
}

static void uart_task(void const *argument) {
  for (;;) {
    osSignalWait(0x01, osWaitForever);
    BSP_LED_Toggle(LED9);
    for (int i = 0; i < UART_NUM_DEVICE; i++) {
      if (gUartContexts[i] == NULL) {
        continue;
      }

      struct UartContext *ctxt = gUartContexts[i];

      BSP_LED_Toggle(LED8);
      if (ringbuf_avaliable(&(ctxt->txBuf), true) > 0) {
        uint16_t sentSize = 0;
        uint16_t curRd = ctxt->txBuf.rd;

        BSP_LED_Toggle(LED7);
        osMutexWait(ctxt->txBuf.mutex, osWaitForever);
        if (ctxt->txBuf.rd > ctxt->txBuf.wr) {
          sentSize = sizeof(ctxt->txBuf.buf) - ctxt->txBuf.rd;
          ctxt->txBuf.rd = 0;
        } else {
          sentSize = ctxt->txBuf.wr - ctxt->txBuf.rd;
          ctxt->txBuf.rd = ctxt->txBuf.wr;
        }
        osMutexRelease(ctxt->txBuf.mutex);

        BSP_LED_Toggle(LED6);
        HAL_UART_Transmit_IT(&(ctxt->handle), &(ctxt->txBuf.buf[curRd]), sentSize);
      }
    }
    BSP_LED_Toggle(LED5);
  }
}

#ifdef UART_USE_USART1
void USART1_IRQHandler(void) {
  HAL_UART_IRQHandler(&(gUartContexts[0]->handle));
}
#endif

#ifdef UART_USE_USART2
void USART2_IRQHandler(void) {
  HAL_UART_IRQHandler(&(gUartContexts[1]->handle));
}
#endif

#ifdef UART_USE_USART3
void USART3_IRQHandler(void) {
  HAL_UART_IRQHandler(&(gUartContexts[2]->handle));
}
#endif

#ifdef UART_USE_UART4
void UART4_IRQHandler(void) {
  HAL_UART_IRQHandler(&(gUartContexts[3]->handle));
}
#endif

#ifdef UART_USE_UART5
void UART5_IRQHandler(void) {
  HAL_UART_IRQHandler(&(gUartContexts[4]->handle));
}
#endif
