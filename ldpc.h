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

#ifndef __LDPC_H__
#define __LDPC_H__

#include <stdint.h>
#include <stdlib.h>


class LDPC
{
  public:
    LDPC();
    void LDPC_Encode(const uint32_t *Data, uint32_t *Parity, const uint32_t ParityGen[48][5]);
    void LDPC_EncodeBlock(const uint32_t *Data, uint32_t *Parity);
    
    int8_t LDPC_Check(const uint8_t *Data);
    int8_t LDPC_CheckBlock(const uint32_t *Packet);
    

    
  protected:
    uint8_t u8Count1s(uint8_t Byte);
    uint8_t u32Count1s(uint32_t uWord);

};

#endif // of __LDPC_H__
