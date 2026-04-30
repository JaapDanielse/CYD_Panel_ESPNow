// L1_Buttons Module

//--------------------------------------------------------------------------------------------
void L1_ButtonsCallback(CYD_channel event)
{ 
  switch (event) 
  {
    case CYD_PanelConnected: // receive panel connected message
      L1_ButtonInit();
      break; 

    case CYD_Button_1:
      Button_1Action();
      break;

    case CYD_Button_2:
      L1_SetSliders();
      break;

    case CYD_Slider_1:
    case CYD_Slider_2:
    case CYD_Slider_3:
    case CYD_Slider_4:
    case CYD_Slider_5:
      Panel.sendf(CYD_Display_5, "Slider_%d : %d", (uint8_t) (event - CYD_Slider_1) + 1, Panel.cydr_int);
      break; 

    case CYD_Button_3:
    case CYD_Button_4:
    case CYD_Button_5:
    case CYD_Button_6:
    case CYD_Button_7:
    case CYD_Button_8:
    case CYD_Button_9:
    case CYD_Button_10:
    case CYD_Button_11:
      Panel.send(CYD_channel (((int)event - (int) CYD_Button_1) + (int) CYD_Led_1), CYD_COLOR, CYD_BLACK);
      break;

    case CYD_Display_1:
    case CYD_Display_2:
    case CYD_Display_3:
    case CYD_Display_4:
    case CYD_Display_5:
    case CYD_Display_6:
    case CYD_Display_7:
    case CYD_Display_8:
    case CYD_Display_9:
    case CYD_Display_10:
      Panel.send(event, "Clicked!");
    break;     

    case CYD_Slider_6:
    case CYD_Button_12:
      if( ++PanelMode == Endmode ) PanelMode = 0;
      Panel.send(CYD_Reset);
      break;

    default: break;
  }
}

//--------------------------------------------------------------------------------------------
void Button_1Action()
{
  Panel.send(CYD_Display_1, "Display_1");
  Panel.send(CYD_Display_1, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Display_1, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Display_1, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Display_2, CYD_BACKCOLOR, CYD_BLUE);
  Panel.send(CYD_Display_3, CYD_FONT, CYD_SMALL);
  Panel.send(CYD_Display_3, "Display_3\n_SMALL font.\nUp to 3 lines!");
  Panel.send(CYD_Display_4, CYD_COLOR, CYD_BLACK);
  Panel.send(CYD_Display_4, CYD_BACKCOLOR, CYD_ORANGE);

  Panel.send(CYD_Display_6, CYD_COLOR, CYD_YELLOW);
  Panel.send(CYD_Display_7, CYD_COLOR, CYD_BLUE);
  Panel.send(CYD_Display_7, CYD_BACKCOLOR, CYD_LBLUE);
  Panel.send(CYD_Display_8, "Display_8");
  Panel.send(CYD_Display_8, CYD_COLOR, CYD_RED);
  Panel.send(CYD_Display_8, CYD_BACKCOLOR, CYD_BLACK);

  Panel.send(CYD_Button_2, CYD_FONT, CYD_SMALL);
  Panel.send(CYD_Button_2, "Button\n2\n(small)");
  Panel.send(CYD_Button_3, CYD_COLOR, CYD_RED);
  Panel.send(CYD_Button_3, "Button\n3");
  Panel.send(CYD_Button_4, CYD_FONT, CYD_BIG);
  Panel.send(CYD_Button_4, "B4");

  Panel.send(CYD_Button_9,  CYD_BACKCOLOR, CYD_YELLOW);
  Panel.send(CYD_Button_10, CYD_BACKCOLOR, CYD_GREEN);
  Panel.send(CYD_Button_11, CYD_BACKCOLOR, CYD_RED);
}

//--------------------------------------------------------------------------------------------
void L1_SetSliders()
{
  Panel.send(CYD_Button_1, false);
  Panel.send(CYD_Button_2, false);
  Panel.send(CYD_Slider_1, "Slider_1");

  Panel.send(CYD_Button_3, false);
  Panel.send(CYD_Button_4, false);
  Panel.send(CYD_Slider_2, "Slider_2");

  Panel.send(CYD_Button_5, false);
  Panel.send(CYD_Button_6, false);
  Panel.send(CYD_Slider_3, "Slider_3");

  Panel.send(CYD_Button_7, false);
  Panel.send(CYD_Button_8, false);
  Panel.send(CYD_Slider_4, "Slider_4");

  Panel.send(CYD_Button_9,  false);
  Panel.send(CYD_Button_10, false);
  Panel.send(CYD_Slider_5, "Slider_5");

  Panel.send(CYD_Button_11, false);
  Panel.send(CYD_Button_12, false);
  Panel.send(CYD_Slider_6, "Slider_6");
  Panel.send(CYD_Slider_6, CYD_BACKCOLOR, CYD_LBLUE);
}

//--------------------------------------------------------------------------------------------
void L1_ButtonInit()
{
  Panel.send(CYD_ApplicationName, "CYD_Panel L1 Buttons");
  Panel.send(CYD_Display_1, "Display_1");
  Panel.send(CYD_Display_2, "Display_2");
  Panel.send(CYD_Display_3, "Display_3");
  Panel.send(CYD_Display_4, "Display_4 met een heel erg lange regel");
  Panel.send(CYD_Display_5, "Display_5");
  Panel.send(CYD_Display_6, "Display_6");
  Panel.send(CYD_Display_7, "Display_7");
  // Panel.send(Display_8, "Display_8");

  Panel.send(CYD_Graph_1, CYD_VALUECOUNT, (int16_t) 60);
  Panel.send(CYD_Graph_1, CYD_COLOR, CYD_LBLUE);
  Panel.send(CYD_Graph_2, CYD_VALUECOUNT, (int16_t) 60);
  Panel.send(CYD_Graph_2, CYD_COLOR, CYD_YELLOW);

  Panel.send(CYD_Button_1,  "button\n1");
  Panel.send(CYD_Button_1,  CYD_COLOR, CYD_BLUE);
  Panel.send(CYD_Button_1,  CYD_BACKCOLOR, CYD_PINK);
  Panel.send(CYD_Button_2,  "button\n2");
  Panel.send(CYD_Button_2,  CYD_BACKCOLOR, CYD_PINK);
  Panel.send(CYD_Button_3,  "button\n3");
  Panel.send(CYD_Button_4,  "button\n4");
  Panel.send(CYD_Button_5,  "button\n5");
  Panel.send(CYD_Button_6,  "button\n6");
  Panel.send(CYD_Button_7,  "button\n7");
  Panel.send(CYD_Button_8,  "button\n8");
  Panel.send(CYD_Button_9,  "button\n9");
  Panel.send(CYD_Button_10, "button\n10");
  Panel.send(CYD_Button_11, "button\n11");
  Panel.send(CYD_Button_12, "button\n12");
  Panel.send(CYD_Button_12, CYD_BACKCOLOR, CYD_LBLUE);

  Panel.send(CYD_Led_1,  CYD_COLOR, CYD_LBLUE);
  Panel.send(CYD_Led_2,  CYD_COLOR, CYD_GREEN);
  Panel.send(CYD_Led_3,  CYD_COLOR, CYD_YELLOW);
  Panel.send(CYD_Led_4,  CYD_COLOR, CYD_RED);
  Panel.send(CYD_Led_5,  CYD_COLOR, CYD_PINK);
  Panel.send(CYD_Led_6,  CYD_COLOR, CYD_PURPLE);

  Panel.send(CYD_Led_7,  CYD_COLOR, CYD_ORANGE);
  Panel.send(CYD_Led_8,  CYD_COLOR, CYD_BLUE);
  Panel.send(CYD_Led_9,  CYD_COLOR, CYD_LBLUE);
  Panel.send(CYD_Led_10, CYD_COLOR, CYD_RED);
  Panel.send(CYD_Led_11, CYD_COLOR, CYD_YELLOW);
  Panel.send(CYD_Led_12, CYD_COLOR, CYD_RED);

  Panel.send(CYD_Led_13, CYD_COLOR, CYD_LBLUE);
  Panel.send(CYD_Led_14, CYD_COLOR, CYD_GREEN);
  Panel.send(CYD_Led_15, CYD_COLOR, CYD_YELLOW);
  Panel.send(CYD_Led_16, CYD_COLOR, CYD_RED);

  Panel.send(CYD_Led_17, CYD_COLOR, CYD_PINK);
  Panel.send(CYD_Led_18, CYD_COLOR, CYD_PURPLE);
  Panel.send(CYD_Led_19, CYD_COLOR, CYD_GREEN);
  Panel.send(CYD_Led_20, CYD_COLOR, CYD_YELLOW);

  AllPanelInit();

}
