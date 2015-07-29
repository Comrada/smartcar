/**
 * |----------------------------------------------------------------------
 * | Copyright (C) Pavel Lobyrin, 2015
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "stm32f4xx_dma.h"
#include "string.h"

#include "main.h"
#include "usart.h"
#include "motors.h"
#include "stack.h"

DMA_InitTypeDef  USART_DMA_Struct;

void logToUSART(const char *message)
{
	if (USARTLog == 1 )
	{
		sendToUSART(message);
	}
}

void printJSONToUSART(const char * data)
{
	if (RCMode == 1)
	{
		char * json;
		json = malloc(strlen(data) + 2);
		sprintf(json, "{%s}\r\n", data);
		sendToUSART(json);
		free(json);
	}
}

void sendToUSART(const char *str)
{
	//char * init_buff = "\0";
	//push(sendBuffer, str);

	DMA_DeInit(DMA2_Stream7);
	USART_DMA_Struct.DMA_BufferSize = strlen(str);	//strlen(init_buff);
	USART_DMA_Struct.DMA_Memory0BaseAddr = str;	//(uint32_t)init_buff;
	DMA_Init(DMA2_Stream7, &USART_DMA_Struct);
	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
	DMA2_Stream7->CR |= (uint32_t)DMA_SxCR_EN;
	while (DMA2_Stream7->NDTR);
}

uint8_t parseUSARTCommand(const char * command)
{
	if (strcmp(command, "start") == 0)
	{
		Forward(1);
		return 1;
	} else if (strcmp(command, "stop") == 0)
	{
		Stop();
		return 2;
	} else if (strcmp(command, "left") == 0)
	{
		Left();
		return 3;
	} else if (strcmp(command, "right") == 0)
	{
		Right();
		return 4;
	} else if (strcmp(command, "back") == 0)
	{
		Back(1);
		return 4;
	} else

		return 0;
}

void initUSART(uint32_t baudrate)
{

	char * init_buff = "\0";
	//sendBuffer = newStack();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Struct.GPIO_OType = GPIO_OType_PP;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Struct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_Struct);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

	USART_InitTypeDef USART_Struct;
	USART_Struct.USART_BaudRate = baudrate;
	USART_Struct.USART_WordLength = USART_WordLength_8b;
	USART_Struct.USART_StopBits = USART_StopBits_1;
	USART_Struct.USART_Parity = USART_Parity_No;
	USART_Struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Struct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_Struct);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	// Configure the Receiving Interrupt
	USART_Cmd(USART1, ENABLE);	//// Enables USART1

	NVIC_InitTypeDef NVIC_Struct;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_Struct.NVIC_IRQChannel = DMA2_Stream7_IRQn;
	NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Struct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Struct);

	DMA_DeInit(DMA2_Stream7);
	USART_DMA_Struct.DMA_Channel = DMA_Channel_4;
	USART_DMA_Struct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	USART_DMA_Struct.DMA_BufferSize = strlen(init_buff);
	USART_DMA_Struct.DMA_Memory0BaseAddr = (uint32_t)init_buff;
	USART_DMA_Struct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	USART_DMA_Struct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	USART_DMA_Struct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	USART_DMA_Struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	USART_DMA_Struct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	USART_DMA_Struct.DMA_Mode = DMA_Mode_Normal;
	USART_DMA_Struct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	USART_DMA_Struct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	USART_DMA_Struct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	USART_DMA_Struct.DMA_FIFOMode = DMA_FIFOMode_Enable;
	USART_DMA_Struct.DMA_Priority = DMA_Priority_Medium;

	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	NVIC_EnableIRQ(USART1_IRQn);
	//sendToUSART("\0");
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IRQ Handlers
void DMA2_Stream7_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7))
	{
		/*char * data;
		while (data = top(sendBuffer))
		{
			DMA_DeInit(DMA2_Stream7);
			USART_DMA_Struct.DMA_BufferSize = strlen(data);
			USART_DMA_Struct.DMA_Memory0BaseAddr = (uint32_t)data;
			DMA_Init(DMA2_Stream7, &USART_DMA_Struct);
			DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
			DMA2_Stream7->CR |= (uint32_t)DMA_SxCR_EN; //DMA_Cmd(DMA2_Stream7, ENABLE);
			while (DMA2_Stream7->NDTR) {};
			pop(sendBuffer);
		}*/
		// Clear DMA Stream Transfer Complete interrupt pending bit
		DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
	}
}

void USART1_IRQHandler(void)
{
	// check if the USART1 receive interrupt flag was set
	if( USART_GetITStatus(USART1, USART_IT_RXNE) )
	{
		static uint8_t cnt = 0;
		char t = USART1->DR; // the character from the USART1 data register is saved in t
		if( (t != '\n') && ((t != '\r')) && (cnt < RECEIVE_BUFFER_SIZE) ){
			receive_buffer[cnt] = t;
			cnt++;
		} else{ // otherwise reset the character counter
			receive_buffer[cnt] = '\0';
			cnt = 0;
			if (parseUSARTCommand(receive_buffer));
				//logToUSART(receive_buffer);
			else
			{
				char error[50];
				sprintf(error, "Unknown command: '%s'\r\n", receive_buffer);
				logToUSART(error);
			}
		}
	}
}
