
// PanelControl module

//--------------------------------------------------------------------------------------------
void PanelCallback(CYD_channel event) 
{ 
  // PanelCallback is called when a message is received from the panel 
  // only channel and type are passed as parameters. Panel data available as public library vars

   switch (PanelMode)
   {
     case L1_Buttons:
       L1_ButtonsCallback(event); 
       break;

     case L1_Rolling: 
       L1_RollingCallback(event); 
       break;

     case L1_Static: 
       L1_StaticCallback(event); 
       break;

     case L2_Buttons: 
       L2_ButtonsCallback(event); 
       break;

     case L3_Buttons: 
       L3_ButtonsCallback(event); 
       break;

     case L4_Buttons: 
       L4_ButtonsCallback(event); 
       break;

     default: break;
   }
}

void AllPanelInit()
{
  Panel.send(CYD_MonitorField_1, CYD_BACKCOLOR, CYD_RED);
  Panel.send(CYD_MonitorField_1, CYD_COLOR, CYD_YELLOW);
  Panel.send(CYD_MonitorField_1, "MonitorField_1 ");
  Panel.send(CYD_MonitorField_2, CYD_FONT, CYD_SMALL);
  Panel.send(CYD_MonitorField_2, "MonitorField_2\nTwo lines");
  Panel.send(CYD_MonitorField_3, "MonitorField_3 ");
  Panel.send(CYD_MonitorField_4, "MonitorField_4 ");
  Panel.send(CYD_MonitorField_5, "MonitorField_5 ");
  Panel.send(CYD_MonitorField_6, "MonitorField_6 ");

  Panel.send(CYD_InfoPanel, CYD_TITLE, "CYD_Panel");

  Panel.send(CYD_InfoPanel, "CYD_Panel V0.12.0 - 27-10-2025");
  Panel.send(CYD_InfoPanel, "(c) 2025 - J.C. Danielse");
  Panel.send(CYD_InfoPanel, "(DUAL CORE)");
  Panel.send(CYD_InfoPanel, "Arduino IDE experiment control panel");
  Panel.send(CYD_InfoPanel, "Runs on Cheap Yellow Display (CYD)");
  Panel.send(CYD_InfoPanel, "(ESP32 wroom, TFT 320 x 240, Touch screen)");
  Panel.send(CYD_InfoPanel, "Design and implementation: Jaap Danielse.");
  Panel.send(CYD_InfoPanel, "For documentation see:");
  Panel.send(CYD_InfoPanel, "https://github.com/JaapDanielse/CYD_Panel");

}