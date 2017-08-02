#include <HCMAX7219.h>
#include "SPI.h"

// The three contact setup is as follows:

// Speaker - D8 and Ground

// 8-seg display 
// VCC - 5V
// gnd - gnd
// DIN - D11
// CSI - D10
// CLK - D13

// Wand lead - D2 (interupt detect) pulled up to 3.3v

// Home washer - A0 - pulled low with 10k resistor to ground
// End  washer - A2 - pulled low with 10k resistor to ground
// Game wire - A5 -pulled low with 10k resistor to ground


/* Set the LOAD (CS) digital pin number*/
#define LOAD 10

#define GAMESTATE_WAITINGTOREGISTER_START 0
#define GAMESTATE_REGISTERED_START 1
#define GAMESTATE_CROSSINGWIRE 2
#define GAMESTATE_TOUCHEDWIRE 3
#define GAMESTATE_REACHEDEND 4
 
/* Create an instance of the library */
HCMAX7219 HCMAX7219(LOAD);

unsigned long highScore = 0;
unsigned long lastScore = 0;

unsigned long startTime;
unsigned long time;

const byte interruptPin = 2;
unsigned int gameState = GAMESTATE_WAITINGTOREGISTER_START;

const byte startZonePin = A0;
const byte endZonePin = A2;
const byte wirePin = A5;

volatile int startZonePinValue = 0;
volatile int endZonePinValue = 0;
volatile int wirePinValue = 0;

void setup() 
{        
  startTime = 0;
  Serial.begin(9600);
  // Attach an interrupt to pin D2
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);  

  // Setup
  pinMode(startZonePin, INPUT);
  digitalWrite(startZonePin, LOW);
  pinMode(endZonePin, INPUT);
  digitalWrite(endZonePin, LOW);    
  pinMode(wirePin, INPUT);
  digitalWrite(wirePin, LOW);    
}

long sZTot;
long eZTot;
long wireTot;

void blink() {

  int loopN = 10;
  for (int i=0; i < loopN; i++)
  {
    sZTot+=analogRead(startZonePin);
    eZTot+=analogRead(endZonePin);
    wireTot+=analogRead(wirePin);
  }
  startZonePinValue = sZTot / loopN;
  endZonePinValue = eZTot / loopN;
  wirePinValue = wireTot / loopN;
  
  startZonePinValue = analogRead(startZonePin);
  //Serial.print("startZonePinValue: ");
  //Serial.println(startZonePinValue);

  endZonePinValue = analogRead(endZonePin);
  //Serial.print("endZonePinValue: ");
  //Serial.println(endZonePinValue);

  wirePinValue = analogRead(wirePin);
  //Serial.print("wirePinValue: ");
  //Serial.println(wirePinValue);
  
  
  if (startZonePinValue > 150)
  {
      Serial.println("Touched start zone!");
      gameState = GAMESTATE_REGISTERED_START;
  }
  else if (gameState == GAMESTATE_CROSSINGWIRE && wirePinValue > 150)
  {
      gameState = GAMESTATE_TOUCHEDWIRE;
  }
  else if (gameState == GAMESTATE_CROSSINGWIRE && endZonePinValue > 150)
  {
      gameState = GAMESTATE_REACHEDEND;
  }
 
}// end blink



void StartGame()
{
  startTime = millis();
  gameState = GAMESTATE_CROSSINGWIRE;
  Serial.print("Starting game at ");
  Serial.println(startTime);
  StartBuzz();
}

void StartBuzz()
{
    Serial.print("Start Buzzing!");
    tone(8, 880,100);
}


// notes in the melody:
int melody[] = {
  262, 196, 196, 220, 19, 0, 247, 262
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void GoodBuzz()
{
    Serial.print("Good Tune Buzzing!");
     for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(8, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
}

void BadBuzz()
{
    Serial.print("Bad Buzzing!");
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
    HCMAX7219.Clear();
}//end FlashFailedMessage

void FlashSuccessMessage()
{
        HCMAX7219.Clear();
        HCMAX7219.Refresh();  
      Serial.println("Flashing Success Message ");
      for (int Loopcounter = 0; Loopcounter <= 5; Loopcounter++)
      {
        HCMAX7219.print7Seg("Success!!",8);
        HCMAX7219.Refresh();  
        delay(400);
        HCMAX7219.Clear();
        HCMAX7219.Refresh();  
        delay(200);
    }// end for loop counter
    HCMAX7219.Clear();
}//end FlashFailedMessage

void RecordLastScore()
{ 
  Serial.print("Completed in new time ");
  Serial.println(time);
  lastScore = (time-startTime)/10;
  if (lastScore < highScore)
  {
    highScore = lastScore;
  }
  else if (highScore == 0)
  {
    highScore = lastScore;
  }
}

void ScrollThisScore()
{
    if (lastScore == highScore)
    {
      // New high score!
      /*      
      int spinDelay = 200;
      for (int i=0; i < 2; i++)
      {
      HCMAX7219.print7Seg("--------",8);
      HCMAX7219.Refresh();  
      delay(spinDelay);
      HCMAX7219.print7Seg("\\\\\\\\\\\\\\\\",8);
      HCMAX7219.Refresh();  
      delay(spinDelay);
      
      HCMAX7219.print7Seg("||||||||",8);
      HCMAX7219.Refresh();  
      delay(spinDelay);
      
      HCMAX7219.print7Seg("\/\/\/\/\/\/\/\/",8);
      HCMAX7219.Refresh();  
      delay(spinDelay);
      }
      
      HCMAX7219.print7Seg("********",8);
      HCMAX7219.Refresh();  
      delay(spinDelay);

      HCMAX7219.Clear();
      delay(spinDelay);
      
      HCMAX7219.print7Seg("********",8);
      HCMAX7219.Refresh();  
      delay(3*spinDelay);
      */
      
      HCMAX7219.Clear();
      HCMAX7219.Refresh();  
      for (int Position = 0; Position <= DISPLAYBUFFERSIZE + 15 && !GameQuickStart(); Position++)
      { 
        HCMAX7219.print7Seg("New highscore! ",Position);
        HCMAX7219.Refresh();  
        delay(200);
       }// end for position
    }
    HCMAX7219.Clear();

    
    // Convert the highscore to a string of known length and append to message...
    String highscoreAsString = String(lastScore);
    String highscoreMessage =  String("score = "+highscoreAsString+" ");
    
    char textString[50];
    highscoreMessage.toCharArray(textString,50);
    
    for (int Loopcounter = 0; Loopcounter <= 2 && !GameQuickStart(); Loopcounter++)
    {
      for (int Position = 0; Position <= DISPLAYBUFFERSIZE + 7 && !GameQuickStart(); Position++)
      { 
        HCMAX7219.print7Seg(textString,Position);
        HCMAX7219.Refresh();  
        delay(200);
      }// end for position
      delay(1000);
      HCMAX7219.Clear();
    }// end for loop counter
    HCMAX7219.Clear();
    
}

void ScrollHighScore()
{
  if (highScore == 0)
  {
        HCMAX7219.Clear();
        HCMAX7219.Refresh();  

    // No highscore
    for (int Loopcounter = 0; Loopcounter < 2 && !GameQuickStart(); Loopcounter++)
    {
      for (int Position = 0; Position <= DISPLAYBUFFERSIZE + 12 && !GameQuickStart(); Position++)
      { 
        HCMAX7219.print7Seg("No highscore ",Position);
        HCMAX7219.Refresh();  
        delay(200);
      }// end for position
    HCMAX7219.Clear();
    }// end for loop counter

    HCMAX7219.Clear();
  }
  else
  {
      // Convert the highscore to a string of known length and append to message...
      String highscoreAsString = String(highScore);
      String highscoreMessage =  String("highscore = "+highscoreAsString+" ");

      char textString[50];
      highscoreMessage.toCharArray(textString,50);
      
      for (int Loopcounter = 0; Loopcounter <= 2 && !GameQuickStart(); Loopcounter++)
    {
      for (int Position = 0; Position <= DISPLAYBUFFERSIZE + 17 && !GameQuickStart(); Position++)
      { 
        HCMAX7219.print7Seg(textString,Position);
        HCMAX7219.Refresh();  
        delay(200);
      }// end for position
    }// end for loop counter

    HCMAX7219.Clear();
  }
  
}

void DisplayBeginMessage()
{
  Serial.println("Display Begin Message");
      for (int Position = 0; Position <= DISPLAYBUFFERSIZE+21 && !GameQuickStart(); Position++)
      { 
        HCMAX7219.print7Seg("Touch start to begin! ",Position);
        HCMAX7219.Refresh();  
        delay(200);
      }// end for position

      HCMAX7219.Clear();
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

bool GameQuickStart()
{
  return gameState == GAMESTATE_REGISTERED_START;
}

/* Main program */
void loop() 
{

  //highScore = 63000;
  //lastScore = 63000;
  //ScrollThisScore();

  
  time = millis();
  
    if (gameState == GAMESTATE_CROSSINGWIRE)
    {
        DisplayTimer();
    }
    else if (gameState == GAMESTATE_REGISTERED_START)
    {
        StartGame();    
    }
    else if (gameState == GAMESTATE_WAITINGTOREGISTER_START)
    {
        DisplayBeginMessage();       
    }
    else if (gameState == GAMESTATE_TOUCHEDWIRE)
    {
        BadBuzz();
        FlashFailedMessage();
        gameState = GAMESTATE_WAITINGTOREGISTER_START;
        ScrollHighScore();
        DisplayBeginMessage();
        
    }
    else if (gameState == GAMESTATE_REACHEDEND)
    {
        RecordLastScore();
        GoodBuzz();
        FlashSuccessMessage();
        ScrollThisScore();
        ScrollHighScore();
        gameState = GAMESTATE_WAITINGTOREGISTER_START;   
    }
    
    
}// end main loop
