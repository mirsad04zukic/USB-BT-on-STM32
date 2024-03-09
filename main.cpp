#include "stm32f10x.h"                  // Device header
#include "GPIODriver.h"
#include <stdio.h>
#include <stdint.h>
#include "string.h"											//string lib
#include "stdlib.h"											//standarad functions lib
#include "stdarg.h"											//standard arguments lib


char data[32]; 
int i=0;	
bool resetuj=false;

void reset(char data[32]){
	for(int i = 0;i < 33;i++) {data[i] = 0;}
}

static void println(char *msg, ...);  
void println(char a);
void BluetoothRead(char data[32]);
void printAr(char data[32]);

int main(void) {
	RCC -> APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;
	
	// USART1 (Bluetooth)
	ZeroRegister(GPIOA,A9);
	GPIOA -> CRH |= (11 << 4);
	
	USART1 -> BRR = 0x1D4C;
	
	USART1 -> CR1 |= (1 << 2);
	USART1 -> CR1 |= (1 << 3);
	USART1 -> CR1 |= (1 << 13);
	
	//USART2 USB
	ZeroRegister(GPIOA,A2);
	GPIOA -> CRL |= (11 << 8);
	
	USART2 -> BRR = 0xEA6; // 36million / 9600 = 3750
	
	USART2 -> CR1 |= (1 << 2 );
	USART2 -> CR1 |= (1 << 3 );
	USART2 -> CR1 |= (1 << 13);
	
	//MAIN LOOP
	for(;;) {
		BluetoothRead(data);
		printAr(data);
				
	}

}


static void println(char *msg, ...) {
	char buffer[150];					//make a 50 byte buffer
	
	va_list args; 				// get the list of arguments
	va_start(args,msg);			// extract argumest from message, reformating the message
	vsprintf(buffer,msg,args);	//formating message acording to passed arguments
	
	//fill the buffer
	
	for(int i=0; i<strlen(buffer); i++) {
		USART2 -> DR=buffer[i];									//byte by byte filing saljem podatake sa TX
		while(!(USART2->SR & USART_SR_TXE)); //fill the array until the TX registar is empty
		
	}

}

 void println(char a) {
	USART2 -> DR = a;
	while(!(USART2->SR & USART_SR_TXE));
}

void BluetoothRead(char data[32]) {

	if(USART1->SR & USART_SR_RXNE) {
		
		while(data[i] != '\n') {
			if(resetuj) {reset(data);resetuj = false;} //Ako je u prethodnom packetu postojao \n resetuj niz
			
			data[i] = USART1->DR; // Kupi bajt i cuva
			i++;
			
			while(!(USART1 -> SR & USART_SR_TC)){;}
			if(i > 32) {i = 0;}				

			if(data[i -1] == '\n') {
				for(int j = i; j < 33;j++) {
				data[j] = 0;
			}
			i = 0; 
			resetuj = true;
			}
		}
	}
}

void printAr(char data[32]) {
	int r=0;
	while(data[r] != 10) {
		println(data[r]);
	}
}
