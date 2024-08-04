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

// CRC-16 (Modbus) for reading 26 registers starting from address 0x27 || CRC-16 (Modbus) 0x0A74
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
  reset_DERE; // Set both DE and RE to LOW for initial state
  Serial.begin(9600);
  Serial2.begin(19200, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("Sensor Online"); // initialize serial
}

void loop() {
  byte IncomingBuffer[57] = {0}; // Adjusted size to match the expected response size
  set_DERE; // Enable RS-485 driver
  Serial2.write(ParametersBuf, 8);
  Serial2.flush();
  reset_DERE; // Disable RS-485 driver
  
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

    // Extract and print the values
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

    // Print and calculate Ia
    int IaDecimal = (IncomingBuffer[15] << 8) | IncomingBuffer[16];
    float Ia = IaDecimal / 10.0;
    Serial.print("Ia: ");
    Serial.println(Ia);

    // Print and calculate Ib
    int IbDecimal = (IncomingBuffer[17] << 8) | IncomingBuffer[18];
    float Ib = IbDecimal / 10.0;
    Serial.print("Ib: ");
    Serial.println(Ib);

    // Print and calculate Ic
    int IcDecimal = (IncomingBuffer[19] << 8) | IncomingBuffer[20];
    float Ic = IcDecimal / 10.0;
    Serial.print("Ic: ");
    Serial.println(Ic);

    // Print and calculate Pa
    int PaDecimal = (IncomingBuffer[21] << 8) | IncomingBuffer[22];
    float Pa = PaDecimal / 10.0;
    Serial.print("Pa: ");
    Serial.println(Pa);

    // Print and calculate Pb
    int PbDecimal = (IncomingBuffer[23] << 8) | IncomingBuffer[24];
    float Pb = PbDecimal / 10.0;
    Serial.print("Pb: ");
    Serial.println(Pb);

    // Print and calculate Pc
    int PcDecimal = (IncomingBuffer[25] << 8) | IncomingBuffer[26];
    float Pc = PcDecimal / 10.0;
    Serial.print("Pc: ");
    Serial.println(Pc);

    // Print and calculate Pt
    int PtDecimal = (IncomingBuffer[27] << 8) | IncomingBuffer[28];
    float Pt = PtDecimal / 10.0;
    Serial.print("Pt: ");
    Serial.println(Pt);

    // Print and calculate Qa
    int QaDecimal = (IncomingBuffer[29] << 8) | IncomingBuffer[30];
    float Qa = QaDecimal / 10.0;
    Serial.print("Qa: ");
    Serial.println(Qa);

    // Print and calculate Qb
    int QbDecimal = (IncomingBuffer[31] << 8) | IncomingBuffer[32];
    float Qb = QbDecimal / 10.0;
    Serial.print("Qb: ");
    Serial.println(Qb);

    // Print and calculate Qc
    int QcDecimal = (IncomingBuffer[33] << 8) | IncomingBuffer[34];
    float Qc = QcDecimal / 10.0;
    Serial.print("Qc: ");
    Serial.println(Qc);

    // Print and calculate Qt
    int QtDecimal = (IncomingBuffer[35] << 8) | IncomingBuffer[36];
    float Qt = QtDecimal / 10.0;
    Serial.print("Qt: ");
    Serial.println(Qt);

    // Print and calculate Sa
    int SaDecimal = (IncomingBuffer[37] << 8) | IncomingBuffer[38];
    float Sa = SaDecimal / 10.0;
    Serial.print("Sa: ");
    Serial.println(Sa);

    // Print and calculate Sb
    int SbDecimal = (IncomingBuffer[39] << 8) | IncomingBuffer[40];
    float Sb = SbDecimal / 10.0;
    Serial.print("Sb: ");
    Serial.println(Sb);

    // Print and calculate Sc
    int ScDecimal = (IncomingBuffer[41] << 8) | IncomingBuffer[42];
    float Sc = ScDecimal / 10.0;
    Serial.print("Sc: ");
    Serial.println(Sc);

    // Print and calculate St
    int StDecimal = (IncomingBuffer[43] << 8) | IncomingBuffer[44];
    float St = StDecimal / 10.0;
    Serial.print("St: ");
    Serial.println(St);

    // Print and calculate PFa
    int PFaDecimal = (IncomingBuffer[45] << 8) | IncomingBuffer[46];
    float PFa = PFaDecimal / 10.0;
    Serial.print("PFa: ");
    Serial.println(PFa);

    // Print and calculate PFb
    int PFbDecimal = (IncomingBuffer[47] << 8) | IncomingBuffer[48];
    float PFb = PFbDecimal / 10.0;
    Serial.print("PFb: ");
    Serial.println(PFb);

    // Print and calculate PFc
    int PFcDecimal = (IncomingBuffer[49] << 8) | IncomingBuffer[50];
    float PFc = PFcDecimal / 10.0;
    Serial.print("PFc: ");
    Serial.println(PFc);

    // Print and calculate PFt
    int PFtDecimal = (IncomingBuffer[51] << 8) | IncomingBuffer[52];
    float PFt = PFtDecimal / 10.0;
    Serial.print("PFt: ");
    Serial.println(PFt);

    // Print and calculate FREQ
    int FREQDecimal = (IncomingBuffer[53] << 8) | IncomingBuffer[54];
    float FREQ = FREQDecimal / 10.0;
    Serial.print("Freq: ");
    Serial.println(FREQ);

  } else {
    Serial.println("CRC error");
  }

  delay(10);
  Serial.println("-------------------------------------------------------");
  delay(5000); // wait a while before next loop
}
