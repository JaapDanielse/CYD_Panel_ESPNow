
// CYD_Panel_ESPNow G-ESPN-Blink-Remote-Led example - Documentation https://github.com/JaapDanielse/CYD_Panel/wiki/Basic-Examples
// GNU Licence - Copyright (c) 2026 Jaap Danielse - https://github.com/JaapDanielse/CYD_Panel
// This sketch is a bit of a gamble since the LED_BUILTIN define was not defined or defined wrong.
// Just try it out.


#include "CYD_Panel_ESPNow.h"

bool      Power = false; // Power status
int       Speed = 0;  // Integer variabel
uint32_t  Timer = 0;
bool      LedOn = false;

#define LED_PIN 2 // tested on ESP32D and ESP8266 (Board: ESP32 Dev Module)
// #define LED_PIN 8 // tested on ESP32C3 (Board: Nologo ESPC3 Super Mini)

#define LED_ON HIGH   // select required setting (board dependent)
// #define LED_ON LOW // 


void setup() 
{
  Panel.begin(); // init port and protocol
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, !LED_ON); 
}

void loop() 
{
  Panel.receive(); // handle panel events form the panel (must be in the loop)

  if(Power)
  {
    if(Timer < millis()) 
    {
      Timer = millis() + (1000 / (Speed + 1));
      if(LedOn) 
        digitalWrite(LED_PIN, !LED_ON); 
      else
        digitalWrite(LED_PIN, LED_ON); 
      LedOn = !LedOn;
    }   
  }
  else
  {
    digitalWrite(LED_PIN, !LED_ON); 
    delay(250);
  }
}

void PanelCallback(CYD_channel event) 
{ 
  switch (event) 
  {
    case CYD_PanelConnected: // receive panel connected event
      Panel.send(CYD_ApplicationName,"Blink Remote Led"); // set the application name
      Panel.send(CYD_Led_1, CYD_COLOR, CYD_BLACK); // Turn Led_1 off (black)
      Panel.send(CYD_Button_1,"on/off"); // Button_3 visible and set text "on/off"
      Panel.send(CYD_Slider_2, CYD_MAXVALUE, 9); // Slider_2 maximum value 10 
      Panel.send(CYD_Slider_2, Speed); // Slider_2 current value (1)
      Panel.send(CYD_Slider_2,"Blink Speed"); // Slider_2 visible and set text "Blink Speed"
      break;

    case CYD_Button_1: // Catch button pressed
      Power = !Power;
      if (Power)
        Panel.send(CYD_Led_1, CYD_COLOR, CYD_RED); // Turn Led_1 on (red)
      else
        Panel.send(CYD_Led_1, CYD_COLOR, CYD_BLACK); // Turn Led_1 off (black)
      break;

    case CYD_Slider_2: // Catch Slider change
      Speed = Panel.cydr_int;
      Panel.sendf(CYD_Display_1, "Blink speed: %d", Speed + 1); // Show Factor on display_3
      break;

      default: break;
  }
}
