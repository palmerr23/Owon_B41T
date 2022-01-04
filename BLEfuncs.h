#ifndef MY_BLEfuncs_H_
#define MY_BLEfuncs_H_

static void GattNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                        uint8_t* pData, size_t length, bool isNotify) ;
//Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEAddress pAddress) {
   BLEClient* pClient = BLEDevice::createClient();
 
  // Connect to the remove BLE Server.
  pClient->connect(pAddress);
//  Serial.println(" - Connected to server");
 
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(bmeServiceUUID);
  if (pRemoteService == nullptr) {
//    Serial.print("Failed to find our service UUID: ");
//    Serial.println(bmeServiceUUID.toString().c_str());
    return (false);
  }
 
  // Obtain a reference to the characteristics in the service of the remote BLE server.
  myCharacteristic = pRemoteService->getCharacteristic(myCharacteristicUUID);
 

  if (myCharacteristic == nullptr) {
  //  Serial.print("Failed to find our characteristic UUID");
    return false;
  }
 // Serial.println(" - Found our characteristics");
 
  //Assign callback functions for the Characteristics
  myCharacteristic->registerForNotify(GattNotifyCallback);
 
  return true;
}

//Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
 //   Serial.print("BLE Advertised Device found: ");
 //   Serial.println(advertisedDevice.toString().c_str());
 //   Serial.printf("[%s]\n", advertisedDevice.getName().c_str());
    const char *aGn = advertisedDevice.getName().c_str();

    if (strcmp(aGn, bleServerName) == 0) { //Check if the name of the advertiser matches
      advertisedDevice.getScan()->stop(); //Scan can be stopped, we found what we are looking for
      pServerAddress = new BLEAddress(advertisedDevice.getAddress()); //Address of advertiser is the one we need
      doConnect = true; //Set indicator, stating that we are ready to connect
   //   Serial.println("Device found. Connecting!");
    }
  }
};
 
//When the BLE Server sends a new temperature reading with the notify property
static void GattNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                               uint8_t* pData, size_t length, bool isNotify) 
{
  //store value
  GATTchar = (char*)pData;
  newGATTmsg = true;
  mLen = length;
  lastGATT = millis();
}
void BLEconnect(void)
{
  // Serial.println("..connecting..");
    if (connectToServer(*pServerAddress)) {
    //  Serial.println("We are now connected to the BLE Server.");
      //Activate the Notify property of each Characteristic
      myCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
 
      connected = true;
    } else {
     // Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
    }
    doConnect = false;
}
void BLEinit(void)
{
    //Init BLE device
  BLEDevice::init("MyClient");
}
void BLEscan(void)
{
    // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}
void BLEconnectMe(void)
{
  while (1)
  {
    BLEscan();
    if (doConnect == true) 
    {
     BLEconnect(); 
     break;  
    }
    Serial.println("Disconnected");
    delay(5000);
  }
}
#endif
