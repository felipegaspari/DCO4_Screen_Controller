// Definitions for serial-related shared state.
// These were previously defined in Serial.h; they now live here with
// extern declarations in the header to avoid multiple-definition issues.
volatile byte    presetNumber       = 0;
String           presetNameString   = "Vacio";

volatile bool    presetScrollFlag   = false;

volatile byte    paramNumber        = 0;
volatile int32_t paramValue         = 0;
String           paramName;

volatile bool    paramChangeFlag    = false;

volatile bool    updateADSR1Flag    = false;
volatile bool    updateADSR2Flag    = false;

volatile bool    signalFlag         = false;
volatile byte    serialSignal       = 1;

volatile bool    presetCharFlag     = false;
volatile byte    presetChar         = 0;

volatile byte    levelBarFlag       = true;

volatile char    presetNameBytes[13];
volatile char    presetNameBytesOLD[13];


void serial_read_n2() {
  while (Serial2.available() > 0) {
    char commandCharacter = Serial2.read();  //we use characters (letters) for controlling the switch-case
    switch (commandCharacter) {
      case 'p':
        {
          uint8_t payload[4];
          // Wait for full 'p' payload: [id, hi, lo, finish]
          while (Serial2.available() < 4) {}
          Serial2.readBytes(payload, 4);

          ParamFrame frame;
          decode_param_p(payload, frame);
          paramNumber = frame.id;
          paramValue  = frame.value;

          setDisplayParam();
          paramChangeFlag = true;
          break;
        }
      case 'w':
        {
          uint8_t payload[3];
          // Wait for full 'w' payload: [id, value, finish]
          while (Serial2.available() < 3) {}
          Serial2.readBytes(payload, 3);

          ParamFrame frame;
          decode_param_w(payload, frame);
          paramNumber = frame.id;
          paramValue  = frame.value;

          setDisplayParam();
          paramChangeFlag = true;
          break;
        }
      case 'q':
        {
          // preset scroll: [presetNumber, 16 chars] followed by finish byte.
          byte presetMessage[17];

          while (Serial2.available() < 17) {}
          Serial2.readBytes(presetMessage, 17);

          presetNumber = presetMessage[0];
          for (int i = 0; i < 16; i++) {
            if (presetMessage[i + 1] < 32) {
              presetMessage[i + 1] = 32;
            }
            presetNameBytes[i] = (char)presetMessage[i + 1];
          }
          // Ensure null termination for LVGL/String:
          presetNameBytes[16] = '\0';

          presetNameString = String((char*)presetNameBytes);
          presetScrollFlag = true;

          break;
        }
      case 's':
        {
          while (Serial2.available() < 1) {}
          serialSignal = Serial2.read();
          signalFlag = true;
          // Debug: log incoming signals on Serial2
          Serial.print("Screen Serial2 signal: ");
          Serial.println(serialSignal);
          break;
        }
      case 'c':
        {
          while (Serial2.available() < 1) {}
          presetChar = Serial2.read();
          presetCharFlag = true;
          break;
        }
      case 'x':
        {
          uint8_t payload[6];

          // Wait for full 'x' payload: [id, b0..b3, finish]
          while (Serial2.available() < 6) {}
          Serial2.readBytes(payload, 6);

          ParamFrame frame;
          decode_param_x(payload, frame);
          paramNumber = frame.id;
          paramValue  = frame.value;

          setDisplayParam();
          paramChangeFlag = true;
          break;
        }
    }
  }
}

void serial_read_n() {
  while (Serial1.available() > 0) {
    char commandCharacter = Serial1.read();  //we use characters (letters) for controlling the switch-case
    switch (commandCharacter) {
      case 'a':
        {
          byte byteArray[8];
          Serial1.readBytes(byteArray, 8);


          //MAP AND CONSTRAIN functions should be implemented on the input board.
          ADSR1Attack = word(byteArray[0], byteArray[1]);  //map(constrain(word(byteArray[0], byteArray[1]), 20, 4075), 20, 4075, 0, 4095);
          ADSR1Decay = word(byteArray[2], byteArray[3]);   //map(constrain(word(byteArray[2], byteArray[3]), 20, 4075), 20, 4075, 0, 4095);
          ADSR1Sustain = word(byteArray[4], byteArray[5]);
          ADSR1Release = word(byteArray[6], byteArray[7]);  //map(constrain(word(byteArray[6], byteArray[7]), 15, 4075), 15, 4075, 0, 4095);

          updateADSR1Flag = true;
          break;
        }
      case 'b':
        {
          byte byteArray[8];
          Serial1.readBytes(byteArray, 8);

          ADSR2Attack = word(byteArray[0], byteArray[1]);  //map(constrain(word(byteArray[0], byteArray[1]), 20, 4075), 20, 4075, 5, 4095);
          ADSR2Decay = word(byteArray[2], byteArray[3]);   //map(constrain(word(byteArray[2], byteArray[3]), 20, 4075), 20, 4075, 0, 4095);
          ADSR2Sustain = word(byteArray[4], byteArray[5]);
          ADSR2Release = word(byteArray[6], byteArray[7]);  //map(constrain(word(byteArray[6], byteArray[7]), 20, 4075), 20, 4075, 13, 4095);

          updateADSR2Flag = true;
          break;
        }
      case 'p':
        {
          uint8_t payload[4];

          while (Serial1.available() < 4) {}
          Serial1.readBytes(payload, 4);

          ParamFrame frame;
          decode_param_p(payload, frame);
          paramNumber = frame.id;
          paramValue  = frame.value;

          setDisplayParam();

          if (serialSignal != 6) {
            paramChangeFlag = true;
          }
          //Serial1.flush();
          break;
        }
      case 'w':
        {
          uint8_t payload[3];

          while (Serial1.available() < 3) {}
          Serial1.readBytes(payload, 3);

          ParamFrame frame;
          decode_param_w(payload, frame);
          paramNumber = frame.id;
          paramValue  = frame.value;

          setDisplayParam();
          if (serialSignal != 6) {
            paramChangeFlag = true;
          }
          //Serial1.flush();
          break;
        }
      case 'x':
        {
          uint8_t payload[6];

          while (Serial1.available() < 6) {}
          Serial1.readBytes(payload, 6);

          ParamFrame frame;
          decode_param_x(payload, frame);
          paramNumber = frame.id;
          paramValue  = frame.value;

          setDisplayParam();

          if (serialSignal != 6) {
            paramChangeFlag = true;
          }
          break;
        }
            case 'y':
        {
          byte paramBytes[2];
          byte finishByte = 1;
          byte readByte = 0;

          while (Serial1.available() < 1) {}

          Serial1.readBytes(paramBytes, 2);

          while (readByte != finishByte) {
            readByte = Serial1.read();
          }

          paramNumber = paramBytes[0];
          paramValue = (int16_t)paramBytes[1];

          updateParameters(paramNumber, (uint16_t)paramValue);

          break;
        }
      case 'q':
        {
          byte finishByte = 1;
          byte readByte = 0;
          byte presetMessage[13];
          // byte presetNameBytes[8];  // = { 32, 32, 32, 32, 32, 32, 32, 32 };
          //while (Serial1.available() < 1) {}

          while (Serial1.available() < 1) {}

          Serial1.readBytes(presetMessage, 13);

          while (readByte != finishByte) {
            readByte = Serial1.read();
          }

          presetNumber = presetMessage[0];
          for (int i = 0; i < 12; i++) {
            if (presetMessage[i + 1] < 32) {
              presetMessage[i + 1] = 32;
            }
            presetNameBytes[i] = presetMessage[i + 1];
          }

          presetNameString = String((char*)presetNameBytes);
          presetScrollFlag = true;

          break;
        }
      case 's':
        {
          while (Serial1.available() < 1) {}
          serialSignal = Serial1.read();
          signalFlag = true;
          // Debug: log incoming signals on Serial1
          Serial.print("Screen Serial1 signal: ");
          Serial.println(serialSignal);
          break;
        }
      case 'c':
        {
          while (Serial1.available() < 1) {}
          presetChar = Serial1.read();
          presetCharFlag = true;
          break;
        }
    }
  }
}