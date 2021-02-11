/*
 * main.c
 *
 *  Created on: Dec 8, 2019
 *      Author: Mohamad
 */
/*
 * using 1MHz internal clock
 * using 1024 prescaler
 * using Timer1
 * Ftimer1 = 1MHz/1024 = 976.5Hz
 * counts needed by timer1/counter to use 1 second is = 977 as integer
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//////////////////////// Declaring & initializing global array of unsigned characters ///////////////////////
	unsigned char time[6] = {9,5,9,5,0,1};
	const unsigned char selections[6] = {0x01,0x02,0x04,0x08,0x10,0x20};
//Timer1 on Compare interrupt service routine
ISR(TIMER1_COMPA_vect)
{
	time[0]++;
	if(time[0] > 9 && time[1]<5)
	{
		time[0] = 0;
		time[1]++;
	}else if(time[1] == 5 && time[0] > 9){
		time[0] = 0;
		time[1] = 0;
		time[2]++;
	}
	if(time[2] > 9 && time[3] < 5){
		time[2] = 0;
		time[3]++;
	}else if(time[3] == 5 && time[2] > 9){
		time[2] = 0;
		time[3] = 0;
		time[4]++;
	}
	if(time[4] > 9 && time[5] < 1)
	{
		time[4] = 0;
		time[5]++;
	}else if(time[4] > 9 && time[5] == 1)
	{
		time[4] = 0;
		time[5] = 0;
	}

}
//External interrupt0 interrupt service routine
ISR(INT0_vect)
{
	time[0] = time[1] = time[2] = time[3] = time[4] = time[5] = 0;
}
//External interrupt1 interrupt service routine
ISR(INT1_vect)
{
	TCCR1B &=~((1<<CS12)|(1<<CS10)); //Disabling Timer/Counter1
}
//External interrupt2 interrupt service routine
ISR(INT2_vect)
{
	TCCR1B |=(1<<CS12)|(1<<CS10); //Enabling Timer/Counter1
}
//Timer/Counter1 initializing function definition
void timer1_init_ctc(unsigned short counts){
	TCCR1A = 0;
	TCCR1B = (1<<CS12)|(1<<CS10)|(1<<WGM12); // starting timer on CTC mode and 1024 prescaler
	TCNT1 = 0;//initializing Timer with zero
	OCR1A = counts; //setting the compare value
	TIMSK |= (1<<OCIE1A); //  Enabling Timer1/Counter output compare interrupt on channel A
}
void _int_init(void){
	// Enabling External interrupts
	MCUCR |= (1<<ISC01)|(1<<ISC11)|(1<<ISC10); //Enabling interrupt0 on falling edge & interrupt1 on rising edge
	MCUCR &=~(1<<ISC00);
	MCUCSR &=~(1<<ISC2); // Enabling interrupt2 event on falling edge
	GICR |= (1<<INT1)|(1<<INT0)|(1<<INT2); //Enabling external interrupts 0,1 and 2
	SREG |=(1<<7); //Enabling Global Interrupts / Setting I-bit in status Register
}
int main(void)
{
	///////////DIO///////////////////////////////////////
	DDRD &=~((1<<2)|(1<<3)); // setting pins 2,3 in PORTD as input
	DDRB &=~(1<<2); // setting pin 2 in PORTD as input
	PORTD |= (1<<2); // enabling internal pull-up resistor
	PORTB |= (1<<2); // enabling internal pull-up resistor
	//Setting Output pins in Port C,D
	DDRA |= 0x3F; // to the multiplexer select
	DDRC |= 0x0F; //setting half PORTC as output to the decoder
	///////////////////////////////////initializing functions call//////////////////////
	_int_init(); //initializing External interrupts 0,1 and 2
	timer1_init_ctc(977); // initializing timer1/counter1 with compare value = 977
	/////////////////test cases///////////////////////////////////////
	//time[0] = 9; time[1] = 5;time[2] = 9; time[3] = 5;
	/*
	 *I have problems using initial values to the clock
	 *What if I used a lot of bitwise operators/operations together without parentheses??
	 */
	while(1)
	{
		unsigned char i = 0;
			for(i = 0; i<6;i++){
				PORTA = selections[i];
				PORTC &=~(0x0F);
				PORTC |=time[i];
				_delay_ms(5);

			}
	}

	return 0;
}

