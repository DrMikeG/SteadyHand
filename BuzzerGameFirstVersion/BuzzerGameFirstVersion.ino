#include <HCMAX7219.h>
#include "SPI.h"

/* Set the LOAD (CS) digital pin number*/
#define LOAD 10

#define GAMESTATE_WAITINGTOREGISTER_START 0
#define GAMESTATE_CROSSINGWIRE 1
#define GAMESTATE_TOUCHEDWIRE 2
#define GAMESTATE_REACHEDEND 3
 
/* Create an instance of the library */
HCMAX7219 HCMAX7219(LOAD);

unsigned long highScore = 6300;

unsigned long startTime;
unsigned long time;

const byte interruptPin = 2;
unsigned int gameState = GAMESTATE_WAITINGTOREGISTER_START;

volatile boolean state = false;


void setup() 
{        
  startTime = 0;
  Serial.begin(9600);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);  
}

void blink() {
  state = !state;
  if (gameState == GAMESTATE_CROSSINGWIRE && !state)
  {
    gameState = GAMESTATE_TOUCHEDWIRE;
  }
 
}// end blink



void StartGame()
{
  startTime = millis();
  gameState = GAMESTATE_CROSSINGWIRE;
  Serial.print("Starting game at ");
  Serial.println(startTime);
}


void Buzz()
{
    Serial.print("Buzzing!");
    tone(8, 196,1500);
}

void FlashFailedMessage()
{
        HCMAX7219.Clear();
        HCMAX7219.Refresh();  
      Serial.println("Flashing Failed Message ");
      for (int Loopcounter = 0; Loopcounter <= 5; Loopcounter++)
      {
        HCMAX7219.print7Seg("FAILED",8);
        HCMAX7219.Refresh();  
        delay(400);
        HCMAX7219.Clear();
        HCMAX7219.Refresh();  
        delay(200);
    }// end for loop counter
}//end FlashFailedMessage

void ScrollHighScore()
{
  if (highScore == 0)
  {
        HCMAX7219.Clear();
        HCMAX7219.Refresh();  

    // No highscore
    for (int Loopcounter = 0; Loopcounter <= 2; Loopcounter++)
    {
      for (int Position = 0; Position <= DISPLAYBUFFERSIZE + 12; Position++)
      { 
        HCMAX7219.print7Seg("No highscore ",Position);
        HCMAX7219.Refresh();  
        delay(200);
      }// end for position
    }// end for loop counter
  }
  else
  {
      // Convert the highscore to a string of known length and append to message...
      String highscoreAsString = String(highScore);
      String highscoreMessage =  String("highscore = "+highscoreAsString+" ");

      char textString[50];
      highscoreMessage.toCharArray(textString,50);
      
      for (int Loopcounter = 0; Loopcounter <= 2; Loopcounter++)
    {
      for (int Position = 0; Position <= DISPLAYBUFFERSIZE + 17; Position++)
      { 
        HCMAX7219.print7Seg(textString,Position);
        HCMAX7219.Refresh();  
        delay(200);
      }// end for position
    }// end for loop counter
  }
  
}

void DisplayBeginMessage()
{
  Serial.println("Display Begin Message");
    for (int Loopcounter = 0; Loopcounter <= 2; Loopcounter++)
    {
      for (int Position = 0; Position <= DISPLAYBUFFERSIZE+21; Position++)
      { 
        HCMAX7219.print7Seg("Touch start to begin! ",Position);
        HCMAX7219.Refresh();  
        delay(200);
      }// end for position
    }// end for loop counter
}//end DisplayBeginMessage


void DisplayTimer()
{
    if (gameState == GAMESTATE_CROSSINGWIRE)
    {
        time = ((time-startTime) / 100)+1;        
        int decimalPlaces = floor(log(time)/log(10))+1; 
        HCMAX7219.Clear();
        HCMAX7219.print7Seg(time,1,decimalPlaces);
        HCMAX7219.Refresh();    
        delay(100);
    }
}



/* Main program */
void loop() 
{
  time = millis();
  
    if (gameState == GAMESTATE_CROSSINGWIRE)
    {
        DisplayTimer();
    }
    else if (gameState == GAMESTATE_WAITINGTOREGISTER_START)
    {
        //DisplayBeginMessage();  
        // Force start...
        StartGame();     
    }
    else if (gameState == GAMESTATE_TOUCHEDWIRE)
    {
        Buzz();
        FlashFailedMessage();
        ScrollHighScore();
        DisplayBeginMessage();
        // Force restart...
        gameState = GAMESTATE_WAITINGTOREGISTER_START;
    }
    
    
}// end main loop
