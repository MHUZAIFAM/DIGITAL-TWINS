
#include "I2CScanner.h"
#include <Wire.h>

I2CScanner scanner;

//Connect the SDA pin with D21 of esp32 and SCL pin with D22 of the esp32, then connect the Vin with 3.3V of the esp32 and gnd with the end of the esp32
//the address of the i2c device is known to be at 0x2A
// Define custom SDA and SCL pins for ESP32
#define SDA_PIN 21  // You can change this to your preferred SDA pin
#define SCL_PIN 22  // You can change this to your preferred SCL pin

void setup() 
{	
    // Initialize I2C with custom SDA and SCL pins for ESP32
    Wire.begin(SDA_PIN, SCL_PIN);
	
    Serial.begin(9600);
    while (!Serial) {};

    scanner.Init();
}

void loop() 
{
    scanner.Scan();
    delay(5000);
}
