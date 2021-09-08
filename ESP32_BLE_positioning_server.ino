
/* 
  Based on the sketch to emulate parts of the Polar H7 Heart Rate Sensor by Andreas Spiess (https://github.com/SensorsIot/Bluetooth-BLE-on-Arduino-IDE), which was based on the Neil Kolban's example file: https://github.com/nkolban/ESP32_BLE_Arduino
  
  Legal notice from the Andreas Spiess sketch:
 
  Copyright <2017> <Andreas Spiess>

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
 */

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>


// Flags as defined in Table 3.2 of the Indoor Positioning Service Specification
// https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=302114
byte flags[1] = {0b00110101};

// Variables storing the position
int localNorth = 10;    // in dm
int localEast = 60;     // in dm
int floorNumber = 1;
const char locationName[] PROGMEM = {"Room_13"};

// Define BLE transmit power by changing ESP_PWR_LVL_XY
// to one of the following values of XY: N14, N11, N8, N5, N2, P1, P4, P4
// where N stands for negative, P stands for positive
// and number is signal strength in dBm
#define transmitPowerSetting ESP_PWR_LVL_N14

bool _BLEClientConnected = false;


/*
 * BLE structure:
 *  Service: https://www.bluetooth.com/specifications/gatt/services/
 *    Characteristic: https://www.bluetooth.com/specifications/gatt/characteristics/
 *      Descriptor: https://www.bluetooth.com/specifications/gatt/descriptors/
 */

// Define service type
#define indoorPositioningService BLEUUID((uint16_t)0x1821)

// Define characteristic - Indoor Positioning Configuration
BLECharacteristic indPosConfigurationCharacteristic(BLEUUID((uint16_t)0x2AAD), BLECharacteristic::PROPERTY_READ);
// Descriptor for the characteristic
BLEDescriptor indPosConfigurationDescriptor(BLEUUID((uint16_t)0x2903));

// Define characteristic - Local North Coordinate
BLECharacteristic localNorthCoordinateCharacteristic(BLEUUID((uint16_t)0x2AB0), BLECharacteristic::PROPERTY_READ);
// Descriptor for the characteristic
BLEDescriptor localNorthCoordinateDescriptor(BLEUUID((uint16_t)0x2903));

// Define characteristic - Local East Coordinate
BLECharacteristic localEastCoordinateCharacteristic(BLEUUID((uint16_t)0x2AB1), BLECharacteristic::PROPERTY_READ);
// Descriptor for the characteristic
BLEDescriptor localEastCoordinateDescriptor(BLEUUID((uint16_t)0x2903));

// Define characteristic - Floor Number
BLECharacteristic floorNumberCharacteristic(BLEUUID((uint16_t)0x2AB2), BLECharacteristic::PROPERTY_READ);
// Descriptor for the characteristic
BLEDescriptor floorNumberDescriptor(BLEUUID((uint16_t)0x2903));

// Define characteristic - Location Name
BLECharacteristic locationNameCharacteristic(BLEUUID((uint16_t)0x2AB5), BLECharacteristic::PROPERTY_READ);
// Descriptor for the characteristic
BLEDescriptor locationNameDescriptor(BLEUUID((uint16_t)0x2903));


class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer* pServer)
    {
      _BLEClientConnected = true;
    };

    void onDisconnect(BLEServer* pServer)
    {
      _BLEClientConnected = false;
    }
};

void InitBLE()
{
  BLEDevice::init("Position beacon");

  // Set BLE transmit power
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, transmitPowerSetting);
  
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pPosition = pServer->createService(indoorPositioningService);

  pPosition->addCharacteristic(&indPosConfigurationCharacteristic);
  indPosConfigurationDescriptor.setValue("Configuration");
  indPosConfigurationCharacteristic.addDescriptor(&indPosConfigurationDescriptor);
  
  pPosition->addCharacteristic(&localNorthCoordinateCharacteristic);
  localNorthCoordinateDescriptor.setValue("Local north coordinate in dm");
  localNorthCoordinateCharacteristic.addDescriptor(&localNorthCoordinateDescriptor);

  pPosition->addCharacteristic(&localEastCoordinateCharacteristic);
  localEastCoordinateDescriptor.setValue("Local east coordinate in dm");
  localEastCoordinateCharacteristic.addDescriptor(&localEastCoordinateDescriptor);

  pPosition->addCharacteristic(&floorNumberCharacteristic);
  floorNumberDescriptor.setValue("Floor number");
  floorNumberCharacteristic.addDescriptor(&floorNumberDescriptor);

  pPosition->addCharacteristic(&locationNameCharacteristic);
  locationNameDescriptor.setValue("Location name");
  locationNameCharacteristic.addDescriptor(&locationNameDescriptor);

  pServer->getAdvertising()->addServiceUUID(indoorPositioningService);

  pPosition->start();
  // Start advertising
  pServer->getAdvertising()->start();
}


void setup()
{
  // Turn off the WiFi radio
  WiFi.mode(WIFI_OFF);
  
  InitBLE();

  indPosConfigurationCharacteristic.setValue(flags, 1);
  localNorthCoordinateCharacteristic.setValue(localNorth);
  localEastCoordinateCharacteristic.setValue(localEast);

  /* 
   *  Floor number is encoded by adding 20 to the real floor number
   *  except the ground floor, which is encoded as 253
   */
  if (floorNumber == 0)
  {
    floorNumber = 253;
  }
  else
  {
    floorNumber = floorNumber + 20;
  }
  floorNumberCharacteristic.setValue(floorNumber);

  locationNameCharacteristic.setValue(locationName);
}

void loop()
{

}
