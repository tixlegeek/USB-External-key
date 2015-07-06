
/**
 *					█▀▀▀▀▀█ █▄▄▀█  ▀▄ █▀▀▀▀▀█
 *					█ ███ █   █▀▀▀▄▀█ █ ███ █
 *					█ ▀▀▀ █ █▀▀█    █ █ ▀▀▀ █
 *					▀▀▀▀▀▀▀ █ █▄█ ▀▄▀ ▀▀▀▀▀▀▀
 *					▀▀▀▄ █▀▄███  █ ▀▄███▀▄ ██
 *					  ▀█▀ ▀ ▄▄ ▀ ▀█▀  ▀ ██▀ ▀
 *					▀▄  █▀▀ █▀ ▀█  █ ▀██   ▄█
 *					▀▀▀█▄▄▀█▄▀▄▄█▀██▄ ▄▀ ▀▀ ▀
 *					▀▀   ▀▀ █▄▄  █▀ █▀▀▀█ ▄▀▄
 *					█▀▀▀▀▀█ ▄ █▀ ▄  █ ▀ █▀ ▄▀
 *					█ ███ █   ▀▀█▀ █▀█▀▀██▄▄▀
 *					█ ▀▀▀ █ █ ▀▄█▀██▄█▄▄█  ▀▀
 *					▀▀▀▀▀▀▀ ▀    ▀▀ ▀▀▀  ▀  ▀
 * 					  http://bit.ly/1S1YXIa
 *					
 *	External "<" key for QWERTY keyboards and AZERTY layout.
 *	By Tixlegeek <tixlegeek@whoarehackers.com> @tixlegeek
 *	http://www.tixlegeek.com
 *
 *	Adaptation of:
 *
 * Keyboard example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/usb_keyboard.html
 *
 * Copyright (c) 2008 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "usb_keyboard.h"

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

#define LED_BIT _BV(PD5)
#define BTN_BIT _BV(PD6)

#define KEY_CHREVRON 100

uint16_t idle_count=0;

int main(void)
{
	// disable the watch dog timer
	MCUSR &= ~_BV(WDRF);
	wdt_disable();

	// set for 16 MHz clock
	CPU_PRESCALE(0);

	DDRD = 0xFF;
	DDRD &= ~(BTN_BIT);
	PORTD = 0xFF;

	// Configure timer 0 to generate a timer overflow interrupt every
	// 256*1024 clock cycles, or approx 61 Hz when using 16 MHz clock
	// This demonstrates how to use interrupts to implement a simple
	// inactivity timeout.
	TCCR0A = 0x00;
	TCCR0B = 0x05;
	TIMSK0 = (1<<TOIE0);
	
	// Initialize the USB, and then wait for the host to set configuration.
	// If the Teensy is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured())
	{
		PORTD ^= LED_BIT;
		_delay_ms(50); 	
	}

	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	_delay_ms(1000);

	while (1) 
	{
		PORTD ^= LED_BIT;
		_delay_ms(1000); 	
	}

}

/**
	Timer interrupt, checking for the BTN's pressed
*/
ISR(TIMER0_OVF_vect)
{
	if((PIND & BTN_BIT))
	{
		PORTD |= LED_BIT;
		usb_keyboard_press(KEY_CHREVRON, 0);
		_delay_ms(200);
	}
}




