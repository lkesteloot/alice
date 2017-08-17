#include <stm32f4xx_hal.h>
#include "defs.h"
#include "uart.h"

/*--------------------------------------------------------------------------*/
/* USART - serial comms ----------------------------------------------------*/

static UART_HandleTypeDef gUARTHandle;

#define TRANSMIT_BUFFER_SIZE 512
volatile unsigned char gTransmitBuffers[2][TRANSMIT_BUFFER_SIZE];
volatile int gNextTransmitBuffer = 0;
volatile int gTransmitBufferLengths[2] = {0, 0};
volatile int gUARTTransmitBusy = 0;

void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&gUARTHandle);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  // __HAL_RCC_GPIOB_CLK_ENABLE();
  
  /* Enable USART clock */
  __HAL_RCC_USART1_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_6;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* NVIC for USART */
  HAL_NVIC_SetPriority(USART1_IRQn, 3, 1);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_USART1_FORCE_RESET();
    __HAL_RCC_USART1_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    gUARTTransmitBusy = 0;
}

void SERIAL_try_to_transmit_buffers()
{
    if(!gUARTTransmitBusy && gTransmitBufferLengths[gNextTransmitBuffer] > 0) {
        gUARTTransmitBusy = 1;

        if(HAL_UART_Transmit_IT(&gUARTHandle, (uint8_t *)&gTransmitBuffers[gNextTransmitBuffer][0], gTransmitBufferLengths[gNextTransmitBuffer]) != HAL_OK) {
            panic();
        }

        gNextTransmitBuffer ^= 1;
        gTransmitBufferLengths[gNextTransmitBuffer] = 0;
    }
}

void SERIAL_flush()
{
    while(gUARTTransmitBusy || gTransmitBufferLengths[gNextTransmitBuffer] > 0)
        SERIAL_try_to_transmit_buffers();
}

void SERIAL_enqueue_one_char(char c)
{
    do {
        // Transmit the current buffer if there is one and serial
        // port is not busy
        SERIAL_try_to_transmit_buffers();

        // While there's no room in the current buffer, repeat until buffer becomes available
    } while(gTransmitBufferLengths[gNextTransmitBuffer] >= TRANSMIT_BUFFER_SIZE);

    int length = gTransmitBufferLengths[gNextTransmitBuffer];
    gTransmitBuffers[gNextTransmitBuffer][length] = c;
    gTransmitBufferLengths[gNextTransmitBuffer]++;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    panic();
}

volatile char gSerialCharBuffer;
volatile char gStartAnotherUARTReceive = 1;

extern void uart_received(char c);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uart_received(gSerialCharBuffer);
    gStartAnotherUARTReceive = 1;
}

void SERIAL_init()
{
    /*##-1- Configure the UART peripheral ######################################*/
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* UART1 configured as follow:
        - Word Length = 8 Bits
        - Stop Bit = One Stop bit
        - Parity = ODD parity
        - BaudRate = 115200 baud
        - Hardware flow control disabled (RTS and CTS signals) */
    gUARTHandle.Instance          = USART1;
    
    gUARTHandle.Init.BaudRate     = 115200;
    gUARTHandle.Init.WordLength   = UART_WORDLENGTH_8B;
    gUARTHandle.Init.StopBits     = UART_STOPBITS_1;
    gUARTHandle.Init.Parity       = UART_PARITY_NONE;
    gUARTHandle.Init.HwFlowCtl    = UART_HWCONTROL_RTS_CTS;
    gUARTHandle.Init.Mode         = UART_MODE_TX_RX;
    gUARTHandle.Init.OverSampling = UART_OVERSAMPLING_16;
      
    if(HAL_UART_Init(&gUARTHandle) != HAL_OK)
    {
      /* Initialization Error */
      panic(); 
    }

    if(HAL_UART_Receive_IT(&gUARTHandle, (uint8_t *)&gSerialCharBuffer, 1) != HAL_OK)
    {
      /* Transfer error in reception process */
      panic();
    }
}

void SERIAL_poll_continue()
{
    // This is terrible; UART interrupt should fill a buffer and we should examine in here, not poll
    if(gStartAnotherUARTReceive) {
        gStartAnotherUARTReceive = 0;
        int result;
        if((result = HAL_UART_Receive_IT(&gUARTHandle, (uint8_t *)&gSerialCharBuffer, 1)) != HAL_OK) {
            /* error_code = gUARTHandle.State; */
        }
    }
}

