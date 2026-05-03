// CYD_Panel_ESPNow D-ESPN-RollingGraph example - Documentation https://github.com/JaapDanielse/CYD_Panel/wiki/Basic-Examples
// GNU Licence - Copyright (c) 2026 Jaap Danielse - https://github.com/JaapDanielse/CYD_Panel

#include "CYD_Panel_ESPNow.h"

uint32_t Timer = 0; // display timer

byte yellow = 0;
byte green = 0;   
byte blue = 0;   

void setup() 
{
  Panel.begin(); // init port and protocol
}

void loop() 
{
  Panel.receive(); // handle panel events form the panel (must be in the loop)

  if( Timer < millis()) // pace the application and display effort
  {
    Timer = millis() + 100; // set timer  

    yellow = random(0,255); // generate yellow "sensor" value
    green  = random(0,255); // generate orange "sensor" value
    blue   = random(0,255); // generate red "sensor" value
    
    Display();
  }
}

void Display()
{
  Panel.sendf(CYD_Display_1,"y:%03d  g:%03d  r:%03d", yellow, green, blue); 
  Panel.send(CYD_Graph_1, (byte) map(yellow, 0, 255, 145, 215)); // map yellow to top part of the display
  Panel.send(CYD_Graph_2, (byte) map(green, 0, 255, 75, 145)); // map green to the middle part 
  Panel.send(CYD_Graph_3, (byte) map(blue, 0, 255, 5, 75)); // map blue to the bottom part
}

void PanelCallback(CYD_channel event) 
{ 
  switch (event) 
  {
    case CYD_PanelConnected: // receive panel connected event
      Panel.send(CYD_ApplicationName,"RollingGraph"); // set the application name
      
      Panel.send(CYD_Graph_1, CYD_COLOR, CYD_YELLOW);
      Panel.send(CYD_Graph_2, CYD_COLOR, CYD_GREEN);
      Panel.send(CYD_Graph_3, CYD_COLOR, CYD_RED);
      
      Panel.send(CYD_InfoPanel, CYD_TITLE, "Roling Graph"); // Info Title 
      Panel.send(CYD_InfoPanel, "The graph panel is set visible");
      Panel.send(CYD_InfoPanel, "By simply pushing value (between 0 and 255)"); 
      Panel.send(CYD_InfoPanel, "to the different Graph_x value events");
      Panel.send(CYD_InfoPanel, "you \"feed\" the graphs");
      Panel.send(CYD_InfoPanel, "In this case values are mapped to a spec. sect.");
      Panel.send(CYD_InfoPanel, "In Rolling graph values scroll from the right.");
      Panel.send(CYD_InfoPanel, "The number of values can be set.");
      Panel.send(CYD_InfoPanel, "Default (used here) is 50.");
      break;
    
    default: break;
  }
}
