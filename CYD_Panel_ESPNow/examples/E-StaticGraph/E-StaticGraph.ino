// CYD_Panel_ESPNow E-StaticGraph example - Documentation https://github.com/JaapDanielse/CYD_Panel/wiki/Basic-Examples
// GNU Licence - Copyright (c) 2026 Jaap Danielse - https://github.com/JaapDanielse/CYD_Panel

#include "CYD_Panel_ESPNow.h"

uint32_t Timer = 0; // display timer

int   vpos = 128; // possition the graph in the middle 
float vdiv = 60.0; // max wave height
float sincount =  7.0; // angle step per value point
int   SampleCount = 80; // number of samples in the graph (Max 80)
bool  connected = false; // only start writing after panel init.


void setup() 
{
  Panel.begin(); // init port and protocol
}

void loop() 
{
  Panel.receive(); // handle panel events form the panel (must be in the loop)

  if( Timer < millis() && connected) // pace the application and display effort
  {
    Timer = millis() + 100; // set timer 
    Sinus();
  }
}

void PanelCallback(CYD_channel event) 
{ 
  switch (event) 
  {
    case CYD_PanelConnected: // receive panel connected event
     // initialize panel layout
      Panel.send(CYD_ApplicationName,"StaticGraph"); // set the application name
      Panel.send(CYD_Graph_1, CYD_VALUECOUNT, SampleCount);
      Panel.send(CYD_Graph_1, CYD_STATIC);
      Panel.send(CYD_Graph_1, CYD_COLOR, CYD_GREEN);
      break; 

    default: break;
  }
  connected = true;
}

void Sinus()
{
   float angle = 0.0;
   float vsincount;
   byte graphvalue = 0;

   vsincount = sincount + ((float)random(0,5)/10.0); // simulate small frequency variations
    
   for(int j=0; j<SampleCount; j++) // wite all points to the graph
   {
     angle += 360.0/((float) SampleCount / vsincount); // calulate the current angle in degrees
     float RadAngle = (float)(PI/180.0) * angle; // convert to radians
     graphvalue = (byte) constrain((vdiv * sin(RadAngle) + vpos), 0, 255); // map to graph values (0-255)
     Panel.send(CYD_Graph_1, graphvalue); // send the value
   }
}
