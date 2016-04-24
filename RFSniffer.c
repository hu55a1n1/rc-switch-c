/*
---------------------------------------------------------------------
  RFSniffer: RCSwitche's sample code.
  Modified by Sufi Al Hussaini to work with rc-switch-c
---------------------------------------------------------------------

  RFSniffer
  
  Hacked from http://code.google.com/p/rc-switch/
  
  by @justy to provide a handy RF code sniffer
*/

#include "RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


RCSwitchT *mySwitch;

// void handleInterrupt() {
//   printf("Interrupted!\n");
// }

int main(int argc, char *argv[]) {

  // This pin is not the first pin on the RPi GPIO header!
  // Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
  // for more information.
  int PIN = 64;
  char PIN_NAME[10] = {};

  strcpy(PIN_NAME, "_pi15");

  if(wiringOliSetup() == -1)
   return 0;

  mySwitch = RCSwitchInit();

  if(!mySwitch) {
    printf("mySwitch failed!\n");
    return -1;
  }

  enableReceive(mySwitch, PIN, PIN_NAME);  // Receiver on inerrupt 0 => that is pin #2

  while(1) 
  {
    if (available(mySwitch)) 
    {
      int value = getReceivedValue(mySwitch);

      if (value == 0) {
        printf("Unknown encoding");
      } else {    

        printf("Received %i\n", getReceivedValue(mySwitch) );
      }

      resetAvailable(mySwitch);
    }
  }

  return 0;
}

