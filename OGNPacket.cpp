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

#include "OGNPacket.h"

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

OGNPacket::OGNPacket()
{
  int16_t i;
  for(i=0;i<OGNPACKETSIZE;i++)
  {
    RawPacket[i] = 0x00;
  }
  
  Sync = (uint32_t *)RawPacket;
  Payload = (uint32_t *)&RawPacket[0x04];
  FEC = (uint32_t *)&RawPacket[0x18];
}

void OGNPacket::MakeSync(void)
{
  int16_t i;
  uint8_t *sync;
  
  sync = (uint8_t *) Sync;
  
  for(i=0;i<4;i++)
    sync[i] = DefaultSync[i];
}

void OGNPacket::WriteEC(uint8_t *PC)
{
  int16_t i;
  for(i=0;i<8;i++)
    Sync[i] = PC[i];
    
  //FixEndianess(0x18);
}

void OGNPacket::MakeHeader(uint8_t Emergency, uint8_t Crypto, uint8_t Relay, uint8_t Meto, uint8_t Type, uint32_t Address)
{
  uint32_t *Header;
  uint32_t ulRelay = 0L;
  uint32_t ulType = 0L;
  uint8_t i;
  uint8_t ParityCount = 0;
  
  Header = Payload;
  
  ulRelay = Relay; ulType = Type;
  
  ulRelay = (ulRelay & 0x3)<<28;
  ulType = (ulType & 0x3)<<24;
  
  *Header = Address & 0x0FFFFFF;
  
  for(i=0;i<4;i++)
    ParityCount += u8Count1s(Header[i]);
    
  if((ParityCount % 2)!=1) 
     *Header = *Header | BIT28;

  if(Emergency) *Header = *Header | BIT31;
  if(Crypto) *Header = *Header | BIT30;
  if(Meto) *Header = *Header | BIT27;

  *Header = *Header | ulRelay | ulType;
  
  //FixEndianess(0x04);
}  

void OGNPacket::MakeLatitude(uint8_t Quality, uint8_t UTCSecond, int32_t Latitude)
{
  uint32_t *PayloadWord;
  int32_t lQuality = 0;
  int32_t lSecond = 0;
  
  PayloadWord = (uint32_t *)&Payload[1];
  
  *PayloadWord = 0;
  
  *PayloadWord = Latitude & 0x00FFFFFF;
  
  lQuality = Quality; lSecond = UTCSecond;
  
  lQuality = (lQuality & 0x00000003)<<30;
  lSecond =  (lSecond &  0x0000003F)<<24;
  
  *PayloadWord = *PayloadWord | lQuality  | lSecond;
  
  //FixEndianess(0x08);
}  

void OGNPacket::MakeLongitude(uint8_t Fix, uint8_t Baro, uint16_t DOP, int32_t Longitude)
{
  int32_t *PayloadWord;
  int32_t lDOP = 0;

  PayloadWord = (int32_t *)&Payload[2];
  
  lDOP = DOP;
  lDOP = (lDOP & 0x3F)<<24;
  
  *PayloadWord = Longitude & 0x0FFFFFF;

  if(Fix) *PayloadWord = *PayloadWord | BIT31;
  if(Baro) *PayloadWord = *PayloadWord | BIT30;

  *PayloadWord = *PayloadWord | lDOP;
  
  //FixEndianess(0x0C);
}  
 
void OGNPacket::MakeAltitude(uint16_t Rate, uint16_t Speed, uint16_t Altitude)
{
  uint32_t *PayloadWord;
  uint32_t ulRate = 0;
  uint32_t ulSpeed = 0;
  uint32_t ulAltitude = 0;

  PayloadWord = (uint32_t *)&Payload[3];
  
  *PayloadWord = 0;
  
  ulRate = Rate; ulSpeed = Speed; ulAltitude = Altitude;
  
  ulRate = (ulRate & 0x0F)<<24;
  ulSpeed = (ulSpeed & 0x3F)<<18;
  ulAltitude = (ulAltitude & 0x3FFF);
  
  *PayloadWord = ulRate | ulSpeed | ulAltitude;
  
  //FixEndianess(0x10);
}  

void OGNPacket::MakeHeading(uint8_t Type, uint8_t Private, int16_t Climb, uint16_t Heading)
{
  uint32_t *PayloadWord;
  int32_t lClimb = 0;
  uint32_t ulType = 0;

  PayloadWord = (uint32_t *)&Payload[4];
  
  *PayloadWord = 0;
  
  lClimb = Climb; ulType = Type;
  
  *PayloadWord = Heading & 0x3FF;
  
  lClimb = (lClimb & 0x1FF)<<10;
  ulType = (ulType & 0x0F)<<20;
  
  *PayloadWord = *PayloadWord | lClimb | ulType;
  if(Private) *PayloadWord = *PayloadWord | BIT20;
  
  //FixEndianess(0x14);
}  

void print_hex(uint8_t i)
{
    if(i<0x10)
      Serial.print("0");
    else
      Serial.print(i>>4,HEX);
    
    if(i==0)
      Serial.print("0");
    else
      Serial.print(i&0x0f,HEX);
}
    


void OGNPacket::PrintRawPacket(void)
{
  int16_t i;
  
  for(i=0;i<OGNPACKETSIZE;i++)
  {
    print_hex(RawPacket[i]); 
  }
  Serial.println();
}

void OGNPacket::FixEndianess(uint16_t Index)
{
  uint8_t temp;
  return;
  temp = RawPacket[Index];
  RawPacket[Index] = RawPacket[Index+3];
  RawPacket[Index+3] = temp;
  temp = RawPacket[Index+1];
  RawPacket[Index+1] = RawPacket[Index+2];
  RawPacket[Index+2] = temp;
}


void OGNPacket::Whiten(void)
{
  TEA Coder;
  
  Coder.TEAEncrypt(Payload+1,8);
  Coder.TEAEncrypt(Payload+3,8);
}


void OGNPacket::DeWhiten(void)
{
  TEA Coder;
  
  Coder.TEADecrypt(Payload+1,8);
  Coder.TEADecrypt(Payload+3,8);
  
}

void OGNPacket::AddFEC(void)
{
  LDPC Coder;
  
  Coder.LDPC_EncodeBlock(Payload, FEC);
}

int8_t OGNPacket::CheckFEC(void)
{
  int8_t result;
  LDPC Coder;
  
  result = Coder.LDPC_CheckBlock(Payload);
  
  return result;
}

const uint8_t ManchesterEncodeTable[0x10] =  // lookup table for 4-bit nibbles for quick Manchester encoding
{
   0xAA, // hex: 0, bin: 0000, manch: 10101010
   0xA9, // hex: 1, bin: 0001, manch: 10101001
   0xA6, // hex: 2, bin: 0010, manch: 10100110
   0xA5, // hex: 3, bin: 0011, manch: 10100101
   0x9A, // hex: 4, bin: 0100, manch: 10011010
   0x99, // hex: 5, bin: 0101, manch: 10011001
   0x96, // hex: 6, bin: 0110, manch: 10010110
   0x95, // hex: 7, bin: 0111, manch: 10010101
   0x6A, // hex: 8, bin: 1000, manch: 01101010
   0x69, // hex: 9, bin: 1001, manch: 01101001
   0x66, // hex: A, bin: 1010, manch: 01100110
   0x65, // hex: B, bin: 1011, manch: 01100101
   0x5A, // hex: C, bin: 1100, manch: 01011010
   0x59, // hex: D, bin: 1101, manch: 01011001
   0x56, // hex: E, bin: 1110, manch: 01010110
   0x55  // hex: F, bin: 1111, manch: 01010101
};

void OGNPacket::ManchesterEncodePacket(void)
{
  uint8_t i;
  
  for(i=0;i<OGNPACKETSIZE;i++)
  {
     ManchesterPacket[2*i] =  ManchesterEncodeTable [((RawPacket[i]&0xf0)>>4)];
     ManchesterPacket[(2*i) + 1] = ManchesterEncodeTable [(RawPacket[i]&0x0f)];
  }
}

uint8_t OGNPacket::ManchesterDecode(uint8_t InpByte)
{
  switch(InpByte)
  {
    case 0xAA : return 0x00;
    case 0xA9 : return 0x01;
    case 0xA6 : return 0x02;
    case 0xA5 : return 0x03;
    case 0x9A : return 0x04; 
    case 0x99 : return 0x05;
    case 0x96 : return 0x06;
    case 0x95 : return 0x07;
    case 0x6A : return 0x08;
    case 0x69 : return 0x09;
    case 0x66 : return 0x0A;
    case 0x65 : return 0x0B;
    case 0x5A : return 0x0C;
    case 0x59 : return 0x0D;
    case 0x56 : return 0x0E;
    case 0x55 : return 0x0F;
    default : return 0;
  }
}

    
  

void OGNPacket::ManchesterDecodePacket(void)
{
  uint8_t i;
  uint8_t Upper,Lower;
  
  for(i=0;i<OGNPACKETSIZE;i++)
  {
    Upper = ManchesterDecode(ManchesterPacket[2*i]);
    Lower = ManchesterDecode(ManchesterPacket[(2*i) + 1]);
    RawPacket[i] = (Upper << 4) + Lower;
  }
}

uint8_t OGNPacket::u8Count1s(uint8_t Byte)
{
  uint8_t Count = 0;
  uint8_t i;
  
  for(i=0;i<8;i++)
  {
    if( (Byte & 0x01) == 0x01)
     Count ++;
  }
  return Count;
}

