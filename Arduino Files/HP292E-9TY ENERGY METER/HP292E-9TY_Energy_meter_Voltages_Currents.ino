// GPIO Pins
const int TX_PIN = 17;
const int RX_PIN = 16;
const int DE = 4;
const int RE = 5;
const int DE_RE = 4;
float Ua = 0, Ub = 0, Uc = 0;
float Uab = 0, Ubc = 0, Uca = 0;
float Iab = 0, Ibc = 0, Ica = 0;

// CRC-16 (Modbus) for reading 9 registers starting from address 0x27H word
uint8_t readVoltageBuf[8] = {0x01, 0x03, 0x00, 0x27, 0x00, 0x09, 0x35, 0xC7};

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
  Serial2.begin(19200, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("Sensor Online"); // initialize serial
}

void loop() {
  byte IncomingBuffer[23] = {0}; // Corrected size to match the expected response size
  set_DERE;
  Serial2.write(readVoltageBuf, 8);
  Serial2.flush();
  reset_DERE;
  
  unsigned long startTime = millis();
  while (Serial2.available() < 23) {
    if (millis() - startTime > 1000) {
      Serial.println("Timeout waiting for response");
      return;
    }
  }
  
  Serial2.readBytes(IncomingBuffer, 23); // Reading 23 bytes into IncomingBuffer

  // Print each byte in IncomingBuffer in hexadecimal format
  Serial.print("Incoming Buffer: ");
  for (int i = 0; i < 23; i++) {
    if (IncomingBuffer[i] < 0x10) Serial.print("0");
    Serial.print(IncomingBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");

  // Calculate and print the CRC
  uint16_t calculatedCRC = calculateCRC(IncomingBuffer, 21); // Calculating CRC for the first 21 bytes
  uint16_t receivedCRC = (IncomingBuffer[22] << 8) | IncomingBuffer[21]; // CRC bytes are at positions 21 and 22

  Serial.print("Calculated CRC: ");
  Serial.print(calculatedCRC, HEX);
  Serial.print("\nReceived CRC: ");
  Serial.print(receivedCRC, HEX);
  Serial.println("");

  if (calculatedCRC == receivedCRC) {
    Serial.println("CRC is correct");

    // Print and calculate Ua
    int UaDecimal = (IncomingBuffer[3] << 8) | IncomingBuffer[4];
    float Ua = UaDecimal / 10.0;
    Serial.print("Ua: ");
    Serial.println(Ua);

    // Print and calculate Ub
    int UbDecimal = (IncomingBuffer[5] << 8) | IncomingBuffer[6];
    float Ub = UbDecimal / 10.0;
    Serial.print("Ub: ");
    Serial.println(Ub);

    // Print and calculate Uc
    int UcDecimal = (IncomingBuffer[7] << 8) | IncomingBuffer[8];
    float Uc = UcDecimal / 10.0;
    Serial.print("Uc: ");
    Serial.println(Uc);

    // Print and calculate Uab
    int UabDecimal = (IncomingBuffer[9] << 8) | IncomingBuffer[10];
    float Uab = UabDecimal / 10.0;
    Serial.print("Uab: ");
    Serial.println(Uab);

    // Print and calculate Ubc
    int UbcDecimal = (IncomingBuffer[11] << 8) | IncomingBuffer[12];
    float Ubc = UbcDecimal / 10.0;
    Serial.print("Ubc: ");
    Serial.println(Ubc);

    // Print and calculate Uca
    int UcaDecimal = (IncomingBuffer[13] << 8) | IncomingBuffer[14];
    float Uca = UcaDecimal / 10.0;
    Serial.print("Uca: ");
    Serial.println(Uca);

    // Print and calculate Iab
    int IabDecimal = (IncomingBuffer[15] << 8) | IncomingBuffer[16];
    float Iab = IabDecimal / 10.0;
    Serial.print("Iab: ");
    Serial.println(Iab);

    // Print and calculate Ibc
    int IbcDecimal = (IncomingBuffer[17] << 8) | IncomingBuffer[18];
    float Ibc = IbcDecimal / 10.0;
    Serial.print("Ibc: ");
    Serial.println(Ibc);

    // Print and calculate Ica
    int IcaDecimal = (IncomingBuffer[19] << 8) | IncomingBuffer[20];
    float Ica = IcaDecimal / 10.0;
    Serial.print("Ica: ");
    Serial.println(Ica);

  } else {
    Serial.println("CRC error");
  }

  delay(10);
  Serial.println("-------------------------------------------------------");
  delay(5000); // wait a while before next loop
}
