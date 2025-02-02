#include "ICHack.h"
#include <LittleFS.h>

// Create the frame buffer. The first 24 bits of each uint32_t
// represent one column of the LED matrix. You can write to this
// buffer directly and patters will appear on the display.
volatile uint32_t fb[FRAME_BUFFER_WIDTH];
char message[256];


void setup() {
  Serial.begin(115200);
  delay(5000);

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed!");
    return;
  }

  
  File i = LittleFS.open("/message.txt", "r");
  int index = 0;
  if (i) {
    while (i.available()) {
      message[index++] = i.read();  // Read character-by-character
    }
    i.close();
  } else {
    char default_message[23] = "Input Big5 report data";
    for (uint8_t i = 0; i<24 ; i++) {
      message[index++] = default_message[i];
    }
    Serial.println("Failed to open message.txt.");
  }

   // Initialise the display by passing it the frame buffer. This
  // will create a thread that continuously renders the frame
  // buffer to the matrix.
  display_init(fb);
  delay(1000);
}

void loop() {
  // // Scroll the text "HELLO WORLD" down the display
  // char message[] = "HELLO WORLD";
  for (uint8_t i = 0; i<256 ; i++) {
    display_clear();
    display_draw_string(message, 20, i);
    delay(100);
  }
  

  delay(1000);
}
