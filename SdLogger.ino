const int CHIP_SELECT = 4;

void initSd() {
  if (!SD.begin(CHIP_SELECT)) {
    Serial.println("Card initialization failed");
    stopForError();
  }
  Serial.println("SD card initialized");
}

String fileName = "";
int fileCount = 0;

void newFile() {
  while (SD.exists(getFileName(fileCount))) {
    fileCount++;
  }
  fileName = getFileName(fileCount);
  File logFile = SD.open(fileName, FILE_WRITE);
  if (!logFile) {
    Serial.println("Error creating file");
    stopForError();
  }
}

String getFileName(int i){
  return "LOG_" + String(fileCount) + ".TXT";
}

void sdWrite(float data[][4], int len) {
  File logFile = SD.open(fileName, FILE_WRITE);
  if (logFile) {
    for (int i = 0; i < len; i++) {
      logFile.print(data[i][0], 3);
      logFile.print(", ");
      logFile.print(data[i][1], 1);
      logFile.print(", ");
      logFile.print(data[i][2], 1);
      logFile.print(", ");
      logFile.println(data[i][3], 1);
    }
    logFile.close();
  } else {
    Serial.println("Error opening file");
    stopForError();
  }
}

void stopForError() {
  digitalWrite(GREEN_PIN, LOW);
  while(1) {
    digitalWrite(RED_PIN, LOW);
    delay(500);
    digitalWrite(RED_PIN, HIGH);
    delay(500);
  }
}