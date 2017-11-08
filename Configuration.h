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
 
#ifndef CONFIGURATION_h
#define CONFIGURATION_h

#include <stdint.h>

#define ADDRESS_TYPE_RANDOM 0
#define ADDRESS_TYPE_ICAO 1
#define ADDRESS_TYPE_FLARM 2
#define ADDRESS_TYPE_OGN 3

#define AIRCRAFT_TYPE_UNKNOWN 0
#define AIRCRAFT_TYPE_GLIDER 1
#define AIRCRAFT_TYPE_TOW_PLANE 2
#define AIRCRAFT_TYPE_HELICOPTER 3
#define AIRCRAFT_TYPE_PARACHUTE 4
#define AIRCRAFT_TYPE_DROP_PLANE 5
#define AIRCRAFT_TYPE_HANG_GLIDER 6
#define AIRCRAFT_TYPE_PARA_GLIDER 7
#define AIRCRAFT_TYPE_POWERED_AIRCRAFT 8 
#define AIRCRAFT_TYPE_JET_AIRCRAFT 9
#define AIRCRAFT_TYPE_UFO 10
#define AIRCRAFT_TYPE_BALLOON 11
#define AIRCRAFT_TYPE_AIRSHIP 12
#define AIRCRAFT_TYPE_UAV 13
#define AIRCRAFT_TYPE_STATIC_OBJECT 15

#define STOREVERSION 0x0003
struct Configuration_Struct
{
  uint16_t StoreVersion;
  uint32_t Address;
  uint8_t Private;
  uint8_t DataInPin;
  uint8_t DataOutPin;
  uint32_t SerialBaud;
  uint32_t GPSBaud;
  uint8_t AddressType; 
  uint8_t AircraftType;
  int8_t TxPower;
  uint8_t NMEAOut;
  uint32_t NMEADelay;
};   

/////////////////////////////////////////////////////////////////////
class Configuration 
{
  public:
    Configuration();
    void LoadConfiguration(uint32_t TempAddress = 0xBADADD);
    void WriteConfiguration(void);
    
    uint32_t GetAddress(void); 
    void SetAddress(uint32_t Address); 
    
    uint8_t GetAddressType(void);
    void SetAddressType(uint8_t Type);
    
    uint8_t GetAircraftType(void);
    void SetAircraftType(uint8_t Type);

    uint8_t GetPrivate(void);
    void SetPrivate(uint8_t Private);

    uint32_t GetSerialBaud(void);
    void SetSerialBaud(uint32_t SerialBaud);  
    
    uint32_t GetGPSBaud(void);
    void SetGPSBaud(uint32_t GPSBaud);  

    uint8_t GetDataInPin(void);
    void SetDataInPin(uint8_t Pin);
      
    uint8_t GetDataOutPin(void);
    void SetDataOutPin(uint8_t Pin);

    int8_t GetTxPower(void);
    void SetTxPower(int8_t Power);
    
    uint8_t GetNMEAOut(void);
    void SetNMEAOut(uint8_t Out);
    
    uint32_t GetNMEADelay(void);
    void SetNMEADelay(uint32_t Delay);
    
      
  protected:
				
  private:
    struct Configuration_Struct cs;
};


#endif 

