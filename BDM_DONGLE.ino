#include "BLEDevice.h"

//BLE Server name (the other ESP32 name running the server sketch)
#define bleServerName "BDM"
int mLen;
volatile uint32_t lastGATT;
/* UUID's of the service, characteristic that we want to read*/
// BLE Service
static BLEUUID bmeServiceUUID("0000FFF0-0000-1000-8000-00805F9B34FB");

// BLE Characteristics
static BLEUUID myCharacteristicUUID("0000FFF4-0000-1000-8000-00805F9B34FB");

//Flags stating if should begin connecting and if the connection is up
static boolean doConnect = false;
static boolean connected = false;

//Address of the peripheral device. Address will be found during scanning...
static BLEAddress *pServerAddress;
 
//Characteristicd that we want to read
static BLERemoteCharacteristic* myCharacteristic;

//Activate notify
const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

//Variables to store temperature and humidity
char* GATTchar;

//Flags to check whether new temperature and humidity readings are available
boolean newGATTmsg = false;

#include "BLEfuncs.h"
char functions[][6] = {"DCV", "ACV", "DCA", "ACA", "Ohm", "Cap", "Hz", "Duty", "TempC", "TempF", "Diode", "Cont", "hFe"};
char scales[][6] = {"nano", "micro", "unit", "kilo", "mega"};
char outBuf[64];

void setup() {
  Serial.begin(19200); 
  delay(1000);
 // Serial.println("Starting Arduino BLE Client application...");
  BLEinit();
  BLEconnectMe();
  lastGATT = millis() + 15000; // allow some connect time initially
 // Serial.println("Found ...");
}
uint8_t BDMfunction, BDMscale, BDMdecimals, BDMflags;
int16_t BDMvalue;
float rValue;
union ch2Int {
  char chrs[2];
  int intVal;
} BLEvalx;

#define TIMEOUT 5000   // it can take quite a while to connect
void loop() {
  int tg = millis() - lastGATT;
  if (tg > TIMEOUT) 
  {
   // Serial.printf("Lost service: %i > %i : %i\n", tg, TIMEOUT,tg > TIMEOUT );
    BLEconnectMe();
    delay(5000);
  }
  
  //if new readings are available, transmit
  if (newGATTmsg)
  {
    newGATTmsg = false;
    convertToString();
    // printReadings();
    sendToSerial();
  }
  // 3 readings per second
  delay(100); 
}
void convertToString(void)
{
  float mulVal;
  int sgn;

  // note: bytes are reversed when read as 16 bit numbers
  BDMfunction = ((GATTchar[1] & 0x03) << 2) | ((GATTchar[0] >> 6) & 0x03); 
 // Serial.printf("FU %i %i %i\n",(GATTchar[1] & 0x03) << 2,(GATTchar[0] >> 6) & 0x03, BDMfunction);
  BDMscale = (GATTchar[0] >> 3) & 0x07; 
 //mulVal = pow(10, 3*(3 - BDMscale));
  BDMdecimals = GATTchar[0] & 0x07; 
  mulVal = pow(10, 3*(BDMscale - 4) - BDMdecimals);
  //mulVal = pow(10, - BDMdecimals);
  BDMflags = GATTchar[2];
   // sign bit + unsigned 15 bits
  BDMvalue = (((GATTchar[5] & 0x7f) << 8)) | GATTchar[4];  
  sgn = (GATTchar[5] & 0x80) ? -1 : 1;
  BLEvalx.chrs[1] = GATTchar[4]; 
  BLEvalx.chrs[0] = GATTchar[5];

  rValue = (float)BDMvalue * mulVal * sgn;  
  if((BDMdecimals & 0x07) == 0x07)
    sprintf((char *)&outBuf,"OL\n");
  else
   sprintf((char *)&outBuf,"%5.5f %s\n", rValue, functions[BDMfunction]);
  
}
void sendToSerial(void)
{
   Serial.print(outBuf);
}
void printReadings(){  
  // Serial.printf("GATT [%i] ",mLen);

  // note: bytes are reversed when read as 16 bit numbers  
  Serial.printf("GATT: %02x, %02x, %02x, %02x, %02x, %02x \n", GATTchar[1],GATTchar[0],GATTchar[3],GATTchar[2],GATTchar[5],GATTchar[4]);
  Serial.printf("VA %i, Fn %i, SC %i, De %i, Fl %i\n", BDMvalue, BDMfunction, BDMscale, BDMdecimals, BDMflags);
 // Serial.printf("VA2 %i\n",BLEvalx.intVal);
  //Serial.println(outBuf);
}
