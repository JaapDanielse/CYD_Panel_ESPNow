// CYD_Panel_ESPNow C-ESPN-Monitor-Info example - Documentation https://github.com/JaapDanielse/CYD_Panel/wiki/Basic-Examples
// GNU Licence - Copyright (c) 2026 Jaap Danielse - https://github.com/JaapDanielse/CYD_Panel

// CYD_Panel has an extra panel called monitor providing extra display space. 
// It comes particularly handy when debugging.
// The info panel is to provide info on the experiment
// (function, which pins are used for what etc.)
// Use the small buttons on top of the screen to switch between Display, Monitor and Info


#include "CYD_Panel_ESPNow.h"

uint32_t Timer = 0;

bool    Power = false; // Power status
int16_t Value = 0;  // Integer variabel
int16_t Factor = 50; //

void setup() 
{
  Panel.begin(); // init port and protocol
}

void loop() 
{
  Panel.receive(); // handle panel events form the panel (must be in the loop)

  if(Timer < millis())
  {
    Timer = millis() + 500;
    if (Power) Value = random(1,100); else Value = 0; // my own proces
    Panel.sendf(CYD_Display_1, "Value %d",  Value); // Show Value on display_1
    Panel.sendf(CYD_Display_3, "Result %d", Value*Factor); // Multiplication of Value and Factor on display_3
    Panel.sendf(CYD_MonitorField_1, "Value %d", Value); // Show Value on MonitorField_1
    Panel.sendf(CYD_MonitorField_2, "Factor %d", Factor); // Show Value on MonitorField_2
    Panel.sendf(CYD_MonitorField_3, "Calculated: %d", Value * Factor ); // Show Value on MonitorField_3
  }
}

void PanelCallback(CYD_channel event) 
{ 
  switch (event) 
  {
    case CYD_PanelConnected: // receive panel connected event
      Panel.send(CYD_ApplicationName,"Monitor-Info"); // set the application name
      Panel.send(CYD_Led_1, CYD_COLOR, CYD_BLACK); // Turn Led_1 off (black)
      Panel.sendf(CYD_Display_2, "Factor %d", Factor); // Show Factor on display_2
      Panel.send(CYD_Button_1,"on/off"); // Button_3 visible and set text "on/off"
      Panel.send(CYD_Slider_2, "factor"); // Set Slider label
      Panel.send(CYD_Slider_2, CYD_MAXVALUE, (int16_t)100); // Set Slider maximum value
      Panel.send(CYD_Slider_2, Factor); // Set Slider initial Value

      Panel.send(CYD_InfoPanel, CYD_TITLE,"Monitor and Info"); // Info Title 
      Panel.send(CYD_InfoPanel, "The Monitor can be used for extra displays");
      Panel.send(CYD_InfoPanel, "particularly handy for debugging.");
      Panel.send(CYD_InfoPanel, "Info is for informational texts like this.");
      Panel.send(CYD_InfoPanel, "Both can be activated via the small top buttons");
      break;

    case CYD_Button_1: // Catch button pressed
      Power = !Power;
      if (Power) 
        Panel.send(CYD_Led_1, CYD_COLOR, CYD_RED); // show Power
      else
        Panel.send(CYD_Led_1, CYD_COLOR, CYD_BLACK); // show !Power
      break;

    case CYD_Slider_2: // catch slider change
      Factor = Panel.cydr_int; 
      Panel.sendf(CYD_Display_2, "Factor %d", Factor); // Show Factor on display_2
      break;

      default: break;
  }
}

