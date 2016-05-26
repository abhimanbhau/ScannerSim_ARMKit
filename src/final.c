#include <stdio.h>
#include "lpc214x.h"
#include <string.h>
#include <stdlib.h>

// Function Prototypes
void wait_us (unsigned long);
void GPIO_Port_Low(unsigned int);
void GPIO_Port_High(unsigned int);
void GPIO_Set_Port(unsigned int , unsigned int);
void disp_cmd(unsigned char);
void disp_write(unsigned char);
void lcd_init(void);
void Delay(int);
void Delay2(void);

// Macros
#define DISP_FUNC 0x38 // sets the display to 4-bit, 2 lines, 5x7 chars + cursor-off
#define DISP_ENTRY 0x06
#define DISP_CNTL 0x08
#define DISP_ON 0x04
#define DISP_CURSOR 0x02
#define DISP_CLEAR 0x01
#define DISP_HOME 0x02
#define DISP_POS 0x80
#define DISP_BLINK 0x01
#define DISPDATA_MASK 0x00ff0000  // four(!) bit data bus to the display panel connected to GPIO[12..15]
#define ENABLE_MASK 1<<24 //toggal BIT for testing BY SP
#define REGSEL_MASK 1<<25 //toggal BIT for testing BY SP
#define LONG_DELAY 2000
#define SHORT_DELAY 400

// Generic Delay Function
void Delay(int delay)
{
  unsigned int i,j;
  for(i=0; i<delay; i++)
  for(j=0; j<1234; j++);
}

void wait_us (unsigned long usdelay )
{
  static unsigned long i;
  usdelay=usdelay/0x10;
  for(i=0;i<=usdelay;i++)
  {
    // Nothing
  }
}
void GPIO_Port_Low(unsigned int value)
{
  IO1CLR = value; // clear zeros'
  wait_us(1); // slow down timing for LCD compatibility
}
void GPIO_Port_High(unsigned int value){
  IO1SET = value; // set ones'
  wait_us(1); // slow down timing for LCD compatibility
}
void GPIO_Set_Port(unsigned int mask, unsigned int value)
{
  IO1SET = mask & value; // set ones'
  IO1CLR = mask & (~value); // clear zeros'
  wait_us(1); // slow down timing for LCD compatibility
}
void disp_cmd(unsigned char cmd)
{
  IO1DIR = (IO1DIR | DISPDATA_MASK | REGSEL_MASK | ENABLE_MASK); // set ports to outputs
  GPIO_Set_Port(DISPDATA_MASK, cmd<<16); // latch the command
  GPIO_Port_Low(REGSEL_MASK); // select the command reg = 0
  wait_us(1);
  GPIO_Port_High(ENABLE_MASK); // latch the command into the LCD panel = 1 - first nibble
  wait_us(1);
  GPIO_Port_Low(ENABLE_MASK); // ENABLE=0;
  wait_us(1);
}
void disp_write(unsigned char value)
{
  IO1DIR = IO1DIR | DISPDATA_MASK | REGSEL_MASK | ENABLE_MASK; // select write mode RDWR=0
  GPIO_Set_Port(DISPDATA_MASK, value<<16); // latch the data upper nibble
  GPIO_Port_High(REGSEL_MASK); // select the command reg = 1 select data reg
  wait_us(1);
  GPIO_Port_High(ENABLE_MASK); // latch the command into the LCD panel = 1
  wait_us(1);
  GPIO_Port_Low(ENABLE_MASK); // ENABLE=0;
  wait_us(1);
}
void lcd_init(void) {
  unsigned char counter;
  IO1DIR = IO1DIR | DISPDATA_MASK | REGSEL_MASK |ENABLE_MASK;  // select port direction=output
  wait_us(20000); // wait for LCD to reset itself
  for(counter = 0; counter <3; counter++){
    GPIO_Set_Port(DISPDATA_MASK, (0x00020000)); // latch the command 0x30
    GPIO_Port_Low(REGSEL_MASK); // select the command reg REGSEL=0
    wait_us(1);
    GPIO_Port_High(ENABLE_MASK); // latch the command into the LCD panel = 1
    wait_us(1);
    GPIO_Port_Low(ENABLE_MASK); // latch the command into the LCD panel ENABLE=0
    wait_us(5000); // wait for LCD
  }
  disp_cmd(DISP_FUNC); // set the display for an 8
  wait_us(5000); // wait for LCD
  disp_cmd(DISP_CNTL | DISP_ON ); // turn the display on, cursor off
  wait_us(5000); // wait for LCD
  /*********************Added Function **************************************/
  disp_cmd(0x14);
  wait_us(5000);
  disp_cmd(DISP_CLEAR); // clear the display
  wait_us(5000); // wait for LCD
  disp_cmd(DISP_ENTRY); // set the character entry
  // mode to increment display
  // address for each
  // character, but not to scroll
  wait_us(5000);
}
void display(unsigned char arr[])
{
  int i=0;
  disp_cmd(0x80);
  wait_us(5000);
  for(i=0;i<10;i++)
  {
    if(arr[i])
    {
      disp_write(arr[i]);
      wait_us(5000);
    }
    else
    return;
  }
}

char hex2char(unsigned char arr[])
{
  int num = atoi(arr);
  num %= 100;
  num *= 100;
  switch(num)
  {
    case 0: return 'A'; break;
    case 1: return 'B'; break;
    case 2: return 'C'; break;
    case 3: return 'D'; break;
    case 4: return 'E'; break;
    case 5: return 'F'; break;
    case 6: return 'G'; break;
    case 7: return 'H'; break;
    case 8: return 'I'; break;
    case 9: return 'J'; break;
    case 10: return 'K'; break;
    case 11: return 'L'; break;
    case 12: return 'M'; break;
    case 13: return 'N'; break;
    case 14: return 'O'; break;
    case 15: return 'P'; break;
    case 16: return 'Q'; break;
    case 17: return 'R'; break;
    case 18: return 'S'; break;
    case 19: return 'T'; break;
    case 20: return 'U'; break;
    case 21: return 'V'; break;
    case 22: return 'W'; break;
    case 23: return 'X'; break;
    case 24: return 'Y'; break;
    case 25: return 'Z'; break;
    case 26: return '1'; break;
    case 27: return '2'; break;
    case 28: return '3'; break;
    case 29: return '4'; break;
    case 30: return '5'; break;
    case 31: return '6'; break;
    case 32: return '7'; break;
    default: return '#'; break;
  }
}

int main(void)
{
  int i, counter = 10;
  unsigned int value; unsigned char d_hex[4];
  char vals[10];
  PINSEL0=0X00000C00;
  lcd_init();
  display(".........");
  display("SCN_START");
  Delay(LONG_DELAY*2);
  while(counter)
  {
    Delay(SHORT_DELAY);
    AD0CR=0X01200680;//select chnl 1,divide pclk by 06+1,burst controlled by software,
    //10clks 9bits,a/d on operational mode,start conversion on rising edge.
    do
    {
      value=AD0DR7;
    } while(value & 0x80000000 );//wait for the conversion done bit to be 1
    value=((value >>6)& 0x3ff );//
    value= (value * 3300)/0x3ff;//
    i=3;
    while(i>=0)
    {
      d_hex[i]=(value%10)+48;
      value=value/10;
      i--;
    }
    vals[counter - 1] = hex2char(& d_hex[0]);
    counter--;
    Delay(SHORT_DELAY);
  }
  display("SCN_COMPLT");
  Delay(LONG_DELAY*3);
  display(vals);
  return 0;
}
