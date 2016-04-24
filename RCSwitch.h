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
#ifndef _RCSwitch_h
#define _RCSwitch_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#elif defined(RPI) // Raspberry Pi
    #define RaspberryPi
    // PROGMEM och _P functions are for AVR based microprocessors,
    // so we must normalize these for the ARM processor:
    #define PROGMEM
    #define memcpy_P(dest, src, num) memcpy((dest), (src), (num))
    // Include libraries for RPi:
    #include <string.h> /* memcpy */
    #include <stdlib.h> /* abs */
    #include <stddef.h> /* NULL */
    #include <wiringPi.h>
    #include <stdint.h>
    #define CHANGE 1
    // The following typedefs are needed to be able to compile RCSwitch.cpp
    // with the RPi C++ compiler (g++)
    #ifdef __cplusplus
        extern "C"{
    #endif
        typedef uint8_t boolean;
        typedef uint8_t byte;
    #ifdef __cplusplus
    }
    #endif
    // Last line within Raspberry Pi block
#else
    #include "WProgram.h"
#endif

#if !defined(NULL)
    #define NULL 0
#endif

// Number of maximum High/Low changes per packet.
// We can handle up to (unsigned long) => 32 bit * 2 H/L changes per bit + 2 for sync
#define RCSWITCH_MAX_CHANGES 67

typedef struct RCSwitch_
{
    int nReceiverInterrupt;
    char nReceiverInterruptName[10];
    int nTransmitterPin;
    char nTransmitterPinName[10];
    int nPulseLength;
    int nRepeatTransmit;
    char nProtocol;

} RCSwitchT;

RCSwitchT* RCSwitchInit(void);

void enableReceive(RCSwitchT *RCSwitch, int interrupt, char *interruptName);
void disableReceive(RCSwitchT *RCSwitch);
bool available();
void resetAvailable();

unsigned long getReceivedValue();
unsigned int getReceivedBitlength();
unsigned int getReceivedDelay();
unsigned int getReceivedProtocol();
unsigned int* getReceivedRawdata();

#endif
