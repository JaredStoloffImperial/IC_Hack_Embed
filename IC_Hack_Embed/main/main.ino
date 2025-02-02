#include "ICHack.h"
#include <LittleFS.h>

// Create the frame buffer. The first 24 bits of each uint32_t
// represent one column of the LED matrix. You can write to this
// buffer directly and patters will appear on the display.
volatile uint32_t fb[FRAME_BUFFER_WIDTH];
bool writeFile = false;
bool skipDisp = false;
const int interruptPin = 0;
const int skipPin = 3;
char message[256];


void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    // Serial.println("LittleFS mount failed!");
    return;
  }


  // test open message file or write default message
  
  File i = LittleFS.open("/message.txt", "r");
  if (!i) { // If file does not exist
    char default_message[256] = "Input Big5 report data";
    strncpy(message, default_message, sizeof(message) - 1);
    message[sizeof(message) - 1] = '\0';

    // Create new default file
    File f = LittleFS.open("/message.txt", "w"); 
    if (f) {
      f.write((uint8_t *)default_message, strlen(default_message)); // Write the sentence to the file
      f.flush();  // Ensure data is fully written before closing
      f.close();  // Close the file
    } 
  } 
  else { // If file does exist
    int index = 0;
    while (i.available() && index < sizeof(message) - 1) {
      message[index++] = i.read();
    }
    message[index] = '\0';  // Ensure null termination
    i.flush();  // Ensure data is fully written before closing
    i.close();
  }

  // interrupt for switch Y
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), yButton, RISING);

  // interrupt for switch A
  pinMode(skipPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(skipPin), aButton, RISING);

   // Initialise the display by passing it the frame buffer. This
  // will create a thread that continuously renders the frame
  // buffer to the matrix.
  display_init(fb);
  delay(1000);

  // Scroll the text "HELLO WORLD" down the display
  char text[] = "Initialized. Press Y to connect.";
  for (uint8_t i = 0; i<strlen(text)*6; i++) {
    display_clear();
    display_draw_string(text, strlen(text), i);
    delay(100);
    if (skipDisp) {
      break;
    }
  }

}

void loop() {
  if (!writeFile){
    // Display from message.txt
    for (uint8_t i = 0; i<strlen(message)*6 ; i++) {
      display_clear();
      display_draw_string(message, strlen(message), i);
      delay(100);
      if (writeFile) {
        break;
      }
    }
    delay(1000);
    } else{ 
    // writeFile==True
    char text[] = "Connect";
    char buff[256];
    for (uint8_t i = 0; i < 100; i++) {
      display_clear();
      display_draw_string(text, 11, i);
      delay(100);
    }
    getSerialString(buff, sizeof(buff));
    
    strncpy(message, buff, sizeof(message) - 1);
    message[sizeof(message) - 1] = '\0';

    File f = LittleFS.open("/message.txt", "w");  // Create or overwrite file2.txt
    if (f) {
      f.write((uint8_t *)buff, strlen(buff)); // Write the sentence to the file
      f.flush();  // Ensure data is fully written before closing
      f.close();  // Close the file
    } 
    // else {
    //   Serial.println("Failed to open message.txt for writing.");
    // }
  writeFile = false; // reset to normal mode
  }
  
}

void yButton(){
  writeFile = true;
}

void aButton(){
  skipDisp = true;
}

void getSerialString(char* buffer, size_t bufferSize) {
  bool confirmation = false;
  while (!confirmation) {
    if (Serial.available()) {
      String inputString = Serial.readString();
      inputString.trim();  // Remove leading/trailing spaces

      if (inputString.length() > 0) {  // Ensure there's actual input
        inputString.toCharArray(buffer, bufferSize - 1);   // Convert String to char[]
        buffer[bufferSize - 1] = '\0'; 
        confirmation = true;
      } else {
        continue;
      }
    }
  }
}
