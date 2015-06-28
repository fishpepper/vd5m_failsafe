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

//lets assume the led flickers with VD5M_FLICKER_RATE Hz
//so lets see if we have activity within the last 1000/VD5M_FLICKER_RATE ms
//we sample 16x within 2x this time
#define SAMPLING_DELAY_MS ((double)((1000.0/VD5M_FLICKER_RATE)*2/16))
//we want to time out after n ms of no activity
#define NO_ACTIVITY_TIMEOUT_MS 1500
#define NO_ACTIVITY_TIMEOUT_INITVALUE ((NO_ACTIVITY_TIMEOUT_MS)/(16*SAMPLING_DELAY_MS))

int main(void) {
	uint16_t status = 0;
	uint8_t  state  = 0;
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
		_delay_ms(SAMPLING_DELAY_MS);
		state++;

		if (state < 16){
			//shift in data:
			status = (status<<1) | ((PINB & VD5M_LED)?1:0);
		}else{
			//ok, sampling done, lets see if there was activity:
			if ((status == 0x0000) || (status == 0xFFFF)){
				//no activity, either always high or always low!
				//decrease no activity counter:
				no_activity_timeout--;
			}else{
				//activity! reset timeout:
				no_activity_timeout = NO_ACTIVITY_TIMEOUT_INITVALUE;
			}

			//restart with sampling
			state = 0;
		}

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
			state = 0;
			status = 0;
   			no_activity_timeout = NO_ACTIVITY_TIMEOUT_INITVALUE;
		}
	}	
	
}

