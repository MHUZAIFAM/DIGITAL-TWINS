# Flir lepton thermal camera Breakout v1.4 

## FLIR_LEPTON_I2C_ADDRESS 
This sketch scans for I2C devices connected to an ESP32 using custom SDA and SCL pins.

### Description

This Arduino sketch demonstrates how to scan for I2C devices connected to an ESP32 microcontroller using custom SDA and SCL pin configurations. It initializes the Wire library with specific pins and uses an I2C scanner utility to detect devices on the bus.
### Notes
- Modify the `SDA_PIN` and `SCL_PIN` constants if you are using different GPIO pins for SDA and SCL.
- This sketch initializes the Wire library with custom pins and uses the `I2CScanner` library to scan for devices.
### Example Output
Upon running the sketch, the Serial Monitor will display detected I2C addresses similar to:

    I2C device found at address 0x2A !

## ModuleInfo
This Arduino sketch demonstrates how to initialize and print module information for the Lepton-FLiR thermal camera using the LeptonFLiR library.
### Description
The sketch enables debug output support in the LeptonFLiR library to print module information including initialized states, register values, current settings, etc. It initializes the Wire and SPI interfaces required by the LeptonFLiR library and sets the necessary configurations for the thermal camera.
### Requirements
- Arduino IDE
- Lepton-FLiR thermal camera module
- Arduino board with SPI and I2C capabilities

### Hardware Setup
- Connect the Lepton-FLiR module to the SPI and I2C interfaces of your Arduino board.
- Ensure proper power supply to the Lepton-FLiR module.

### Usage
1. Upload the sketch to your Arduino board using the Arduino IDE.
2. Open the Serial Monitor to view the module information printed periodically.
3. Ensure the Lepton-FLiR module is correctly connected and powered.

### Notes

- Modify the `Wire.setClock()` parameter to match your desired I2C baud rate (supported rates are 100kHz, 400kHz, and 1000kHz).
- Uncomment the `#define LEPFLIR_ENABLE_DEBUG_OUTPUT 1` line in `LeptonFLiR.h` to enable debug output support if necessary.

### Example Output
Upon running the sketch, the Serial Monitor will display module information similar to:
    
         ~~~ LeptonFLiR Module Info ~~~
    
    Chip Select Pin:
    D53 (active-low)
    
    SPI Port Speed:
    15.00MHz (SPI_CLOCK_DIV16)
    
    Image Storage Mode:
    3: LeptonFLiR_ImageStorageMode_40x30_8bpp
    
    Temperature Mode:
    0: LeptonFLiR_TemperatureMode_Celsius
    
    Memory Footprint:
    Image Data: 1447B, SPI Frame Data: 367B, Telemetry Data: 0B, Total: 1814B
    
    Power Register:
        LeptonFLiR::readRegister regAddress: 0x0
          LeptonFLiR::readRegister retVal: 0x0
    0x0
    
    Status Register:
        LeptonFLiR::readRegister regAddress: 0x2
          LeptonFLiR::readRegister retVal: 0x6
    0x6
    ...

