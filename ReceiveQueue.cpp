/* 
    OGN Tracker Client
    Copyright (C) <2015>  <Mike Roberts>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ReceiveQueue.h"

#include <Arduino.h>

/* OGN Packet Map
[00:03] [SSSS SSSS] [SSSS SSSS] [SSSS SSSS] [SSSS SSSS]  Sync Field 4 Bytes 0x0AF3656C
[04:07] [ECRR PMTT] [AAAA AAAA] [AAAA AAAA] [AAAA AAAA]  [E]mergency, [C]rypt, [R]elay Count, Even [P]arity, Address [T]ype [A]ddress 32 Bits
[08:0B] [QQTT TTTT] [LLLL LLLL] [LLLL LLLL] [LLLL LLLL]  [Q]uality, [T}ime, [L]atitude 
[0C:0F] [MBDD DDDD] [LLLL LLLL] [LLLL LLLL] [LLLL LLLL]  [F]ix, [B]aro, [D]OP, [L]ongitude
[10:13] [RRRR RRRR] [SSSS SSSS] [SSAA AAAA] [AAAA AAAA]  Turn [R]ate, [S]peed, [A]ltitude
[14:17] [TTTT TTTT] [AAAA PCCC] [CCCC CCDD] [DDDD DDDD]  [T]emperature, [A]ircraft Type, [P]rivate [C]limb, Hea[D]ing
[18:2B] [CCCC CCCC] [CCCC CCCC] [CCCC CCCC] [CCCC CCCC]  FEC Codes
[2C:2F] [CCCC CCCC] [CCCC CCCC] [CCCC CCCC] [CCCC CCCC]
*/

ReceiveQueue::ReceiveQueue(void)
{
    ReadPtr = 0; WritePtr = 0;
}

void ReceiveQueue::AddPacket(uint32_t *Data)
{
  if( ((WritePtr+1)%QUEUESIZE) == ReadPtr )
  {
    ReadPtr ++; ReadPtr = ReadPtr % QUEUESIZE;
  }
  memcpy(&Queue[WritePtr], Data, sizeof(ReceivePacket));
  WritePtr ++; WritePtr = WritePtr % QUEUESIZE;
  } 

uint8_t ReceiveQueue::Available(void)
{
  if(ReadPtr == WritePtr)
    return 0;
  else
    return 1;
}

void ReceiveQueue::RemovePacket(void)
{
  if(ReadPtr != WritePtr)
  {
    ReadPtr ++;  ReadPtr = ReadPtr % QUEUESIZE;
  }
}

float ReceiveQueue::GetLatitude(void)
{
  int32_t Latitude;
  float fLatitude;
  
  Latitude = Queue[ReadPtr].Latitude & 0x00FFFFFF;
  fLatitude = Latitude;
  
  fLatitude = fLatitude * 8 / 600000;
  
  return fLatitude;
}

float ReceiveQueue::GetLongitude(void)
{
  int32_t Longitude;
  float fLongitude;
  
  Longitude = Queue[ReadPtr].Longitude & 0x00FFFFFF;

  if(Longitude & 0x00800000) Longitude = Longitude | 0xFF000000;
  
  fLongitude = Longitude;
  
  fLongitude = fLongitude * 16 / 600000;
 
  return fLongitude;
}

uint32_t ReceiveQueue::GetID(void)
{
  uint32_t ID;
  
  ID = (Queue[ReadPtr].Address & 0x0FFFFFF);
  
  return ID;
}

uint8_t ReceiveQueue::GetType(void)
{
  uint32_t Type;
  
  Type = (Queue[ReadPtr].Heading >> 20)& 0x0F;
  
  return (uint8_t)Type;
}

uint32_t ReceiveQueue::GetAltitude(void)
{
  uint32_t Altitude;
  uint8_t Range;
  
  Altitude = Queue[ReadPtr].Altitude & 0x00000FFF;
  Range = (Queue[ReadPtr].Altitude>>12) & 0x00000003;
  switch(Range)
  {
    case 0 : return Altitude;
    case 1 : return 0x1000 + (Altitude * 2);
    case 2 : return 0x3000 + (Altitude * 4);
    default : return 0x7000 + (Altitude * 8);
  } 
}

uint16_t ReceiveQueue::GetHeading(void)
{
  float fHeading;
  
  fHeading = Queue[ReadPtr].Heading & 0x03FF;
  fHeading = fHeading / 28.4444;
  fHeading = fHeading + 0.5;
  
  return (uint16_t)fHeading;
}
