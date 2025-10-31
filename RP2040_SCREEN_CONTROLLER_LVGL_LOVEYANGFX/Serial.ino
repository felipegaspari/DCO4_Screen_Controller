void serial_read_n2() {
  while (Serial2.available() > 0) {
    char commandCharacter = Serial2.read();  //we use characters (letters) for controlling the switch-case
    switch (commandCharacter) {
      case 'p':
        {
          byte paramBytes[3];
          byte finishByte = 1;
          byte readByte = 0;

          while (Serial2.available() < 3) {}

          Serial2.readBytes(paramBytes, 3);

          while (readByte != finishByte) {
            readByte = Serial2.read();
          }

          paramNumber = paramBytes[0];
          paramValue = (int16_t)word(paramBytes[1], paramBytes[2]);

          setDisplayParam();

          paramChangeFlag = true;
          //Serial2.flush();
          break;
        }
      case 'w':
        {
          byte paramBytes[2];
          byte finishByte = 1;
          byte readByte = 0;

          while (Serial2.available() < 2) {}

          Serial2.readBytes(paramBytes, 2);

          while (readByte != finishByte) {
            readByte = Serial2.read();
          }

          paramNumber = paramBytes[0];
          paramValue = (int16_t)paramBytes[1];

          setDisplayParam();

          paramChangeFlag = true;
          //Serial2.flush();
          break;
        }
      case 'q':
        {
          byte finishByte = 1;
          byte readByte = 0;
          byte presetMessage[13];
          // byte presetNameBytes[8];  // = { 32, 32, 32, 32, 32, 32, 32, 32 };
          //while (Serial2.available() < 1) {}

          while (Serial2.available() < 13) {}

          Serial2.readBytes(presetMessage, 13);

          while (readByte != finishByte) {
            readByte = Serial2.read();
          }

          presetNumber = presetMessage[0];
          for (int i = 0; i < 12; i++) {
            if (presetMessage[i + 1] < 32) {
              presetMessage[i + 1] = 32;
            }
            presetNameBytes[i] = presetMessage[i + 1];
          }

          //Serial2.flush();

          presetNameString = String((char*)presetNameBytes);
          presetScrollFlag = true;

          break;
        }
      case 's':
        {
          while (Serial2.available() < 1) {}
          serialSignal = Serial2.read();
          signalFlag = true;
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

          byte paramBytes[5];
          byte paramValueArray[4];
          byte finishByte = 1;
          byte readByte = 0;
          uint32_t paramValue32;

          while (Serial2.available() < 1) {}

          Serial2.readBytes(paramBytes, 5);

          while (readByte != finishByte) {
            readByte = Serial2.read();
          }

          paramNumber = paramBytes[0];
          paramValueArray[0] = paramBytes[1];
          paramValueArray[1] = paramBytes[2];
          paramValueArray[2] = paramBytes[3];
          paramValueArray[3] = paramBytes[4];

          memcpy(&paramValue32, paramValueArray, 4);
          paramValue = (int32_t)paramValue32;

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
          byte paramBytes[3];
          byte finishByte = 1;
          byte readByte = 0;

          while (Serial1.available() < 1) {}

          Serial1.readBytes(paramBytes, 3);

          while (readByte != finishByte) {
            readByte = Serial1.read();
          }

          paramNumber = paramBytes[0];
          paramValue = (int16_t)word(paramBytes[1], paramBytes[2]);

          setDisplayParam();

          if (serialSignal != 6) {
            paramChangeFlag = true;
          }
          //Serial1.flush();
          break;
        }
      case 'w':
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

          setDisplayParam();
          if (serialSignal != 6) {
            paramChangeFlag = true;
          }
          //Serial1.flush();
          break;
        }
      case 'x':
        {
          byte paramBytes[5];
          byte paramValueArray[4];
          byte finishByte = 1;
          byte readByte = 0;
          uint32_t paramValue32;

          while (Serial1.available() < 1) {}

          Serial1.readBytes(paramBytes, 5);

          while (readByte != finishByte) {
            readByte = Serial1.read();
          }

          uint8_t paramNumber = paramBytes[0];
          paramValueArray[0] = paramBytes[1];
          paramValueArray[1] = paramBytes[2];
          paramValueArray[2] = paramBytes[3];
          paramValueArray[3] = paramBytes[4];

          memcpy(&paramValue32, paramValueArray, 4);
          paramValue = (int32_t)paramValue32;

          setDisplayParam();

          if (serialSignal != 6) {
            paramChangeFlag = true;
          }

          paramChangeFlag = true;
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