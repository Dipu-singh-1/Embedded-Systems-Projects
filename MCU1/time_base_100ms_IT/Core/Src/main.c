/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "main.h"
#include <string.h>

void SystemClockConfig(void);
void Error_handler(void);
void TIMER6_Init(void);
void GPIO_Init();

TIM_HandleTypeDef htime6;

/*
 * main.c starts HAL_TIM_Base_Start_IT.
 * When Timer count clk = 0 --> TIM6_DAC_IRQHandler --> HAL_TIM_IRQHandler --> HAL_TIM_PeriodElapsedCallback
 */

int main(void){

	HAL_Init();
	SystemClockConfig();
	GPIO_Init();
	// initialize timer6
	TIMER6_Init();

	//start timer6
	HAL_TIM_Base_Start_IT(&htime6);

	while(1);

	return 0;

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

}

void GPIO_Init(){

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef ledgpio;
	ledgpio.Pin = GPIO_PIN_5;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &ledgpio);

}

void TIMER6_Init(void){

	/*
	 * Timer clk fequency = 16Mhz (APB1 bus)
	 * Prescaler = variable (set as 24)
	 * Timer count clk = x
	 * Time base required = 0.1s (100ms)
	 * x = time base required / time period of clk, time period of clk = 1/Timer clk fequency
	 * So, 0.1 = (1/(16e^(6) / (24+1)))*x --> x = 0.1*(16e^(6)/(24+1)) = 64000
	 */
	htime6.Instance	= TIM6;
	htime6.Init.Prescaler = 24; // Prescaler + 1 = 24 + 1 = 25
	htime6.Init.Period = 64000-1; // 0x0<= ARR <= 0xFFFF
	// Initialize/set up the timer (but the it must be fired)
	if(HAL_TIM_Base_Init(&htime6)!= HAL_OK){
		Error_handler();
	}

}

void SystemClockConfig(){

}

void Error_handler(void){

	while(1);
}





