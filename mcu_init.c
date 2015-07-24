#include "stm32f4xx.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_rcc.h"
#include "math.h"
#include "mcu_init.h"
#include "main.h"

ADC_InitTypeDef ADC_InitStruct;
ADC_CommonInitTypeDef ADC_CommonInitStruct;

void initMCU(void)
{
	initADC();
	getVbat();
	getTemp();
	DevSign = MCU_GetSignature();
	DevRev = MCU_GetRevision();
	FlashSize = MCU_GetFlashSize();
	DevID[0] = MCU_GetUnique32(0);
	DevID[1] = MCU_GetUnique32(1);
	DevID[2] = MCU_GetUnique32(2);
	char str[100];
	sprintf(str, "Dev signature: 0x%04X\r\n", DevSign);
	logToUSART(str);
	sprintf(str, "Dev revision: 0x%04X\r\n", DevRev);
	logToUSART(str);
	sprintf(str, "Flash size: %u kB\r\n", FlashSize);
	logToUSART(str);
	sprintf(str, "Dev unique ID: 0x%04X 0x%04X 0x%04X \r\n", DevID[0], DevID[1], DevID[2]);
	logToUSART(str);
}

void getVbat(void)
{
	char str[150];
	uint32_t result;

	ADC1->CR2 |= (uint32_t)ADC_CR2_ADON; // Enable ADC
	ADC->CCR |= ADC_CCR_VBATE;			// Enables the VBAT (Voltage Battery) channel.
	ADC_RegularChannelConfig(ADC1, ADC_Channel_Vbat, 1, ADC_SampleTime_112Cycles);
	ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART; // Start software conversion
	while (!(ADC1->SR & ADC_SR_EOC));
	result = ADC1->DR; //Returns the last ADC1 conversion result data for regular channel
	ADC1->CR2 &= (uint32_t)(~ADC_CR2_ADON);	// Disable ADC

	result = result * 2 * 3300 / 0xFFF;	// Convert to voltage
	int d1 = (int)(result / 1000);
	int d2 = (int)(result % 1000);
	Vbat = d1 + d2 * 0.001f;
	sprintf (str, "Vbat voltage: %d.%d V\r\n", d1, d2);
	logToUSART(str);
}

void getTemp(void)
{
	char str[150];
	ADC1->CR2 |= (uint32_t)ADC_CR2_ADON;	// Enable ADC
	ADC->CCR |= (uint32_t)ADC_CCR_TSVREFE;	// Enables the temperature sensor and Vrefint channels
	Delay(200);								// Time for normalization. Page 414 of datasheet
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_144Cycles);
	ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART; //Enables the selected ADC software start conversion of the regular channels.
	while (!(ADC1->SR & ADC_SR_EOC));	// Wait till done
	CoreTemp = ADC1->DR;	//Returns the last ADC1 conversion result data for regular channel
	ADC1->CR2 &= (uint32_t)(~ADC_CR2_ADON);	// Disable ADC

	CoreTemp *= 3300;
	CoreTemp /= 0xfff; //Reading in mV
	CoreTemp /= 1000.0; //Reading in Volts
	CoreTemp -= 0.760; // Subtract the reference voltage at 25°C
	CoreTemp /= .0025; // Divide by slope 2.5mV
	CoreTemp += 25.0; // Add the 25°C
	int d1 = (int)CoreTemp;
	int d2 = ((int)(CoreTemp * 1000) % 1000);
	sprintf (str, "CoreTemp: %d.%d C\r\n", d1, d2);
	logToUSART(str);
}

void initADC(void)
{
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConv = DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_NbrOfConversion = 1;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_8Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);
	ADC_Init(ADC1, &ADC_InitStruct);
}

