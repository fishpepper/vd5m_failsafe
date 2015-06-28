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

#define F_CPU 10000000UL
#define F_OSC F_CPU

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <stdio.h>

#define VD5M_RESET (1<<1)
#define VD5M_LED   (1<<0)
#define VD5M_FLICKER_RATE 50

//we want to time out after n ms of no activity
#define NO_ACTIVITY_TIMEOUT_MS 1500
//sampling delay
#define SAMPLING_DELAY_US 100
//how many samples of no change trigger a timeout?
#define NO_ACTIVITY_TIMEOUT_INITVALUE ( NO_ACTIVITY_TIMEOUT_MS*1000L/SAMPLING_DELAY_US )

int main(void) {
	uint8_t  state_now = 0;
	uint8_t  state_old = 0;
	uint16_t no_activity_timeout = NO_ACTIVITY_TIMEOUT_INITVALUE;

	WORK IN PROGRESS, UNTESTED ;)

	//port directions: reset = out
	DDRB = VD5M_RESET;
	//set reset high:
	PORTB = VD5M_RESET;
	//enable pullup on led input
	PORTB |= VD5M_LED;

	//do main loop
	while(1){
		//test if there was activity on the status led
		status_now = (PINB & VD5M_LED);
		if (status_now != status_old){
			//activity detected -> reset timeout
                        no_activity_timeout = NO_ACTIVITY_TIMEOUT_INITVALUE;
                        status_old = status_now;
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
			//wait 100ms
			_delay_ms(100);
			//release reset 
			PORTB |= VD5M_RESET;
			//start all over again
   			no_activity_timeout = NO_ACTIVITY_TIMEOUT_INITVALUE;
		}
	}	
	
}

