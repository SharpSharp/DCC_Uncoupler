// Kamiack LH module DCC magnets for module 1 - dcc addresses 23 24 27 28 30 31
// based on ant-003

const unsigned long magnetTime = 9000; // 1000 milli seconds = 1 second


// Accessory Decoder - www.dccinterface.com

#include <NmraDcc.h>
#include <Wire.h>

typedef struct
{
 int address;
 uint8_t arduinoPin;
 unsigned long offTime;
}
DCCAccessoryAddress;

// set number of DCC addresses
DCCAccessoryAddress gAddresses[6];

NmraDcc  Dcc;
uint16_t lastAddr = 0xFFFF;
uint8_t lastDirection = 0xFF;

//
// Decoder Init
//
void ConfigureDecoder()
{
 // DCC addresses corresponding to arduino pins
 gAddresses[0].address = 23;
 gAddresses[0].arduinoPin = 4;

 gAddresses[1].address = 27;
 gAddresses[1].arduinoPin = 5;

 gAddresses[2].address = 30;
 gAddresses[2].arduinoPin = 6;

 gAddresses[3].address = 31;
 gAddresses[3].arduinoPin = 7;

 gAddresses[4].address = 24;
 gAddresses[4].arduinoPin = 8;

 gAddresses[5].address = 28;
 gAddresses[5].arduinoPin = 9;

 // set the pin for output
 for (int i = 0; i<(int)(sizeof(gAddresses) / sizeof(gAddresses[0])); i++)
 {
   pinMode(gAddresses[i].arduinoPin, OUTPUT);
 }
}

// This function is called whenever a normal DCC Turnout Packet is received
void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower)
{
 for (int i = 0; i < (sizeof(gAddresses) / sizeof(DCCAccessoryAddress)); i++)
 {
   if (Addr == gAddresses[i].address && OutputPower)
   {
    // xx astAddr = Addr;
     // xx lastDirection = Direction;

      if (Direction)
     {
      digitalWrite(gAddresses[i].arduinoPin, HIGH);
      delay(magnetTime);
      digitalWrite(gAddresses[i].arduinoPin, LOW);
       break;
     }
         else
     {
      digitalWrite(gAddresses[i].arduinoPin, HIGH);
      delay(magnetTime);
      digitalWrite(gAddresses[i].arduinoPin, LOW);
       break;
     }
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
}