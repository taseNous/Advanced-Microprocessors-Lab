#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define del 10
#define ped 20

int interr = 0;

ISR(PORTF_PORT_vect){
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intflags; //breakpoint
	interr = 1;
}

ISR(TCA0_CMP0_vect){
	TCA0_SINGLE_CTRLA = 0;
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags; //breakpoint
	interr = 0;
}

int main() {
	while(1){
		int sensor = random() % 9 + 0; //small road sensor

		PORTD_DIR |= PIN0_bm; //big road traffic lights
		PORTD_DIR |= PIN1_bm; //small road traffic lights
		PORTD_DIR |= PIN2_bm; //pedestrian traffic lights
		PORTD_OUTCLR |= PIN0_bm; //on
		PORTD_OUT |= PIN1_bm; //off
		PORTD_OUT |= PIN2_bm; //off

		//pullup enable and Interrupt enabled with sense on both edges
		PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
		
		sei(); //enable interrupts

		//the sensor realizes that there is car on the small road
		while(sensor==0 || sensor==5 || sensor==8){ //breakpoint
			PORTD.OUT |= PIN0_bm;
			PORTD_OUTCLR = PIN1_bm;
			PORTD_OUTCLR = PIN2_bm;

			sensor = random() % 9 + 0;
			}

			PORTD_OUTCLR = PIN0_bm; //on
			PORTD_OUT |= PIN1_bm; //off
			PORTD_OUT |= PIN2_bm; //off

		//when interr = 1, pedestrian presses switch, so it gets out of the while loop
		while (interr==0){
		}

		PORTD.OUT = PIN0_bm;
		PORTD_OUTCLR = PIN1_bm;
		PORTD_OUTCLR = PIN2_bm;
		TCA0.SINGLE.CNT = 0;
		TCA0.SINGLE.CTRLB = 0;
		TCA0.SINGLE.CMP0 = ped;
		TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc;
		TCA0.SINGLE.CTRLA |= 1;
		TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;

		while (interr==1){
		}

		PORTD.OUTCLR = PIN0_bm;
		PORTD_OUT |= PIN1_bm;
		PORTD_OUT |= PIN2_bm;

		cli();
	}
}