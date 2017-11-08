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

#ifndef TEA_h
#define TEA_h

#include <stdint.h>

/////////////////////////////////////////////////////////////////////
class TEA
{
  public:
    TEA();
    void TEAEncrypt (uint32_t* Data, int Loops=4);
    void TEADecrypt (uint32_t* Data, int Loops=4);

      
  protected:
				
  private:
};


#endif 

