/**
******************************************************************************
* @file    MicoDriverUart.c 
* @author  William Xu
* @version V1.0.0
* @date    05-May-2014
* @brief   This file provide UART driver functions.
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2014 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy 
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************
*/ 


#include "MICORTOS.h"
#include "MICOPlatform.h"

#include "platform.h"
#include "platform_common_config.h"

#include "gpio_irq.h"

#include "string.h"

/******************************************************
*                    Constants
******************************************************/


/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/
/* Ring buffer size */
#define UART_RB_SIZE 128

#define LPC_USART       LPC_USART0
#define LPC_IRQNUM      UART0_IRQn
#define LPC_UARTHNDLR   UART0_IRQHandler

/* Transmit and receive buffers */
static uint8_t rxbuff[UART_RB_SIZE], txbuff[UART_RB_SIZE];

/******************************************************
*                    Structures
******************************************************/

typedef struct
{
  uint32_t            rx_size;
  ring_buffer_t*      rx_buffer;
#ifndef NO_MICO_RTOS
  mico_semaphore_t    rx_complete;
  mico_semaphore_t    tx_complete;
#else
  volatile bool       rx_complete;
  volatile bool       tx_complete;
#endif
  mico_semaphore_t    sem_wakeup;
  OSStatus            tx_dma_result;
  OSStatus            rx_dma_result;
} uart_interface_t;

/******************************************************
*               Variables Definitions
******************************************************/

static uart_interface_t uart_interfaces[NUMBER_OF_UART_INTERFACES];

#ifndef NO_MICO_RTOS
static mico_uart_t current_uart;
#endif

/******************************************************
*               Function Declarations
******************************************************/

static OSStatus internal_uart_init ( mico_uart_t uart, const mico_uart_config_t* config, ring_buffer_t* optional_rx_buffer );
static OSStatus platform_uart_receive_bytes( mico_uart_t uart, void* data, uint32_t size, uint32_t timeout );


/* Interrupt service functions - called from interrupt vector table */
#ifndef NO_MICO_RTOS
static void thread_wakeup(void *arg);
static void RX_PIN_WAKEUP_handler(void *arg);
#endif


/******************************************************
*               Function Definitions
******************************************************/

OSStatus MicoUartInitialize( mico_uart_t uart, const mico_uart_config_t* config, ring_buffer_t* optional_rx_buffer )
{
  
}


OSStatus MicoStdioUartInitialize( const mico_uart_config_t* config, ring_buffer_t* optional_rx_buffer )
{
  return internal_uart_init(STDIO_UART, config, optional_rx_buffer);
}


OSStatus internal_uart_init( mico_uart_t uart, const mico_uart_config_t* config, ring_buffer_t* optional_rx_buffer )
{
  
  return kNoErr;
}

OSStatus MicoUartFinalize( mico_uart_t uart )
{
  
  
  return kNoErr;
}

OSStatus MicoUartSend( mico_uart_t uart, const void* data, uint32_t size )
{
//  /* Reset DMA transmission result. The result is assigned in interrupt handler */
//  uart_interfaces[uart].tx_dma_result = kGeneralErr;
//  
  MicoMcuPowerSaveConfig(false);  
//  
//  uart_mapping[uart].tx_dma_stream->CR  &= ~(uint32_t) DMA_SxCR_CIRC;
//  uart_mapping[uart].tx_dma_stream->NDTR = size;
//  uart_mapping[uart].tx_dma_stream->M0AR = (uint32_t)data;
//  
//  USART_DMACmd( uart_mapping[uart].usart, USART_DMAReq_Tx, ENABLE );
//  USART_ClearFlag( uart_mapping[uart].usart, USART_FLAG_TC );
//  DMA_Cmd( uart_mapping[uart].tx_dma_stream, ENABLE );
//  
//#ifndef NO_MICO_RTOS
//  mico_rtos_get_semaphore( &uart_interfaces[ uart ].tx_complete, MICO_NEVER_TIMEOUT );
//#else 
//  while(uart_interfaces[ uart ].tx_complete == false);
//  uart_interfaces[ uart ].tx_complete = false;
//#endif
//  
  MicoMcuPowerSaveConfig(true);

  return kNoErr;
}

OSStatus MicoUartRecv( mico_uart_t uart, void* data, uint32_t size, uint32_t timeout )
{

  return kNoErr;
}


static OSStatus platform_uart_receive_bytes( mico_uart_t uart, void* data, uint32_t size, uint32_t timeout )
{

  
  return kNoErr;
}


uint32_t MicoUartGetLengthInBuffer( mico_uart_t uart )
{
  return 0; //ring_buffer_used_space( uart_interfaces[uart].rx_buffer );
}

#ifndef NO_MICO_RTOS
static void thread_wakeup(void *arg)
{
  mico_uart_t uart = *(mico_uart_t *)arg;
  
  while(1){
//     if(mico_rtos_get_semaphore(&uart_interfaces[ uart ].sem_wakeup, 1000) != kNoErr){
//      gpio_irq_enable(uart_mapping[uart].pin_rx->bank, uart_mapping[uart].pin_rx->number, IRQ_TRIGGER_FALLING_EDGE, RX_PIN_WAKEUP_handler, &uart);
//      MicoMcuPowerSaveConfig(true);
//    }   
  }
}
#endif

/******************************************************
*            Interrupt Service Routines
******************************************************/
#ifndef NO_MICO_RTOS
void RX_PIN_WAKEUP_handler(void *arg)
{
  (void)arg;
  mico_uart_t uart = *(mico_uart_t *)arg;
  
//  RCC_AHB1PeriphClockCmd(uart_mapping[ uart ].pin_rx->peripheral_clock, ENABLE);
//  uart_mapping[ uart ].usart_peripheral_clock_func ( uart_mapping[uart].usart_peripheral_clock,  ENABLE );
//  uart_mapping[uart].rx_dma_peripheral_clock_func  ( uart_mapping[uart].rx_dma_peripheral_clock, ENABLE );
//  
//  gpio_irq_disable(uart_mapping[uart].pin_rx->bank, uart_mapping[uart].pin_rx->number); 
  
  MicoMcuPowerSaveConfig(false);
  mico_rtos_set_semaphore(&uart_interfaces[uart].sem_wakeup);
}
#endif

// end file






