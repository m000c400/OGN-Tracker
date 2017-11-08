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

#include "TEA.h"

TEA::TEA(void)
{
}

void TEA::TEAEncrypt (uint32_t* Data, int Loops)
{
  uint32_t v0=Data[0], v1=Data[1];                          // set up
  const uint32_t delta=0x9e3779b9; uint32_t sum=0;          // a key schedule constant
  for (int i=0; i < Loops; i++)                             // basic cycle start
  {
    sum += delta;
    v0 += (v1<<4) ^ (v1 + sum) ^ (v1>>5);
    v1 += (v0<<4) ^ (v0 + sum) ^ (v0>>5); 
  }  // end cycle
  Data[0]=v0; Data[1]=v1;
}

void TEA::TEADecrypt(uint32_t* Data, int Loops)
{ 
  uint32_t v0=Data[0], v1=Data[1];                           // set up
  const uint32_t delta=0x9e3779b9; uint32_t sum=delta*Loops; // a key schedule constant
  for (int i=0; i < Loops; i++)                              // basic cycle start */
  {
     v1 -= (v0<<4) ^ (v0 + sum) ^ (v0>>5);
     v0 -= (v1<<4) ^ (v1 + sum) ^ (v1>>5);
     sum -= delta; 
  }                                          // end cycle
  Data[0]=v0; Data[1]=v1;
}
