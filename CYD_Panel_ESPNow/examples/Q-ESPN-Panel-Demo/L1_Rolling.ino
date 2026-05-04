
// L1_Roling Module

bool sinselect = true;
int16_t sinoffset = 128;
int16_t sinamplitude = 50;
int16_t sinfrequency = 10;
int16_t cosoffset = 128;
int16_t cosamplitude = 50;
int16_t cosfrequency = 10;

//--------------------------------------------------------------------------------------------
void L1_RollingCallback(CYD_channel event)
{  
  switch (event) 
  {
    case CYD_PanelConnected: L1_RollingInit(); break; 
    case CYD_Slider_1: L1_Roling_Offset(); break;
    case CYD_Slider_2: L1_Roling_Amplitude(); break;
    case CYD_Slider_3: L1_Roling_Frequency(); break;

    case CYD_Button_7: HideGraph(); break;
    case CYD_Button_8: SetSine(); break;
    case CYD_Button_9: SetCosin(); break;

    case CYD_Button_12:
      AppTimerOn = false;
      if( ++PanelMode == Endmode ) PanelMode = 0;
      Panel.send(CYD_Reset);
      break;

    default: break;
  }
}

//--------------------------------------------------------------------------------------------
void HideGraph()
{
  static bool Visible = true;

  Visible = !Visible;
  Panel.send(CYD_Graph_1, Visible);
  Panel.send(CYD_Graph_2, Visible);
  SetAppTimer(50, &L1_RollingGraph, Visible);
}

//--------------------------------------------------------------------------------------------
void SetSine()
{
  sinselect = true;
  Panel.send(CYD_Slider_1, CYD_BACKCOLOR, CYD_BLUE);
  Panel.send(CYD_Slider_2, CYD_BACKCOLOR, CYD_BLUE);
  Panel.send(CYD_Slider_3, CYD_BACKCOLOR, CYD_BLUE);
  Panel.send(CYD_Slider_1, sinoffset);
  Panel.send(CYD_Slider_2, sinamplitude);
  Panel.send(CYD_Slider_3, sinfrequency);
}

//--------------------------------------------------------------------------------------------
void SetCosin()
{
  sinselect = false;
  Panel.send(CYD_Slider_1, CYD_BACKCOLOR, CYD_YELLOW);
  Panel.send(CYD_Slider_2, CYD_BACKCOLOR, CYD_YELLOW);
  Panel.send(CYD_Slider_3, CYD_BACKCOLOR, CYD_YELLOW);
  Panel.send(CYD_Slider_1, cosoffset);
  Panel.send(CYD_Slider_2, cosamplitude);
  Panel.send(CYD_Slider_3, cosfrequency);
}

//--------------------------------------------------------------------------------------------
void L1_Roling_Offset()
{
  if(sinselect)
    sinoffset = Panel.cydr_int;
  else
    cosoffset = Panel.cydr_int;
}

//--------------------------------------------------------------------------------------------
void L1_Roling_Amplitude()
{
  if(sinselect)
    sinamplitude = Panel.cydr_int;
  else
    cosamplitude = Panel.cydr_int;
}

//--------------------------------------------------------------------------------------------
void L1_Roling_Frequency()
{
  if(sinselect)
    sinfrequency = Panel.cydr_int;
  else
    cosfrequency = Panel.cydr_int;
}

//--------------------------------------------------------------------------------------------
void L1_RollingGraph()
{
  static uint16_t sinangle = 0;
  static uint16_t cosangle = 0;
  static int16_t  sinevalue = 0;
  static int16_t  cosinevalue = 0;
  float sinus = 0;
  float cosin = 0;

    sinangle += sinfrequency; 
    if(sinangle >= 360) sinangle = 0;
    sinus = (sin((M_PI / 180.0) * (float)sinangle));
    sinevalue = ((sinus * (float)sinamplitude) + (float)sinoffset);
    if(sinevalue < 0) sinevalue = 0;
    if(sinevalue > 255) sinevalue = 255;
    Panel.send(CYD_Graph_1, (uint8_t) sinevalue);


    cosangle += cosfrequency; 
    if(cosangle >= 360) cosangle = 0;
    cosin = (cos((M_PI / 180.0) * (float)cosangle));
    cosinevalue = ((cosin * (float)cosamplitude) + (float)cosoffset);
    if(cosinevalue < 0) cosinevalue = 0;
    if(cosinevalue > 255) cosinevalue = 255;
    Panel.send(CYD_Graph_2, (uint8_t) cosinevalue);

    Panel.sendf(CYD_Display_1, "sine: %.3f\ncosine: %.3f", sinus, cosin);
    Panel.sendf(CYD_Display_4, "cos o:%03d a:%03d f:%03d ", cosoffset, cosamplitude, cosfrequency);
    Panel.sendf(CYD_Display_8, "sin o:%03d a:%03d f:%03d ", sinoffset, sinamplitude, sinfrequency);

    if(sinselect)
    {
      Panel.sendf(CYD_Display_5, "angle: %03d", sinangle);
    }
    else
    {
      Panel.sendf(CYD_Display_5, "angle: %03d", cosangle);
    }
    delay(10);
}

//--------------------------------------------------------------------------------------------
void L1_RollingInit()
{
  Panel.send(CYD_ApplicationName, "CYD_Panel L1 Rolling");
  
  Panel.send(CYD_Display_1, CYD_FONT, CYD_SMALL);
  Panel.send(CYD_Display_4, CYD_FONT, CYD_SMALL);
  Panel.send(CYD_Display_8, CYD_FONT, CYD_SMALL);
  Panel.send(CYD_Display_5, CYD_FONT, CYD_BIG);
  
  Panel.send(CYD_Slider_1, "offset");
  Panel.send(CYD_Slider_1, CYD_MAXVALUE, 255);
  Panel.send(CYD_Slider_1, sinoffset);
  Panel.send(CYD_Slider_2, "amplitude");
  Panel.send(CYD_Slider_2, CYD_MAXVALUE, 255);
  Panel.send(CYD_Slider_2, sinamplitude);
  Panel.send(CYD_Slider_3,  "frequency");
  Panel.send(CYD_Slider_3, CYD_MAXVALUE, 20);
  Panel.send(CYD_Slider_3, sinfrequency);
  
  Panel.send(CYD_Button_7, "graph\nvis.");
  Panel.send(CYD_Button_8, "SINE");
  Panel.send(CYD_Button_8, CYD_BACKCOLOR, CYD_BLUE);
  Panel.send(CYD_Button_8, CYD_COLOR, CYD_WHITE);
  Panel.send(CYD_Button_9, "CO-\nSINE");
  Panel.send(CYD_Button_9, CYD_BACKCOLOR, CYD_YELLOW);

  // Panel.send(Led_15, _Color, _BLUE);
  // Panel.send(Led_18, _Color, _YELLOW);

  Panel.send(CYD_Button_12, "Next\nPanel");
  Panel.send(CYD_Button_12, CYD_BACKCOLOR, CYD_LBLUE);

  Panel.send(CYD_Graph_1, CYD_VALUECOUNT, 80);
  Panel.send(CYD_Graph_1, CYD_COLOR, CYD_LBLUE);
  Panel.send(CYD_Graph_2, CYD_VALUECOUNT, 80);
  Panel.send(CYD_Graph_2, CYD_COLOR, CYD_YELLOW);
  
  AllPanelInit();

  SetAppTimer(50, &L1_RollingGraph, true);

  SetSine();
}
