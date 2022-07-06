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
#include <NmraDcc.h>

// define the push button pin numbers 
#define button1 10
#define button2 11
#define button3 12

const unsigned long magnetTime = 9000; // 1000 milli seconds = 1 second
const byte NUMBER_OF_UNCOUPLERS = 6;   // number of unciuplers specified to make changing easier

typedef struct
{
  int dccAddress;
  byte uncouplerPin;
  byte buttonPin;          // pin for optional push button
  bool uncoupling;         // is the uncoupler switched on
  unsigned long offTime;   // time when output will be set to LOW

  void setup()
  {
    pinMode(uncouplerPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);  // pin for push button with pullup resistor enabled
  }

  void buttonPressed()
  {
    if (digitalRead(buttonPin) == LOW) on();  // pullup keeps the input HIGH when not pressed
  }
 
  void on()
  {
    uncoupling = true;                // change unclouping to on
    offTime = millis() + magnetTime;  // setup time for the switch off
    digitalWrite(uncouplerPin, HIGH);   // switch output on
  }

  void off()
  {
    if (uncoupling && millis() > offTime) { // is theuncoupler on and is it after the offTime
      uncoupling = false;                   // change uncoupling to off
      digitalWrite(uncouplerPin, LOW);        // switch output off
   }
  }
}
DCCAccessoryAddress;

// set number of DCC address, uncoupling pins and optional push button pins
DCCAccessoryAddress uncoupler[NUMBER_OF_UNCOUPLERS] = {
  {23, 4, button1},
  {27, 5, button2},
  {30, 6},
  {31, 7, button3},
  {24, 8},
  {28, 9}
};

NmraDcc  Dcc;

// This function is called whenever a normal DCC Turnout Packet is received
// It returns the Acc Address (1 to 2044) 
// The direction 0 (close) or 1 (throw)
// The output Power true  (is it on)
void notifyDccAccTurnoutOutput(uint16_t receivedAddress, uint8_t direction, uint8_t outputPower)
{
  for (int i = 0; i < NUMBER_OF_UNCOUPLERS; i++)
  {
    if (receivedAddress == uncoupler[i].dccAddress)
    {
      uncoupler[i].on();
      break;
    }
  }
}

void setup()
{
  // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up 
  Dcc.pin(0, 2, 1);
  // Call the main DCC Init function to enable the DCC Receiver
  Dcc.init(MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0);

  for (int i = 0; i<NUMBER_OF_UNCOUPLERS; i++) 
  {
    uncoupler[i].setup();
  }
}

void loop()
{
  Dcc.process();

// loop through the uncouplers
  for (int i = 0; i < NUMBER_OF_UNCOUPLERS; i++)  {
    uncoupler[i].buttonPressed();   // check for a pressed button
    uncoupler[i].off();             // check to see if it's time to switch an uncoupler off.
  }
}
