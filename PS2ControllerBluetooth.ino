#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

int LED_BUILTIN = 2;

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
int txValue = 0;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks
{
  void onConnect(BLEServer* pserver)
  {
    deviceConnected = true;
  };
   void onDisconnect(BLEServer* pserver)
  {
    deviceConnected = false;
  }
};

void CreateConnection()
{
  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

  // BLE2092 needed to notify
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();
  // ESP_LE_AUTH_BOND;

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting for a Client connection to notify...");
}

void NormalBlink()
{
 digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
 delay(1000);                       // wait for a second
 digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
 delay(1000);   
}

void ShortBlink()
{
 digitalWrite(LED_BUILTIN, HIGH);
 delay(250);
 digitalWrite(LED_BUILTIN, LOW);
 delay(250);
}

void TripleShortBlink()
{
  ShortBlink();
  ShortBlink();
  ShortBlink();
}


void setup()
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(9600);

  CreateConnection();
}

void loop() 
{
 if(deviceConnected)
 {
  txValue = random(-10, 20);

  // Conversion of txValue
  char txString[8];
  dtostrf(txValue, 1, 2, txString);

  // Setting the value to the characteristic
  pCharacteristic->setValue(txString);

  // Notifying the connected clinet
  pCharacteristic->notify();
  Serial.println("Sent value: " + String(txString));
  delay(1000);
 }
 else
 {
  CreateConnection();
  while(!deviceConnected)
  {
    NormalBlink();
  }
  TripleShortBlink();
 }
}
