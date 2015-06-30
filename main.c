/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

   author: fishpepper <AT> gmail.com
*/

#define F_CPU 8000000UL
#define F_OSC F_CPU

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <stdio.h>

//           ,..........
//  (PB0) ---|o        |--- (PB3, RESET)
//  (GND) ---|         |--- (VCC)
//  (PB1) ---|         |--- (PB2)
//           ```````````
// connections:
// 
// VCC ---> prog header pin 1 (square)
// GND ---> prog header pin 3 
// PB0 ---> prog header pin 4 (cc2510 reset)
// PB1 ---> resistor R5, non led side (green activity led signal)
// PB2 ---> piezo buzzer --||-- GND (optional)
//
#define VD5M_RESET (1<<0) 
#define VD5M_LED   (1<<1) 
#define BUZZER_IO  (1<<2)

#define VD5M_FLICKER_RATE 50

//we want to time out after n ms of no activity
#define NO_ACTIVITY_TIMEOUT_MS 1000

//sampling delay
#define SAMPLING_DELAY_US 100

//how many samples of no change trigger a timeout?
#define NO_ACTIVITY_TIMEOUT_INITVALUE ( NO_ACTIVITY_TIMEOUT_MS*1000L/SAMPLING_DELAY_US )

//buzzer duration in ms
#define BUZZER_DURATION_MS 400

//buzzer frequency in Hz
#define BUZZER_FREQ 15000
#define BUZZER_DELAY_US (1000000 / BUZZER_FREQ)

void buzzer_on(uint16_t ms){
	//do buzzer
	while(ms--){
		uint16_t one_ms_loop = 1000 / BUZZER_DELAY_US;
		while(one_ms_loop--){
			PINB |= BUZZER_IO; //toggle io
			_delay_us(BUZZER_DELAY_US);
		}
	}
}

int main(void) {
	//set clock prescaler to 1:1
	CCP = 0xD8;    //enable configuration
	CLKPSR = 0x00;

	uint8_t  state_now = 0;
	uint8_t  state_old = 0;
	uint16_t no_activity_timeout = NO_ACTIVITY_TIMEOUT_INITVALUE;

	//port directions: reset + buzzer = out
	DDRB = VD5M_RESET | BUZZER_IO;

	//set reset high:
	PORTB = VD5M_RESET;

	//enable pullups on all inputs:
	PUEB = ~(PORTB);
	
	buzzer_on(BUZZER_DURATION_MS);

	//do main loop
	while(1){
		//test if there was activity on the status led
		state_now = PINB & VD5M_LED;
		if (state_now != state_old){
			//activity detected -> reset timeout
                        no_activity_timeout = NO_ACTIVITY_TIMEOUT_INITVALUE;
                        state_old = state_now;
		}else{
			//no activity, decrement timeout
			no_activity_timeout--;
		}

		//sampling delay:
		_delay_us(SAMPLING_DELAY_US);

		//do we have a no-activity timeout?
		if (no_activity_timeout == 0){
			//oh, we timed out waiting! -> force reset of VD5M:
			//PULL reset low
			PORTB &= ~(VD5M_RESET);
			//wait 400ms and buzz
			buzzer_on(BUZZER_DURATION_MS);
			//release reset 
			PORTB |= VD5M_RESET;
			//start all over again
   			no_activity_timeout = NO_ACTIVITY_TIMEOUT_INITVALUE;
		}
	}	
	
}

