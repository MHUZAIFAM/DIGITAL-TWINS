// GPIO Pins
const int TX_PIN = 17;
const int RX_PIN = 16;
const int DE = 4;
const int RE = 5;
const int DE_RE = 4;
float Ua = 0, Ub = 0, Uc = 0;

//CRC-16 (Modbus) 0x0674 phase voltages
uint8_t readVoltageBUf[8] = {0x01, 0x03, 0x00, 0x3B, 0x00, 0x03, 0x74, 0x06};
//CRC-16 (Modbus)	0x0764 line voltages
//uint8_t readVoltageBUf[8] = {0x01, 0x03, 0x00, 0x3E, 0x00, 0x03, 0x64, 0x07};
//CRC-16 (Modbus)	0xDF55 current
//uint8_t readVoltageBUf[8] = {0x01, 0x03, 0x00, 0x41, 0x00, 0x03, 0x55, 0xDF};

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
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("Sensor Online"); // initialize serial
}

void loop() {
  byte IncomingBuffer[11] = {0}; // Corrected size to match the expected response size
  set_DERE;
  Serial2.write(readVoltageBUf, 8);
  Serial2.flush();
  reset_DERE;
  
  unsigned long startTime = millis();
  while (Serial2.available() < 11) {
    if (millis() - startTime > 1000) {
      Serial.println("Timeout waiting for response");
      return;
    }
  }
  
  Serial2.readBytes(IncomingBuffer, 11); // Reading 11 bytes into IncomingBuffer

  // Print each byte in IncomingBuffer in hexadecimal format
  Serial.print("Incoming Buffer: ");
  for (int i = 0; i < 11; i++) {
    if (IncomingBuffer[i] < 0x10) Serial.print("0");
    Serial.print(IncomingBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");

  // Calculate and print the CRC
  uint16_t calculatedCRC = calculateCRC(IncomingBuffer, 9); // Calculating CRC for the first 9 bytes
  uint16_t receivedCRC = (IncomingBuffer[10] << 8) | IncomingBuffer[9]; // CRC bytes are at positions 9 and 10

  Serial.print("Calculated CRC: ");
  Serial.print(calculatedCRC, HEX);
  Serial.print("\nReceived CRC: ");
  Serial.print(receivedCRC, HEX);
  Serial.println("");

  if (calculatedCRC == receivedCRC) {
    Serial.println("CRC is correct");

    // Print Ua
    Serial.print("Voltage Ua(HEX): ");
    if (IncomingBuffer[3] < 0x10) Serial.print("0");
    Serial.print(IncomingBuffer[3], HEX);
    if (IncomingBuffer[4] < 0x10) Serial.print("0");
    Serial.print(IncomingBuffer[4], HEX);
    Serial.println("");

    int UaDecimal = (IncomingBuffer[3] << 8) | IncomingBuffer[4];
    float Ua = UaDecimal / 10.0;
    Serial.print("Ua: ");
    Serial.println(Ua);

    // Print Ub
    Serial.print("Voltage Ub(HEX): ");
    if (IncomingBuffer[5] < 0x10) Serial.print("0");
    Serial.print(IncomingBuffer[5], HEX);
    if (IncomingBuffer[6] < 0x10) Serial.print("0");
    Serial.print(IncomingBuffer[6], HEX);
    Serial.println("");

    int UbDecimal = (IncomingBuffer[5] << 8) | IncomingBuffer[6];
    float Ub = UbDecimal / 10.0;
    Serial.print("Ub: ");
    Serial.println(Ub);

    // Print Uc
    Serial.print("Voltage Uc(HEX): ");
    if (IncomingBuffer[7] < 0x10) Serial.print("0");
    Serial.print(IncomingBuffer[7], HEX);
    if (IncomingBuffer[8] < 0x10) Serial.print("0");
    Serial.print(IncomingBuffer[8], HEX);
    Serial.println("");

    int UcDecimal = (IncomingBuffer[7] << 8) | IncomingBuffer[8];
    float Uc = UcDecimal / 10.0;
    Serial.print("Uc: ");
    Serial.println(Uc);
  } else {
    Serial.println("CRC error");
  }

  delay(10);
  Serial.println("-------------------------------------------------------");
  delay(5000); // wait a while before next loop
}
