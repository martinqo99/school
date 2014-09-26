/**
 * Author: Michal Lukac, xlukac05@stud.fit.vutbr.cz
 * Description: This is project to IMP Course at BUT FIT 2012/2013
 *              It simulates lightning newspaper at matrices display.
 * Date: 11.12.2012
 */

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#define COLUMNS 64 // number of columns of display
#define SPEEDCOEF 50
#define MAXSPEED 15000
#define INIT 1
#define HORI 2
#define VERT 3

// control INIT HORI VERT
unsigned char CONTROL = 0;

// led diods
volatile unsigned char leddiods[COLUMNS] @0x200;

// buttons = init, horizontal, vertical
volatile unsigned char Initialize @0x195;
volatile unsigned char Horizontal @0x196;
volatile unsigned char Vertical @0x197;

// bar which controls speed
volatile unsigned char SPEED @0x198;

// two global variables for indication of horizontal or vertical movement
int horizvar = 0;
int vertivar = 0;
char temp;

/**
 * Init display to xlukac05
 */
void init()
{
  Initialize = 0;
  Horizontal = 0;
  Vertical = 0;

  // xlukac
  leddiods[0] = 0b00000000;
  leddiods[1] = 0b01000010;
  leddiods[2] = 0b00100100;
  leddiods[3] = 0b00011000;
  leddiods[4] = 0b00011000;
  leddiods[5] = 0b00100100;
  leddiods[6] = 0b01000010;
  leddiods[7] = 0b00000000;

  leddiods[8]  = 0b00000000;
  leddiods[9]  = 0b00000000;
  leddiods[10] = 0b00000000;
  leddiods[11] = 0b01111110;
  leddiods[12] = 0b01111110;
  leddiods[13] = 0b00000100;
  leddiods[14] = 0b00000000;
  leddiods[15] = 0b00000000;

  leddiods[16] = 0b00000000;
  leddiods[17] = 0b00011110;
  leddiods[18] = 0b00100000;
  leddiods[19] = 0b01000000;
  leddiods[20] = 0b01000000;
  leddiods[21] = 0b00100000;
  leddiods[22] = 0b01111110;
  leddiods[23] = 0b00000000;

  leddiods[24] = 0b00000000;
  leddiods[25] = 0b01111110;
  leddiods[26] = 0b00011000;
  leddiods[27] = 0b00011000;
  leddiods[28] = 0b00010100;
  leddiods[29] = 0b00100010;
  leddiods[30] = 0b01000000;
  leddiods[31] = 0b00000000;
                         
  leddiods[32] = 0b00000000;
  leddiods[33] = 0b01110100;
  leddiods[34] = 0b01010010;
  leddiods[35] = 0b01010010;
  leddiods[36] = 0b01010010;
  leddiods[37] = 0b01010010;
  leddiods[38] = 0b01111100;
  leddiods[39] = 0b00000000;

  leddiods[40] = 0b00000000;
  leddiods[41] = 0b00111100;
  leddiods[42] = 0b01000010;
  leddiods[43] = 0b01000010;
  leddiods[44] = 0b01000010;
  leddiods[45] = 0b01000010;
  leddiods[46] = 0b00100100;
  leddiods[47] = 0b00000000;

  // 05
  leddiods[48] = 0b00000000;
  leddiods[49] = 0b01111110;
  leddiods[50] = 0b01000010;
  leddiods[51] = 0b01000010;
  leddiods[52] = 0b01000010;
  leddiods[53] = 0b01000010;
  leddiods[54] = 0b01111110;
  leddiods[55] = 0b00000000;

  leddiods[56] = 0b00000000;
  leddiods[57] = 0b01001110;
  leddiods[58] = 0b01001010;
  leddiods[59] = 0b01001010;
  leddiods[60] = 0b01001010;
  leddiods[61] = 0b01001010;
  leddiods[62] = 0b01111010;
  leddiods[63] = 0b00000000;
}

/**
 * Do one step from top to down.
 */
void doVerticalStep()
{
  int i = 0;
  for(i; i < COLUMNS; i++)
  {
    // http://en.wikibooks.org/wiki/C_Programming/Simple_math
    leddiods[i] = (leddiods[i] << 1) | (leddiods[i] >> (8 - 1)); 
  }
}

/**
 * Do one step from left to right.
 */
void doHorizontalStep()
{
  int i = COLUMNS;
  temp = leddiods[COLUMNS-1];
  for(i; i > 0; i--)
  {
    leddiods[i] = leddiods[i-1];
  }  
  leddiods[0] = temp;
}

/**
 * Check if some button was clicked.
 */
void checkClick()
{
  if(Initialize == 1)
  {    
    CONTROL = INIT;
    init();
    Initialize = 0;
  }
  else if(Horizontal == 1)
  { 
    CONTROL = HORI;
    Horizontal = 0;
    //init();
  }
  else if(Vertical == 1)
  {
    CONTROL = VERT;
    Vertical = 0;
    //init();
  }
}

/**
 * Make actions depends on click.
 */
void makeAction()
{
  if(CONTROL == HORI)
  {
    doHorizontalStep();
  }
  else if(CONTROL == VERT)
  {
    doVerticalStep(); 
  }  
}

/**
 * Main function.
 */
void main(void)
{
  int k = 0;
  SPEED = 200;
  init();
  EnableInterrupts; /* enable interrupts */
  /* include your code here */

  for(;;)
  {
    __RESET_WATCHDOG(); /* feeds the dog */

    for(k = 0; k < (MAXSPEED - (SPEED * SPEEDCOEF)); k++)
    { 
      checkClick();
    }
    makeAction();
  } /* loop forever */
  /* please make sure that you never leave main */
}

