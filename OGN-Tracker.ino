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
 
#include <SPI.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <EEPROM.h>
#include <stdint.h>

#include "OGNGPS.h"
#include "OGNRadio.h"
#include "Configuration.h"
#include "OGNPacket.h"
#include "ReceiveQueue.h"
#include "RFM69Registers.h"


//#define LOCALTEST

void FormRFPacket(OGNPacket *Packet);
void ProcessGPS(OGNGPS *GPS);


OGNGPS GPS;
OGNRadio Radio;
Configuration TrackerConfiguration;
ReceiveQueue ReceivedData;

uint8_t ReceiveActive = false;

uint32_t ReportTime = 0;
#define REPORTDELAY 1000


void setup() 
{
  TrackerConfiguration.LoadConfiguration();
  
  Serial.begin(115200);
  ConfigurationReport();

  Radio.Initialise(TrackerConfiguration.GetTxPower());
}


void loop() 
{
  OGNPacket ReportPacket;
  uint32_t TimeNow;
  
  ProcessGPS(&GPS);

  TimeNow = millis();
  
  if( (TimeNow - ReportTime) > REPORTDELAY)
  {
#ifdef LOCALTEST
    if(1)
#else
    if(GPS.location.isValid())
#endif
    {
      ReportTime = TimeNow;
      
      if(ReceiveActive)
      {
        Radio.EndReceive();
        ReceiveActive = false;
      }
      
      GPS.CalculateTurnRate(TimeNow);
      GPS.CalculateClimbRate(TimeNow);
 
      FormRFPacket(&ReportPacket);
        
      Radio.SendPacket(ReportPacket.ManchesterPacket,OGNPACKETSIZE*2,F8684,TrackerConfiguration.GetTxPower());
            
      Radio.StartReceive(F8684, ReportPacket.ManchesterPacket); ReceiveActive = true;
      
    }
  }

  if(Serial.available())
  {
    ProcessSerial();
  }
  
  if(ReceiveActive)
  {
    if(Radio.CheckReceive())
    {
      Radio.GetReceivePacket(ReportPacket.ManchesterPacket);
      DecodeRFPacket(&ReportPacket);
    }
  }
}

void FormRFPacket(OGNPacket *Packet)
{
  Packet->MakeSync();
 
  Packet->MakeHeader(false,false,0,false,TrackerConfiguration.GetAddressType(),TrackerConfiguration.GetAddress());
   
  Packet->MakeLatitude(GPS.GetOGNFixQuality(), GPS.time.second(), GPS.GetOGNLatitude());
    
  Packet->MakeLongitude(GPS.GetOGNFixMode(), 0, GPS.GetOGNDOP(), GPS.GetOGNLongitude());
  
  Packet->MakeAltitude(GPS.GetOGNTurnRate(), GPS.GetOGNSpeed(), GPS.GetOGNAltitude());
  
  Packet->MakeHeading(TrackerConfiguration.GetAircraftType(), TrackerConfiguration.GetPrivate(), GPS.GetOGNClimbRate(), GPS.GetOGNHeading());

  Packet->Whiten();
  
  Packet->AddFEC();
  
  Packet->ManchesterEncodePacket();
}

void DecodeRFPacket(OGNPacket *Packet)
{
  Packet->ManchesterDecodePacket();
  if(Packet->CheckFEC()!=0)
  {
#ifdef LOCALTEST
    Serial.print("CRC Error "); Serial.println(Packet->CheckFEC(),DEC);
    Packet->PrintRawPacket();
#endif
    return;
  }
  Packet->DeWhiten();
  ReceivedData.AddPacket((uint32_t *)&Packet->RawPacket[4]);
}

#define NMEABUFFERSIZE 80

void ProcessGPS(OGNGPS *GPS)
{
  static uint8_t NMEABuffer[NMEABUFFERSIZE+1];
  static uint8_t BufferUsed = 0;
  uint8_t c;
  
  while( (c = GPS->ProcessInput()) != 0)
  {
    if(TrackerConfiguration.GetNMEAOut())
    {
      if(c == '$')
      {
        NMEABuffer[0] = c;
        BufferUsed = 1;
      }
      else if ( c == '\r' )
      {
         NMEABuffer[BufferUsed] = '\0';
         BufferUsed = 0;
         if( millis() > (TrackerConfiguration.GetNMEADelay()*1000) )
         {
           Serial.println((char *)NMEABuffer);
           if(ReceivedData.Available())
           {
             ProcessReceivedPackets(GPS);
             ReceivedData.RemovePacket();
           }
         }
      }
      else
      {
        if(BufferUsed < NMEABUFFERSIZE)
        {
          NMEABuffer[BufferUsed] = c;
          BufferUsed++;
        }
        else
        {
          BufferUsed = 0;
        }
      }
    }  
  }
}

#define PI_OVER_180 0.0174532925

void ProcessReceivedPackets(OGNGPS *GPS)
{
  float TargetLatitude, TargetLongitude;
  float TargetDistance, TargetBearing;
  
  uint32_t ID;
  int32_t Altitude;
  int32_t NorthDist, EastDist;
  uint8_t AcType;
  uint16_t Heading;

  TargetLatitude = ReceivedData.GetLatitude();
  TargetLongitude = ReceivedData.GetLongitude();
  TargetDistance = GPS->distanceBetween(GPS->location.lat(), GPS->location.lng(),TargetLatitude, TargetLongitude);
  TargetBearing = PI_OVER_180 * GPS->courseTo(GPS->location.lat(), GPS->location.lng(),TargetLatitude, TargetLongitude);
  NorthDist = TargetDistance * cos(TargetBearing);
  EastDist = TargetDistance * sin(TargetBearing);
  ID = ReceivedData.GetID();
  AcType = ReceivedData.GetType();
  Altitude = ReceivedData.GetAltitude();
  Altitude = Altitude - GPS->altitude.meters();
  Heading = ReceivedData.GetHeading();
  SendTargetString(NorthDist,EastDist,ID,AcType,Altitude,Heading);
}  
  
void SendTargetString(int32_t North, int32_t East, uint32_t ID, uint8_t AcType, int32_t Altitude, uint16_t Heading)
{
  String NMEAString;
  uint32_t i;
  uint32_t Check = 0;
  
  NMEAString += F("$PFLAA,0,");
  NMEAString += North;
  NMEAString += F(",");
  NMEAString += East;
  NMEAString += F(",");
  NMEAString += Altitude;
  NMEAString += F(",1,");
  NMEAString += String(ID,HEX);
  NMEAString += F(",");
  NMEAString += String(Heading,HEX);
  NMEAString += F("0,0,0,");
  NMEAString += String(AcType,DEC);
  
  for(i=1;i<NMEAString.length();i++)
  {
    Check ^= NMEAString.charAt(i);
  }
  
  NMEAString += F("*");
  if(Check<0x10) NMEAString += F("0");
  NMEAString += String(Check,HEX);
  
  NMEAString.toUpperCase();
  
  Serial.println(NMEAString);
}



void ConfigurationReport(void)
{
    Serial.print(F("Device Address \t")); Serial.println(TrackerConfiguration.GetAddress(),HEX);
    Serial.print(F("Address Type \t"));
    switch(TrackerConfiguration.GetAddressType())
    {
      case ADDRESS_TYPE_RANDOM  : Serial.println(F("Random")); break;
      case ADDRESS_TYPE_ICAO   : Serial.println(F("ICAO")); break; 
      case ADDRESS_TYPE_FLARM  : Serial.println(F("Flarm")); break;
      case ADDRESS_TYPE_OGN   : Serial.println(F("OGN")); break; 
      default : Serial.println();
    }
    
    Serial.print(F("Aircraft Type is\t"));
    switch(TrackerConfiguration.GetAircraftType())
    {
      case AIRCRAFT_TYPE_UNKNOWN  : Serial.println(F("Unknown")); break;
      case AIRCRAFT_TYPE_GLIDER   : Serial.println(F("Glider")); break; 
      case AIRCRAFT_TYPE_TOW_PLANE : Serial.println(F("Tow Plane")); break;
      case AIRCRAFT_TYPE_HELICOPTER : Serial.println(F("Helicopter")); break;
      case AIRCRAFT_TYPE_PARACHUTE : Serial.println(F("Parachute")); break;
      case AIRCRAFT_TYPE_DROP_PLANE : Serial.println(F("Drop Plane")); break;
      case AIRCRAFT_TYPE_HANG_GLIDER : Serial.println(F("Hang Glider")); break;
      case AIRCRAFT_TYPE_PARA_GLIDER : Serial.println(F("Para Glider")); break;
      case AIRCRAFT_TYPE_POWERED_AIRCRAFT : Serial.println(F("Powered Aircraft")); break; 
      case AIRCRAFT_TYPE_JET_AIRCRAFT : Serial.println(F("Jet Aircraft")); break;
      case AIRCRAFT_TYPE_UFO : Serial.println(F("UFO")); break;
      case AIRCRAFT_TYPE_BALLOON : Serial.println(F("Balloon")); break;
      case AIRCRAFT_TYPE_AIRSHIP : Serial.println(F("Airship")); break;
      case AIRCRAFT_TYPE_UAV : Serial.println(F("UAV")); break;
      case AIRCRAFT_TYPE_STATIC_OBJECT : Serial.println(F("Static")); break;
      default : Serial.println();
    }
    
    Serial.print(F("Serial Baud Rate \t")); Serial.println(TrackerConfiguration.GetSerialBaud());
    Serial.print(F("GPS Type is \t")); Serial.println(F("NMEA"));
    Serial.print(F("GPS Baud Rate \t")); Serial.println(TrackerConfiguration.GetGPSBaud(),DEC);
    Serial.print(F("Listening for data on pin "));  Serial.println(TrackerConfiguration.GetDataInPin(),DEC);
    Serial.print(F("Sending Data to GPS on pin "));  Serial.println(TrackerConfiguration.GetDataOutPin(),DEC);
    Serial.print(F("Privacy is "));  if(TrackerConfiguration.GetPrivate()) Serial.println(F("On")); else Serial.println(F("Off"));
    Serial.print(F("OutputPower ")); Serial.print(TrackerConfiguration.GetTxPower()); Serial.println(F(" dBm"));
    Serial.print(F("NMEA Out is "));  
    if(TrackerConfiguration.GetNMEAOut())
    {
      Serial.print(F("On. Delay is "));Serial.println(TrackerConfiguration.GetNMEADelay());
    } 
    else 
    {
      Serial.println(F("Off"));
    }
    
    Serial.println();
}

void StatusReport(void)
{ 
  Serial.print(F("Tracking "));Serial.print(GPS.satellites.value()); Serial.println(F(" satellites"));

}

#define MAXLENGTH 40
void ProcessSerial(void)
{
  static String Buffer = "";
  int8_t Byte;
  uint32_t Address;
  uint8_t Type;
  uint8_t Power;
  
  while (Serial.available())
  {
    if(Buffer.length() < MAXLENGTH)
    {
      Byte = toupper(Serial.read());
      
      if ((Byte == '\r' ) || (Byte == '\n' ))
      {
        Serial.println(Buffer);
        if(Buffer.startsWith("STATUS"))
        {
          StatusReport();
        }
        else if(Buffer.startsWith("CONFIG"))
        {
          ConfigurationReport();
        }         
        else if(Buffer.startsWith("ADDRESS "))
        {
          Buffer.remove(0,8);
          Address = strtol(Buffer.c_str(),NULL,16);
          if(Address)
            TrackerConfiguration.SetAddress(Address);
          ConfigurationReport();
        }         
        else if(Buffer.startsWith("SAVE"))
        {
          Serial.println(F("Saving Config"));
          TrackerConfiguration.WriteConfiguration();
          TrackerConfiguration.LoadConfiguration();
          Serial.print(F("\r\nSaved Configuration\r\n"));
          ConfigurationReport();          
        }         
        else if(Buffer.startsWith("ADDRESSTYPE "))
        {
          Buffer.remove(0,12);
          Type = (uint8_t)Buffer.toInt();
          TrackerConfiguration.SetAddressType(Type);
          ConfigurationReport();
        }         
        else if(Buffer.startsWith("AIRCRAFTTYPE "))
        {
          Buffer.remove(0,13);
          Type = (uint8_t)Buffer.toInt();
          TrackerConfiguration.SetAircraftType(Type);
          ConfigurationReport();
        }
        else if(Buffer.startsWith("POWER "))
        {
          Buffer.remove(0,6);
          Power = (uint8_t)Buffer.toInt();
          TrackerConfiguration.SetTxPower(Power);
          ConfigurationReport();
        }         

        Buffer = "";
      }
      else
      {
         Buffer = Buffer + (char)Byte;
      }
    }
    else
    {
      Buffer = "";
    }  
  }
} 
