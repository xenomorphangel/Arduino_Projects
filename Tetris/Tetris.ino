/*
Ported to use LedControll 2015 AnneJan IJhack
Original Author: Jae Yeong Bae
                 UBC ECE
                 jocker.tistory.com       
Date:   Jan.18.2013
File:   Tetris

Ported by AnneJan 2015 to use Max7219 LED Matrix 
display and LedControl library
*/


#include "LedControl.h"

LedControl lc = LedControl(3,2,4,2);  // Pins: DIN,CLK,CS, # of Display connected
LedControl scoreLC = LedControl(12,11,10,1,true);  // Pins: DIN,CLK,CS, # of Display connected

const int buttonRotate = 8; // Rotate
const int buttonRight = 6;  // Right
const int buttonLeft = 5;   // Left
const int buttonDown = 7;   // Down

boolean block[8][18]; //2 extra for rotation
boolean pile[8][16];
boolean disp[8][16];

unsigned char blocktype;
unsigned char blockrotation;

long delays = 0;
short delay_ = 500;

unsigned long btnDelay = 0;
short btnRotDelay = 250;
short btnDownDelay = 180;
short btnSideDelay = 220;

short score = 0;



void setup() {
  lc.shutdown(0,false);  // Wake up displays
  lc.shutdown(1,false);
  lc.setIntensity(0,5);  // Set intensity levels
  lc.setIntensity(1,5);
  lc.clearDisplay(0);  // Clear Displays
  lc.clearDisplay(1);

  scoreLC.shutdown(0,false);  // Wake up displays
  scoreLC.setIntensity(0,10);  // Set intensity levels
  scoreLC.clearDisplay(0);  // Clear Displays

  // seeding the random number generator
  int seed = 
  (analogRead(0)+1)*
  (analogRead(1)+1)*
  (analogRead(2)+1)*
  (analogRead(3)+1);
  randomSeed(seed);
  random(10,5644218);
  seed = seed *random(3336,7831354325)+analogRead(random(4));
  randomSeed(seed);  
  random(10,4813268);

  pinMode(buttonRotate, INPUT_PULLUP); // Rotate
  pinMode(buttonRight, INPUT_PULLUP);  // Right
  pinMode(buttonLeft, INPUT_PULLUP);   // Left
  pinMode(buttonDown, INPUT_PULLUP);   // Down

  newBlock();
  updateHoldingArray();
}

void loop() {

  if (delays < millis())
  {
    delays = millis() + delay_;
    movedown();
  }

  //button actions
  int button = readBtn();

  if (button == 1) //rotate
    rotate();
  if (button == 2) //moveright
    moveright();
  if (button == 3) //moveleft
    moveleft();
  if (button == 4) //movedown
    movedown();


  LEDRefresh();
}

//-------------------- button reading -------------------------------------
int readBtn()
{
  if (btnDelay > millis())
  {
    return 0;
  }
  if ((digitalRead(buttonLeft) == LOW))
  {
    //left
    btnDelay = millis() + btnSideDelay;
    return 3;
  }

  if ((digitalRead(buttonDown) == LOW))
  {
    //down
    btnDelay = millis() + btnDownDelay;
    return 4;
  }
  if ((digitalRead(buttonRight) == LOW))
  {
    //right
    btnDelay = millis() + btnSideDelay;
    return 2;
  }
  if ((digitalRead(buttonRotate) == LOW))
  {
    //rotate
    btnDelay = millis() + btnRotDelay;
    return 1;
  }

  return 0;
}

//----------- movement ------------------------------------------------------
boolean moveleft()
{
  if (space_left()) {
    int i;
    int j;
    for (i=0;i<7;i++) {
      for (j=0;j<16;j++) {
        block[i][j]=block[i+1][j];
      }
    }

    for (j=0;j<16;j++) {
      block[7][j]=0;
    }

    updateHoldingArray();
    return 1;
  }

  return 0;
}

boolean moveright()
{
  if (space_right()) {
    int i;
    int j;
    for (i=7;i>0;i--) {
      for (j=0;j<16;j++) {
        block[i][j]=block[i-1][j];
      }
    }

    for (j=0;j<16;j++) {
      block[0][j]=0;
    }

   updateHoldingArray();
   return 1;

  }
  return 0;
}

void movedown()
{
  if (space_below())
  {
    //move down
    int i;
    for (i=15;i>=0;i--)
    {
      int j;
      for (j=0;j<8;j++)
      {
        block[j][i] = block[j][i-1];
      }
    }
    for (i=0;i<7;i++)
    {
      block[i][0] = 0;
    }
  }
  else
  {
    //merge and new block
    int i;
    int j;
    for (i=0;i<8;i++)
    {
     for(j=0;j<16;j++)
     {
       if (block[i][j])
       {
         pile[i][j]=1;
         block[i][j]=0;
       }
     }
    }
    newBlock();
  }
  updateHoldingArray();
}

void rotate()
{
  //skip for square block(3)
  if (blocktype == 3) return;

  int xi;
  int yi;
  int i;
  int j;
  //detect left
  for (i=7;i>=0;i--)
  {
    for (j=0;j<16;j++)
    {
      if (block[i][j])
      {
        xi = i;
      }
    }
  }

  //detect up
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
      if (block[j][i])
      {
        yi = i;
      }
    }
  }

  if (blocktype == 0)
  {
    if (blockrotation == 0)
    {


      if (!space_left())
      {
        if (space_right3())
        {
          if (!moveright())
            return;
          xi++;
        }
        else return;
      }
      else if (!space_right())
      {
        if (space_left3())
        {
          if (!moveleft())
            return;
          if (!moveleft())
            return;
          xi--;
          xi--;
        }
        else
          return;
      }
      else if (!space_right2())
      {
        if (space_left2())
        {
          if (!moveleft())
            return;
          xi--;
        }
        else
          return;
      }

      block[xi][yi]=0;
      block[xi][yi+2]=0;
      block[xi][yi+3]=0;

      block[xi-1][yi+1]=1;
      block[xi+1][yi+1]=1;
      block[xi+2][yi+1]=1;

      blockrotation = 1;
    }
    else
    {
      block[xi][yi]=0;
      block[xi+2][yi]=0;
      block[xi+3][yi]=0;

      block[xi+1][yi-1]=1;
      block[xi+1][yi+1]=1;
      block[xi+1][yi+2]=1;

      blockrotation = 0;
    }
  }

  //offset to mid
  xi ++;
  yi ++;

  if (blocktype == 1)
  {
    if (blockrotation == 0)
    {
      block[xi-1][yi-1] = 0;
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;

      block[xi][yi-1] = 1;
      block[xi+1][yi-1] = 1;
      block[xi][yi+1] = 1;

      blockrotation = 1;
    }
    else if (blockrotation == 1)
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }
      xi--;

      block[xi][yi-1] = 0;
      block[xi+1][yi-1] = 0;
      block[xi][yi+1] = 0;

      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      block[xi+1][yi+1] = 1;

      blockrotation = 2;
    }
    else if (blockrotation == 2)
    {
      yi --;

      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;
      block[xi+1][yi+1] = 0;

      block[xi][yi-1] = 1;
      block[xi][yi+1] = 1;
      block[xi-1][yi+1] = 1;

      blockrotation = 3;
    }
    else
    {
      if (!space_right())
      {
        if (!moveleft())
          return;
        xi--;
      }
      block[xi][yi-1] = 0;
      block[xi][yi+1] = 0;
      block[xi-1][yi+1] = 0;

      block[xi-1][yi-1] = 1;
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;

      blockrotation = 0;
    }
  }

  if (blocktype == 2)
  {
    if (blockrotation == 0)
    {
      block[xi+1][yi-1] = 0;
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;

      block[xi][yi-1] = 1;
      block[xi+1][yi+1] = 1;
      block[xi][yi+1] = 1;

      blockrotation = 1;
    }
    else if (blockrotation == 1)
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }
      xi--;

      block[xi][yi-1] = 0;
      block[xi+1][yi+1] = 0;
      block[xi][yi+1] = 0;

      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      block[xi-1][yi+1] = 1;

      blockrotation = 2;
    }
    else if (blockrotation == 2)
    {
      yi --;

      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;
      block[xi-1][yi+1] = 0;

      block[xi][yi-1] = 1;
      block[xi][yi+1] = 1;
      block[xi-1][yi-1] = 1;

      blockrotation = 3;
    }
    else
    {
      if (!space_right())
      {
        if (!moveleft())
          return;
        xi--;
      }
      block[xi][yi-1] = 0;
      block[xi][yi+1] = 0;
      block[xi-1][yi-1] = 0;

      block[xi+1][yi-1] = 1;
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;

      blockrotation = 0;
    }
  }

  if (blocktype == 4)
  {
    if (blockrotation == 0)
    {
      block[xi+1][yi-1] = 0;
      block[xi-1][yi] = 0;

      block[xi+1][yi] = 1;
      block[xi+1][yi+1] = 1;

      blockrotation = 1;
    }
    else
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }
      xi--;

      block[xi+1][yi] = 0;
      block[xi+1][yi+1] = 0;

      block[xi-1][yi] = 1;
      block[xi+1][yi-1] = 1;

      blockrotation = 0;
    }
  }


  if (blocktype == 5)
  {
    if (blockrotation == 0)
    {
      block[xi][yi-1] = 0;
      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;

      block[xi][yi-1] = 1;
      block[xi+1][yi] = 1;
      block[xi][yi+1] = 1;

      blockrotation = 1;
    }
    else if (blockrotation == 1)
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }
      xi--;

      block[xi][yi-1] = 0;
      block[xi+1][yi] = 0;
      block[xi][yi+1] = 0;

      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;
      block[xi][yi+1] = 1;

      blockrotation = 2;
    }
    else if (blockrotation == 2)
    {
      yi --;

      block[xi-1][yi] = 0;
      block[xi+1][yi] = 0;
      block[xi][yi+1] = 0;

      block[xi][yi-1] = 1;
      block[xi-1][yi] = 1;
      block[xi][yi+1] = 1;

      blockrotation = 3;
    }
    else
    {
      if (!space_right())
      {
        if (!moveleft())
          return;
        xi--;
      }
      block[xi][yi-1] = 0;
      block[xi-1][yi] = 0;
      block[xi][yi+1] = 0;

      block[xi][yi-1] = 1;
      block[xi-1][yi] = 1;
      block[xi+1][yi] = 1;

      blockrotation = 0;
    }
  }

  if (blocktype == 6)
  {
    if (blockrotation == 0)
    {
      block[xi-1][yi-1] = 0;
      block[xi][yi-1] = 0;

      block[xi+1][yi-1] = 1;
      block[xi][yi+1] = 1;

      blockrotation = 1;
    }
    else
    {
      if (!space_left())
      {
        if (!moveright())
          return;
        xi++;
      }
      xi--;

      block[xi+1][yi-1] = 0;
      block[xi][yi+1] = 0;

      block[xi-1][yi-1] = 1;
      block[xi][yi-1] = 1;

      blockrotation = 0;
    }
  }

  //if rotating made block and pile overlap, push rows up
  while (!check_overlap())
  {
    for (i=0;i<18;i++)
    {
      for (j=0;j<8;j++)
      {
         block[j][i] = block[j][i+1];
      }
    }
    delays = millis() + delay_;
  }


  updateHoldingArray();
}


// ------------ update holding array before displaying to LED matrix --------------
void updateHoldingArray()
{
  int i;
  int j;
  for (i=0;i<8;i++)
  {
    for (j=0;j<16;j++)
    {
      disp[i][j] = block[i][j] | pile[i][j];
    }
  }
}


// ----------------- check game state ---------------------------------------------
boolean check_overlap()
{
  int i;
  int j;
  for (i=0;i<16;i++)
  {
    for (j=0;j<7;j++)
    {
       if (block[j][i])
       {
         if (pile[j][i])
           return false;
       }
    }
  }
  for (i=16;i<18;i++)
  {
    for (j=0;j<7;j++)
    {
       if (block[j][i])
       {
         return false;
       }
    }
  }
  return true;
}

void check_gameover()
{
  int i;
  int j;
  int cnt=0;;

  for(i=15;i>=0;i--)
  {
    cnt=0;
    for (j=0;j<8;j++)
    {
      if (pile[j][i])
      {
        cnt ++;
      }
    }
    // remove filled lines
    if (cnt == 8)
    {
      for (j=0;j<8;j++)
      {
        pile[j][i]=0;
      }
      updateHoldingArray();
      delay(50);

      int k;
      for(k=i;k>0;k--)
      {
        for (j=0;j<8;j++)
        {
          pile[j][k] = pile[j][k-1];
        }
      }
      for (j=0;j<8;j++)
      {
        pile[j][0] = 0;
      }
      updateHoldingArray();
      delay(50);
      i++;
      score++;
      updateScore(score);
    }
  }

  for(i=0;i<8;i++)
  {
    if (pile[i][0])
      gameover();
  }
  return;
}

/*
 Method to seperate the 4 digit numbers into thousands, hundreds,
 tens and ones for displaying on the 7 segment display
*/
void updateScore(int num) {
    unsigned int digits[4];
    static unsigned int last = 9999;
    
    if ((num > 9999) || (num == last))
        return;
    
    last = num;
    
    for (int i=3; i>=1; i--) {
        digits[i] = num % 10;
        num = num /10;
    }
    digits[0] = num;
    
    for (int i=3; i>=0; i--) {
        scoreLC.setDigit(0,i,(byte)digits[i],false);
    }
}

void gameover()
{
  delay(150);

  // gameover animation 
  byte ani01 = B11111111;
  byte ani02 = B00000000;
  // on
  // lower
  for(int j=7; j>=0; j--) {
    lc.setRow(1,j, ani01);
    delay(50);
  }

  // upper
  for(int j=7; j>=0; j--) {
    lc.setRow(0,j, ani01);
    delay(50);
  }

  delay(100);
  // off
  // upper
  for(int j=0; j<8; j++) {
    lc.setRow(0,j, ani02);
    delay(50);
  }
  
  // lower
  for(int j=0; j<8; j++) {
    lc.setRow(1,j, ani02);
    delay(50);
  }
  
  while (true) {
    if ((digitalRead(buttonRotate) == LOW)) break;
  }

  // reset the arrays
  memset(disp, 0, sizeof(disp[0][0]) * 8 * 16);
  memset(pile, 0, sizeof(disp[0][0]) * 8 * 16);
  memset(block, 0, sizeof(disp[0][0]) * 8 * 18);
  score = 0;
  updateScore(score);
  
  delay(100);
}


// ----------------- check for space left ------------------------------------
boolean space_left()
{
  int i;
  int j;
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (j == 0)
           return false;
         if (pile[j-1][i])
         {
           return false;
         }
       }
    }
  }
  return true;
}

boolean space_left2()
{
  int i;
  int j;
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (j == 0 || j == 1)
           return false;
         if (pile[j-1][i] | pile[j-2][i])
         {
           return false;
         }
       }
    }
  }
  return true;
}

boolean space_left3()
{
  int i;
  int j;
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (j == 0 || j == 1 ||j == 2 )
           return false;
         if (pile[j-1][i] | pile[j-2][i]|pile[j-3][i])
         {
           return false;
         }
       }
    }
  }
  return true;
}

boolean space_right()
{
  int i;
  int j;
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (j == 7)
           return false;
         if (pile[j+1][i])
         {
           return false;
         }
       }
    }
  }
  return true;
}

boolean space_right2()
{
  int i;
  int j;
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (j == 7 || j == 6)
           return false;
         if (pile[j+1][i] |pile[j+2][i])
         {
           return false;
         }
       }
    }
  }
  return true;
}

boolean space_right3()
{
  int i;
  int j;
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (j == 7||j == 6||j == 5)
           return false;
         if (pile[j+1][i] |pile[j+2][i] | pile[j+3][i])
         {
           return false;
         }
       }
    }
  }
  return true;
}

boolean space_below()
{
  int i;
  int j;
  for (i=15;i>=0;i--)
  {
    for (j=0;j<8;j++)
    {
       if (block[j][i])
       {
         if (i == 15)
           return false;
         if (pile[j][i+1])
         {
           return false;
         }
       }
    }
  }
  return true;
}


//------------------------ Generate blocks ---------------------------------------
void newBlock()
{
  check_gameover();

  blocktype = random(7);


  if (blocktype == 0)
  // 0
  // 0
  // 0
  // 0
  {
    block[3][0]=1;
    block[3][1]=1;
    block[3][2]=1;
    block[3][3]=1;
  }

  if (blocktype == 1)
  // 0
  // 0 0 0
  {
    block[2][0]=1;
    block[2][1]=1;
    block[3][1]=1;
    block[4][1]=1;
  }

  if (blocktype == 2)
  //     0
  // 0 0 0
  {
    block[4][0]=1;
    block[2][1]=1;
    block[3][1]=1;
    block[4][1]=1;
  }

  if (blocktype == 3)
  // 0 0
  // 0 0
  {
    block[3][0]=1;
    block[3][1]=1;
    block[4][0]=1;
    block[4][1]=1;
  }

  if (blocktype == 4)
  //   0 0
  // 0 0
  {
    block[4][0]=1;
    block[5][0]=1;
    block[3][1]=1;
    block[4][1]=1;
  }

  if (blocktype == 5)
  //   0
  // 0 0 0
  {
    block[4][0]=1;
    block[3][1]=1;
    block[4][1]=1;
    block[5][1]=1;
  }

  if (blocktype == 6)
  // 0 0
  //   0 0
  {
    block[3][0]=1;
    block[4][0]=1;
    block[4][1]=1;
    block[5][1]=1;
  }

  blockrotation = 0;
}


// --------------------- update LED Matrix ------------------------------------------------
void LEDRefresh()
{
    int i;
    int k;
    //////////////////////////////////////////////

    for(i=0;i<8;i++)
    {
       byte upper = 0;
       int b;
       for(b = 0;b<8;b++) {
         upper <<= 1;
         if (disp[b][i]) upper |= 1;
       }

       lc.setRow(0, i, upper);

       //Serial.print(upper);

       byte lower = 0;
       for(b = 0;b<8;b++) {
         lower <<= 1;
         if (disp[b][i+8]) lower |= 1;
       }

      lc.setRow(1, i, lower);

      //Serial.println(lower);
    }
}
