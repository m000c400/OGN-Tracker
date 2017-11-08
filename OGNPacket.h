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

#ifndef OGNPACKET_h
#define OGNPACKET_h

#include <stdint.h>
#include <arduino.h>

#include "TEA.h"
#include "ldpc.h"


#define BIT0 0x00000001
#define BIT1 0x00000002
#define BIT2 0x00000004
#define BIT3 0x00000008
#define BIT4 0x00000010
#define BIT5 0x00000020
#define BIT6 0x00000040
#define BIT7 0x00000080
#define BIT8 0x00000100
#define BIT9 0x00000200
#define BIT10 0x00000400
#define BIT11 0x00000800
#define BIT12 0x00001000
#define BIT13 0x00002000
#define BIT14 0x00004000
#define BIT15 0x00008000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000

#define OGNPACKETSIZE 32

/////////////////////////////////////////////////////////////////////
class OGNPacket 
{
  public:
    OGNPacket();
    void MakeSync(void);
    void WriteEC(uint8_t *EC);
    void MakeHeader(uint8_t Emergency, uint8_t Crypto, uint8_t Relay, uint8_t Meto, uint8_t Type, uint32_t Address);
    void MakeLatitude(uint8_t Quality, uint8_t UTCSecond, int32_t Latitude);
    void MakeLongitude(uint8_t Fix, uint8_t Baro, uint16_t DOP, int32_t Longitude);
    void MakeAltitude(uint16_t Rate, uint16_t Speed, uint16_t Altitude);
    void MakeHeading(uint8_t Type, uint8_t Private, int16_t Climb, uint16_t Heading);
    void PrintRawPacket(void);
    void PrintCodedPacket(void);
    void ManchesterEncodePacket(void);
    void ManchesterDecodePacket(void);
    void Whiten(void);
    void DeWhiten(void);
    void AddFEC(void);
    int8_t CheckFEC(void);
    
    
    uint8_t ManchesterPacket[2*OGNPACKETSIZE];
    uint8_t RawPacket[OGNPACKETSIZE];

    uint32_t *Sync;
    uint32_t *Payload;
    uint32_t *FEC;

  protected:
				
  private:
    const uint8_t DefaultSync[4] = { 0x0A, 0xF3, 0x65, 0x6C };
    
    void FixEndianess(uint16_t Index);
    uint8_t u8Count1s(uint8_t Byte);
    uint8_t ManchesterDecode(uint8_t InpByte);    
};


#endif 

