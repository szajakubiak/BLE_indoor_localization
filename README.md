# BLE indoor positioning
Arduino code to create the BLE indoor positioning beacon using ESP32 microcontroller.

## Detailed description
Getting started tutorials by [Andreas Spiess](https://www.youtube.com/channel/UCu7_D0o48KbfhpEohoP7YSQ) on using BLE feature of ESP32 microcontroller in Arduino are [here](https://www.youtube.com/watch?app=desktop&v=2mePPqiocUE) and [here](https://www.youtube.com/watch?app=desktop&v=osneajf7Xkg). Specification for the BLE Indoor Positioning Service is available [here](https://www.bluetooth.com/specifications/specs/indoor-positioning-service-1-0/).

## Configuration options
There are several options to set for each positioning beacon. **localNorth** and **localEast** variables store position of the beacon (in decimetres, dm) from the selected reference point. **floorNumber** defines floor of the building on which beacon in installed. **locationName** is descriptive name of the location where beacon is installed. Additionaly, transmit power of the BLE radio can be set using the **transmitPowerSetting** variable (available options described in the sketch file).

## Dependencies
ESP32 is programmed using Arduino IDE with the additional ESP32 core installed.
