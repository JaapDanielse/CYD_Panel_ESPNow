
// L1_Static Module

int16_t Ssinoffset = 128;
int16_t Ssinamplitude = 50;
int16_t Ssinfrequency = 1;
const int16_t SampleCount = 80;

//--------------------------------------------------------------------------------------------
void L1_StaticCallback(CYD_channel event)
{  
  switch (event) 
  {
    case CYD_PanelConnected: L1_StaticInit(); break; 
    case CYD_Slider_1: L1_Static_Offset(); break;
    case CYD_Slider_2: L1_Static_Amplitude(); break;
    case CYD_Slider_3: L1_Static_Frequency(); break;

    case CYD_Button_7:
      L1_Static_ToggleAppTimer();
      break;

    case CYD_Button_12:
      AppTimerOn = false;
      if( ++PanelMode == Endmode ) PanelMode = 0;
      Panel.send(CYD_Reset, CYD_LAYOUT_2);
      break;

    default: break;
  }
}

//--------------------------------------------------------------------------------------------
void L1_Static_ToggleAppTimer()
{
  static bool Graph = true;

  Graph = !Graph;

  if(Graph)
  {
    SetAppTimer(250, &L1_StaticGraph, true);
  }
  else
  {
    SetAppTimer(250, &L1_StaticGraph, false);
    Panel.send(CYD_Graph_1, CYD_CLEAR);  
  }
}

//--------------------------------------------------------------------------------------------
void L1_Static_Offset()
{
  Ssinoffset = Panel.cydr_int;
  Panel.sendf(CYD_Display_8, "sin o:%03d a:%03d f:%03d ", Ssinoffset, Ssinamplitude, Ssinfrequency);
}

//--------------------------------------------------------------------------------------------
void L1_Static_Amplitude()
{
  Ssinamplitude = Panel.cydr_int;
  Panel.sendf(CYD_Display_8, "sin o:%03d a:%03d f:%03d ", Ssinoffset, Ssinamplitude, Ssinfrequency);
}

//--------------------------------------------------------------------------------------------
void L1_Static_Frequency()
{
  Ssinfrequency = Panel.cydr_int + 1;
  Panel.sendf(CYD_Display_8, "sin o:%03d a:%03d f:%03d ", Ssinoffset, Ssinamplitude, Ssinfrequency);
}

//--------------------------------------------------------------------------------------------
void L1_StaticGraph()
{
  float angle = 0.0;
  byte graphvalue = 0;

  for(int j = 0; j < SampleCount; j++)
  {
    angle += 360.0 / ((float)SampleCount / Ssinfrequency);
    float RadAngle = (float)(PI / 180.0) * angle;
    graphvalue = (byte) constrain((Ssinamplitude * sin(RadAngle) + Ssinoffset),0,255);
    Panel.send(CYD_Graph_1, graphvalue);  
  }
}

//--------------------------------------------------------------------------------------------
void L1_StaticInit()
{
  Panel.send(CYD_ApplicationName, "CYD_Panel L1 Static");
  
  Panel.send(CYD_Display_8, CYD_FONT, CYD_SMALL);
  
  Panel.send(CYD_Slider_1, "offset");
  Panel.send(CYD_Slider_1, CYD_MAXVALUE, 255);
  Panel.send(CYD_Slider_1, Ssinoffset);
  Panel.send(CYD_Slider_2, "amplitude");
  Panel.send(CYD_Slider_2, Ssinamplitude);
  Panel.send(CYD_Slider_2, CYD_MAXVALUE, 255);
  Panel.send(CYD_Slider_3,  "frequency");
  Panel.send(CYD_Slider_3, Ssinfrequency);
  Panel.send(CYD_Slider_3, CYD_MAXVALUE, 11);
  
  Panel.send(CYD_Button_7, "Graph\nOff");
  Panel.send(CYD_Button_12, CYD_BACKCOLOR, CYD_PINK);
  Panel.send(CYD_Button_12, "Next\nPanel");
  Panel.send(CYD_Button_12, CYD_BACKCOLOR, CYD_LBLUE);

  Panel.send(CYD_Graph_1, CYD_VALUECOUNT, SampleCount);
  Panel.send(CYD_Graph_1, CYD_STATIC);
  Panel.send(CYD_Graph_1, CYD_COLOR, CYD_GREEN);
  Panel.sendf(CYD_Display_8, "sin o:%03d a:%03d f:%03d ", Ssinoffset, Ssinamplitude, Ssinfrequency);
  
  AllPanelInit();
  SetAppTimer(250, &L1_StaticGraph, true);
}


