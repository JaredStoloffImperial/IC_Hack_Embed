// Released to the public domain
//
// This sketch will read an uploaded file and increment a counter file
// each time the sketch is booted.

// Be sure to install the Pico LittleFS Data Upload extension for the
// Arduino IDE from:
//    https://github.com/earlephilhower/arduino-pico-littlefs-plugin/
// The latest release is available from:
//    https://github.com/earlephilhower/arduino-pico-littlefs-plugin/releases

// Before running:
// 1) Select Tools->Flash Size->(some size with a FS/filesystem)
// 2) Use the Tools->Pico Sketch Data Upload tool to transfer the contents of
//    the sketch data directory to the Pico

#include <LittleFS.h>

bool waitResponse() {
  // Returns bool based on input y/n regardless of case
  bool confirmation = false;
  while (confirmation == false) {
    if (Serial.available()) {
      char choice = Serial.read();
      switch (choice) {
        case 'y':
        case 'Y':
          confirmation = true;
          return true;
          break;
        case 'n':
        case 'N':
          confirmation = true;
          return false;
          break;
        default:
          Serial.println("Invalid input. Enter 'y' or 'n'.");
      }
    }
  }
  return false;
}

void getSerialString(char* buffer, size_t bufferSize) {
  bool confirmation = false;

  while (!confirmation) {
    if (Serial.available()) {
      String inputString = Serial.readString();
      inputString.trim();  // Remove leading/trailing spaces

      if (inputString.length() > 0) {  // Ensure there's actual input
        inputString.toCharArray(buffer, bufferSize);  // Convert String to char[]
        confirmation = true;
      } else {
        continue;
      }
    }
  }
}

void viewDirectory() {
  Serial.println("Opening and printing the root directory...");
  Dir dir = LittleFS.openDir("");
  // or Dir dir = LittleFS.openDir("/data");
  while (dir.next()) {
      Serial.print(dir.fileName());
      if(dir.fileSize()) {
          File f = dir.openFile("r");
          Serial.println(f.size());
      }
  }
}

void setup() {
  Serial.begin(115200);
  delay(5000);

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed!");
    return;
  }

  Serial.println("\nDo you want to print directory? Y/[n]");
  if (waitResponse()){
    viewDirectory();
  }
  else{
    Serial.println("Process cancelled.");
  }

  char buff[256];
  Serial.println("\nDo you want to write message? Y/[n]");
  if (waitResponse()){
    Serial.print("Enter message:");
    getSerialString(buff, sizeof(buff));
    // Now write the message into message.txt
    File f = LittleFS.open("/message.txt", "w");  // Create or overwrite file2.txt
    if (f) {
      f.write((uint8_t *)buff, strlen(buff)); // Write the sentence to the file
      f.close();  // Close the file
      Serial.println("Sentence written to message.txt");
    } else {
      Serial.println("Failed to open message.txt for writing.");
    }
  }else{
    Serial.println("Process cancelled.");
  }

  Serial.println("---------------");

  Serial.println("\nDo you want to display text from message.txt? Y/[n]");
  if (waitResponse()){
    // Optionally, read and display the contents of file2.txt
    File i = LittleFS.open("/message.txt", "r");
    if (i) {
      while (i.available()) {
        Serial.write(i.read());  // Read and write the content to Serial
      }
      Serial.println();
      Serial.println("---------------");
      i.close();
    } else {
      Serial.println("Failed to open message.txt.");
    }
  }else{
    Serial.println("Process cancelled.");
  }
  Serial.println("Setup complete.");
}

void loop() {
}
