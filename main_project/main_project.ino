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
uint8_t button_press1 = 0;
uint8_t button_press2 = 0;
uint8_t bomb_explode_number = 0x00;

//clear screens
uint8_t clear_screen_ps2 = 0;
uint8_t clear_start_screen = 0;
uint8_t clear_player_screen = 0;
uint8_t clear_ready_screen = 0;
uint8_t clear_win1_screen = 0;
uint8_t clear_win2_screen = 0;

//bomb pick and send 
uint8_t bomb_pick1 = 0;
uint8_t bomb_send1 = 0;
uint8_t bomb_pick2 = 0;
uint8_t bomb_send2 = 0;

//create struct of bombs 
struct bomb{
  uint8_t armed;
  uint8_t explode; 
  uint16_t explode_count;
  uint8_t xpos;
  uint8_t ypos;
};

//create struct of players
struct player{
  uint8_t xpos;
  uint8_t ypos; 
  uint8_t lives;
  uint8_t num_bombs;
  bomb bombs[3]; 
};

//set up players and bombs
player player1, player2;
bomb p1_1, p1_2, p1_3, p2_1, p2_2, p2_3;

void setup() {
  matrix.begin();
  Serial.begin(9600);

  //set task to begin at start 
  task_number = 0x20;

  p1_1.armed = 0;
  p1_1.explode = 0;
  p1_1.explode_count = 0;
  p1_1.xpos = 0;
  p1_1.ypos = 0;
  
  p1_2.armed = 0;
  p1_2.explode = 0;
  p1_2.explode_count = 0;
  p1_2.xpos = 0;
  p1_2.ypos = 0;
  
  p1_3.armed = 0;
  p1_3.explode = 0;
  p1_3.explode_count = 0;
  p1_3.xpos = 0;
  p1_3.ypos = 0;
  
  p2_1.armed = 0;
  p2_1.explode = 0;
  p2_1.explode_count = 0;
  p2_1.xpos = 0;
  p2_1.ypos = 0;
  
  p2_2.armed = 0;
  p2_2.explode = 0;
  p2_2.explode_count = 0;
  p2_2.xpos = 0;
  p2_2.ypos = 0;
  
  p2_3.armed = 0;
  p2_3.explode = 0;
  p2_3.explode_count = 0;
  p2_3.xpos = 0;
  p2_3.ypos = 0;
  
  player1.xpos = 1;
  player1.ypos = 1;
  player1.num_bombs = 3;
  player1.lives = 3;
  player1.bombs[0] = p1_1;
  player1.bombs[1] = p1_2;
  player1.bombs[2] = p1_3;

  player2.xpos = 23;
  player2.ypos = 14;
  player2.num_bombs = 3;
  player2.lives = 3;
  player2.bombs[0] = p2_1;
  player2.bombs[1] = p2_2;
  player2.bombs[2] = p2_3;
}

void controller1()
{
  //set up for player two, clear screen and put pixels where they belong
  if (!clear_screen_ps2) 
  {  
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawRect(0, 0, 25, 16, matrix.Color333(0, 5, 2));
    matrix.drawPixel(1, 1, matrix.Color333(7, 2, 0));
    matrix.drawPixel(23,14, matrix.Color333(0, 2, 5));
    clear_screen_ps2 = 1;
    clear_ready_screen = 0;
  }
  
  //if a button is make previous LED black
  if (controller1_action != 0x00)
  {
    if (!button_press1)
    {
      matrix.drawPixel(player1.xpos, player1.ypos,
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
    if (player1.xpos == player2.xpos && player1.ypos - 1 == player2.ypos) 
    { matrix.drawPixel(player1.xpos, player1.ypos, matrix.Color333(7, 2, 0)); }
    else if (!button_press1)
    {
      if (player1.ypos > 1) { player1.ypos--; }
      matrix.drawPixel(player1.xpos, player1.ypos,
                  matrix.Color333(7, 2, 0));
      button_press1 = 1;
      Serial.println(player1.ypos);
    }
  }
  //DOWN
  else if (controller1_action == 0x02)
  {
    //if player is blocking below
    if (player1.xpos == player2.xpos && player1.ypos + 1 == player2.ypos) 
    { matrix.drawPixel(player1.xpos, player1.ypos, matrix.Color333(7, 2, 0)); }
    else if (!button_press1)
    {
      if (player1.ypos < 14) { player1.ypos++; }
      matrix.drawPixel(player1.xpos, player1.ypos,
                  matrix.Color333(7, 2, 0));
      button_press1 = 1;
      Serial.println(player1.ypos);
    }
  }

  //LEFT
  else if (controller1_action == 0x03)
  {
    //if player is blocking left
    if (player1.xpos - 1 == player2.xpos && player1.ypos == player2.ypos) 
    { matrix.drawPixel(player1.xpos, player1.ypos, matrix.Color333(7, 2, 0)); }
    else if (!button_press1)
    {
      if (player1.xpos > 1) { player1.xpos--; }
      matrix.drawPixel(player1.xpos, player1.ypos,
                  matrix.Color333(7, 2, 0));
      button_press1 = 1;
      Serial.println(player1.xpos);
    }
  }

  //RIGHT
  else if (controller1_action == 0x04)
  {
    //if player is blocking right
    if (player1.xpos + 1 == player2.xpos && player1.ypos == player2.ypos)
    { matrix.drawPixel(player1.xpos, player1.ypos, matrix.Color333(7, 2, 0)); }
    else if (!button_press1)
    {
      if (player1.xpos < 23) { player1.xpos++; }
      matrix.drawPixel(player1.xpos, player1.ypos,
                  matrix.Color333(7, 2, 0));
      button_press1 = 1;
      Serial.println(player1.xpos);
    }
  }

  //SELECT
  else if (controller1_action == 0x05)
  {
    reset_all();
    Serial.write(0x69);
  }

  //B for BOMB
  else if (controller1_action == 0x08)
  {
    Serial.println(player1.num_bombs);
    //find a bomb that is not armed and arm it
    if(player1.num_bombs > 0 && !button_press1)
    {
      //Serial.print("B1");
      bomb_pick1 = 0;
      while(player1.bombs[bomb_pick1].armed != 0) { bomb_pick1++; }
      player1.bombs[bomb_pick1].armed = 1;
      player1.bombs[bomb_pick1].xpos = player1.xpos;
      player1.bombs[bomb_pick1].ypos = player1.ypos;;
      player1.num_bombs--;
      bomb_send1 = 0x71 + bomb_pick1;
      //Serial.println(bomb_send1);
      Serial.write(bomb_send1);
      button_press1 = 1;
    }
  }
}//end of controller1



void controller2()
{
    //if a button is pressed make previous LED black
  if (controller2_action != 0x10)
  {
    if (!button_press2)
    {
      matrix.drawPixel(player2.xpos, player2.ypos,
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
    if (player1.xpos == player2.xpos && player1.ypos == player2.ypos - 1)
    { matrix.drawPixel(player2.xpos, player2.ypos, matrix.Color333(0, 2, 5)); }
    else if (!button_press2)
    {
      if (player2.ypos > 1) { player2.ypos--; }
      matrix.drawPixel(player2.xpos, player2.ypos,
                  matrix.Color333(0, 2, 5));
      button_press2 = 1;
      Serial.println(player2.ypos);
    }
  }
  //DOWN
  else if (controller2_action == 0x12)
  {
    //if player is blocking below
    if (player1.xpos == player2.xpos && player1.ypos == player2.ypos + 1)
    { matrix.drawPixel(player2.xpos, player2.ypos, matrix.Color333(0, 2, 5)); }
    else if (!button_press2)
    {
      if (player2.ypos < 14) { player2.ypos++; }
      matrix.drawPixel(player2.xpos, player2.ypos,
                  matrix.Color333(0, 2, 5));
      button_press2 = 1;
      Serial.println(player2.ypos);
    }
  }

  //LEFT
  else if (controller2_action == 0x13)
  {
    //if player is blocking left
    if (player1.xpos == player2.xpos - 1 && player1.ypos == player2.ypos)
    { matrix.drawPixel(player2.xpos, player2.ypos, matrix.Color333(0, 2, 5)); }
    else if (!button_press2)
    {
      if (player2.xpos > 1) { player2.xpos--; }
      matrix.drawPixel(player2.xpos, player2.ypos,
                  matrix.Color333(0, 2, 5));
      button_press2 = 1;
      Serial.println(player2.xpos);
    }
  }

  //RIGHT
  else if (controller2_action == 0x14)
  {
    //if player is blocking right
    if (player1.xpos == player2.xpos + 1 && player1.ypos == player2.ypos)
    { matrix.drawPixel(player2.xpos, player2.ypos, matrix.Color333(0, 2, 5)); }
    else if (!button_press2)
    {
      if (player2.xpos < 23) { player2.xpos++; }
      matrix.drawPixel(player2.xpos, player2.ypos,
                  matrix.Color333(0, 2, 5));
      button_press2 = 1;
      Serial.println(player2.xpos);
    }
  }

  //SELECT
  else if (controller2_action == 0x15)
  {
    reset_all();
    Serial.write(0x69);
  }

  //B for BOMB
  else if (controller2_action == 0x18)
  {
    //find a bomb that is not armed and arm it
    Serial.println(player2.num_bombs);
    if(player2.num_bombs > 0 && !button_press2)
    {
      //Serial.print("B2");
      bomb_pick2 = 0;
      while(player2.bombs[bomb_pick2].armed != 0) { bomb_pick2++; }
      matrix.drawPixel(player2.xpos, player2.ypos, matrix.Color333(7, 7, 7));
      player2.bombs[bomb_pick2].armed = 1;
      player2.bombs[bomb_pick2].xpos = player2.xpos;
      player2.bombs[bomb_pick2].ypos = player2.ypos;
      player2.num_bombs--;
      bomb_send2 = 0x41 + bomb_pick2;
      //Serial.println(bomb_send2);
      Serial.write(bomb_send2);
      button_press2 = 1;
    }
  }
}//end of controller2

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
    clear_win1_screen = 0;
    clear_win2_screen = 0;
  }
  
  if (task0_display == 1)
  {
    matrix.setCursor(1, 0);   // start at top left, with one pixel of spacing
    matrix.setTextSize(1);    // size 1 == 8 pixels high
    
    // print each letter with a rainbow color
    matrix.setTextColor(matrix.Color333(2,6,0));
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
}//end of task 0

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

      matrix.setTextColor(matrix.Color333(0,6,3));
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

      matrix.setTextColor(matrix.Color333(0,6,3));
      matrix.setCursor(6, 0);
      matrix.print('P');
      matrix.print('S');
      matrix.print(' ');
      matrix.print('1');
    if (controller1_action == 0x01) { task1_pick = 1; clear_player_screen = 0; }
  }

  //if controller is pressed write data to atmel
  if (controller1_action == 0x07) { Serial.write(task1_pick); }
}//end of task1


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
}//end of task2

//this function takes care of setting bomb explode to 1
void bomb_explode1()
{
  if (bomb_explode_number == 0x51)
  { player1.bombs[0].explode = 1; data = 0xFF; }
  if (bomb_explode_number == 0x52)
  { player1.bombs[1].explode = 1; data = 0xFF; }
  if (bomb_explode_number == 0x53)
  { player1.bombs[2].explode = 1; data = 0xFF; }
  if (bomb_explode_number == 0x54)
  { player2.bombs[0].explode = 1; data = 0xFF; }
  if (bomb_explode_number == 0x55)
  { player2.bombs[1].explode = 1; data = 0xFF; }
  if (bomb_explode_number == 0x56)
  { player2.bombs[2].explode = 1; data = 0xFF; }
}//end of bomb_explode1 function

uint8_t bomb1_x;
uint8_t bomb1_y;
uint8_t bomb2_x;
uint8_t bomb2_y;

void bomb_explode2()
{
  //animation for bomb and adds to explode.count for all player1 bombs
  for (unsigned char explode_inc1 = 0; explode_inc1 < 3; explode_inc1++)
  {
    //if a bomb is armed, display it on the matrix 
    if (player1.bombs[explode_inc1].armed == 1)
    { 
      matrix.drawPixel(player1.bombs[explode_inc1].xpos, 
                       player1.bombs[explode_inc1].ypos, 
                       matrix.Color333(7, 7, 7)); 
    }

    
    if (player1.bombs[explode_inc1].explode == 1)
    {
      player1.bombs[explode_inc1].explode_count++;
      bomb1_x = 1;
      bomb1_y = 1;

      //if the bomb touches anyone, game over send signal to atmega
       //if the bomb touches anyone, game over send signal to atmega after 3 deaths
      //player1 touched explosion standing on same x or y pos
      if (player1.xpos == player1.bombs[explode_inc1].xpos
          || player1.ypos == player1.bombs[explode_inc1].ypos)
      { 
        if (player1.lives > 1) 
        { 
          player1.lives--; 
          player1.xpos = 1;
          player1.ypos = 1;
          if (player1.bombs[explode_inc1].xpos == 1) { player1.xpos = 23; }
          if (player1.bombs[explode_inc1].ypos == 1) { player1.ypos = 14; }
          matrix.drawPixel(player1.xpos, player1.ypos, matrix.Color333(7, 2, 0));
        }
        else { Serial.write(0x81); } 
      }
      
      //player2 touched explosion standing on same x or y pos
      if (player2.xpos == player1.bombs[explode_inc1].xpos
          || player2.ypos == player1.bombs[explode_inc1].ypos)
      { 
        if (player2.lives > 1) 
        { 
          player2.lives--; 
          player2.xpos = 23;
          player2.ypos = 14;
          if (player1.bombs[explode_inc1].xpos == 23) { player2.xpos = 1; }
          if (player1.bombs[explode_inc1].ypos == 14) { player2.ypos = 1; }
          matrix.drawPixel(player2.xpos, player2.ypos, matrix.Color333(0, 2, 5));
        }
        else { Serial.write(0x82); } 
      }

      //up down bomb
      while (bomb1_y < 15) 
      {
        matrix.drawPixel(player1.bombs[explode_inc1].xpos, 
                          bomb1_y, matrix.Color333(7, 7, 7));
        bomb1_y++;
      }
      //left right bomb
      while (bomb1_x < 24) 
      {
        matrix.drawPixel(bomb1_x, player1.bombs[explode_inc1].ypos,
                          matrix.Color333(7, 7, 7));
        bomb1_x++;
      }
    }//end of first if statement

    //if statements to set explosion off and clear explosion animation
    if (player1.bombs[explode_inc1].explode_count >= 200)
    {
      player1.bombs[explode_inc1].explode = 0;
      player1.bombs[explode_inc1].explode_count = 0;
      player1.bombs[explode_inc1].armed = 0;
      player1.num_bombs++;
      bomb1_x = 1;
      bomb1_y = 1;
      //up down bomb erase
      while (bomb1_y < 15) 
      {
        matrix.drawPixel(player1.bombs[explode_inc1].xpos, 
                          bomb1_y, matrix.Color333(0, 0, 0));
        bomb1_y++;
      }
      //left right bomb erase
      while (bomb1_x < 24) 
      {
        matrix.drawPixel(bomb1_x, player1.bombs[explode_inc1].ypos,
                          matrix.Color333(0, 0, 0));
        bomb1_x++;
      }
     }
  }

  //if statements to set explosion off and clear explosion animation

  //animation for bomb and adds to explode.count for all player2 bombs
  for (unsigned char explode_inc2 = 0; explode_inc2 < 3; explode_inc2++)
  {
    //display an armed bomb
    if (player2.bombs[explode_inc2].armed == 1)
    { 
      matrix.drawPixel(player2.bombs[explode_inc2].xpos, 
                       player2.bombs[explode_inc2].ypos, 
                       matrix.Color333(7, 7, 7)); 
    }
    
    if (player2.bombs[explode_inc2].explode == 1)
    {
      player2.bombs[explode_inc2].explode_count++;
      bomb2_x = 1;
      bomb2_y = 1;

      //if the bomb touches anyone, game over send signal to atmega after 3 deaths
      //player1 touched explosion standing on same x or y pos
      if (player1.xpos == player2.bombs[explode_inc2].xpos
          || player1.ypos == player2.bombs[explode_inc2].ypos)
      { 
        if (player1.lives > 1) 
        { 
          player1.lives--; 
          player1.xpos = 1;
          player1.ypos = 1;
          if (player2.bombs[explode_inc2].xpos == 1) { player1.xpos = 23; }
          if (player2.bombs[explode_inc2].ypos == 1) { player1.ypos = 14; }
          matrix.drawPixel(player1.xpos, player1.ypos, matrix.Color333(7, 2, 0));
        }
        else { Serial.write(0x81); } 
      }
      
      //player2 touched explosion standing on same x or y pos
      if (player2.xpos == player2.bombs[explode_inc2].xpos
          || player2.ypos == player2.bombs[explode_inc2].ypos)
      { 
        if (player2.lives > 1) 
        { 
          player2.lives--; 
          player2.xpos = 23;
          player2.ypos = 14;
          if (player2.bombs[explode_inc2].xpos == 23) { player2.xpos = 1; }
          if (player2.bombs[explode_inc2].ypos == 4) { player2.ypos = 1; }
          matrix.drawPixel(player2.xpos, player2.ypos, matrix.Color333(0, 2, 5));
        }
        else { Serial.write(0x82); } 
      }

      //up down bomb
      while (bomb2_y < 15) 
      {
        matrix.drawPixel(player2.bombs[explode_inc2].xpos, 
                          bomb2_y, matrix.Color333(7, 7, 7));
        bomb2_y++;
      }
      //left right bomb
      while (bomb2_x < 24) 
      {
        matrix.drawPixel(bomb2_x, player2.bombs[explode_inc2].ypos,
                          matrix.Color333(7, 7, 7));
        bomb2_x++;
      }
    }//end of first if statement

    //if statements to set explosion off and clear explosion animation
    if (player2.bombs[explode_inc2].explode_count >= 200)
    {
      player2.bombs[explode_inc2].explode = 0;
      player2.bombs[explode_inc2].explode_count = 0;
      player2.bombs[explode_inc2].armed = 0;
      player2.num_bombs++;
      bomb2_x = 1;
      bomb2_y = 1;
      //up down bomb erase
      while (bomb2_y < 15) 
      {
        matrix.drawPixel(player2.bombs[explode_inc2].xpos, 
                          bomb2_y, matrix.Color333(0, 0, 0));
        bomb2_y++;
      }
      //left right bomb erase
      while (bomb2_x < 24) 
      {
        matrix.drawPixel(bomb2_x, player2.bombs[explode_inc2].ypos,
                          matrix.Color333(0, 0, 0));
        bomb2_x++;
      }
     }
  }
}//end of bomb_explode2 function

void lives_display()
{
  if (player1.lives == 3)
  {
    matrix.drawPixel(26, 3, matrix.Color333(7, 2, 0));
    matrix.drawPixel(28, 3, matrix.Color333(7, 2, 0));
    matrix.drawPixel(30, 3, matrix.Color333(7, 2, 0));
  }

  if (player1.lives == 2)
  {
    matrix.drawPixel(26, 3, matrix.Color333(7, 2, 0));
    matrix.drawPixel(28, 3, matrix.Color333(7, 2, 0));
    matrix.drawPixel(30, 3, matrix.Color333(0, 0, 0));
  }

  if (player1.lives == 1)
  {
    matrix.drawPixel(26, 3, matrix.Color333(7, 2, 0));
    matrix.drawPixel(28, 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(30, 3, matrix.Color333(0, 0, 0));
  }

  if (player2.lives == 3)
  {
    matrix.drawPixel(26,5, matrix.Color333(0, 2, 5));
    matrix.drawPixel(28,5, matrix.Color333(0, 2, 5));
    matrix.drawPixel(30,5, matrix.Color333(0, 2, 5));
  }

  if (player2.lives == 2)
  {
    matrix.drawPixel(26, 5, matrix.Color333(0, 2, 5));
    matrix.drawPixel(28, 5, matrix.Color333(0, 2, 5));
    matrix.drawPixel(30, 5, matrix.Color333(0, 0, 0));
  }

  if (player2.lives == 1)
  {
    matrix.drawPixel(26, 5, matrix.Color333(0, 2, 5));
    matrix.drawPixel(28, 5, matrix.Color333(0, 0, 0));
    matrix.drawPixel(30, 5, matrix.Color333(0, 0, 0));
  }
}

void task3()
{
  if (!clear_win1_screen) { matrix.fillScreen(matrix.Color333(0, 0, 0)); clear_win1_screen = 1;}
  matrix.setCursor(1, 0);   // start at top left, with one pixel of spacing
  matrix.setTextSize(1);    // size 1 == 8 pixels high
  
  // print each letter with a rainbow color
  matrix.setTextColor(matrix.Color333(7,2,0));
  matrix.print('P');
  matrix.print('1');

  matrix.setCursor(1, 9);
  matrix.print('W');
  matrix.print('I');
  matrix.print('N');
  matrix.print('S');
  matrix.print('!');

  if(controller1_action == 0x05 || controller2_action == 0x15)
  {
    reset_all();
    Serial.write(0x6A);
  }
}

void task4()
{
  if (!clear_win2_screen) { matrix.fillScreen(matrix.Color333(0, 0, 0)); clear_win2_screen = 1; }
  matrix.setCursor(1, 0);   // start at top left, with one pixel of spacing
  matrix.setTextSize(1);    // size 1 == 8 pixels high
  
  // print each letter with a rainbow color
  matrix.setTextColor(matrix.Color333(0,2,5));
  matrix.print('P');
  matrix.print('2');

  matrix.setCursor(1, 9);
  matrix.print('W');
  matrix.print('I');
  matrix.print('N');
  matrix.print('S');
  matrix.print('!');

  if(controller1_action == 0x05 || controller2_action == 0x15)
  {
    reset_all();
    Serial.write(0x6A);
  }
}

void reset_all()
{
  player1.xpos = 1;
  player1.ypos = 1;
  player1.lives = 3;
  player1.num_bombs = 3;
  player1.bombs[0].armed = 0;
  player1.bombs[0].explode = 0;
  player1.bombs[0].explode_count = 0;
  player1.bombs[0].xpos = 0;
  player1.bombs[0].ypos = 0;
  player1.bombs[1].armed = 0;
  player1.bombs[1].explode = 0;
  player1.bombs[1].explode_count = 0;
  player1.bombs[1].xpos = 0;
  player1.bombs[1].ypos = 0;
  player1.bombs[2].armed = 0;
  player1.bombs[2].explode = 0;
  player1.bombs[2].explode_count = 0;
  player1.bombs[2].xpos = 0;
  player1.bombs[2].ypos = 0;

  player2.xpos = 23;
  player2.ypos = 14;
  player2.lives = 3;
  player2.num_bombs = 3;
  player2.bombs[0].armed = 0;
  player2.bombs[0].explode = 0;
  player2.bombs[0].explode_count = 0;
  player2.bombs[0].xpos = 0;
  player2.bombs[0].ypos = 0;
  player2.bombs[1].armed = 0;
  player2.bombs[1].explode = 0;
  player2.bombs[1].explode_count = 0;
  player2.bombs[1].xpos = 0;
  player2.bombs[1].ypos = 0;
  player2.bombs[2].armed = 0;
  player2.bombs[2].explode = 0;
  player2.bombs[2].explode_count = 0;
  player2.bombs[2].xpos = 0;
  player2.bombs[2].ypos = 0;

  bomb_explode_number = 0xFF;
}


/* Data map
 *  RECIEVE 0x0-: Controller 1 data
 *  RECIEVE 0x1-: Controller 2 data
 *  RECIEVE 0x2-: Task number
 *  SEND 0x7-: Bomb send1
 *  SEND 0x4-: Bomb send2
 *  RECIEVE 0x5-: Bomb explode
 *  SEND 0x69: 
 *  SEND 0x8-: Player died
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
  if (data_number == 0x50) { bomb_explode_number = data; }

/* Task map
 * 0x20: press_start
 * 0x21: player_choose
 * 0x22: ready_wait
 * 0x23: one_player
 * 0x24: two_player
 * 0x2A: player1_win
 * 0x2B: player2_win
 */

  //Serial.println(task_number);
  if (task_number == 0x20) { task0(); }
  else if (task_number == 0x21) { task1(); }
  else if (task_number == 0x22) { task2(); }
  else if (task_number == 0x24) 
  {
    controller1(); controller2(); bomb_explode1(); bomb_explode2(); lives_display();
  }
  else if (task_number == 0x2A) { task3(); } //player1 wins
  else if (task_number == 0x2B) { task4(); } //player2 wins
    
  
}
