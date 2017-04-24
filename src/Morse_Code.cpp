/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#define TICKRATE_HZ1 (10000)
#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif
#include <iomanip>
#include <locale>
#include <math.h>


#include <cr_section_macros.h>


static volatile int counter;
#ifdef __cplusplus
extern "C" {
#endif
/**
* @brief Handle interrupt from SysTick timer
* @return Nothing
*/
void SysTick_Handler(void)
{
	if(counter > 0) counter--;
}
#ifdef __cplusplus
}
#endif

#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

using namespace std;

void Sleep(int ms)
{
counter = ms;
	while(counter > 0) {
	__WFI();
	}
}

///////////////////////////////////////////////////////////////////////////////
const int UNITMS = 250;
const int DOT = 1;
const int DASH = 3;
struct MorseCode {
char symbol;
unsigned char code[7];
};
const MorseCode ITU_morse[] = {
	{ 'A', { DOT, DASH } }, // A
	{ 'B', { DASH, DOT, DOT, DOT } }, // B
	{ 'C', { DASH, DOT, DASH, DOT } }, // C
	{ 'D', { DASH, DOT, DOT } }, // D
	{ 'E', { DOT } }, // E
	{ 'F', { DOT, DOT, DASH, DOT } }, // F
	{ 'G', { DASH, DASH, DOT } }, // G
	{ 'H', { DOT, DOT, DOT, DOT } }, // H
	{ 'I', { DOT, DOT } }, // I
	{ 'J', { DOT, DASH, DASH, DASH } }, // J
	{ 'K', { DASH, DOT, DASH } }, // K
	{ 'L', { DOT, DASH, DOT, DOT } }, // L
	{ 'M', { DASH, DASH } }, // M
	{ 'N', { DASH, DOT } }, // N
	{ 'O', { DASH, DASH, DASH } }, // O
	{ 'P', { DOT, DASH, DASH, DOT } }, // P
	{ 'Q', { DASH, DASH, DOT, DASH } }, // Q
	{ 'R', { DOT, DASH, DOT } }, // R
	{ 'S', { DOT, DOT, DOT } }, // S
	{ 'T', { DASH } }, // T
	{ 'U', { DOT, DOT, DASH } }, // U
	{ 'V', { DOT, DOT, DOT, DASH } }, // V
	{ 'W', { DOT, DASH, DASH } }, // W
	{ 'X', { DASH, DOT, DOT, DASH } }, // X
	{ 'Y', { DASH, DOT, DASH, DASH } }, // Y
	{ 'Z', { DASH, DASH, DOT, DOT } }, // Z
	{ '1', { DOT, DASH, DASH, DASH, DASH } }, // 1
	{ '2', { DOT, DOT, DASH, DASH, DASH } }, // 2
	{ '3', { DOT, DOT, DOT, DASH, DASH } }, // 3
	{ '4', { DOT, DOT, DOT, DOT, DASH } }, // 4
	{ '5', { DOT, DOT, DOT, DOT, DOT } }, // 5
	{ '6', { DASH, DOT, DOT, DOT, DOT } }, // 6
	{ '7', { DASH, DASH, DOT, DOT, DOT } }, // 7
	{ '8', { DASH, DASH, DASH, DOT, DOT } }, // 8
	{ '9', { DASH, DASH, DASH, DASH, DOT } }, // 9
	{ '0', { DASH, DASH, DASH, DASH, DASH } }, // 0
	{ ' ', { 0 } }, // space
	{ 0, { 0 } } // terminating entry - Do not remove!
};

///////////////////////////////////////////////////////////////////////////////

class DigitalIoPin {
	int port;
	int pin;
	bool hold;
	public:
		DigitalIoPin (int, int, bool, bool, bool);
		bool read();
		int count();
	private:
};

DigitalIoPin :: DigitalIoPin(int setPin, int setPort, bool input,bool pullup, bool invert) {
	//ITM_write("IoPin object added.\n");
	port = setPort;
	pin = setPin;
	//int ModeSet = IOCON_MODE_PULLUP;
	Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN | IOCON_INV_EN));
	if (input) {
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
	} else {
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
	}
}


bool DigitalIoPin :: read(){
	return Chip_GPIO_GetPinState(LPC_GPIO, port, pin);
}

int DigitalIoPin :: count (){
	int timeInMs = 0;
	while (Chip_GPIO_GetPinState(LPC_GPIO, port, pin)) {
				Sleep(10);
				timeInMs++;
	}
	return timeInMs * 10;
}

///////////////////////////////////////////////////////////////////////////////

class MorseCodeSender {
	public:
		MorseCodeSender();
		int Setting();
		int CalculateUnit();
		void PrintCurrentSetting();
		void Command();
		void InputOutputMorseCode();
		//void WriteSymbol(int);
	private:
		int word;
		char wpm[3];
		int unit;
		int value;
};

int CheckA(int a){
	if(48<a && a<58 ){a = a - 23;}
	if(a == 48){a = 35;}
	if(a == 13){a = '\0';}
	return a;
}

MorseCodeSender::MorseCodeSender(void){}

void MorseCodeSender::Command(){
	Board_UARTPutSTR("\r\Kirjoita komento jatkaaksesi ... (wpm,set tai send)\r\n");
	char typeIn[5] = {0,0,0,0,0};
	char WPM[4] = {'w', 'p', 'm', 0};
	char SET[4] = "set";
	char SEND[5] = "send";
	char b;
	int B;
	int i;
	i = 0;
	while(1){
		b = Board_UARTGetChar();
		B = b;
		if( b>47 && b<123 && i<4){
			Board_UARTPutChar(b);
			typeIn[i]=b;
			i++;
		}

		if (B == 13){
			if(strcmp(typeIn, WPM) == 0 ){
				Board_UARTPutSTR("\r\n\wpm on\r\n");
				Setting();
				CalculateUnit();

			}
			if(strcmp(typeIn, SET) == 0 ){
				Board_UARTPutSTR("\r\n\set on\r\n");
				Board_UARTPutSTR("\r\n\wpm: ");
				Board_UARTPutSTR(wpm);
				Board_UARTPutSTR("\r\n");
				char dotLength[4] = {0,0,0,0};
				sprintf(dotLength,"%d",unit);
				Board_UARTPutSTR("\r\n\dot length: ");
				Board_UARTPutSTR(dotLength);
				Board_UARTPutSTR("\r\n");

			}
			if(strcmp(typeIn, SEND) == 0 ){
				Board_UARTPutSTR("\r\n\send on\r\n");
				InputOutputMorseCode();
			}
			break;
		}
	}

}

int MorseCodeSender::Setting(void){
	value = 0;
	for(int i = 0; i < 3; i++){wpm[i] = 0;}
	int i = 0;
	char a;
	char x;

	while(1){
		a = Board_UARTGetChar();
		if (a > 47 && a < 58 && i < 3) {
			Board_UARTPutChar(a);
			wpm[i] = a;
			a = a - 48;
			value = a + (value * 10);		//number in decimal
			i++;
		}

		if (a == 13 || i == 3) {
			Board_UARTPutChar('\r');
			Board_UARTPutChar('\n');
			Board_UARTPutSTR("\r\n\wpm in setting is : ");
			Board_UARTPutSTR(wpm);
			Board_UARTPutChar('\r');
			Board_UARTPutChar('\n');
			char VALUE[4] = {0,0,0,0};
			sprintf(VALUE,"%d",value);
			Board_UARTPutSTR("\r\nvalue: ");
			Board_UARTPutSTR(VALUE);
			Board_UARTPutChar('\r');
			Board_UARTPutChar('\n');
			Board_UARTPutChar('\r');
			Board_UARTPutChar('\n');
			return value;
			break;
		}


	}

}


int MorseCodeSender::CalculateUnit(void){
	//int total = Setting();
	int min;
	int word = 600;
	unit = 0;
	min = TICKRATE_HZ1*60;
	unit = (min/value)/word;
	return unit;
}

void MorseCodeSender::PrintCurrentSetting(void){
	for(int i=0; i<10; i++){
		Board_UARTPutChar(wpm[i]);
	}
}



int SetLed(bool state){
	Board_LED_Set(1,state);
	return 0;
}

int OutPutPin(bool state){
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 8, state);
	return 0;
}

void SendMorse(bool state) {
	SetLed(!state);
	OutPutPin(state);
}

int CheckX(int x){
	if(x == 32){x = 36;}
	if(x == 48){x = 35;}
	if(x == 0){x = 37;}
	if(96<x && x<123 ){x = x - 97;}
	if(48<x && x<58 ){x = x - 23;}
	if(64<x && x<91 ){x = x - 65;}
	return x;
}

void /*MorseCodeSender::*/WriteSymbol(int x){
	Board_UARTPutChar(ITU_morse[x].symbol);
	Board_UARTPutChar(':');
	Board_UARTPutChar(' ');
}

void MorseCodeSender::InputOutputMorseCode(){
	int c;
	int n=0;

	unsigned char codeSeq[7];
	char x;
	char array[80];
	char z;
	int code;
	int letter;
	int arrayIndex = 0;

	while(1){
		letter = Board_UARTGetChar();
		//number store in letter
		// space in putty
		if(letter == 32){Board_UARTPutSTR("   ");}
		if(letter > 31 ){
			x = letter;
			x = CheckX(x);
			// x different from morse code write X
			if ((letter > 32 && letter < 48) || (letter > 57 && letter < 65) || (letter > 90 && letter < 97) || letter > 122){x = 23; letter = 88;}
			WriteSymbol(x);
			for (int i=0; i<7; i++) {
				code = ITU_morse[x].code[i];
				if (code == 1) {
					Board_UARTPutSTR(". ");
				} else if (code == 3 ) {
					Board_UARTPutSTR("- ");
				}
			}
			array[n] = letter;
			if (array[n] == letter){n++;}
		}
		//if(letter < 31 ||  )
		//print
		if ( n>80 || letter == 13 ){
			char UnitNow[4] = {0,0,0,0};
			sprintf(UnitNow,"%d",unit);
			Board_UARTPutSTR("\r\nUnit now: ");
			Board_UARTPutSTR(UnitNow);
			Board_UARTPutSTR("\r\n");

			array[n]= '\0';
			for (n=0; n<80; n++){
				x = array[n];
				Board_UARTPutChar(x);
				x = CheckX(x);
				// 36 = space
				if (x != 36){
					for (int i=0; i<7; i++){
						code = ITU_morse[x].code[i];
						if (code == DOT){
							SendMorse(true);
							Sleep(unit*10);
							SendMorse(false);
							Sleep(unit*10);
						} else if (code == DASH) {
							SendMorse(true);
							Sleep(unit*30);
							SendMorse(false);
							Sleep(unit*10);
						} else {
							// End of code, exit for-loop
							SendMorse(false);
							Sleep(unit*20);
							break;
						}
					}
				// Character is space, 7 unit pause
				} else {
					SendMorse(false);
					Sleep(unit*50);
				}

				if (x == 37){
					Board_UARTPutChar(':');
					Board_UARTPutChar('\r');
					Board_UARTPutChar('\n');
					Board_LED_Set(1,false);
					for (int n=79; n>0; n--){
						array[n] = '\0';
					}
				n = 0;
				break;
				}
			}
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

/*void Test1(void) {
	char testStr0[] = "I am a test";
	char testStr1[] = "I am not a test";
	printf("Lol Test");

	Board_UARTPutSTR("\r\n\Test 1\r\n");
	Board_UARTPutSTR(testStr0);
	if (strcmp(testStr0, testStr1) == 0) {
		Board_UARTPutSTR("\r\n\Match!");
	} else {
		Board_UARTPutSTR("\r\n\ - no match -");
	}
	while(1);
}*/

int main(void) {
	uint32_t sysTickRate;
#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable

#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
#endif
#endif
    SystemCoreClockUpdate();
	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / TICKRATE_HZ1);


    Board_UARTPutSTR("\r\nMoi\r\n");
    Board_UARTPutChar('!');
    Board_UARTPutChar('\r');
    Board_UARTPutChar('\n');
    DigitalIoPin button1 (17, 0, true, true, false);
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 8, (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN | IOCON_INV_EN));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 8);

    //Board_UARTPutSTR("\r\Print something before showing single characters...?\r\n");

    //Test1();
    MorseCodeSender Set;

	while(1) {
		Set.Command();
    }
    return 0 ;
}
