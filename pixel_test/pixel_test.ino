#include <avr/pgmspace.h>
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define CLK 8  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2
// Last parameter = 'true' enables double-buffering, for flicker-free,
// buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
// until the first call to swapBuffers().  This is normal.
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

uint8_t data = 0x00;
uint8_t data_number = 0x00;
uint8_t controller1_action = 0x00;
uint8_t controller2_action = 0x10;
int16_t player1_xpos = 0;
int16_t player1_ypos = 0;
uint8_t button_press1 = 0;
int16_t player2_xpos = 31;
int16_t player2_ypos = 15;
uint8_t button_press2 = 0;

void setup() {
  matrix.begin();
  Serial.begin(9600);
  matrix.drawPixel(player1_xpos, player1_ypos,
                    matrix.Color333(7, 7, 7));
  matrix.drawPixel(player2_xpos, player2_ypos,
                    matrix.Color333(7, 7, 7));
}

void controller1()
{
  //if a button is make previous LED black
  if (controller1_action != 0x00)
  {
    if (!button_press1)
    {
      matrix.drawPixel(player1_xpos, player1_ypos,
                matrix.Color333(0, 0, 0));
    }
  }

  //if nothing is preing pressed set button_press to 0
  if (controller1_action == 0x00) { button_press1 = 0; }
  
  //these determine where to move the pixel

  //UP
  if (controller1_action == 0x01)
  {
    if (!button_press1)
    {
      if (player1_ypos > 0) { player1_ypos--; }
      matrix.drawPixel(player1_xpos, player1_ypos,
                  matrix.Color333(7, 7, 7));
      button_press1 = 1;
      Serial.println(player1_ypos);
    }
  }
  //DOWN
  else if (controller1_action == 0x02)
  {
    if (!button_press1)
    {
      if (player1_ypos < 15) { player1_ypos++; }
      matrix.drawPixel(player1_xpos, player1_ypos,
                  matrix.Color333(7, 7, 7));
      button_press1 = 1;
      Serial.println(player1_ypos);
    }
  }

  //LEFT
  else if (controller1_action == 0x03)
  {
    if (!button_press1)
    {
      if (player1_xpos > 0) { player1_xpos--; }
      matrix.drawPixel(player1_xpos, player1_ypos,
                  matrix.Color333(7, 7, 7));
      button_press1 = 1;
      Serial.println(player1_xpos);
    }
  }

  //RIGHT
  else if (controller1_action == 0x04)
  {
    if (!button_press1)
    {
      if (player1_xpos < 31) { player1_xpos++; }
      matrix.drawPixel(player1_xpos, player1_ypos,
                  matrix.Color333(7, 7, 7));
      button_press1 = 1;
      Serial.println(player1_xpos);
    }
  }
}

void controller2()
{
    //if a button is make previous LED black
  if (controller2_action != 0x10)
  {
    if (!button_press2)
    {
      matrix.drawPixel(player2_xpos, player2_ypos,
                matrix.Color333(0, 0, 0));
    }
  }

  //if nothing is preing pressed set button_press to 0
  if (controller2_action == 0x10) { button_press2 = 0; }
  
  //these determine where to move the pixel

  //UP
  if (controller2_action == 0x11)
  {
    if (!button_press2)
    {
      if (player2_ypos > 0) { player2_ypos--; }
      matrix.drawPixel(player2_xpos, player2_ypos,
                  matrix.Color333(7, 7, 7));
      button_press2 = 1;
      Serial.println(player2_ypos);
    }
  }
  //DOWN
  else if (controller2_action == 0x12)
  {
    if (!button_press2)
    {
      if (player2_ypos < 15) { player2_ypos++; }
      matrix.drawPixel(player2_xpos, player2_ypos,
                  matrix.Color333(7, 7, 7));
      button_press2 = 1;
      Serial.println(player2_ypos);
    }
  }

  //LEFT
  else if (controller2_action == 0x13)
  {
    if (!button_press2)
    {
      if (player2_xpos > 0) { player2_xpos--; }
      matrix.drawPixel(player2_xpos, player2_ypos,
                  matrix.Color333(7, 7, 7));
      button_press2 = 1;
      Serial.println(player2_xpos);
    }
  }

  //RIGHT
  else if (controller2_action == 0x14)
  {
    if (!button_press2)
    {
      if (player2_xpos < 31) { player2_xpos++; }
      matrix.drawPixel(player2_xpos, player2_ypos,
                  matrix.Color333(7, 7, 7));
      button_press2 = 1;
      Serial.println(player2_xpos);
    }
  }
}

/* Data map
 *  0x0-: Controller 1 data
 *  0x1-: Controller 2 data
 */

void loop() {
  if (Serial.available() > 0) 
  {
    data = Serial.read();
    data_number = (data & 0xF0);
  }

  //set data to correct variable
  if (data_number == 0x00) { controller1_action = data; }
  if (data_number == 0x10) { controller2_action = data; } 

  
  controller1();
  controller2();
  
}
