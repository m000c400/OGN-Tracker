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
 
#include "OGNGPS.h"


OGNGPS::OGNGPS(void):TinyGPSPlus()
{
  OGNGPSStream = new SoftwareSerial(4, 5);
  OGNGPSStream->begin(9800);
  TurnRate = 0;
  ClimbRate = 0;
  LastAltitude = 0;
  LastHeading = 0;
}

uint8_t OGNGPS::ProcessInput(void)
{
  uint8_t c;
  
  if(OGNGPSStream->available() > 0)
  {
    c = OGNGPSStream->read();
    TinyGPSPlus::encode(c);
    return c;
  }
  else
  {
    return 0;
  }
}

void OGNGPS::CalculateClimbRate(int32_t TimeNow)
{
  static uint32_t LastTime = 0;
  int32_t NewAltitude;
  int32_t DeltaH, DeltaT;
  
  if(( TimeNow - LastTime) < 30000)
  {
     return;
  }
 
  DeltaT = TimeNow - LastTime;
  LastTime = TimeNow; 
  
  NewAltitude = (int32_t)TinyGPSPlus::altitude.meters();
  DeltaH = 10*(NewAltitude - LastAltitude);
  LastAltitude = NewAltitude;
  ClimbRate = DeltaH/DeltaT;
}
  
void OGNGPS::CalculateTurnRate(int32_t TimeNow)
{
  static uint32_t LastTime = 0;
  int32_t NewHeading;
  int32_t DeltaH, DeltaT;
 
  if(( TimeNow - LastTime) < 1000)
  {
     return;
  }
 
  DeltaT = TimeNow - LastTime;
  LastTime = TimeNow; 
  
  NewHeading = (int32_t)TinyGPSPlus::course.deg() * 2.84444;
  DeltaH = 10*(NewHeading - LastHeading);
  LastHeading = NewHeading;
  TurnRate = DeltaH/DeltaT;
}


uint32_t OGNGPS::GetOGNLatitude(void)
{
  uint32_t Latitude, Fraction;
  
  Latitude = TinyGPSPlus::location.rawLat().deg;
  Fraction = TinyGPSPlus::location.rawLat().billionths;
  Latitude *= 100000; Fraction /= 10000;
  Latitude += Fraction;
  if(TinyGPSPlus::location.rawLat().negative) Latitude *= -1;
  Latitude *= 6;
  Latitude /= 8;
  return Latitude;
}

uint32_t OGNGPS::GetOGNLongitude(void)
{
  uint32_t Longitude, Fraction;
  Longitude = TinyGPSPlus::location.rawLng().deg;
  Fraction = TinyGPSPlus::location.rawLng().billionths;
  Longitude *= 100000; Fraction /= 10000;
  Longitude += Fraction;
  if(TinyGPSPlus::location.rawLng().negative) Longitude *= -1;
  Longitude *= 6;
  Longitude /= 16;
  return Longitude;
}

uint32_t OGNGPS::GetOGNAltitude(void)
{
  uint32_t Altitude;
  Altitude = TinyGPSPlus::altitude.meters();
  
  if(Altitude <0)
    return 1;
  else if (Altitude < 0x1000)
    return Altitude;
  else if (Altitude < 0x3000)
    return (0x1000 + ((Altitude - 0x1000)/2));
  else if (Altitude < 0x7000)
    return (0x2000 + ((Altitude - 0x3000)/4));
  else if (Altitude < 0xF000)
    return (0x3000 + ((Altitude - 0x7000)/8));
  else return 0x3FFF;  
}

uint32_t OGNGPS::GetOGNSpeed(void)
{
  uint32_t Speed;
  Speed = TinyGPSPlus::speed.mps()*0.61;
  
  
  if(Speed <0)
    return 0;
  else if (Speed < 0x100)
    return Speed;
  else if (Speed < 0x300)
    return (0x100 + ((Speed - 0x100)/2));
  else if (Speed < 0x700)
    return (0x300 + ((Speed - 0x300)/4));
  else if (Speed < 0xF00)
    return (0x700 + ((Speed - 0x700)/8));
  else
    return 0x3FF;
}

uint32_t OGNGPS::GetOGNDOP(void)
{
   return(TinyGPSPlus::hdop.value());
}

uint8_t OGNGPS::GetOGNFixQuality(void)
{
  if(TinyGPSPlus::location.isValid())
   return 1;
  else
    return 0;
}
  
uint8_t OGNGPS::GetOGNFixMode(void)
{
  if(TinyGPSPlus::satellites.value()>4)
   return 1;
  else
    return 0;
}
    
uint16_t OGNGPS::GetOGNHeading(void)
{
  return TinyGPSPlus::course.deg() * 2.84444;
}
 
int16_t OGNGPS::GetOGNTurnRate(void)
{
  return TurnRate;
}
    
int16_t OGNGPS::GetOGNClimbRate(void)        
{
  int16_t Rate = 0;
  int16_t UpDown = 0;
  
  Rate = ClimbRate;
  if(Rate < 0)
  {
    Rate = Rate * -1;
    UpDown = 0x100;
  }
  if(Rate < 0x040)
    Rate = Rate;
  else if (Rate < 0x0C0)
    Rate = 0x040 + (Rate - 0x40)/2;
  else if (Rate < 0x1C0)
    Rate = 0x0C0 + (Rate - 0x0C0)/4;
  else if (Rate < 0x3C0)
     Rate = 0x1C0 + (Rate - 0x1C0)/8;
  else
     Rate = 0x0FF;
     
  return (uint16_t)(UpDown | Rate);
}
