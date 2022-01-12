// Kamiack LH module DCC magnets for module 1 - dcc addresses 23 24 27 28 30 31
// based on ant-003

/*  This sketch uses millis() rather than delay.
 *  millis() returns a unsigned long value that is equal to the time in milli-seconds
 *  that the Arduino has been switched on. Basically it is a counter that starts at zero
 *  which increments by 1 every milli-second. A delay is initiated by added millis() to magnetTime
 *  and storing that in offTime. offtime is then tested against millis(). When offTime is less
 *  than millis() the output is switched off.
 *  
 */  

const unsigned long magnetTime = 9000; // 1000 milli seconds = 1 second
const byte NUMBER_OF_UNCOUPLERS = 6;   // number of unciuplers specified to make changing easier

// Accessory Decoder - www.dccinterface.com

#include <NmraDcc.h>
#include <Wire.h>

typedef struct
{
 int address;
 uint8_t arduinoPin;
 bool uncoupling;         // is the uncoupler switched on
 unsigned long offTime;   // time when output will be set to LOW
}
DCCAccessoryAddress;

// set number of DCC addresses
DCCAccessoryAddress uncoupler[NUMBER_OF_UNCOUPLERS];

NmraDcc  Dcc;
uint16_t lastAddr = 0xFFFF;
uint8_t lastDirection = 0xFF;

//
// Decoder Init
//
void ConfigureDecoder()
{
 // DCC addresses corresponding to arduino pins
 uncoupler[0].address = 23;
 uncoupler[0].arduinoPin = 4;

 uncoupler[1].address = 27;
 uncoupler[1].arduinoPin = 5;

 uncoupler[2].address = 30;
 uncoupler[2].arduinoPin = 6;

 uncoupler[3].address = 31;
 uncoupler[3].arduinoPin = 7;

 uncoupler[4].address = 24;
 uncoupler[4].arduinoPin = 8;

 uncoupler[5].address = 28;
 uncoupler[5].arduinoPin = 9;

 // set the pin for output
 for (int i = 0; i<NUMBER_OF_UNCOUPLERS; i++)
 {
   pinMode(uncoupler[i].arduinoPin, OUTPUT);
 }
}

// This function is called whenever a normal DCC Turnout Packet is received
void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower)
{
 for (int i = 0; i < NUMBER_OF_UNCOUPLERS; i++)
 {
   if (Addr == uncoupler[i].address && OutputPower)
   {
      digitalWrite(uncoupler[i].arduinoPin, HIGH);
      uncoupler[i].offTime = millis() + magnetTime;    // Set the time the magnet will switch off
      uncoupler[i].uncoupling = true;
      break;
    }
 }
}

void setupDCCDecoder()
{
 // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up 
 Dcc.pin(0, 2, 1);

 // Call the main DCC Init function to enable the DCC Receiver
 Dcc.init(MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0);

 // Configure the Decoder
 ConfigureDecoder();
}

void setup()
{
 setupDCCDecoder();
}

void loop()
{
 // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function
 // for correct library operation
 Dcc.process();

  //  Test all addresses.
  //  If the uncoupler is on, test the offTime to see if it is after that time.
  //  When it is, switch magnet off.
  for (int i = 0; i < NUMBER_OF_UNCOUPLERS; i++)  {
    if (uncoupler[i].uncoupling){
      if (millis() > uncoupler[i].offTime) {
        digitalWrite(uncoupler[i].arduinoPin, LOW);
      }
    }
  }
}
