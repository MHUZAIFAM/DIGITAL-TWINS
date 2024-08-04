// GPIO Pins
const int TX_PIN = 17;
const int RX_PIN = 16;
const int DE = 4;
const int RE = 5;
const int DE_RE = 4;

// Variables for readings
float Ua = 0, Ub = 0, Uc = 0;                  // Phase Voltages
float Uab = 0, Ubc = 0, Uca = 0;               // RMS Voltages
float Ia = 0, Ib = 0, Ic = 0;                  // Phase Currents
float Pa = 0, Pb = 0, Pc = 0, Pt = 0;          // Active Powers
float Qa = 0, Qb = 0, Qc = 0, Qt = 0;          // Reactive Power
float Sa = 0, Sb = 0, Sc = 0, St = 0;          // Apparent Power
float PFa = 0, PFb = 0, PFc = 0, PFt = 0;      // Power Factor
float FREQ = 0;                                 // Frequency

// CRC-16 (Modbus) for reading 26 registers starting from address 0x27 || CRC-16 (Modbus)	0x0A74
uint8_t ParametersBuf[8] = {0x01, 0x03, 0x00, 0x27, 0x00, 0x1A, 0x74, 0x0A};

// Define macros to set and reset DE and RE pins
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
  byte IncomingBuffer[57] = {0}; // Adjusted size to match the expected response size
  set_DERE;
  Serial2.write(ParametersBuf, 8);
  Serial2.flush();
  reset_DERE;
  
  unsigned long startTime = millis();
  while (Serial2.available() < 57) {
    if (millis() - startTime > 1000) {
      Serial.println("Timeout waiting for response");
      return;
    }
  }
  
  Serial2.readBytes(IncomingBuffer, 57); // Reading 57 bytes into IncomingBuffer

  // Print each byte in IncomingBuffer in hexadecimal format
  Serial.print("Incoming Buffer: ");
  for (int i = 0; i < 57; i++) {
    if (IncomingBuffer[i] < 0x10) Serial.print("0");
    Serial.print(IncomingBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");

  // Calculate and print the CRC
  uint16_t calculatedCRC = calculateCRC(IncomingBuffer, 55); // Calculating CRC for the first 55 bytes
  uint16_t receivedCRC = (IncomingBuffer[56] << 8) | IncomingBuffer[55]; // CRC bytes are at positions 55 and 56

  Serial.print("Calculated CRC: ");
  Serial.print(calculatedCRC, HEX);
  Serial.print("\nReceived CRC: ");
  Serial.print(receivedCRC, HEX);
  Serial.println("");

  if (calculatedCRC == receivedCRC) {
    Serial.println("CRC is correct");

    // Function to extract float values from buffer
    auto extractFloat = [](byte* buffer, int index) -> float {
      int16_t raw = (buffer[index] << 8) | buffer[index + 1];
      return raw / 10.0;
    };

    // Extract and print the values
    Serial.print("PHASE VOLTAGES");
    Serial.print("Ua: ");
    Serial.println(extractFloat(IncomingBuffer, 3));
    Serial.print("Ub: ");
    Serial.println(extractFloat(IncomingBuffer, 5));
    Serial.print("Uc: ");
    Serial.println(extractFloat(IncomingBuffer, 7));
    
    Serial.print("\nRMS VOLTAGES");
    Serial.print("Uab: ");
    Serial.println(extractFloat(IncomingBuffer, 9));
    Serial.print("Ubc: ");
    Serial.println(extractFloat(IncomingBuffer, 11));
    Serial.print("Uca: ");
    Serial.println(extractFloat(IncomingBuffer, 13));
    
    Serial.print("\nPHASE CURRENTS");
    Serial.print("Ia: ");
    Serial.println(extractFloat(IncomingBuffer, 15));
    Serial.print("Ib: ");
    Serial.println(extractFloat(IncomingBuffer, 17));
    Serial.print("Ic: ");
    Serial.println(extractFloat(IncomingBuffer, 19));
    
    Serial.print("\nPHASE ACTIVE POWERS");
    Serial.print("Pa: ");
    Serial.println(extractFloat(IncomingBuffer, 21));
    Serial.print("Pb: ");
    Serial.println(extractFloat(IncomingBuffer, 23));
    Serial.print("Pc: ");
    Serial.println(extractFloat(IncomingBuffer, 25));
    
    Serial.print("\nTOTAL ACTIVE POWER");
    Serial.print("Pt: ");
    Serial.println(extractFloat(IncomingBuffer, 27));
    
    Serial.print("\nPHASE REACTIVE POWER");
    Serial.print("Qa: ");
    Serial.println(extractFloat(IncomingBuffer, 29));
    Serial.print("Qb: ");
    Serial.println(extractFloat(IncomingBuffer, 31));
    Serial.print("Qc: ");
    Serial.println(extractFloat(IncomingBuffer, 33));
    
    Serial.print("\nTOTAL REACTIVE POWER");
    Serial.print("Qt: ");
    Serial.println(extractFloat(IncomingBuffer, 35));
    
    Serial.print("\nPHASE APPARENT POWER");
    Serial.print("Sa: ");
    Serial.println(extractFloat(IncomingBuffer, 37));
    Serial.print("Sb: ");
    Serial.println(extractFloat(IncomingBuffer, 39));
    Serial.print("Sc: ");
    Serial.println(extractFloat(IncomingBuffer, 41));
    
    Serial.print("\nTOTAL APPARENT POWER");
    Serial.print("St: ");
    Serial.println(extractFloat(IncomingBuffer, 43));
    
    Serial.print("\nPHASE POWER FACTOR");
    Serial.print("PFa: ");
    Serial.println(extractFloat(IncomingBuffer, 45));
    Serial.print("PFb: ");
    Serial.println(extractFloat(IncomingBuffer, 47));
    Serial.print("PFc: ");
    Serial.println(extractFloat(IncomingBuffer, 49));
    
    Serial.print("\nTOTAL POWER FACTOR");
    Serial.print("PFt: ");
    Serial.println(extractFloat(IncomingBuffer, 51));
    
    Serial.print("\nFREQUENCY");
    Serial.print("Freq: ");
    Serial.println(extractFloat(IncomingBuffer, 53));

  } else {
    Serial.println("CRC error");
  }

  delay(10);
  Serial.println("-------------------------------------------------------");
  delay(5000); // wait a while before next loop
}
