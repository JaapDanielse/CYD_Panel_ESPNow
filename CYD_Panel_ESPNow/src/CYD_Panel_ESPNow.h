/*
  CYD_Panel_ESPNow.h
  MIT Licence - Copyright (c) 2025 Jaap Danielse - https://github.com/JaapDanielse/CYD_Panel

  CYD_Panel_ESPNow is an addition to CYD_Panel. It allows to use CYD_Panel as a remote for most Arduino IDE - ESP boards.

  V 1.0.2 29-04-2026 uniformed externally used enum names to CYD_... , Since %F is available, removed _FString support functions
  V 1.0.1 26-04-2026 Added support for Send int32_t and int.
  V 1.0.0 30-03-2026 Original version
		
*/

#pragma once

#include "Arduino.h"

#if defined (ESP32) 
	#include <esp_now.h>
	#include <esp_wifi.h>
	#include <WiFi.h>
#endif

#if defined (ESP8266) 
	#include <ESP8266WiFi.h>
	#include <espnow.h>
#endif

#define SENDFBUFFERSIZE 60 // Buffersize for formatted text sendf
#define RECEIVEBUFFERSIZE 60 // Buffersize for pannel events: 60 + 5 (msg overhead)

enum CYD_layout : uint8_t
{
	CYD_LAYOUT_1,
	CYD_LAYOUT_2,
	CYD_LAYOUT_3,
	CYD_LAYOUT_4
};

enum CYD_channel : uint8_t
{
	CYD_ApplicationName, // 0
	CYD_DisplayPanel, // 1
	CYD_Display_1,  //  2
	CYD_Display_2,  //  3
	CYD_Display_3,  //  4
	CYD_Display_4,  //  5
	CYD_Display_5,  //  6
	CYD_Display_6,  //  7
	CYD_Display_7,  //  8
	CYD_Display_8,  //  9
	CYD_Display_9,  // 10
	CYD_Display_10, // 11
	CYD_Graph_1,   // 12
	CYD_Graph_2,   // 13
	CYD_Graph_3,   // 14
	CYD_Button_1,  // 15
	CYD_Button_2,  // 16
	CYD_Button_3,  // 17
	CYD_Button_4,  // 18
	CYD_Button_5,  // 19
	CYD_Button_6,  // 20
	CYD_Button_7,  // 21
	CYD_Button_8,  // 22
	CYD_Button_9,  // 23
	CYD_Button_10, // 24
	CYD_Button_11, // 25
	CYD_Button_12, // 26
	CYD_Button_13, // 27
	CYD_Button_14, // 28
	CYD_Button_15, // 29
	CYD_Button_16, // 30
	CYD_Button_17, // 31
	CYD_Button_18, // 32
	CYD_Slider_1, // 33
	CYD_Slider_2, // 34
	CYD_Slider_3, // 35
	CYD_Slider_4, // 36
	CYD_Slider_5, // 37
	CYD_Slider_6, // 38
	CYD_Slider_7, // 39
	CYD_Slider_8, // 40
	CYD_Slider_9, // 41
	CYD_Led_1,  // 42
	CYD_Led_2,  // 43
	CYD_Led_3,  // 44
	CYD_Led_4,  // 45
	CYD_Led_5,  // 46
	CYD_Led_6,  // 47
	CYD_Led_7,  // 48
	CYD_Led_8,  // 49
	CYD_Led_9,  // 50
	CYD_Led_10, // 51
	CYD_Led_11, // 52
	CYD_Led_12, // 53
	CYD_Led_13, // 54
	CYD_Led_14, // 55
	CYD_Led_15, // 56
	CYD_Led_16, // 57
	CYD_Led_17, // 58
	CYD_Led_18, // 59
	CYD_Led_19, // 60
	CYD_Led_20, // 61
	CYD_Led_21, // 62
	CYD_Led_22, // 63
	CYD_MonitorField_1, // 64
	CYD_MonitorField_2, // 65
	CYD_MonitorField_3, // 66
	CYD_MonitorField_4, // 67
	CYD_MonitorField_5, // 68
	CYD_MonitorField_6, // 69
	CYD_InfoPanel, // 70
	CYD_MButton_1, // 71
	CYD_MButton_2, // 72
	CYD_MButton_3, // 73
	CYD_EndUI, // 74 - above here only UI components
	CYD_PanelConnected, // 75
	CYD_Reset, // 76
	CYD_NoOverlay, // 77
	CYD_EndChannel // 78
};

// message content type declaration
enum CYD_type : uint8_t
{
	CYD_void,
	CYD_string,
	CYD_boolean,
	CYD_byte,
	CYD_int,
	CYD_uint,
	CYD_long,
	CYD_ulong,
	CYD_float,
	CYD_error 
};

// send parameter declaration
enum CYD_param : uint8_t
{
	CYD_NOPARAM,
	CYD_MAXVALUE,
	CYD_VALUECOUNT,
	CYD_ROLLING,
	CYD_STATIC,
	CYD_CLEAR,
	CYD_COLOR,
	CYD_BACKCOLOR,
	CYD_FONT,
	CYD_TITLE
};

// screen identifier
enum CYD_screen : uint8_t
{
	CYD_ALL,
	CYD_HOME,
	CYD_MONITOR,
	CYD_INFO
};

enum CYD_font : uint8_t
{
	CYD_NOFONT0,
	CYD_SMALL,   // font 1
	CYD_NORMAL,  // font 2
	CYD_NOFONT3,
	CYD_BIG      // font 4
};

// color name definition
enum CYD_color : uint8_t
{
	CYD_BLACK,
	CYD_GRAY,
	CYD_PURPLE,
	CYD_PINK,
	CYD_LBLUE,
	CYD_BLUE,
	CYD_DBLUE,
	CYD_GREEN,
	CYD_YELLOW,
	CYD_ORANGE,
	CYD_RED,
	CYD_BROWN,
	CYD_WHITE
};


//-----------------------------------------------------------------------------
class CYD_Panel_ESPNow
{
	public:
		using PanelCallbackFunction = void(*)(CYD_channel);

		CYD_Panel_ESPNow(PanelCallbackFunction CallBackPointer);
		void setScreen(CYD_screen screen);
		void begin(CYD_layout layout = CYD_LAYOUT_1);
		void reset(CYD_layout layout = CYD_LAYOUT_1);
		void receive();
		void GetScreenTouch();
		void HandleSend();
		void sendf(CYD_channel channel, const char* message, ...);
		void send(CYD_channel channel, const char* message);
		void send(CYD_channel channel, uint8_t value);
		void send(CYD_channel channel, int16_t value);
		void send(CYD_channel channel, int value);
#ifndef ESP8266
		void send(CYD_channel channel, int32_t value);
#endif
		void send(CYD_channel channel, CYD_param param);
		void send(CYD_channel channel, CYD_param param, CYD_color colorId);
		void send(CYD_channel channel, CYD_param param, CYD_font fontId);
		void send(CYD_channel channel, CYD_param param, int16_t value);
		void send(CYD_channel channel, CYD_param param, int value);
#ifndef ESP8266
		void send(CYD_channel channel, CYD_param param, int32_t value);
#endif
		void send(CYD_channel channel, CYD_param param, const char* message);
		void send(CYD_channel channel, bool visible);
		void send(CYD_channel channel, CYD_layout layoutId = CYD_LAYOUT_1);

  public:
// receive vars	
		volatile bool cydr_received = false;
		CYD_channel cydr_channel = CYD_EndChannel;
	  CYD_param		cydr_param = CYD_NOPARAM;
		CYD_type    cydr_type = CYD_type::CYD_error;
		char*       cydr_string = nullptr;
		bool        cydr_boolean = false;
		uint8_t     cydr_byte = 0;
		int16_t     cydr_int = 0;
		uint16_t    cydr_uint = 0;
		int32_t     cydr_long = 0;
		uint32_t    cydr_ulong = 0;
		float       cydr_float = 0.0f;	
	
// send vars	
	  volatile bool	cyds_sent = false; 
		CYD_channel cyds_channel = CYD_EndChannel;
	  CYD_param		cyds_param = CYD_NOPARAM;
		CYD_type    cyds_type = CYD_error;
		const char* cyds_string = nullptr;
	  uint8_t  		cyds_byte = 0;
	  int16_t  		cyds_int = 0;
	  uint32_t    cyds_long = 0;
		bool        cyds_boolean = false;
				
	private:
		PanelCallbackFunction _CallBackPointer = nullptr;

};

void PanelCallback(CYD_channel event); // Callback Function declaration. Calback routine itself must be created in the sketch

extern CYD_Panel_ESPNow Panel; // Panel object declaration

#ifdef ESP8266
void OnDataSent(uint8_t *mac_addr, uint8_t status);

void OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t data_len);

#elif defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
void OnDataSent(const esp_now_send_info_t *tx_info, esp_now_send_status_t status);

void OnDataRecv(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len);

#else
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);

#endif

void PairRequest(const uint8_t *src_addr, const uint8_t * data, int data_len);

void HandleData(const uint8_t *src_addr, const uint8_t *data, int data_len);

void HandleAck(const uint8_t *src_addr, const uint8_t *data, int data_len);

bool SendEspNowData(const uint8_t * peer_addr, const uint8_t * data, size_t len);

void FindPeer(CYD_layout layout);

extern int WiFiChannel; // channel numbers are 1 trough 14 (default 1)







