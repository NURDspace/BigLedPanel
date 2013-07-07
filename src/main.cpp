#define F_CPU 16000000UL
#define BAUD 9600 

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>
#include "font.h"

/*
 Init global vals
*/
int bufferpos = 0;

unsigned char rxstate;

unsigned char framebuffer[90]={
font[78-32][0],font[78-32][1],font[78-32][2],font[78-32][3],font[78-32][4],
font[85-32][0],font[85-32][1],font[85-32][2],font[85-32][3],font[85-32][4],
font[82-32][0],font[82-32][1],font[82-32][2],font[82-32][3],font[82-32][4],

font[68-32][0],font[68-32][1],font[68-32][2],font[68-32][3],font[68-32][4],
font[115-32][0],font[115-32][1],font[115-32][2],font[115-32][3],font[115-32][4],
font[112-32][0],font[112-32][1],font[112-32][2],font[112-32][3],font[112-32][4],

font[97-32][0],font[97-32][1],font[97-32][2],font[97-32][3],font[97-32][4],
font[99-32][0],font[99-32][1],font[99-32][2],font[99-32][3],font[99-32][4],
font[101-32][0],font[101-32][1],font[101-32][2],font[101-32][3],font[101-32][4],

0,0,0,0,0,
0,0,0,0,0,
0,0,0,0,0,

0,0,0,0,0,
0,0,0,0,0,
0,0,0,0,0,

0,0,0,0,0,
0,0,0,0,0,
0,0,0,0,0
};


/*
Pin setup
PORT_D
 0 (PD0) - UART RX
 1 (PD1) - D5
 2 (PD6) - D3 A0
 3 (PD5) - D4 A1
 4 (PD4) - D6 A2
 5 (PD3) - D2
 6 (PD2) - D0
 7 (PD1) - D1

PORT_B
 8  (PB0) - MR
 9  (PB1) - E1
 10 (PB3) - E2

PORT_C
 14 (A0,PC0) - A0
 15 (A1,PC1) - A1
 16 (A2,PC2) - A2
*/

/*
 Set up serial port
*/
void setupSerial() {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */ 
    UCSR0B |= (1<<RXEN0) | (1<<RXCIE0); /* Enable RX and RX interrupt */
    sei(); /* Enable interrups */
}

/*
 This is the RX interrupt function
*/
ISR(USART_RX_vect) 
{
    unsigned char b;
    b=UDR0;
    //reset buffer
    if (b==0x81) {
        bufferpos=0;
        rxstate=0;
        return;
    }

    //Start filling the buffer when rxstate = 3
    if (rxstate==3){
        framebuffer[bufferpos] = b;
        if (bufferpos < 90) {
            bufferpos++;
        }else{
            bufferpos=0;
        }
    } else {
        switch(rxstate)
        {
            case 0:
                //If the start bit is there start filling the framebuffer
                if(b==0x80) {
                    rxstate=3;
                }else{
                    rxstate=0;
                }
            break;
        }
    }
}

/*
 This functions toggles U7's E1
*/
void toggleE1() {
    PORTB = 0b000001;
    PORTB = 0b000011;
}

/*
 This function toggles the chip clean pins (MR)
*/
void clean() {
    for (int nopje = 0; nopje < 70; nopje++){
        asm("nop"); asm("nop"); asm("nop"); asm("nop");
    } 
 
    PORTB = 0b000010;
    PORTB = 0b000011;


    // Init latches of U8 and U9
    for (int col=0;col < 15; col++) {
        //Clock Col data
        if (col < 8) {
            //Clock data +8 is to set the D PIN to LOW 
            PORTD = (col << 2) + 2;

            //Shift col 1-8 (IC U8 PIN E)
            PORTC = 6;
        }else{
            //Clock data +8 is to set the D PIN to LOW 
            PORTD = (col-8 << 2) + 2;

            //Shift col 9-15 (IC U9 PIN E)
            PORTC = 7;
        }
        toggleE1();
    }
}

/*
 Setup function initializes all the registers
*/
void setup() {
    //Set up serial
    setupSerial();

    //Set registers
    DDRB = DDRB | 0b111111;
    DDRC = 0b000111;
    DDRD = DDRD | 0b11111110;

    PORTB = 0b000011;
 
    clean();
    toggleE1();
}

/*
 Function to loop pixels
*/
void setPixels(){
    /*
        Loop all 15 col's * 6
    */
    for (int col = 0; col < 15; col++) {
        /*
            Write row data for all the boards
        */
        for (int board = 0; board < 6; board++){
            int datablock = col + ( 15 * board);

            //Write row data to data registers
            PORTD = (framebuffer[datablock] << 1);

            //chip select 0-5 (IC U1-U6 PIN CP)
            PORTC = board;
            toggleE1();
        }

        //Clock Col data 
        if (col < 8) {
            //Clock data +8 is to set the D PIN to HIGH
            PORTD = (col << 2) + 2;

            //Shift col 1-8 (IC U8 PIN E)
            PORTC = 6;

            toggleE1();

            //Clock data +8 is to set the D PIN to LOW 
            PORTD = col << 2;
        }else{
            //Clock data +8 is to set the D PIN to HIGH
            PORTD = ((col-8) << 2) + 2;

            //Shift col 9-15 (IC U9 PIN E)
            PORTC = 7;

            toggleE1();
            
            //Clock data +8 is to set the D PIN to LOW 
            PORTD = (col-8) << 2;
        }
        toggleE1();
        clean();
    }
}

/*
 Main program loop
*/
void loop() {
    setPixels();
}

/*
 Main function
*/
int main(void)
{
	setup();
	while(1) 
	{
		loop();
	}
}
