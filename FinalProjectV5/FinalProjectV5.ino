#include <Point.h>
#define numPlaces 58
#define threshhold .0001

Point database[numPlaces];
float distance[numPlaces];
int previous[numPlaces];
float currentLon = 39.508475;
float currentLat = -84.734232;
int path[numPlaces];
int stepCounter = 0;
boolean finished = false;
//----------
String monthDB[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
char input = ' ';
float lat,lon = 0;
double targLon = -95.3711651;
double targLat = 29.7646147;
float compass,knots = 0;
double targcomp = 0;
double compDif = 0;
//-----------
void setup() {
 //----------
 Serial1.begin(9600);
 Serial1.println("$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28");
 Serial1.println("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29");
 Serial1.println("$PMTK251,9600*17");
 Serial1.println("$PMTK220,200*2C");
 Serial1.println("$PMTK300,200,0,0,0,0*2F");
 Serial.begin(9600);
 pinMode(8, OUTPUT);
 pinMode(9, OUTPUT);
 pinMode(10, OUTPUT);
 pinMode(11, OUTPUT);
 pinMode(13, OUTPUT);
 pinMode(12, INPUT);
 //----------
 //Serial.begin(9600);
 buildDatabase();
 

 //setDestination();

 
  
}

void loop() {

  //gps ------------------------------------------------------------
  String thing,gmt,latstr,ns,lonstr,ew,knotstr,compstr,day,month,year = "";
  boolean fix = false;
    //delay(500);
    if(Serial1.read() == '$'){
      thing = Serial1.readStringUntil('$');
      //Serial.println(thing);
    }
    //Serial.print("test");
    if(thing[3] == 'M') {
      char current[thing.length()];
      thing.toCharArray(current,thing.length());
      int i = 0;
      
      //skip the letters
      while(current[i] != ','){
        i++;
      }
      i++;

      //get Time
      gmt = current[i];
      i++;
      gmt += current[i];
      gmt += ':';
      i++;
      gmt += current[i];
      i++;
      gmt += current[i];
      while(current[i] != ','){
       i++;
      }
      i++;

      //Check Fixed or not
      if(current[i] == 'A'){
        fix = true;
      }
      i += 2;

      //get Latitude
      while(current[i] != ','){
        latstr += current[i];
        i++;
      }
      i++;

      //get North/South
      if(current[i] == 'N'){
        ns = "North";
      } else if(current[i] == 'S'){
        ns = "South";
      }
      i += 2;

      //get Longitude
      while(current[i] != ','){
        lonstr += current[i];
        i++;
      }
      i++;

      //get East/West
      if(current[i] == 'E'){
        ew = "East";
      } else if(current[i] == 'W'){
        ew = "West";
      }
      i += 2;

      //get knots
      while(current[i] != ','){
        knotstr += current[i];
        i++;
      }
      i++;

      //get compass direction
      while(current[i] != ','){
        compstr += current[i];
        i++;
      }
      /*Might Fix big Issue
      i++;

      day += current[i];
      i++;
      day += current[i];
      i++;

      month += current[i];
      i++;
      month += current[i];
      i++;

      month = monthDB[month.toInt() - 1];

      year += current[i];
      i++;
      year += current[i];
      i++;
      */
      
      //Testing Converting to Decimal Degrees
      
      //latstr = "3857.5634";
      //ns = "North";
      //lonstr = "9515.9289";
      //ew = "West";
      

      lat = latstr.toInt() / 100;
      lat += (latstr.toFloat() - lat*100) / 60;
      if(ns == "South"){
        lat *= -1;
      }

      lon = lonstr.toInt() / 100;
      lon += (lonstr.toFloat() - lon*100) / 60;
      if(ew == "West"){
        lon *= -1;
      }

      //Forcing Data to test without reads
      //lon = 39.508475;
      //lat = -84.734232;

      compass = compstr.toFloat();
      knots = knotstr.toFloat();

      /* Inverted Angles, keeping incase it becomes useful
      if(targLat > lat && targLon < lon){
        targcomp = atan((lon-targLon)/(targLat-lat))*(180/3.14);
      } else if(targLat < lat && targLon < lon){
        targcomp = (atan((lat-targLat)/(lon-targLon))*(180/3.14)) + 90;
      } else if(targLat < lat && targLon > lon){
        targcomp = (atan((targLon-lon)/(lat-targLat))*(180/3.14))  + 180;
      } else if(targLat > lat && targLon > lon){
        targcomp = (atan((targLat-lat)/(targLon-lon))*(180/3.14)) + 270;
      } else {
        targcomp = 0;
      }
      */

      //Determines the angle at which the user needs turn
      if(targLat > lat && targLon < lon){
        targcomp = atan((targLat-lat)/(lon-targLon))*(180/3.14) + 270;
      } else if(targLat < lat && targLon < lon){
        targcomp = (atan((lon-targLon)/(lat-targLat))*(180/3.14)) + 180;
      } else if(targLat < lat && targLon > lon){
        targcomp = (atan((lat-targLat)/(targLon-lon))*(180/3.14))  + 90;
      } else if(targLat > lat && targLon > lon){
        targcomp = (atan((targLon-lon)/(targLat-lat))*(180/3.14));
      } else {
        targcomp = 0;
      }

      //Forcing Data
      //compass = 45;

      //Accounts for users current direction to prepare to calculate turn
      compDif = targcomp - compass;
      if(compDif < 0){
        compDif += 360;
      }

      digitalWrite(8,LOW);
      digitalWrite(9,LOW);
      digitalWrite(10,LOW);
      digitalWrite(11,LOW);
      

      //Forcing Data
      //fix = true;

      //Tells user to turn according to current path
      if(fix){
        digitalWrite(13,HIGH);
        currentLon = lon;
        currentLat = lat;
        //Serial.println("Fixed");
        if(compDif <= 10 || compDif >= 350){
          //Forward
          //Serial.print("Forward");
          digitalWrite(8,HIGH);
        } else if(compDif > 10 && compDif <= 90){
          //Right
          //Serial.print("Right");
          digitalWrite(9,HIGH);
        } else if(compDif > 90 && compDif <= 270){
          //Behind
          //Serial.print("Behind");
          digitalWrite(10,HIGH);
        } else if(compDif > 270 && compDif <= 350){
          //Left
          //Serial.print("Left");
          digitalWrite(11,HIGH);
        } else {
          //Serial.print("Unknown");
        }
      }
    }

    if(digitalRead(12) == HIGH){
      digitalWrite(13,LOW);
      setDestination();
    }

    if(fix && abs(currentLon - targLon) < threshhold && abs(currentLat - targLat) < threshhold && !(finished)){
      stepCounter--;
      if(stepCounter == -1){
        //Serial.println("Destination reached!");
        finished = true;
      } else{
        targLon = database[path[stepCounter]].lon;
        targLat = database[path[stepCounter]].lat;
      }
    }
//end gps----------------------------------------------------------


}

void setDestination(){
  Serial.println("\nInput Destination: ");
  String destination = "";
  char temp;
  float target = 12;
  float source = 0;
  float seperation = 9000;
  finished = false;

  while( destination.equals("") ){
    destination = Serial.readString();
  }


  for(int i = 0; i < numPlaces; i++){
    path[i] = 9000;
    if(database[i].location.equals(destination)){
      target = i;
    }
    if(sqrt( sq(database[i].lon - currentLon) + sq(database[i].lat - currentLat) ) < seperation ){
      seperation = sqrt( sq(database[i].lon - currentLon) + sq(database[i].lat - currentLat) );
      source = i;
    }
  }

  findPath(source, target);

  //Debugging
 for(int i = 0; path[i] != 9000; i++){
  Serial.print(path[i]);
  Serial.print(" ");
 }

  for(int i = 0; path[i] < 9000; i++){
    targLon = database[path[i]].lon;
    targLat = database[path[i]].lat;
    stepCounter = i;
  }
}

void buildDatabase(){
  database[0] = Point(39.507148, -84.730606,"other",1,2,9000,9000);
  database[1] = Point(39.507256, -84.730611,"Bachelor",3,4,0,9000);
  database[2] = Point(39.507152, -84.730753,"other",0,3,9000,9000);
  database[3] = Point(39.507264, -84.730755,"other",1,2,6,9000);
  database[4] = Point(39.507670, -84.730584,"other",1,5,6,9000);
  database[5] = Point(39.507844, -84.730541,"Cook",4,7,9000,9000);
  database[6] = Point(39.507687, -84.730750,"Shideler",3,4,7,9000);
  database[7] = Point(39.507809, -84.730747,"other",5,6,9000,9000);
  database[8] = Point(39.508119, -84.730800,"other",7,14,49,9000);
  database[9] = Point(39.507153, -84.732817,"Shriver",2,10,11,9000);
  database[10] = Point(39.507321, -84.732810,"other",3,9,12,9000);
  database[11] = Point(39.507159, -84.733123,"other",9,12,22,25);
  database[12] = Point(39.507327, -84.733132,"Armstrong",10,11,13,9000);
  database[13] = Point(39.507337, -84.734007,"other",12,14,17,9000);
  database[14] = Point(39.508186, -84.733982,"other",8,13,16,32);
  database[15] = Point(39.507371, -84.735075,"other",16,17,19,9000);
  database[16] = Point(39.508204, -84.735055,"other",14,15,26,31);
  database[17] = Point(39.507349, -84.734542,"Kreger",13,15,22,9000);
  database[18] = Point(39.507183, -84.736058,"other",19,20,22,24);
  database[19] = Point(39.507347, -84.736049,"other",15,18,21,9000);
  database[20] = Point(39.507183, -84.736291,"other",18,21,56,9000);
  database[21] = Point(39.507348, -84.736300,"other",19,20,53,9000);
  database[22] = Point(39.507165, -84.734551,"other",11,17,18,23);
  database[23] = Point(39.507165, -84.734551,"other",22,24,25,9000);
  database[24] = Point(39.506477, -84.736074,"other",18,23,9000,9000);
  database[25] = Point(39.506426, -84.733102,"other",11,23,9000,9000);

  database[26] = Point(39.508228, -84.735959,"other",16,19,27,30);
  database[27] = Point(39.508243, -84.736400,"Alumni",26,18,9000,9000);
  database[28] = Point(39.508266, -84.737389,"other",27,29,53,9000);
  database[29] = Point(39.508802, -84.737386,"King",28,30,36,9000);
  database[30] = Point(39.508778, -84.735919,"other",26,29,31,35);
  database[31] = Point(39.508764, -84.735022,"other",16,30,32,34);
  database[32] = Point(39.508746, -84.733967,"Upham",14,31,33,9000);
  database[33] = Point(39.509229, -84.733955,"other",32,34,37,45);
  database[34] = Point(39.509236, -84.735020,"other",31,33,35,9000);
  database[35] = Point(39.509244, -84.735902,"other",30,34,36,9000);
  database[36] = Point(39.509280, -84.737611,"other",29,35,9000,9000);
  database[37] = Point(39.509214, -84.732682,"other",33,38,9000,9000);
  database[38] = Point(39.509700, -84.732638,"Hughes",37,39,9000,9000);
  database[39] = Point(39.510295, -84.732646,"other",38,40,41,47);
  database[40] = Point(39.510449, -84.732655,"EGB",39,42,48,9000);
  database[41] = Point(39.510285, -84.733130,"Laws",41,39,42,43);
  database[42] = Point(39.510454, -84.733155,"other",40,41,44,9000);
  database[43] = Point(39.510303, -84.733583,"other",41,44,45,9000);
  database[44] = Point(39.510459, -84.733622,"Benton",42,43,46,9000);
  database[45] = Point(39.510316, -84.733942,"other",43,46,9000,9000);
  database[46] = Point(39.510457, -84.733964,"other",44,45,9000,9000);
  database[47] = Point(39.510289, -84.731825,"other",39,48,49,9000);
  database[48] = Point(39.510430, -84.731827,"Pearson",40,47,50,9000);
  database[49] = Point(39.510218, -84.730802,"other",8,47,50,51);
  database[50] = Point(39.510427, -84.730791,"other",48,49,52,9000);
  database[51] = Point(39.510212, -84.730509,"other",5,49,52,9000);
  database[52] = Point(39.510424, -84.730488,"Farmer",50,51,9000,9000);
  
  database[53] = Point(39.507370, -84.737441,"other",21,28,54,9000);
  database[54] = Point(39.507366, -84.737835,"other",53,55,9000,9000);
  database[55] = Point(39.507215, -84.737827,"other",54,56,57,9000);
  database[56] = Point(39.507207, -84.737445,"Wells",20,55,9000,9000);
  database[57] = Point(39.507222, -84.738737,"CAB",55,9000,9000,9000);
  


}

void findPath(int source, int targ){
  
  float shortest = 9000;
  int current = source;
  float temp = 9000;
  boolean visited[numPlaces];
  boolean done = false;
  for(int i = 0; i < numPlaces; i++){
    distance[i] = 9000;
    previous[i] = 9000;
    visited[i] = false;
  }

  distance[source] = 0;

  while(!(done)){

    shortest = 9000;
    for(int i = 0; i < numPlaces; i++){
      if( !(visited[i]) && distance[i] < shortest){
        shortest = distance[i];
        current = i;
      }
    }

    
    if(current == targ){
      done = true;
    }

    visited[current] = true;

    
    for(int i = 0; i < 4; i++){
      if( (database[current].connections[i] != 9000) && (!(visited[database[current].connections[i]]))){
      //if( (!(visited[database[current].connections[i]])) && (database[current].connections[i] != 9000)){
        temp = (distance[current] + sqrt(sq(database[current].lon - database[database[current].connections[i]].lon) + sq(database[current].lat - database[database[current].connections[i]].lat)));
        if( temp < distance[database[current].connections[i]]){
          distance[database[current].connections[i]] = temp;
          previous[database[current].connections[i]] = current;
        }
      }
    }
    done = true;
    for(int i = 0; i < numPlaces; i++){
      if(!(visited[i])){
        done = false;
      }
    }
    if(current == targ){
      done = true;
    }
  }

  int next = targ;
  path[0] = targ;
  int i = 1;
  for(; previous[next] != source; i++){
    path[i] = previous[next];
    next = previous[next];
  }
  path[i] = source;
}

