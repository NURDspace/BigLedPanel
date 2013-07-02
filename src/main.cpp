#define F_CPU 16000000UL
#define BAUD 9600
#define SIZE 3

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
font[69-32][0],font[69-32][1],font[69-32][2],font[69-32][3],font[69-32][4],
font[82-32][0],font[82-32][1],font[82-32][2],font[82-32][3],font[82-32][4],
font[68-32][0],font[68-32][1],font[68-32][2],font[68-32][3],font[68-32][4],
font[115-32][0],font[115-32][1],font[115-32][2],font[115-32][3],font[115-32][4],
font[112-32][0],font[112-32][1],font[112-32][2],font[112-32][3],font[112-32][4],
font[97-32][0],font[97-32][1],font[97-32][2],font[97-32][3],font[97-32][4],
font[99-32][0],font[99-32][1],font[99-32][2],font[99-32][3],font[99-32][4],
font[101-32][0],font[101-32][1],font[101-32][2],font[101-32][3],font[101-32][4],
};


/*
Pin setup
PORT_D
 0 (PD0) - UART RX
 1 (PD1) - D1
 2 (PD2) - D0
 3 (PD3) - D2
 4 (PD4) - D6 A2
 5 (PD5) - D4 A1
 6 (PD6) - D3 A0
 7 (PD7) - D5 

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
 This functions toggles U7's E1
*/
void toggleE1() {
    PORTB = 0b000001;
    PORTB = 0b000011;
}

/*
 This is the RX interrupt function
*/
ISR(USART0_RX_vect) 
{
    unsigned char b;
    b=UDR0;
    //reset buffer
    if (b==0x81) {
        bufferpos=0;
        rxstate=0;
    }

    //Start filling the buffer when rxstate = 3
    if (rxstate==3){
        framebuffer[bufferpos] = b;
        if (bufferpos < 90) {
            bufferpos++;
        }else{
            bufferpos=0;
        }
    }

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

/*
 This function toggles the chip clean pins (MR)
*/
void clean() {
    for (int nopje = 0; nopje < 900; nopje++){
        asm("nop");
    }
    PORTB = 0b000010;
    PORTB = 0b000011;
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
 Function to rotate the bits (tnx to cha0z97)
 input -> output
 001   -> 100
*/
unsigned char rotate(char input)
{
    char output =0;
    char cursor =0;
    char sketch =0;
 
    int  i = 0;
 
    for(i;i<=SIZE;i++)
    {
        cursor = (input >> i) & 1;
        sketch = cursor << SIZE - 1 - i;
        output = output | sketch;
        //output = output | sketch;
    }
    return output;
}

/*
 Function to loop pixels
*/
void setPixels(){
    /*
        Loop al the boards 
    */
    for (int board = 0; board < 6; board++) {
        /*
            Loop colom per board
        */
        for (int col = 0; col < 16; col++) {
            //Clock Col data 
            if (col < 8) {
                //Clock data +8 is to set the D PIN to HIGH
                PORTD = rotate(col)+8 << 4;

                //Shift col 1-8 (IC U8 PIN E)
                PORTC = 6;
            }else{
                //Clock data +8 is to set the D PIN to HIGH
                PORTD = rotate(col-8)+8 << 4;

                //Shift col 9-15 (IC U9 PIN E)
                PORTC = 7;
            }
            toggleE1();
    
            /*
                Write row data
            */
            //Write row data to data registers
            PORTD = (framebuffer[col * board] << 1);
                    
            //chip select 0-5 (IC U1-U6 PIN CP)
            PORTC = board;
            toggleE1();
        }
    }
}

/*
 Main program loop
*/
void loop() {
    setPixels();
    clean();
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
