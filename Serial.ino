void serial_read_n() {
  while (Serial2.available() > 0) {
    char commandCharacter = Serial2.read();  //we use characters (letters) for controlling the switch-case
    //Serial.println(Serial2.read());
    switch (commandCharacter) {
      case 'p':
        {
          byte paramBytes[3];
          byte finishByte = 1;
          byte readByte = 0;

          while (Serial2.available() < 1) {}

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

          while (Serial2.available() < 1) {}

          Serial2.readBytes(paramBytes, 2);

          while (readByte != finishByte) {
            readByte = Serial2.read();
          }

          paramNumber = paramBytes[0];
          paramValue = paramBytes[1];

          setDisplayParam();

          paramChangeFlag = true;
          //Serial2.flush();
          break;
        }
      case 'q':
        {
          byte finishByte = 1;
          byte readByte = 0;
          byte presetMessage[9];
          // byte presetNameBytes[8];  // = { 32, 32, 32, 32, 32, 32, 32, 32 };
          //while (Serial2.available() < 1) {}

          while (Serial2.available() < 9) {}

          Serial2.readBytes(presetMessage, 9);

          while (readByte != finishByte) {
            readByte = Serial2.read();
          }

          presetNumber = presetMessage[0];
          for (int i = 0; i < 8; i++) {
            presetNameBytes[i] = presetMessage[i + 1];
          }

          Serial2.flush();

          presetNameString = String((char*)presetNameBytes);
          presetScrollFlag = true;

          break;
        }
      case 'r':
        {
          byte presetNameBytes[8] = { 32, 32, 32, 32, 32, 32, 32, 32 };
          while (Serial2.available() < 1) {}
          presetNumber = Serial2.read();
          while (Serial2.available() < 1) {}

          Serial2.readBytes(presetNameBytes, 8);

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
    }
  }
}
