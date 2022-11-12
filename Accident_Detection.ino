//Libraries
#include "TinyGPS++.h"
#include "SoftwareSerial.h"
SoftwareSerial GPRS(2,3); //RX=pin 2, TX=pin 3
SoftwareSerial serial_connection(10, 11); //RX=pin 10, TX=pin 11
TinyGPSPlus gps;//This is the GPS object that will pretty much do all the grunt work with the NMEA data

//Pins and variables declarations

//variables corresponding to GPS Module
float latitude=0; 
float longitude=0;                       
String Speed="";
int Contrast=20;

//Acceleometer pins
#define x A1
#define y A2
#define z A3

//variables corresponding to acceleremoter
int xsample=0;
int ysample=0;
int zsample=0;
#define samples 10 //number of sample readings in still case

//threshold maximum and minimum values of change in acceleration when accident is detected
#define minVal -50
#define MaxVal 50

void setup()
{
  Serial.begin(9600);//This opens up communications to the Serial monitor in the Arduino IDE
  serial_connection.begin(9600);//This opens up communications to the GPS 
  Serial.println("successfully Initialized....");
  Serial.println("GPS Start");//Just show to the monitor that the sketch has started

  //taking samples in still case
  for(int i=0;i<samples;i++)
  {
    xsample+=analogRead(x);
    ysample+=analogRead(y);
    zsample+=analogRead(z);
  }
  
  //Taking average
  xsample/=samples;
  ysample/=samples;
  zsample/=samples;

  Serial.println(xsample);
  Serial.println(ysample);
  Serial.println(zsample);
  delay(1000);
}

void loop()
{
  while(serial_connection.available())//While there are characters to come from the GPS
  {
    gps.encode(serial_connection.read());//This feeds the serial NMEA data into the library one char at a time
    //Serial.println("Satellite Count:");
  }
  if(gps.location.isUpdated())//This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
  {
    //Get the latest info from the gps object which it derived from the data sent by the GPS unit
    Serial.println("Satellite Count:");
    Serial.println(gps.satellites.value());
    Serial.println("Latitude:");
    Serial.println(gps.location.lat(), 6);
    Serial.println("Longitude:");
    Serial.println(gps.location.lng(), 6);
    Serial.println("Speed MPH:");
    Serial.println(gps.speed.mph());
    Serial.println("Altitude Feet:");
    Serial.println(gps.altitude.feet());
    Serial.println("");
  }
    int value1=analogRead(x);
    int value2=analogRead(y);
    int value3=analogRead(z);

    int xValue=xsample-value1;
    int yValue=ysample-value2;
    int zValue=zsample-value3;
    
    Serial.print("x=");
    Serial.println(xValue);
    Serial.print("y=");
    Serial.println(yValue);
    Serial.print("z=");
    Serial.println(zValue);
  //condition for accident detection
    if(xValue < minVal || xValue > MaxVal  || yValue < minVal || yValue > MaxVal  || zValue < minVal || zValue > MaxVal)
    {
      for(int i=0;i<5;i++)
      {
        Serial.println("Calling");
        GPRS.begin(9600);
        Serial.println("Connecting to network");
        delay(2000);
        Serial.println("Should be connected to network by now");
        delay(5000);
        GPRS.print("ATD+919101971185;\r");
        Serial.println("Dialing");
        delay(1200);
        GPRS.print("ATH\r");
        Serial.println("Disconnect");

        // Message
        GPRS.print("Accident Detected; ");
        Serial.println();
        GPRS.print("Location; ");
        Serial.println();
        GPRS.print("Latitude - ");
        Serial.println();
        GPRS.print(latitude);
        Serial.println();
        GPRS.print("Longitude - ");
        Serial.println();
        GPRS.print(longitude);
        Serial.println();
        GPRS.print("http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=");
        GPRS.println(latitude);
        GPRS.println("+");
        GPRS.println(longitude);
        GPRS.write(26);
        GPRS.print("ATD+919101971185;\r");
      }
    }
    delay(1000);
}