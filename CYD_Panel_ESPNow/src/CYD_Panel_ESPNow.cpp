// CYD_Panel_ESPNow.cpp - part of CYD_Panel_ESPNow

#include "CYD_Panel_ESPNow.h"

//-- ESP_Now ----------------------------------------------------------------

int WiFiChannel = 1; // channel numbers are 1 trough 14 (default 1)
//const int WiFiChannel = 13; // channel numbers are 1 trough 14 (default 1)

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // generic broadcast MAC address

enum PairStatusList {SINGLE, PAIRED};
volatile PairStatusList PairStatus = SINGLE;

enum MessageStatusList {WAITING, SENT, DELIVERED, ACKNOWLEGED};
volatile MessageStatusList MessageStatus = WAITING;

enum MessageType {NO_MESSAGE, PAIR, DATA, ACK};

uint8_t MessageId = 0;

const char PanelDeviceType[12] = "[CYD-V1]";

// Pairing message structure
typedef struct __attribute__((packed)) struct_pairing   // new structure for pairing
{
  uint8_t MsgType = PAIR; // PAIRING
  uint8_t MsgId; // message number
  char    DeviceType[12];
	uint8_t PanelLayout = (uint8_t) CYD_LAYOUT_1;
} struct_pairing;

struct_pairing PairingMessage;

// Data message structure
// Data message structure
typedef struct __attribute__((packed)) struct_data 
{
  uint8_t  MsgType = DATA; // DATA
  uint8_t  MsgId; // message number
  uint8_t  PanelChannel; // Panel channel (not ESPNow)
  uint8_t  Param; // Panel param
  uint8_t  ValueType; // Panel value type
  bool     BoolValue; // Actual bool value 
  uint8_t  ByteValue; // Actual byte value 
  int16_t  IntValue;  // Actual int value
  int32_t  LongValue;  // Actual long value
  char     StringValue[60]; // Actual string value
} struct_data;

struct_data SendDataMessage;
struct_data ReceiveDataMessage;

// Pairing message structure
typedef struct __attribute__((packed)) struct_ack   // new structure for pairing
{
  uint8_t MsgType = ACK; // PAIRING
  uint8_t MsgId; // message number
} struct_ack;

struct_ack AckMessage;

uint8_t PeerMac[6];

#ifndef ESP8266
esp_now_peer_info_t BroadcastInfo;
esp_now_peer_info_t peerInfo;
#endif

CYD_Panel_ESPNow Panel(PanelCallback); // instantiation of the Panel object


//------------------------------------------------------------------------- 
// ESP32 Callback when data is sent

#ifdef ESP8266
// Esp8266
void OnDataSent(uint8_t *mac_addr, uint8_t status) 
{
	(void)mac_addr;
	
	bool success = (status == 0);
#elif defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
// Voor de nieuwe ESP32 Core (v3.0.0+)
void OnDataSent(const esp_now_send_info_t *tx_info, esp_now_send_status_t status)
{
	bool success = (status == ESP_NOW_SEND_SUCCESS);
#else
// Voor de oudere ESP32 Core (v2.x.x) en de Nano ESP32
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
	bool success = (status == ESP_NOW_SEND_SUCCESS);
#endif	
  if(!success)
  {
    Serial.println("ERR - ESPNOW Message Delivery failed");
    MessageStatus = WAITING;
    return;
  }

  // Serial.println("INF - ESPNOW Message Delivered");
  MessageStatus = DELIVERED;
  return;
}


//-------------------------------------------------------------------------------------
// ESP32 Callback when data is received

#ifdef ESP8266
void OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t data_len) 
{
#elif defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
// Voor de nieuwe ESP32 Core (v3.0.0+)
void OnDataRecv(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len)
{
  const uint8_t* mac_addr = esp_now_info->src_addr; 
#else
// Voor de oudere ESP32 Core (v2.x.x) en de Nano ESP32
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *data, int data_len) 
{
#endif	
	
  MessageType messageType = (MessageType) data[0];

  switch(messageType)
  {
     case NO_MESSAGE: 
      Serial.println("Invalid message type received");
      return; 
      break;

     case PAIR: 
      PairRequest(mac_addr, data, data_len); // handle pair request
      break;

    case DATA:
      HandleData(mac_addr, data, data_len); // handle data message
      break;

    case ACK:
      HandleAck(mac_addr, data, data_len); // handle ack message
      break;   
  }
}


//-----------------------------------------------------------------------------------------
void PairRequest(const uint8_t *src_addr, const uint8_t * data, int data_len)
{
  Serial.printf("Pair request (ack) from: SourceMac %02x:%02x:%02x:%02x:%02x:%02x\n",
                  src_addr[0], src_addr[1], src_addr[2], src_addr[3], src_addr[4], src_addr[5]);

  memcpy(&PairingMessage, data, data_len); // copy message data to pair struct

  // are we already paired?
  if(PairStatus == PAIRED) return;

  // we are not paired yet!
  if(strcmp(PairingMessage.DeviceType, PanelDeviceType)) // check if this request is for our device type
  {
    // not the right devicetype!
    Serial.println("ERR - CYDP Invalid Panel Device Type");
    return;
  }
  // request is for our device type

  memcpy(PeerMac, src_addr, 6); // store source mac 

#ifdef ESP8266
  // esp8266
	esp_now_add_peer(PeerMac, ESP_NOW_ROLE_COMBO, WiFiChannel, NULL, 0);
#else
	// esp32
  memcpy(peerInfo.peer_addr, PeerMac, 6); // copy PeerMac
  peerInfo.channel = WiFiChannel; // set Peer channel
  peerInfo.encrypt = false; // 

  // add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("ERR - ESPNOW Failed to add peer");
    return;
  }
#endif

  MessageStatus = ACKNOWLEGED; // this pair request was an implict acknowledgment

  // setup ack message
  AckMessage.MsgId = PairingMessage.MsgId;

#ifdef ESP8266
  if(esp_now_send(PeerMac,  (uint8_t *) &AckMessage, sizeof(AckMessage)) != 0)
#else
  if(esp_now_send(PeerMac,  (uint8_t *) &AckMessage, sizeof(AckMessage)) != ESP_OK)
#endif
  {
 	  Serial.println("ERR - ESPNOW Error sending data"); // log error 
    return;
  }
  
  PairStatus = PAIRED;

  Serial.println("INF - ESPNOW device PAIRED");
}


//-----------------------------------------------------------------------------------------
void HandleData(const uint8_t *src_addr, const uint8_t *data, int data_len)
{
	(void)src_addr;
	
  if(PairStatus != PAIRED) return; // ignore data while not paired

  memcpy(&ReceiveDataMessage, data, data_len); // copy message data to pair struct
  
  // Serial.println("INF - ESPNOW Data received");

  // copy data to panel receive vars.
	Panel.cydr_channel = (CYD_channel) ReceiveDataMessage.PanelChannel;
	Panel.cydr_type = (CYD_type) ReceiveDataMessage.ValueType;
	Panel.cydr_boolean = ReceiveDataMessage.BoolValue;
	Panel.cydr_byte = ReceiveDataMessage.ByteValue;
	Panel.cydr_int = ReceiveDataMessage.IntValue;
	Panel.cydr_string = ReceiveDataMessage.StringValue;
	
	Panel.cydr_received = true; // signal receive message available
	
  // Send Acknowledge
  AckMessage.MsgType = ACK;  
  AckMessage.MsgId = ReceiveDataMessage.MsgId;

#ifdef ESP8266
  if(esp_now_send(PeerMac,  (uint8_t *) &AckMessage, sizeof(AckMessage)) != 0)
#else
  if(esp_now_send(PeerMac,  (uint8_t *) &AckMessage, sizeof(AckMessage)) != ESP_OK)
#endif
  {
 	  Serial.println("ERR - ESPNOW Error sending ack"); // log error 
    return;
  }
}

//-----------------------------------------------------------------------------------------
void HandleAck(const uint8_t *src_addr, const uint8_t *data, int data_len)
{
	(void)src_addr;

  //Serial.println("INF - ESPNOW Ack received");
  memcpy(&AckMessage, data, data_len); // copy message data to pair struct
  if(AckMessage.MsgId == MessageId) MessageStatus = ACKNOWLEGED;
}


//-------------------------------------------------------------------------------------
// Send data
bool SendEspNowData(const uint8_t * peer_addr, const uint8_t * data, size_t len)
{
  uint8_t RetryCount = 3;
	uint32_t MessageTimer = 0;
  MessageStatus = WAITING;

	while(MessageStatus == WAITING)
	{
    if(!RetryCount--)
    {
      Serial.println("ERR - ESPNOW Max Send Retry");
      return false;
    }

    MessageStatus = SENT; // set MessageStatus = SENT

#ifdef ESP8266
		if(esp_now_send((uint8_t*)peer_addr, (uint8_t*)data, len) != 0)
#else
    if(esp_now_send(peer_addr, data, len) != ESP_OK)
#endif
    {
 			Serial.println("ERR - ESPNOW Error sending data"); // log error 
      MessageStatus = WAITING;
  	  delay(100); // send went wrong, wait a bit
			continue; // and try again
    }

	  while(MessageStatus == SENT) { delay(1); } // wait for message transmission

		if(MessageStatus == WAITING) // Message was not delivered (set by send callback)
		{
			delay(100); // delivery went wrong, wait a bit
			continue; // and try again
		}

    // MessageStatus == DELIVERED (set by send callback)  

		MessageTimer = millis() + 500; // set maximum wait time

	  while(MessageStatus == DELIVERED) // wait for message acknowledgement
		{
			delay(1);
			if(MessageTimer <= millis()) 
      {
        Serial.println("ERR - ESPNOW Acknowledge timeout"); // log error 
        MessageStatus = WAITING; // time is up. Try again
      }
		}	
  }

  // MessageStatus == ACKNOWLEGED (set by receive callback)

  return true;
}


//=====================================================================================
// CYD_Panel 

//-------------------------------------------------------------------------------------
CYD_Panel_ESPNow::CYD_Panel_ESPNow(PanelCallbackFunction CallBackPointer) : _CallBackPointer(CallBackPointer) // constructor
{ }

//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::begin(CYD_layout layout)
{
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA); 
	
#ifdef ESP8266
  // Voor ESP8266: simpele functie zonder extra parameters
  wifi_set_channel(WiFiChannel); 
	
	// Init ESP-NOW ESP8266
  if (esp_now_init() != 0) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO); // Verplicht op 8266

    esp_now_register_send_cb(OnDataSent);

    esp_now_register_recv_cb(OnDataRecv);
		
		esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, WiFiChannel, NULL, 0);
		
#else
  // Voor ESP32 (zowel v2 als v3): de bekende IDF functie
  esp_wifi_set_channel(WiFiChannel, WIFI_SECOND_CHAN_NONE);
		
   // Init ESP-NOW ESP32
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent); // register send calback routine

  esp_now_register_recv_cb(OnDataRecv); // register receive callback routin

  // register broadcast peer
  memcpy(BroadcastInfo.peer_addr, broadcastAddress, 6);
  BroadcastInfo.channel = WiFiChannel;  
  BroadcastInfo.encrypt = false;
  
  if (esp_now_add_peer(&BroadcastInfo) != ESP_OK)
  {
    Serial.println("ERR - ESPNOW Failed to add peer");
    return;
  }
#endif
 
  FindPeer(layout);
	
}


//------------------------------------------------------------------------- 
void FindPeer(CYD_layout layout)
{
  Serial.println("INF - ESPNOW Find peer");
  // setup pairing message
  PairingMessage.MsgType = PAIR;
  PairingMessage.MsgId = ++MessageId;
  strcpy(PairingMessage.DeviceType, PanelDeviceType);
	PairingMessage.PanelLayout = layout;
  
	while(PairStatus != PAIRED)
  {
    if(!SendEspNowData(broadcastAddress, (uint8_t *) &PairingMessage, sizeof(PairingMessage)))
    delay(1000);
  }
}


//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::receive()
{
  if(cydr_received)
	{
    _CallBackPointer(cydr_channel);
  	cydr_received = false;	
  }
}

//SENDF ===========
// channel with char* ...
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::sendf(CYD_channel channel, const char* message, ...)
{
	static char buf[SENDFBUFFERSIZE];
	va_list args;
	va_start(args, message);
	vsnprintf(buf, SENDFBUFFERSIZE, message, args);
	va_end(args);	
	
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = CYD_NOPARAM;
  SendDataMessage.ValueType = CYD_string;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = 0;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = 0;
  strcpy(SendDataMessage.StringValue, buf);	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}

// SEND ===========
// channel with char*
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, const char* message)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = CYD_NOPARAM;
  SendDataMessage.ValueType = CYD_string;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = 0;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = 0;
  strcpy(SendDataMessage.StringValue, message);	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}

// channel with byte value
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, uint8_t value)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = CYD_NOPARAM;
  SendDataMessage.ValueType = CYD_byte;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = value;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = 0;
  strcpy(SendDataMessage.StringValue, "");	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}

// channel with int16_t value
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, int16_t value)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = CYD_NOPARAM;
  SendDataMessage.ValueType = CYD_int;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = 0;
  SendDataMessage.IntValue = value;
  SendDataMessage.LongValue = 0;
  strcpy(SendDataMessage.StringValue, "");	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}

// channel with int value
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, int value)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = CYD_NOPARAM;
  SendDataMessage.ValueType = CYD_long;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = 0;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = value;
  strcpy(SendDataMessage.StringValue, "");	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}

#ifndef ESP8266
// channel with int32_t value
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, int32_t value)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = CYD_NOPARAM;
  SendDataMessage.ValueType = CYD_long;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = 0;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = value;
  strcpy(SendDataMessage.StringValue, "");	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}
#endif

// chanel with parameter (_MaxValue, _ValueCount) and int16_t value 
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, CYD_param param, int16_t value)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = param;
  SendDataMessage.ValueType = CYD_int;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = 0;
  SendDataMessage.IntValue = value;
  SendDataMessage.LongValue = 0;
  strcpy(SendDataMessage.StringValue, "");	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}

// chanel with parameter (_MaxValue, _ValueCount) and int value 
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, CYD_param param, int value)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = param;
  SendDataMessage.ValueType = CYD_long;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = 0;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = value;
  strcpy(SendDataMessage.StringValue, "");	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}

#ifndef ESP8266
// chanel with parameter (_MaxValue, _ValueCount) and int32_t value 
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, CYD_param param, int32_t value)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = param;
  SendDataMessage.ValueType = CYD_long;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = 0;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = value;
  strcpy(SendDataMessage.StringValue, "");	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}
#endif

// channel with (void) parameter (_Clear)
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, CYD_param param)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = param;
  SendDataMessage.ValueType = CYD_void;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = 0;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = 0;
  strcpy(SendDataMessage.StringValue, "");	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}

// channel parameter (_Color, _BackColor) and color 
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, CYD_param param, CYD_color colorId)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = param;
  SendDataMessage.ValueType = CYD_byte;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = colorId;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = 0;
  strcpy(SendDataMessage.StringValue, "");	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}

// channel parameter (_Font) and font
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, CYD_param param, CYD_font fontId)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = param;
  SendDataMessage.ValueType = CYD_byte;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = fontId;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = 0;
  strcpy(SendDataMessage.StringValue, "");	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}

// channel param (_InfoTitle, InfoPanel) and string
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, CYD_param param, const char* message)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = param;
  SendDataMessage.ValueType = CYD_string;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = 0;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = 0;
  strcpy(SendDataMessage.StringValue, message);	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}

// channel with parameter and boolean (true = visible, false = invisible)
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, bool visible)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = CYD_NOPARAM;
  SendDataMessage.ValueType = CYD_boolean;
	SendDataMessage.BoolValue = visible;
  SendDataMessage.ByteValue = 0;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = 0;
  strcpy(SendDataMessage.StringValue,"");	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}

// Reset channel and layout
//-------------------------------------------------------------------------------------
void CYD_Panel_ESPNow::send(CYD_channel channel, CYD_layout layoutId)
{
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = channel;
  SendDataMessage.Param = CYD_NOPARAM;
  SendDataMessage.ValueType = CYD_byte;
	SendDataMessage.BoolValue = false;
  SendDataMessage.ByteValue = (uint8_t) layoutId;
  SendDataMessage.IntValue = 0;
  SendDataMessage.LongValue = 0;
  strcpy(SendDataMessage.StringValue,"");	

  SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage));
}
