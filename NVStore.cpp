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
#include "NVStore.h"

#include <arduino.h>
#include <eeprom.h>

NVStore::NVStore(void)
{
}

uint16_t NVStore::Store_Write(void *Object, uint16_t Size)
{
  const uint8_t* p = (uint8_t*)Object;
  uint16_t i;
  
  for (i = 0; i < Size; i++)
    EEPROM.write(i, *p++);
  return i;
}

uint16_t NVStore::Store_Read(void *Object, uint16_t Size)
{
    uint8_t* p = (uint8_t*)(void*)Object;
    uint16_t i;
    
    for (i = 0; i < Size; i++)
          *p++ = EEPROM.read(i);
    return i;
}
