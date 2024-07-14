#include <Wire.h>

#define FLIR_LEPTON_ADDRESS 0x2A  // Example I2C address of FLiR Lepton module

void setup() {
    Serial.begin(9600);
    Wire.begin();
    delay(1000);  // Delay to let serial monitor open
    Serial.println("Starting FLiR Lepton register dump...");
    dumpAllRegisters();
}

void loop() {
    // Not used in this example, since we only want to dump registers once
}

void dumpAllRegisters() {
    Wire.beginTransmission(FLIR_LEPTON_ADDRESS);
    Wire.write(0x00);  // Start reading from register address 0x00
    Wire.endTransmission();

    Wire.requestFrom(FLIR_LEPTON_ADDRESS, 256);  // Read 256 bytes (adjust as needed)

    int count = 0;

    while (Wire.available() && count < 20) {  // Limit to print the first 20 valid registers
        byte regAddress = Wire.read();
        byte regValue = Wire.read();
        
        if (regAddress != 0x00) {  // Filter out zeroed registers
            Serial.print("Register Address: 0x");
            if (regAddress < 0x10) {
                Serial.print("0");  // Ensure two-digit hex display
            }
            Serial.print(regAddress, HEX);
            Serial.print(", Value: 0x");
            if (regValue < 0x10) {
                Serial.print("0");  // Ensure two-digit hex display
            }
            Serial.println(regValue, HEX);
            count++;
        }
    }
}
