/*
 * 010i2c_master_tx_testing.c
 *
 *  Created on: 20 de jul de 2020
 *      Author: tiago
 */

/* I2C Pin
 * PB6 -> SCL
 * PB7 -> SDA
 */

#include <string.h>
#include <stdio.h>
#include "stm32f407xx.h"

I2C_Handle_t I2C1Handle;

// some data
uint8_t some_data[] = "We are testing I2C master Tx\n";

#define MY_ADDR		0x61
#define SLAVE_ADDR 	0x68

void delay(void){
	for(uint32_t i=0;i<500000/2;i++);
}

// configure the ports/pins
void I2C1_GPIOInits(void){
	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//SCL
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = 6;
	GPIO_Init(&SPIPins);

	//SDA
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = 7;
	GPIO_Init(&SPIPins);

}


void I2C1_Inits(void){

	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_ACKControl 	= I2C_ACK_ENABLE;
	// the address doesn't matter because it's in master mode
	// but it cannot use the reserved address (pg. 17 of I2C User Manual)
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle	= I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed 		= I2C_SCL_SPEED_SM;

	I2C_Init(&I2C1Handle);

}


void GPIO_ButtonInit(){

	GPIO_Handle_t GPIOBtn, GpioLed;
	// INPUT = read the state of an electrical signal
	GPIOBtn.pGPIOx 								= 	GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber 		=	GPIO_PIN_NO_0;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode 		= 	GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed		= 	GPIO_SPEED_FAST;
	//GpioLed.GPIO_PinConfig.GPIO_PinOPType		=	GPIO_OP_TYPE_OD; because it is an input
	// there is an internal pull-down register in the board button
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl	=	GPIO_NO_PUPD;

	GPIO_Init(&GPIOBtn);

	// Output = drive a signal HIGH or LOW
	GpioLed.pGPIOx 								= 	GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber 		=	GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode 		= 	GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed		= 	GPIO_SPEED_FAST;
	// GPIOD output has Pull-up and Pull-down resisters.
	// When GPIOD -> HIGH -> signal HIGH, When GPIOD -> LOW -> signal LOW
	GpioLed.GPIO_PinConfig.GPIO_PinOPType		=	GPIO_OP_TYPE_OD; // ATTENTION test OD also....
	// Pull-down or Pull-up resistors are needed for OP (OPEN-DRAIN), instead of PP.
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl	=	GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOD,ENABLE);
	GPIO_Init(&GpioLed);
}

int main(void){

	GPIO_ButtonInit();

	// i2c pin inits
	I2C1_GPIOInits();

	//	i2c peripheral configuration
	I2C1_Inits();

	// enable the i2c peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	while(1){

		// wait until the button is pressed
		while( !GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0) );
		// to avoid de-boucing
		delay();
		// send some data to the slave
		I2C_MasterSendData(&I2C1Handle, some_data, strlen((char*)some_data), SLAVE_ADDR);
	}


	return 0;
}

