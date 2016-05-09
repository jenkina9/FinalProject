#include "Arduino.h"
#include "Point.h"

Point::Point(){
	
}
Point::Point(float longitude, float latitude, String locationName, int zero, int one, int two, int three)
{
  lon = longitude;
  lat = latitude;
  location = locationName;
  connections[0] = zero;
  connections[1] = one;
  connections[2] = two;
  connections[3] = three;
}

void Point::setConnections(int cons[])
{
	connections[0] = cons[0];
	connections[1] = cons[1];
	connections[2] = cons[2];
	connections[3] = cons[3];
}

void Point::displayStuff()
{
	String temp = "";
	for(int i = 0; i < 4; i++){
		if(connections[i] != 9000){
			temp += connections[i];
			temp += " ";
		}
	}
	Serial.print("Longitude: ");
	Serial.println(lon);
	Serial.print("Latitude: ");
	Serial.println(lat);
	Serial.println("Name: " + location + "\nConnections: " + temp);
}

