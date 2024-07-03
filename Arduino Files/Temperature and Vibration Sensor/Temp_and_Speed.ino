// GPIO Pins
const int TX_PIN = 17;
const int RX_PIN = 16;
const int DE = 4;
const int RE = 5;
const int DE_RE = 4;
float Temperature = 0;
float Speed_x_axis = 0;

uint8_t readTemperatureBUf[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};
uint8_t readSpeedBuf[8] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA};

#define setDE {digitalWrite(DE, HIGH);}
#define resetDE {digitalWrite(DE, LOW);}
#define setRE {digitalWrite(RE, HIGH);}
#define resetRE {digitalWrite(RE, LOW);}
#define set_DERE {setDE; setRE;}
#define reset_DERE {resetDE; resetRE;}

// Function to calculate CRC-16
uint16_t calculateCRC(uint8_t *buf, int len) {
  uint16_t crc = 0xFFFF;

  for (int pos = 0; pos < len; pos++) {
    crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) { // Loop over each bit
      if ((crc & 0x0001) != 0) { // If the LSB is set
        crc >>= 1; // Shift right and XOR 0xA001
        crc ^= 0xA001;
      } else // Else LSB is not set
        crc >>= 1; // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

void setup() {
  pinMode(DE, OUTPUT);
  pinMode(RE, OUTPUT);
  reset_DERE;
  Serial.begin(9600);
  Serial2.begin(4800, SERIAL_8N1, 16, 17);
  Serial.print("Sensor Online \r\n"); // initialize serial
}

void loop() {
  byte TempBuffer[7] = {0};
  byte SpeedBuffer[7] = {0};
  
  // Read temperature
  set_DERE;
  Serial2.write(readTemperatureBUf, 8);
  Serial2.flush();
  reset_DERE;
  while (Serial2.available() < 7);
  Serial2.readBytes(TempBuffer, 7);

  // Print Incoming Buffer for Temperature in hexadecimal format
  Serial.print("Incoming Buffer for Temperature: ");
  for (int i = 0; i < 7; i++) {
    if (TempBuffer[i] < 0x10) Serial.print("0");
    Serial.print(TempBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");

  // Calculate and print the CRC for Temperature
  uint16_t calculatedCRC_Temp = calculateCRC(TempBuffer, 5); // Calculating CRC for the first 5 bytes
  uint16_t receivedCRC_Temp = (TempBuffer[6] << 8) | TempBuffer[5];

  Serial.print("CRC for Temperature: ");
  Serial.print(calculatedCRC_Temp, HEX);
  Serial.print(" ");
  Serial.println(receivedCRC_Temp, HEX);
  Serial.println("");

  if (calculatedCRC_Temp == receivedCRC_Temp) {
    Serial.print("Temperature(HEX): ");
    if (TempBuffer[3] < 0x10) Serial.print("0");
    Serial.print(TempBuffer[3], HEX);
    if (TempBuffer[4] < 0x10) Serial.print("0");
    Serial.print(TempBuffer[4], HEX);
    Serial.println("");

    // Calculate and print temperature in decimal
    int temperatureDecimal = (TempBuffer[3] << 8) | TempBuffer[4];
    float temperature = temperatureDecimal / 10.0;
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Temperature = temperature; // Store temperature value
  } else {
    Serial.println("CRC error for Temperature");
  }

  delay(10);

  // Read speed
  set_DERE;
  Serial2.write(readSpeedBuf, 8);
  Serial2.flush();
  reset_DERE;
  while (Serial2.available() < 7);
  Serial2.readBytes(SpeedBuffer, 7);

  // Print Incoming Buffer for Speed in hexadecimal format
  Serial.print("Incoming Buffer for Speed: ");
  for (int i = 0; i < 7; i++) {
    if (SpeedBuffer[i] < 0x10) Serial.print("0");
    Serial.print(SpeedBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");

  // Calculate and print the CRC for Speed
  uint16_t calculatedCRC_Speed = calculateCRC(SpeedBuffer, 5); // Calculating CRC for the first 5 bytes
  uint16_t receivedCRC_Speed = (SpeedBuffer[6] << 8) | SpeedBuffer[5];

  Serial.print("CRC for Speed: ");
  Serial.print(calculatedCRC_Speed, HEX);
  Serial.print(" ");
  Serial.println(receivedCRC_Speed, HEX);
  Serial.println("");

  if (calculatedCRC_Speed == receivedCRC_Speed) {
    Serial.print("Speed(HEX): ");
    if (SpeedBuffer[3] < 0x10) Serial.print("0");
    Serial.print(SpeedBuffer[3], HEX);
    if (SpeedBuffer[4] < 0x10) Serial.print("0");
    Serial.print(SpeedBuffer[4], HEX);
    Serial.println("");

    // Calculate and print speed in decimal
    int speedDecimal = (SpeedBuffer[3] << 8) | SpeedBuffer[4];
    float speed = speedDecimal / 10.0;
    Serial.print("Speed X Axis: ");
    Serial.println(speed);
    Speed_x_axis = speed; // Store speed value
  } else {
    Serial.println("CRC error for Speed");
  }

  delay(10);

  Serial.println("------------------------------------------------------------------");
  delay(5000); // wait a while before next loop
}
