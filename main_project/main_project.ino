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
uint8_t task_number = 0x00;
uint8_t controller1_action = 0x00;
uint8_t controller2_action = 0x10;
int16_t player1_xpos = 0;
int16_t player1_ypos = 0;
uint8_t button_press1 = 0;
int16_t player2_xpos = 31;
int16_t player2_ypos = 15;
uint8_t button_press2 = 0;


//clear screens
uint8_t clear_screen_ps2 = 0;
uint8_t clear_start_screen = 0;
uint8_t clear_player_screen = 0;
uint8_t clear_ready_screen = 0;

void setup() {
  matrix.begin();
  Serial.begin(9600);

  //set task to begin at start 
  task_number = 0x20;
}

void controller1()
{
  //set up for player two, clear screen and put pixels where they belong
  if (!clear_screen_ps2) 
  {  
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7));
    matrix.drawPixel(31,15, matrix.Color333(7, 7, 7));
    clear_screen_ps2 = 1;
    clear_ready_screen = 0;
  }
  
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
    //if player is blocking above
    if (player1_xpos == player2_xpos && player1_ypos - 1 == player2_ypos) 
    { matrix.drawPixel(player1_xpos, player1_ypos, matrix.Color333(7, 7, 7)); }
    else if (!button_press1)
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
    //if player is blocking below
    if (player1_xpos == player2_xpos && player1_ypos + 1 == player2_ypos) 
    { matrix.drawPixel(player1_xpos, player1_ypos, matrix.Color333(7, 7, 7)); }
    else if (!button_press1)
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
    //if player is blocking left
    if (player1_xpos - 1 == player2_xpos && player1_ypos == player2_ypos) 
    { matrix.drawPixel(player1_xpos, player1_ypos, matrix.Color333(7, 7, 7)); }
    else if (!button_press1)
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
    //if player is blocking right
    if (player1_xpos + 1 == player2_xpos && player1_ypos == player2_ypos)
    { matrix.drawPixel(player1_xpos, player1_ypos, matrix.Color333(7, 7, 7)); }
    else if (!button_press1)
    {
      if (player1_xpos < 31) { player1_xpos++; }
      matrix.drawPixel(player1_xpos, player1_ypos,
                  matrix.Color333(7, 7, 7));
      button_press1 = 1;
      Serial.println(player1_xpos);
    }
  }

  //SELECt
  else if (controller1_action == 0x05)
  {
    Serial.write(69);
    player1_xpos = 0;
    player1_ypos = 0;
  }
}

void controller2()
{
    //if a button is pressed make previous LED black
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
    //if player is blocking above
    if (player1_xpos == player2_xpos && player1_ypos == player2_ypos - 1)
    { matrix.drawPixel(player2_xpos, player2_ypos, matrix.Color333(7, 7, 7)); }
    else if (!button_press2)
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
    //if player is blocking below
    if (player1_xpos == player2_xpos && player1_ypos == player2_ypos + 1)
    { matrix.drawPixel(player2_xpos, player2_ypos, matrix.Color333(7, 7, 7)); }
    else if (!button_press2)
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
    //if player is blocking left
    if (player1_xpos == player2_xpos - 1 && player1_ypos == player2_ypos)
    { matrix.drawPixel(player2_xpos, player2_ypos, matrix.Color333(7, 7, 7)); }
    else if (!button_press2)
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
    //if player is blocking right
    if (player1_xpos == player2_xpos + 1 && player1_ypos == player2_ypos)
    { matrix.drawPixel(player2_xpos, player2_ypos, matrix.Color333(7, 7, 7)); }
    else if (!button_press2)
    {
      if (player2_xpos < 31) { player2_xpos++; }
      matrix.drawPixel(player2_xpos, player2_ypos,
                  matrix.Color333(7, 7, 7));
      button_press2 = 1;
      Serial.println(player2_xpos);
    }
  }

  //SELECT
  else if (controller2_action == 0x15)
  {
    Serial.write(69);
    player2_xpos = 31;
    player2_ypos = 15;
  }
}

uint16_t task0_counter = 0;
uint8_t task0_display = 1;

void task0()
{
  //clear start at beginning
  if (!clear_start_screen) 
  { 
    matrix.fillScreen(matrix.Color333(0, 0, 0)); 
    clear_start_screen = 1; 
    clear_screen_ps2 = 0;
  }
  
  if (task0_display == 1)
  {
    matrix.setCursor(1, 0);   // start at top left, with one pixel of spacing
    matrix.setTextSize(1);    // size 1 == 8 pixels high
    
    // print each letter with a rainbow color
    matrix.setTextColor(matrix.Color333(2,0,3));
    matrix.print('P');
    matrix.print('R');
    matrix.print('E');
    matrix.print('S');
    matrix.print('S');
  
    matrix.setCursor(1, 9);
    matrix.print('S');
    matrix.print('T');
    matrix.print('A');
    matrix.print('R');
    matrix.print('T');
  }
  if (task0_display == 0)
  {
    matrix.fillScreen(matrix.Color333(0, 0, 0));
  }

  if (task0_counter < 100) { task0_counter++; } 
  else if (task0_counter >= 100) 
  { 
    if (task0_display == 1) { task0_display = 0; } 
    else if (task0_display == 0) { task0_display = 1; }
    task0_counter = 0;
  }
}

uint8_t task1_pick = 1;
void task1()
{
  if (!clear_player_screen) 
  { 
    matrix.fillScreen(matrix.Color333(0, 0, 0)); 
    clear_player_screen = 1; 
    clear_start_screen = 0;
   }

  //ps1 is yellow with an arrow
  if (task1_pick == 1)
  {
      matrix.setCursor(1, 0);   // start at top left, with one pixel of spacing
      matrix.setTextSize(1);    // size 1 == 8 pixels high
      
      // print each letter with a rainbow color
      matrix.setTextColor(matrix.Color333(6,2,0));
      matrix.print('>');
      matrix.print('P');
      matrix.print('S');
      matrix.print(' ');
      matrix.print('1');

      matrix.setTextColor(matrix.Color333(2,0,6));
      matrix.setCursor(6, 8);
      matrix.print('P');
      matrix.print('S');
      matrix.print(' ');
      matrix.print('2');
    if (controller1_action == 0x02) { task1_pick = 2; clear_player_screen = 0; }
  }
  //ps2 is yellow with an arrow
  else if (task1_pick == 2)
  {
      matrix.setCursor(1, 8);   // start at top left, with one pixel of spacing
      matrix.setTextSize(1);    // size 1 == 8 pixels high
      
      // print each letter with a rainbow color
      matrix.setTextColor(matrix.Color333(6,2,0));
      matrix.print('>');
      matrix.print('P');
      matrix.print('S');
      matrix.print(' ');
      matrix.print('2');

      matrix.setTextColor(matrix.Color333(2,0,6));
      matrix.setCursor(6, 0);
      matrix.print('P');
      matrix.print('S');
      matrix.print(' ');
      matrix.print('1');
    if (controller1_action == 0x01) { task1_pick = 1; clear_player_screen = 0; }
  }

  //if controller is pressed write data to atmel
  if (controller1_action == 0x07) { Serial.write(task1_pick); task1_pick = 1; }
}


void task2()
{
  if (!clear_ready_screen) 
  { 
    matrix.fillScreen(matrix.Color333(0, 0, 0)); 
    clear_ready_screen = 1; 
    clear_player_screen = 0;
  }
  
  matrix.setCursor(1, 4);   // start at top left, with one pixel of spacing
  matrix.setTextSize(1);    // size 1 == 8 pixels high
  
  // print each letter with a rainbow color
  matrix.setTextColor(matrix.Color333(0,2,6));
  matrix.print('R');
  matrix.print('E');
  matrix.print('A');
  matrix.print('D');
  matrix.print('Y');
  return;
}

/* Data map
 *  0x0-: Controller 1 data
 *  0x1-: Controller 2 datas
 *  0x2-: Task number
 */

void loop() {
  if (Serial.available() > 0) 
  {
    data = Serial.read();
    data_number = (data & 0xF0);
    Serial.println(data);
  }

  //set data to correct variable
  if (data_number == 0x00) { controller1_action = data; }
  if (data_number == 0x10) { controller2_action = data; } 
  if (data_number == 0x20) { task_number = data; }

/* Task map
 * 0x20: press_start
 * 0x21: player_choose
 * 0x22: ready_wait
 * 0x23: one_player
 * 0x24: two_player
 */

  //Serial.println(task_number);
  if (task_number == 0x20) { task0(); }
  else if (task_number == 0x21) { task1(); }
  else if (task_number == 0x22) { task2(); }
  else if (task_number == 0x24) 
  {
    controller1(); controller2(); 
  }
    
  
}
