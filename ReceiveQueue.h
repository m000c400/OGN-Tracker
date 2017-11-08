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

#ifndef ReceivEQUEUE_h
#define ReceivEQUEUE_h

#include <stdint.h>

#define QUEUESIZE 5

typedef struct
{
  uint32_t Address;
  uint32_t Latitude;
  uint32_t Longitude;
  uint32_t Altitude;
  uint32_t Heading;
} ReceivePacket;
  

/////////////////////////////////////////////////////////////////////
class ReceiveQueue 
{
  public:
    ReceiveQueue(void);
    void AddPacket(uint32_t *Data);
    uint8_t Available(void);
    void RemovePacket(void);
    float GetLatitude(void);
    float GetLongitude(void);
    uint32_t GetID(void);
    uint8_t GetType(void);
    uint32_t GetAltitude(void);
    uint16_t GetHeading(void);
  protected:
				
  private:
    uint8_t ReadPtr,WritePtr;
    ReceivePacket Queue[QUEUESIZE];        
};


#endif 

