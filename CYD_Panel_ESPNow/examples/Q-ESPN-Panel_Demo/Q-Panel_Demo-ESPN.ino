
// CYD_Panel_ESPNow Q_Panel_Demo example - Documentation https://github.com/JaapDanielse/CYD_Panel/wiki/Basic-Examples
// GNU Licence - Copyright (c) 2026 Jaap Danielse - https://github.com/JaapDanielse/CYD_Panel
/*
  V1.0.0 29-04-2026 Original Version
*/ 

#include "CYD_Panel_ESPNow.h"


enum ModeList 
{
  L1_Buttons,
  L1_Rolling,
  L1_Static,
  L2_Buttons,
  L3_Buttons,
  L4_Buttons,
  Endmode
};
int PanelMode = L1_Buttons;

// AppTimer
typedef void (*functionPtr)(void);
functionPtr AppTimerPtr;
bool AppTimerOn = false;
uint32_t AppTimerEnd = 0;
uint16_t AppTimerInterval = 0;


//--------------------------------------------------------------------------------------------
void setup() 
{
  Serial.begin(115200);
  Serial.println("CYD_TestPanel");

  WiFiChannel = 1; // set the WiFi / ESPNow channel

  Panel.begin();
}


//--------------------------------------------------------------------------------------------
void loop() 
{
  Panel.receive();
  AppTimer();
}


//--------------------------------------------------------------------------------------------
void AppTimer()
{
  if(!AppTimerOn) return;
  
  if(millis() > AppTimerEnd)
  {
    AppTimerEnd = millis() + AppTimerInterval; 
    AppTimerPtr();
  }
}

//--------------------------------------------------------------------------------------------
void SetAppTimer(uint16_t interval, functionPtr appPtr, bool run)
{
  AppTimerInterval = interval;
  AppTimerPtr = appPtr;
  AppTimerOn = run;
}


