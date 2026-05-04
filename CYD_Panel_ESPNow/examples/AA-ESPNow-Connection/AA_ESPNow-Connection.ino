
// CYD_Panel AA_ESPNow-Connection example - Documentation https://github.com/JaapDanielse/CYD_Panel/wiki/Basic-Examples
// GNU Licence - Copyright (c) 2025 Jaap Danielse - https://github.com/JaapDanielse/CYD_lPanel
// CYD_Panel_ESPNow (V1.0 30-03-2026) - Use in combination with CYD_Panel V1.0.0 

#include "CYD_Panel.h"
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

//-- ESP_Now ----------------------------------------------------------------
// Optional set alternate wifi channel. If used set same value in user application using CYD_Panel_ESPNow.h
const int WiFiChannel = 1; //available channel numbers are 1 trough 14 (default 1)

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
 	uint8_t PanelLayout;
} struct_pairing;

struct_pairing PairingMessage;

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
esp_now_peer_info_t peerInfo;

// Panel buffer for InfoText
const uint8_t InfoTextSize = 12;
uint8_t InfoTextIdx = 0;
char InfoText[InfoTextSize][50];


//-------------------------------------------------------------------------------------------- 
void setup() 
{
  Serial.begin(115200); // Init Serial Monitor
  Panel.begin(); // Init Panel
  Panel.sendf(CYD_Display_2, "waiting for connection ch. %d         ", WiFiChannel);
  InitESP_Now(); // Init ESP_Now protocol
}

//-------------------------------------------------------------------------------------------- 
void loop() 
{
  Panel.receive();
}

//--------------------------------------------------------------------------------------------
void InitESP_Now()
{
  WiFi.mode(WIFI_STA); //Set device as a Wi-Fi Station

  esp_wifi_set_channel(WiFiChannel, WIFI_SECOND_CHAN_NONE); // set (fixed) wifi channel

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("ERR - ESPNOW Failed to initialize");
    return;
  }

  esp_now_register_send_cb(OnDataSent); // register send calback routine

  esp_now_register_recv_cb(OnDataRecv); // register receive callback routine
}


//-----------------------------------------------------------------------------------------
void OnDataSent(const esp_now_send_info_t *tx_info, esp_now_send_status_t status)
{
  if(status != ESP_NOW_SEND_SUCCESS)
  {
    Serial.println("ERR - ESPNOW Message Delivery failed");
    MessageStatus = WAITING;
    return;
  }

  // Serial.println("INF - ESPNOW Message Delivered");
  MessageStatus = DELIVERED;
  return;
}


//-----------------------------------------------------------------------------------------
void OnDataRecv(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len)
{
  MessageType messageType = (MessageType) data[0];

  switch(messageType)
  {
     case NO_MESSAGE: 
      Serial.println("Invalid message type received");
      return; 
      break;

     case PAIR: 
      PairRequest(esp_now_info, data, data_len); // handle pair request
      break;

    case DATA:
      HandleData(esp_now_info, data, data_len); // handle data message
      break;

    case ACK:
      HandleAck(esp_now_info, data, data_len); // handle ack message
      break;   
  }
}

//-----------------------------------------------------------------------------------------
void PairRequest(const esp_now_recv_info_t * esp_now_info, const uint8_t * data, int data_len)
{
  Serial.printf("Pair request from: SourceMac %02x:%02x:%02x:%02x:%02x:%02x\n",
                  esp_now_info->src_addr[0], esp_now_info->src_addr[1], esp_now_info->src_addr[2],
                  esp_now_info->src_addr[3], esp_now_info->src_addr[4], esp_now_info->src_addr[5]);

  memcpy(&PairingMessage, data, data_len); // copy message data to pair struct

  // are we already paired?
  if(PairStatus != PAIRED) // are we already paired?
  {
    // we are not paired yet!
    if(strcmp(PairingMessage.DeviceType, PanelDeviceType)) // check if this request is for our device type
    {
      // not the right devicetype!
      Serial.println("ERR - CYDP Invalid Panel Device Type");
      return;
    }
    // request is for our device type
   
    memcpy(PeerMac, esp_now_info->src_addr, 6); // store source mac 
  
    memcpy(peerInfo.peer_addr, PeerMac, 6); // copy PeerMac
    peerInfo.channel = WiFiChannel; // set Peer channel
    peerInfo.encrypt = false; // 

    // add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.println("ERR - ESPNOW Failed to add peer");
      return;
    }
  }
  else
  { 
    // we are paired already
    if(memcmp(esp_now_info->src_addr, PeerMac, 6)) //check if the request is from our peer
    {
      // not from our peer!
      Serial.println("ERR - ESPNOW already paired");
      return;
    }
  }

  // send pairing response message
  Serial.println("INF - ESPNOW Send pairing reply");
  PairingMessage.MsgType = PAIR;
  PairingMessage.MsgId = ++MessageId;
  strcpy(PairingMessage.DeviceType, PanelDeviceType);
  // PairingMessage.PanelLayout = 0;

  if(esp_now_send(PeerMac, (uint8_t *) &PairingMessage, sizeof(PairingMessage)) != ESP_OK)
  {
    Serial.println("ERR - ESPNOW Error sending data"); // log error 
    // MessageStatus = WAITING
    delay(100); // send went wrong, wait a bit
  }

  PairStatus = PAIRED;
  Serial.println("INF - ESPNOW device PAIRED");

  Panel.cyds_channel = (CYD_channel) CYD_Reset; // reset channel
  Panel.cyds_param = (CYD_param) CYD_NOPARAM; // no parameter
  Panel.cyds_type = (CYD_type) CYD_byte; // byte value
  Panel.cyds_byte = (CYD_layout) PairingMessage.PanelLayout; // layout id
  Panel.cyds_int = 0; // no int value
  Panel.cyds_long = 0; // no long value
  Panel.cyds_string = ""; // no string value
  Panel.cyds_sent = true; // flag to panel we have data

  while(Panel.cyds_sent) { delay(1); } // wait for panel to finish updating

}

//-----------------------------------------------------------------------------------------
void HandleData(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len)
{
  if(PairStatus != PAIRED) return; // ignore data while not paired
  
  memcpy(&ReceiveDataMessage, data, data_len); // copy message data to pair struct

  // Copy Receive data to panel
  Panel.cyds_channel = (CYD_channel) ReceiveDataMessage.PanelChannel;
	Panel.cyds_param = (CYD_param) ReceiveDataMessage.Param;
  Panel.cyds_type = (CYD_type) ReceiveDataMessage.ValueType;
  Panel.cyds_boolean = ReceiveDataMessage.BoolValue;
  Panel.cyds_byte = ReceiveDataMessage.ByteValue;
  Panel.cyds_int = ReceiveDataMessage.IntValue;
  Panel.cyds_long = ReceiveDataMessage.LongValue;
	Panel.cyds_string = ReceiveDataMessage.StringValue;

  if(Panel.cyds_channel == CYD_InfoPanel) // handle the InfoPanel text
  {
    ReceiveDataMessage.StringValue[49] = 0; // no more than 50 chars per line
    strcpy(InfoText[InfoTextIdx], ReceiveDataMessage.StringValue); //copy string
    Panel.cyds_string = InfoText[InfoTextIdx]; // add string to panel
    if(++InfoTextIdx >= InfoTextSize) InfoTextIdx = 0; // update idx (goes round)
  }

  Panel.cyds_sent = true; // flag to pannel we have data
	while(Panel.cyds_sent) {delay(1);} // wait till panel is ready
  
  AckMessage.MsgType = ACK;
  AckMessage.MsgId = ReceiveDataMessage.MsgId;

  if(esp_now_send(PeerMac, (uint8_t *) &AckMessage, sizeof(AckMessage)) != ESP_OK)
  {
 	  Serial.println("ERR - ESPNOW Error sending ack"); // log error 
    return;
  }
  
  // Serial.printf("INF - ESPNOW AckSent %d\n", ReceiveDataMessage.MsgId);
}


//-----------------------------------------------------------------------------------------
void HandleAck(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len)
{
  //Serial.println("INF - ESPNOW Ack received");
  memcpy(&AckMessage, data, data_len); // copy message data to pair struct
  if(AckMessage.MsgId == MessageId) MessageStatus = ACKNOWLEGED;
}


//-----------------------------------------------------------------------------------------
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

    if(esp_now_send(peer_addr, data, len) != ESP_OK)
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


//--------------------------------------------------------------------------------------------
void PanelCallback(CYD_channel event) 
{ 
  if(PairStatus != PAIRED) 
  {
    delay(1);
    return; // if not paired skip
  }
  
  if(event == CYD_PanelConnected) delay(10); // on panel reset delay to prevent collisions

  // copy panel data to SendStruct
  SendDataMessage.MsgType = DATA;
  SendDataMessage.MsgId = ++MessageId;
	SendDataMessage.PanelChannel = Panel.cydr_channel;
	SendDataMessage.Param = CYD_NOPARAM;
  SendDataMessage.ValueType = Panel.cydr_type;
  SendDataMessage.BoolValue = Panel.cydr_boolean;
  SendDataMessage.ByteValue = Panel.cydr_byte;
  SendDataMessage.IntValue = Panel.cydr_int;
  SendDataMessage.LongValue = Panel.cydr_long;
  SendDataMessage.StringValue[0] = 0;

  if(!SendEspNowData(PeerMac, (uint8_t *) &SendDataMessage, sizeof(SendDataMessage)))
  {
    Serial.println("ERR - ESPNOW error sending data");
    return;   
  }
}

