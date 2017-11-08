/* 
 OGN Tracker Client>
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

#ifndef OGNRADIO_h
#define OGNRADIO_h

#include <stdint.h>
#include <Arduino.h>

#define F8682 2
#define F8684 4

#include <SPI.h>
#include "RFM69registers.h"

class OGNRadio 
{
  public:
    OGNRadio(void);
    void Initialise(uint8_t Power);
    void SendPacket(uint8_t *Packet, uint16_t Size, uint16_t Freq, uint8_t TxPower);
    
    void StartReceive(uint8_t Freq, uint8_t *Sync);
    uint8_t CheckReceive(void);
    void GetReceivePacket(uint8_t *Packet);
    void EndReceive(void);
      
  protected:
				
  private:
    void ClearIRQFlags(void);
    void SetTxPower(int8_t Power);
    void SetFrequency(uint8_t Freq);
    void WriteRegister(uint8_t Register, uint8_t Data);
    uint8_t ReadRegister(uint8_t Register);
};



#endif 

