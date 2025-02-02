#include "ICHack.h"
#include <LittleFS.h>

// Create the frame buffer. The first 24 bits of each uint32_t
// represent one column of the LED matrix. You can write to this
// buffer directly and patters will appear on the display.
volatile uint32_t fb[FRAME_BUFFER_WIDTH];
bool writeFile = false;
const int interruptPin = 0;


void setup() {
  Serial.begin(115200);
  delay(5000);

  // // Initialize LittleFS
  // if (!LittleFS.begin()) {
  //   // Serial.println("LittleFS mount failed!");
  //   return;
  // }


  // test open message file or write default message
  int index = 0;
  File i = LittleFS.open("/message.txt", "r");
  if (!i) {
    char default_message[256] = "Input Big5 report data";
    // Create new default file
    File f = LittleFS.open("/message.txt", "w"); 
    if (f) {
      f.write((uint8_t *)default_message, strlen(default_message)); // Write the sentence to the file
      f.close();  // Close the file
    } 
    // else {
    //   Serial.println("Failed to open message.txt for writing.");
    // }
  } 
  i.close();

  // interrupt for switch y
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), yButton, RISING);

   // Initialise the display by passing it the frame buffer. This
  // will create a thread that continuously renders the frame
  // buffer to the matrix.
  display_init(fb);
  delay(1000);

  // Scroll the text "HELLO WORLD" down the display
  char text[] = "Initialize Complete";
  for (uint8_t i = 0; i<strlen(text)+100; i++) {
    display_clear();
    display_draw_string(text, strlen(text), i);
    delay(100);
  }

}

void loop() {
  if (!writeFile){
    // Open message file or write default message
    int index = 0;
    char message[256];
    File i = LittleFS.open("/message.txt", "r");
    if (i) {
      while (i.available()) {
        message[index++] = i.read();  // Read character-by-character
      }
      Serial.println(message);
      i.close();
    } 
    // else {
    //   Serial.println("Failed to open message.txt.");
    // }

    // Display from message.txt
    for (uint8_t i = 0; i<strlen(message)+100 ; i++) {
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
    char message[] = "Connect";
    char buff[256];
    for (uint8_t i = 0; i < 100; i++) {
      display_clear();
      display_draw_string(message, 11, i);
      delay(100);
    }
    getSerialString(buff, sizeof(buff));
    Serial.println(buff);
    File f = LittleFS.open("/message.txt", "w");  // Create or overwrite file2.txt
    if (f) {
      f.write((uint8_t *)buff, strlen(buff)); // Write the sentence to the file
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
