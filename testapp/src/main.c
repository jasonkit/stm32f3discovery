#include "main.h"
#include "uart.h"
#include "cmsis_os.h"

osThreadId main_task_handle;

struct UartContext uartContext;

#define UART_ID_IN_USE 1

static void SystemClock_Config(void);
static void Error_Handler(int);
static void main_task(void const *argument);

static void wakeMainTask(void) {
  osSignalSet(main_task_handle, 0x01);
}

int main(void) {
  HAL_Init();

  /* Configure the system clock to 72 MHz */
  SystemClock_Config();

  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);
  BSP_LED_Init(LED6);
  BSP_LED_Init(LED7);
  BSP_LED_Init(LED8);
  BSP_LED_Init(LED9);
  BSP_LED_Init(LED10);

  uart_task_init();

  /* Init uart */
  if (uart_context_init(UART_ID_IN_USE, &uartContext, 115200, 16, wakeMainTask) != UART_OK) {
    Error_Handler(3);
  }

  osThreadDef(MAIN_TASK, main_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  main_task_handle = osThreadCreate(osThread(MAIN_TASK), NULL);

  osKernelStart();

  for (;;)
    ;
}

int gCnt = 0;

static void main_task(void const *argument) {
  struct UartContext *ctxt = gUartContexts[UART_ID_IN_USE - 1];
  for (;;) {
    /*
    char buf[5];
    buf[0] = (gCnt / 1000) + '0';
    buf[1] = ((gCnt % 1000) / 100) + '0';
    buf[2] = ((gCnt % 100) / 10) + '0';
    buf[3] = (gCnt % 10) + '0';
    buf[4] = '\r';
    uart_send(UART_ID_IN_USE, buf, 5);
    gCnt++;
    */
    if (!ringbuf_avaliable(&(ctxt->rxBuf), true)) {
      osSignalWait(0x01, osWaitForever);
    }

    uint8_t buf[16];
    ringbuf_read(&(ctxt->rxBuf), &buf, 16);
    uart_send(UART_ID_IN_USE, &buf, 16);

    BSP_LED_Toggle(LED10);
    //osDelay(50);
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV                     = 1
  *            PLLMUL                         = RCC_PLL_MUL9 (9)
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void) {
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler(2);
  }
}

static void Error_Handler(int type) {
  switch (type) {
    case 1:
      BSP_LED_On(LED3);
      break;
    case 2:
      BSP_LED_On(LED4);
      break;
    case 3:
      BSP_LED_On(LED5);
      break;
    case 4:
      BSP_LED_On(LED6);
      break;
  }
  while (1) {
  }
}
