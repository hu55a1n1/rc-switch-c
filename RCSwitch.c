/*
---------------------------------------------------------------------
  RFReader: Pure c library for reading RF remotes.
  Modified by Sufi Al Hussaini

  Essentially a c version of the original RCSwitch arduino library. 
  All credit to those guys.
---------------------------------------------------------------------

  RCSwitch - Arduino libary for remote control outlet switches
  Copyright (c) 2011 Suat Özgür.  All right reserved.
  
  Contributors:
  - Andre Koehler / info(at)tomate-online(dot)de
  - Gordeev Andrey Vladimirovich / gordeev(at)openpyro(dot)com
  - Skineffect / http://forum.ardumote.com/viewtopic.php?f=2&t=48
  
  Project home: http://code.google.com/p/rc-switch/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "RCSwitch.h"
// #include <stdio.h>
#include <string.h>
#include <stdlib.h>
  
static void handleInterrupt();
static bool receiveProtocol1(unsigned int changeCount);
static bool receiveProtocol2(unsigned int changeCount);
// static void setReceiveTolerance(int nPercent);

static unsigned long nReceivedValue = 0;
static unsigned int nReceivedBitlength = 0;
static unsigned int nReceivedDelay = 0;
static unsigned int nReceivedProtocol = 0;
static unsigned int timings[RCSWITCH_MAX_CHANGES];
static int nReceiveTolerance = 60;

RCSwitchT* RCSwitchInit() {
  RCSwitchT *RCSwitch = malloc(sizeof(RCSwitchT));

  RCSwitch->nReceiverInterrupt = -1;
  strcpy(RCSwitch->nReceiverInterruptName, "");
  RCSwitch->nTransmitterPin = -1;
  strcpy(RCSwitch->nTransmitterPinName, "");
  nReceivedValue = 0;
  // setReceiveTolerance((int)60);

  return RCSwitch;
}

// /**
//  * Set Receiving Tolerance
//  */
// void setReceiveTolerance(int nPercent) {
//   nReceiveTolerance = nPercent;
// }

void enableReceive(RCSwitchT *RCSwitch, int interrupt, char *interruptName) {
  RCSwitch->nReceiverInterrupt = interrupt;
  // gpio_export(this->nReceiverInterrupt);

  strcpy(RCSwitch->nReceiverInterruptName, interruptName);
  // printf("Pin Name = %s\n", RCSwitch->nReceiverInterruptName);
  // _enableReceive(RCSwitch);
  if (RCSwitch->nReceiverInterrupt != -1) {
    nReceivedValue = 0;
    nReceivedBitlength = 0;

    // Ideally wiringPiISR() must be passed RCSwitch->nReceiverInterruptName instead of interruptName
    // But that's causing a problem!
    // printf("Pin Name = %s\n", RCSwitch->nReceiverInterruptName);
#if defined(RaspberryPi) // Raspberry Pi
    wiringPiISR(this->nReceiverInterrupt, INT_EDGE_BOTH, &handleInterrupt);
#else // Arduino
    attachInterrupt(this->nReceiverInterrupt, handleInterrupt, CHANGE);
#endif
  }
}

/**
 * Disable receiving data
 */
void disableReceive(RCSwitchT *RCSwitch) {
#if not defined(RaspberryPi) // Arduino
  detachInterrupt(this->nReceiverInterrupt);
#endif // For Raspberry Pi (wiringPi) you can't unregister the ISR
  RCSwitch->nReceiverInterrupt = -1;
}

bool available() {
  if(nReceivedValue)
    return 1;

  return 0;
}

void resetAvailable() {
  nReceivedValue = 0;
}

unsigned long getReceivedValue() {
  return nReceivedValue;
}

unsigned int getReceivedBitlength() {
  return nReceivedBitlength;
}

unsigned int getReceivedDelay() {
  return nReceivedDelay;
}

unsigned int getReceivedProtocol() {
  return nReceivedProtocol;
}

unsigned int* getReceivedRawdata() {
  return timings;
}

bool receiveProtocol1(unsigned int changeCount){
    
	  unsigned long code = 0;
      unsigned long delay = timings[0] / 31;
      unsigned long delayTolerance = delay * nReceiveTolerance * 0.01;    
      int i;

      for (i = 1; i<changeCount ; i=i+2) {
      
          if (timings[i] > delay-delayTolerance && timings[i] < delay+delayTolerance && timings[i+1] > delay*3-delayTolerance && timings[i+1] < delay*3+delayTolerance) {
            code = code << 1;
          } else if (timings[i] > delay*3-delayTolerance && timings[i] < delay*3+delayTolerance && timings[i+1] > delay-delayTolerance && timings[i+1] < delay+delayTolerance) {
            code+=1;
            code = code << 1;
          } else {
            // Failed
            i = changeCount;
            code = 0;
          }
      }      
      code = code >> 1;
    if (changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
      nReceivedValue = code;
      nReceivedBitlength = changeCount / 2;
      nReceivedDelay = delay;
	  nReceivedProtocol = 1;
    }

	if (code == 0){
		return false;
	}  else {
		return true;
	}
}

bool receiveProtocol2(unsigned int changeCount){
    
	  unsigned long code = 0;
      unsigned long delay = timings[0] / 10;
      unsigned long delayTolerance = delay * nReceiveTolerance * 0.01;    
      int i;
      for (i = 1; i<changeCount ; i=i+2) {
      
          if (timings[i] > delay-delayTolerance && timings[i] < delay+delayTolerance && timings[i+1] > delay*2-delayTolerance && timings[i+1] < delay*2+delayTolerance) {
            code = code << 1;
          } else if (timings[i] > delay*2-delayTolerance && timings[i] < delay*2+delayTolerance && timings[i+1] > delay-delayTolerance && timings[i+1] < delay+delayTolerance) {
            code+=1;
            code = code << 1;
          } else {
            // Failed
            i = changeCount;
            code = 0;
          }
      }      
      code = code >> 1;
    if (changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
      nReceivedValue = code;
      nReceivedBitlength = changeCount / 2;
      nReceivedDelay = delay;
	  nReceivedProtocol = 2;
    }

	if (code == 0){
		return false;
	}  else {
		return true;
	}
}

void handleInterrupt() 
{
  static unsigned int duration;
  static unsigned int changeCount;
  static unsigned long lastTime;
  static unsigned int repeatCount;
  long time = micros();
  duration = time - lastTime;

  if (duration > 5000 && duration > timings[0] - 200 && duration < timings[0] + 200) {    
    repeatCount++;
    changeCount--;

    if (repeatCount == 2) {
                if (receiveProtocol1(changeCount) == false){
                        if (receiveProtocol2(changeCount) == false){
                                //failed
                        }
                }
      repeatCount = 0;
    }
    changeCount = 0;
  } else if (duration > 5000) {
    changeCount = 0;
  }

  if (changeCount >= RCSWITCH_MAX_CHANGES) {
    changeCount = 0;
    repeatCount = 0;
  }
  timings[changeCount++] = duration;
  lastTime = time;  
}
