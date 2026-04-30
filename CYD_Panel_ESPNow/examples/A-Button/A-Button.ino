
// CYD_Panel_ESPNow A-Button example - Documentation https://github.com/JaapDanielse/CYD_Panel/wiki/Basic-Examples
// GNU Licence - Copyright (c) 2026 Jaap Danielse - https://github.com/JaapDanielse/CYD_Panel

#include "CYD_Panel_ESPNow.h"

bool    Power = false; // Power status
int16_t Value = 0;  // Integer variabel

void setup() 
{
  Panel.begin(); // init port and protocol
}

void loop() 
{
  Panel.receive(); // handle panel events form the panel (must be in the loop)
  if (Power) Value = random(1,100); else Value = 0; // my own proces
  Panel.sendf(CYD_Display_1,"%d",Value); // Show Value on display_1
  delay(250); // delay 
}

void PanelCallback(CYD_channel event) 
{ 
  switch (event) 
  {
    case CYD_PanelConnected: // receive panel connected event
      Panel.send(CYD_ApplicationName,"Button"); // set the application name
      Panel.send(CYD_Led_1, CYD_COLOR, CYD_BLACK); // Turn Led_1 off (black)
      Panel.send(CYD_Button_1,"on/off"); // Button_3 visible and set text "on/off"
      break;

    case CYD_Button_1: // Catch button pressed
      Power = !Power;
      if (Power)
        Panel.send(CYD_Led_1, CYD_COLOR, CYD_RED); // Turn Led_1 on (red)
      else
        Panel.send(CYD_Led_1, CYD_COLOR, CYD_BLACK); // Turn Led_1 off (black)
      break;

      default: break;

  }
}
