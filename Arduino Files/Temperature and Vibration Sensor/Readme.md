Esp32 integrated with temperature and Vibration sensor type RS485

Temperature.ino displays the output for the Sensor using the buffer:
        
        uint8_t readTemperatureBUf[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};

Temp_and_Speed.ino is a combined .ino file to display the speed and the Temperature sensed by the sensor using the following buffer:
        
        uint8_t readTemperatureBUf[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};
        uint8_t readSpeedBuf[8] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA};
