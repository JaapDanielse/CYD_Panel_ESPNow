CYD_Panel_ESPNow Release 1.0.0 (May 2026)

CYD_Panel_ESPNow is an Arduino libray remotely access a Cheap Yellow Display running CYD_Panel and the ESPNow sketch.
CYD_Panel_ESPNow can only be used on ESP boards (ESP23 ESP8266)

Cheap Yellow Display (CYD)
  The Cheap Yellow Display is a specific module based on the ESP32 wroom, 320 x 240 TFT display and a Touch screen.
	While running the CYD_Panel library and the ESPNow sketch provided in the examples the CYD_Panel_ESPNow library can be used 
	on a different ESP board to connect to the CYD_Panel. 

Required underlying libraries
  The remote functionality used by CYD_Panel_ESPNow is implemented using the esp_now and Wifi libraries for ESP .

Installing the CYD_Panel_ESPNow library
  CYD_Panel_ESPNow can be installed as a standard Arduino .zip library.
  . In the Arduino IDE select: Sketch->Include Library->Add .ZIP Library...
  . Select CYD_Panel.zip from your download location.

Examples
  See examples in: myDocuments\Arduino\libraries\CYD_Panel_ESPNow\examples

Extras
  See Quick reference in: myDocuments\Arduino\libraries\CYD_Panel_ESPNow\extras


CYD_Panel_ESPNow V1.0.0 was built and tested using Arduino 2.3.6 IDE
