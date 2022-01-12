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

 void setup()
 {
  pinMode(arduinoPin, OUTPUT);
 }
 
 void on()
 {
  uncoupling = true;                // change unclouping to on
  offTime = millis() + magnetTime;  // setup time for the switch off
  digitalWrite(arduinoPin, HIGH);   // switch output on
 }


 void off()
 {
  if (uncoupling && millis() > offTime) { // is theuncoupler on and is it after the offTime
    uncoupling = false;                   // change uncoupling to off
    digitalWrite(arduinoPin, LOW);        // switch output off
  }
 }
}
DCCAccessoryAddress;

// set number of DCC addresses
DCCAccessoryAddress uncoupler[NUMBER_OF_UNCOUPLERS] = {
  {23, 4},
  {27, 5},
  {30, 6},
  {31, 7},
  {24, 8},
  {28, 9}
};

NmraDcc  Dcc;
uint16_t lastAddr = 0xFFFF;
uint8_t lastDirection = 0xFF;


// This function is called whenever a normal DCC Turnout Packet is received
void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower)
{
 for (int i = 0; i < NUMBER_OF_UNCOUPLERS; i++)
 {
   if (Addr == uncoupler[i].address && OutputPower)
   {
      uncoupler[i].on();
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

}

void setup()
{
 setupDCCDecoder();
 for (int i = 0; i<NUMBER_OF_UNCOUPLERS; i++) {
  uncoupler[i].setup();
 }
}

void loop()
{
 // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function
 // for correct library operation
 Dcc.process();

// check to see if it's time to switch an uncoupler off.
  for (int i = 0; i < NUMBER_OF_UNCOUPLERS; i++)  {
    uncoupler[i].off();
  }
}
