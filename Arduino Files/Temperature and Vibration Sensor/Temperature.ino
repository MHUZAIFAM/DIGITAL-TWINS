// GPIO Pins
const int TX_PIN = 17;
const int RX_PIN = 16;
const int DE = 4;
const int RE = 5;
const int DE_RE = 4;
float Temperature = 0;

uint8_t readTemperatureBUf[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};
// CRC-16 (Modbus) 0xCAD5
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
  byte IncomingBuffer[7] = {0};
  set_DERE;
  Serial2.write(readTemperatureBUf, 8);
  Serial2.flush();
  reset_DERE;
  while (Serial2.available() < 7);
  Serial2.readBytes(IncomingBuffer, 7);

  // Print each byte in IncomingBuffer in hexadecimal format
  Serial.print("Incoming Buffer: ");
  for (int i = 0; i < 7; i++) {
    if (IncomingBuffer[i] < 0x10) Serial.print("0");
    Serial.print(IncomingBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");

  // Calculate and print the CRC
  uint16_t calculatedCRC = calculateCRC(IncomingBuffer, 5); // Calculating CRC for the first 5 bytes
  uint16_t receivedCRC = (IncomingBuffer[6] << 8) | IncomingBuffer[5];

  Serial.print("Calculated CRC: ");
  Serial.print(calculatedCRC, HEX);
  Serial.print("\nReceived CRC: ");
  Serial.print(receivedCRC, HEX);
  Serial.println("");

  if (calculatedCRC == receivedCRC) {
    Serial.println("CRC is correct");

    Serial.print("Temperature(HEX): ");
    if (IncomingBuffer[3] < 0x10) Serial.print("0");
    Serial.print(IncomingBuffer[3], HEX);
    if (IncomingBuffer[4] < 0x10) Serial.print("0");
    Serial.print(IncomingBuffer[4], HEX);
    Serial.println("");

    // Calculate and print temperature in decimal
    int temperatureDecimal = (IncomingBuffer[3] << 8) | IncomingBuffer[4];
    float temperature = temperatureDecimal / 10.0;
    Serial.print("Temperature= ");
    Serial.println(temperature);
  } else {
    Serial.println("CRC error");
  }

  delay(10);
  Serial.println("-------------------------------------------------------");
  delay(5000); // wait a while before next loop
}
