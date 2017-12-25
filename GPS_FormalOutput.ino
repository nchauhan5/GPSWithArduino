#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);

#define GPSECHO true

boolean usingInterrupt = false;
void useInterrupt(boolean);

void setup() {
  //Connect 
  Serial.begin(115200);//baud rate
  GPS.begin(9600);

  //GPS module commands
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);//set outputs to only RMC and GGA sentences
  
  
  //set output to 1/second
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  

  GPS.sendCommand(PGCMD_ANTENNA);//to see if antenna is connected or not

  useInterrupt(true);
  
  delay(1000);
}

SIGNAL(TIMER0_COMPA_vect) {
 char c = GPS.read();
 //to debug
#ifdef UDR0
 if (GPSECHO)
   if (c) UDR0 = c;  
#endif
}

void useInterrupt(boolean v) {
 if (v) {
   OCR0A = 0xAF;
   TIMSK0 |= _BV(OCIE0A);
   usingInterrupt = true;
 } else {
   TIMSK0 &= ~_BV(OCIE0A);
   usingInterrupt = false;
 }
}

uint32_t timer = millis();
void loop()                     // run over and over again
{
 if (! usingInterrupt) {
   // read data from the GPS in the 'main loop'
   char c = GPS.read();
   if (GPSECHO)
     if (c) Serial.print(c);
 }
 
 if (GPS.newNMEAreceived()) {
   if (!GPS.parse(GPS.lastNMEA()))  
     return;  
 }

 if (timer > millis())  timer = millis();

 // approximately every 2 seconds or so, print out the current stats
 if (millis() - timer > 2000) { 
   timer = millis(); // reset the timer
   //prints Time
   //Serial.print("\nTime: ");
   //Serial.print(GPS.hour, DEC); Serial.print(':');
   //Serial.print(GPS.minute, DEC); Serial.print(':');
   //Serial.print(GPS.seconds, DEC); Serial.print('.');
   //Serial.println(GPS.milliseconds);

   //prints date
   Serial.print("\nDate: ");
   Serial.print(GPS.day, DEC); Serial.print('/');
   Serial.print(GPS.month, DEC); Serial.print("/20");
   Serial.println(GPS.year, DEC);
   Serial.print("Fix: "); Serial.print((int)GPS.fix);
   Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 
   if (GPS.fix) {
     Serial.print("Location: ");
     Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
     Serial.print(", "); 
     Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
     
     Serial.print("Altitude: "); Serial.println(GPS.altitude);
   }
 }
}
