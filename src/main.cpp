#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>

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

#define F_CPU 16000000UL
#define BAUD 9600
#define SIZE 3

void setup() {
 //Set registers
 DDRB = DDRB | B111111;
 DDRC = B000111;
 DDRD = DDRD | B11111110;

 PORTB = B000011;
 
 clean();
 toggleE1();
}

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

void setPixels(int RowData, int ColData, int ledBlock){
  /*
    Colom thing
  */
  for (int col = 0; col < 16; col++) {
    if (ColData == col){
      continue;
    }
    
    //Clock Col data 
    if (col < 8) {
      //Clock data
      PORTD = rotate(col)+8 << 4;
      //Shift col 1-8
      PORTC = 6;
    }else{
      //Clock data
      PORTD = rotate(col-8)+8 << 4;
      //Shift col 9-15
      PORTC = 7;
    }
    toggleE1();
  }
  
  /*
    Row stuff
  */
  //Write row data to data registers
  PORTD = (RowData << 1);
      
  //chip select
  PORTC = ledBlock;
  toggleE1();
}
void toggleE1() {
  PORTB = B000001;
  PORTB = B000011;
}
void clean() {
  //Toggle MR (PB0)
  for (int nopje = 0; nopje < 900; nopje++){
    asm("nop");
  }
  PORTB = B000010;
  PORTB = B000011;
}

void loop() {
  setPixels(127,0,0);
  clean();
  setPixels(8,1,0);
  clean();
  setPixels(8,2,0);
  clean();
  setPixels(8,3,0);
  clean();
  setPixels(127,4,0);
  clean();
  
  setPixels(127,5,0);
  clean();
  setPixels(72,6,0);
  clean();
  setPixels(72,7,0);
  clean();
  setPixels(72,8,0);
  clean();
  setPixels(127,9,0);
  clean();
  
  setPixels(127,10,0);
  clean();
  setPixels(1,11,0);
  clean();
  setPixels(1,12,0);
  clean();
  setPixels(1,13,0);
  clean();
  setPixels(1,14,0);
  clean();
  
  setPixels(127,0,1);
  clean();
  setPixels(1,1,1);
  clean();
  setPixels(1,2,1);
  clean();
  setPixels(1,3,1);
  clean();
  setPixels(1,4,1);
  clean();
  
  setPixels(127,5,1);
  clean();
  setPixels(72,6,1);
  clean();
  setPixels(72,7,1);
  clean();
  setPixels(72,8,1);
  clean();
  setPixels(127,9,1);
  clean();
  
  setPixels(127,10,1);
  clean();
  setPixels(1,11,1);
  clean();
  setPixels(1,12,1);
  clean();
  setPixels(1,13,1);
  clean();
  setPixels(1,14,1);
  clean();
}

int main(void)
{
	setup();
	while(1) 
	{
		loop();
	}
}
