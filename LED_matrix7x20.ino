
#include "OctoWS2811.h"
#include "font.h"
#include <SPI.h>
//#include <Ethernet.h>
#include <UIPEthernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

IPAddress myIP(192,168,0,6);

// telnet defaults to port 23
EthernetServer server(23);

EthernetClient clients[4];

//Character ring buffer
const uint8_t c_buffer_size = 0xFF; // Size
uint8_t c_buffer_toprint = 0;       // read counter
uint8_t c_buffer_wcount = 0;        // write counter
char c_buffer[c_buffer_size+1];     // buffer

// The display size and color to use
const unsigned int width = 20;
const unsigned int height = 8;

// Invert the display
const byte invert_display = 0;
// Collor
uint32_t fg_col = 0xffffff;
uint32_t bg_col = 0x030303;

const uint32_t time_delay = 1000;
uint32_t time_next_update = time_delay;

// These parameters control the fire appearance
// (try controlling these with knobs / analogRead....)
//unsigned int heat = width / 5;
//unsigned int focus = 9;
//unsigned int cool = 26;

// Arrays for fire animation
unsigned char canvas[width*height];
extern const unsigned int fireColor[100];

char text[] = "Test";

// OctoWS2811 objects
const int ledsPerPin = width * height / 8;
DMAMEM int displayMemory[ledsPerPin*6];
int drawingMemory[ledsPerPin*6];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerPin, displayMemory, drawingMemory, config);


void colorWipe(int color, int wait)
{
  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  }
}

// Run setup once
void setup() {
  colorWipe(0x0000ff, 10);
  //Initialice first c_buffer to seperatio char
  c_buffer[c_buffer_wcount++] = 0xff;
  c_buffer_toprint++;
  c_buffer[c_buffer_wcount++] = 'R';
  c_buffer_toprint++;
  c_buffer[c_buffer_wcount++] = 'e';
  c_buffer_toprint++;
  c_buffer[c_buffer_wcount++] = 'a';
  c_buffer_toprint++;
  c_buffer[c_buffer_wcount++] = 'l';
  c_buffer_toprint++;
  c_buffer[c_buffer_wcount++] = 'r';
  c_buffer_toprint++;
  c_buffer[c_buffer_wcount++] = 'a';
  c_buffer_toprint++;
  c_buffer[c_buffer_wcount++] = 'u';
  c_buffer_toprint++;
  c_buffer[c_buffer_wcount++] = 'm';
  c_buffer_toprint++;
  c_buffer[c_buffer_wcount++] = 0xff;
  c_buffer_toprint++;

  
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Ethernet.begin(mac, myIP);
  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
//  if (Ethernet.begin(mac) == 0) {
//    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
//    for (;;)
//      ;
//  }
  // start listening for clients
  server.begin();
  // print your local IP address:
  Serial.print("Chat server address:");
  Serial.println(Ethernet.localIP());
  
  Serial.println("");
  Serial.println("Start LED Matrix 7x20");
  // turn on the display

/*
  for (int i=0; i<4; i++){
    uint8_t abc = uint8_t(text[i]); //text[i];
    Serial.print  (i);
    Serial.print  (": ");
    Serial.print  (text[i]);
    Serial.print  ("|");
    Serial.println(abc);
    uint16_t x_ = i*5;
    for (x=0; x<5; x++){
      col = font[abc-32][x];
      for (y=0;y<8;y++){
        if ( col & (0x40 >> y) ) {
          Serial.print("X");
          leds.setPixel(xy( x+x_, y), fg_col);
        }
        else {
          Serial.print(" ");
          leds.setPixel(xy( x+x_, y), bg_col);
        }
      }
      
      Serial.print("...");
      Serial.print(col);
      Serial.println(" ");
    }
//    Serial.println("------------------------------------");
  }
*/
  leds.setPixel(xy( 0, 10), 0x0F0000);
  leds.setPixel(xy( 0, 1), 0x0F0000);
  leds.setPixel(xy( 1, 0), 0x00000F);
  leds.setPixel(xy( 1, 1), 0x000f00);
  leds.show();
}

void printIPAddress() {
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

// A simple xy() function to turn display matrix coordinates
// into the index numbers OctoWS2811 requires.  If your LEDs
// are arranged differently, edit this code...
unsigned int xy(unsigned int x, unsigned int y) {
  if ( invert_display == 1 ) {  
    return width*height - y*width - x;
  }
  else {
    return y * width + x;
  }
}

void printText() {  
  Serial.println("Enter print text");
  /*leds.begin();
  leds.show(); */
  uint32_t fg_col = 0x00ff00;
  uint32_t bg_col = 0x030303;
  uint8_t curent_char = 0;
  for (byte i=0; i<4; i++){
    curent_char = uint8_t(c_buffer[c_buffer_wcount-c_buffer_toprint+i]); //text[i];

    uint16_t x_off = i*5; // pixel offset
    Serial.println(curent_char);
    for (byte x=0; x<5; x++){
      byte col = font[curent_char][x];
      Serial.println(col);
      for (byte y=0;y<7;y++){
        Serial.print("coordinates: ");
        Serial.print(x);
        Serial.print(" ");
        Serial.print(y);
        Serial.print(" ");
        Serial.println(xy( x+x_off, y));
        if ( col & (0x04 >> y) ) {
          leds.setPixel(xy( x+x_off, y), fg_col);
        }
        else {
          leds.setPixel(xy( x+x_off, y), bg_col);
        }
      }
    }
  }
  
  Serial.println("Update leds");
  leds.show();
  c_buffer_toprint--;
} // end display refresh

void loop() {
////////////////////////////////////////////////////////////////////////////////////
// Telnet client:
  // wait for a new client:
  EthernetClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    boolean newClient = true;
    for (byte i=0; i < 4; i++) {
      //check whether this client refers to the same socket as one of the existing instances:
      if (clients[i] == client) {
        newClient = false;
        break;
      }
    }

    if (newClient) {
      //check which of the existing clients can be overridden:
      for (byte i=0; i < 4; i++) {
        if (!clients[i] && clients[i] != client) {
          clients[i] = client;
          // clear out the input buffer:
          client.flush();
          Serial.println("We have a new client");
          client.print("Hello, client number: ");
          client.print(i);
          client.println();
          break;
        }
      }
    }

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      char thisChar = client.read();
      if ( thisChar < 32 ) {
        if (c_buffer[c_buffer_wcount-1] != 0xff){
          c_buffer[c_buffer_wcount++] = 0x00; // add text seperator if no already
          c_buffer_toprint++;
          Serial.println("#");
        }
      }
      else {
        c_buffer[c_buffer_wcount++] = thisChar-32; // Offset of the font.
        c_buffer_toprint++;
        Serial.print(char(c_buffer[c_buffer_wcount-1]+32));
      }
    }
  }
  for (byte i=0; i < 4 ; i++) {
    if (!(clients[i].connected())) {
      // client.stop() invalidates the internal socket-descriptor, so next use of == will allways return false;
      clients[i].stop();
    }
  }

////////////////////////////////////////////////////////////////////////////////////
// Display part: 
//  Serial.print(time_next_update);
//  Serial.print(" ");
//  Serial.println(millis());
//  delay(500);

  if ( time_next_update < millis() && c_buffer_toprint) {
    Serial.println(c_buffer_toprint);
    time_next_update = millis() + time_delay;

 //   printText();
  }
  //Serial.print("x");
}  // End of void loop


