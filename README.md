OGN Tracker Client

This repository implements the code for an Open Glider Network Tracker client.

Details of the tracking network and tracking client are provided here

http://wiki.glidernet.org/
http://wiki.glidernet.org/ogn-tracking-protocol

Hardware

The original target hardware was the Low Power Lab Moteino USB
https://lowpowerlab.com/shop/index.php?_route_=moteinousb

the version with the RRM69HW radio and no additional FLASH was used but 
the code should run on any similar hardware.

You will also need a TTL GPS module. There are plenty to choose from on your 
favourite internet auction site.

To build.
Attach the GPS power wires to the Moteino power.
Attach the GPS Tx and Rx wires to pins 4 & 5 on the Moteino

The data needs to flow out of the GPS into pin 4 and out of pin 5 to the GPS.
GPS TX > Pin 4
GPS RX < Pin 5

You're done

Software

I used the Arduino build environment. It is available to download here
www.arduino.cc

In addition you need the following libraries
TinyGPSPlus and RFM69 

RFM69 is available here https://github.com/LowPowerLab/RFM69
TinyGPSPlus here https://github.com/mikalhart/TinyGPSPlus/releases

Download and install them into your appropriate folders in your Arduion\libraries folder

Setup / Configuration

After building and uploading the source you can setup your new tracker by 
accessing through the virtual serial port. 
Default access is 115200,8,N,1

At the very least, you should set a tracker address. 
If an address is not set, you tracker will use a default address

The commands on the interface are as follows. They are not case sensitive, but (for now) the 
interface is very very basic--No backspace or cursor keys, but you only need to do this once.
Remember to save when you are happy.

Status : Prints out the GPS status

Config : Prints out the current configuration

Address XXXXXX : Sets the tracker address to XXXXXX where XXXXXX is a 6 character hex address.

Addresstype X : Sets the address type where address types are:
	0 - Random
	1 - ICAO
	2 - FLARM
	3 - OGN

AircraftType XX : Sets the aircraft type, there XX types are:
	0 : Unknown
	1 : Glider
	2 : Tow Plane
	3 : Helicopter
	4 : Parachute
	5 : Drop Plane
	6 : Hang Glider
	7 : Paraglider
	8 : Powered Aircraft
	9 : Jet
	10 : UFO
	11 : Balloon
	12 : Airship
	13 : UAV
	14 : Static Object
	
Save : Once you have fiddled with the settings, Save will store the configuration in the EEPROM. 
If you don't save the tracker will revert next time you switch on.

Support.
Good luck with you tracker. If you need support, please post on the Open Glider Network forum on 
Google Groups. I will try to spot questions there, or there will be other who can help.

Mike R





Copyright (C) <2015> <Mike Roberts>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
