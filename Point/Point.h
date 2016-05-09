#ifndef Point_h
#define Point_h

#include "Arduino.h"


class Point
{
  public:
   Point();
   Point(float lon, float lat, String location, int zero, int one, int two, int three);
   void setConnections(int cons[]);
   void displayStuff();
   float lon;
   float lat;
   String location;
   int connections[4];
  private:
    
};

#endif
