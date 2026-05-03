// CYD_Panel_ESPNow B-ESPN-Slider example - Documentation https://github.com/JaapDanielse/CYD_Panel/wiki/Basic-Examples
// GNU Licence - Copyright (c) 2026 Jaap Danielse - https://github.com/JaapDanielse/CYD_Panel

#include "CYD_Panel_ESPNow.h"

uint32_t Timer = 0; // display timer

bool Power  = false; // Power status
int  Value  = 0;  // Integer variabel
int  Factor = 50; //


void setup() 
{
  Panel.begin(); // init port and protocol
}

void loop() 
{
  Panel.receive(); // handle panel events form the panel (must be in the loop)

  if( Timer < millis()) // pace the application and display effort
  {
    Timer = millis() + 500; // set timer 
    if (Power) Value = random(1,100); else Value = 0; // my own proces 
    Panel.sendf(CYD_Display_1, "Value %d",  Value); // Show Value on display 1
    Panel.sendf(CYD_Display_3, "Result %d", Value * Factor); // Multiplication of Value and Factor on display 3
  }
}

void PanelCallback(CYD_channel event) 
{ 
  switch (event) 
  {
    case CYD_PanelConnected: // receive panel connected event
      Panel.send(CYD_ApplicationName,"ScrollBar"); // set the application name
      Panel.sendf(CYD_Display_2, "Factor %d", Factor); // Show Factor on display_2
      Panel.send(CYD_Led_1, CYD_COLOR, CYD_BLACK); // Turn Led_1 off (black)
      Panel.send(CYD_Button_1,"on/off"); // Button_3 visible and set text "on/off"
      Panel.send(CYD_Slider_2, "factor"); // Set Slider label
      Panel.send(CYD_Slider_2, CYD_MAXVALUE, 100); // Set Slider maximum value
      Panel.send(CYD_Slider_2, Factor); // Set Slider initial Value
      break;

    case CYD_Button_1: // Catch button pressed
      Power = !Power;
      if (Power)
        Panel.send(CYD_Led_1, CYD_COLOR, CYD_RED); // Turn Led_1 on (red)
      else
        Panel.send(CYD_Led_1, CYD_COLOR, CYD_BLACK); // Turn Led_1 off (black)
      break;

    case CYD_Slider_2: // Catch Slider change
      Factor = Panel.cydr_int;
      Panel.sendf(CYD_Display_2, "Factor %d", Factor); // Show Factor on display_3
      break;
      
      default: break;
  }
}
