#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <Servo.h>
const char *ssid = "test";
const char *password = "password";
IPAddress myIP;
Servo myservo;
ESP8266WebServer server(80);
// Hardware setup - FPS connected to:
//	  digital pin 4(arduino rx, fps tx)
//	  digital pin 5(arduino tx - 560ohm resistor - fps tx - 1000ohm resistor - ground)
//		this voltage divider brings the 5v tx line down to about 3.2v so we dont fry our fps
//long option = 0;
int pin_switch = 4;
//boolean Fstatus = LOW;
FPS_GT511C3 fps(13,2);
void handleRoot() {
server.send(200, "text/html", "<h1><CENTER>You Are Connected</CENTER></h1><a href='/toggle'><button type='button' class='button' style='background-color:Tomato'>Click Me!</button></a>");
//server.send(200, "text/html", "<button>You are connected</button>");
}
void tog(){
  int i=0;
  int pos =30;
  Serial.println("jio");
  for (i = 0; i <= pos; i += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    Serial.println(i);
    myservo.write(i);
    delay(15);
  } 
   delay(2000);

   //server.send(200, "text/html", "<h1><CENTER>You Are Connected2</CENTER></h1>");
   if (pos==30)
    {
      delay(100);
        for (i = pos; i >= 0; i -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
          myservo.write(i);
          delay(15);
          } 
      }
}
void setup()
{
	Serial.begin(9600);
 myservo.attach(5);
	delay(100);
 Serial.println();
Serial.print("Configuring access point...");
WiFi.softAP(ssid, password);
myIP = WiFi.softAPIP();
Serial.print("AP IP address: ");
Serial.println(myIP);
server.on("/", handleRoot);
server.on("/toggle", tog);
server.begin();
Serial.println("HTTP server started");
//      }
  //pinMode(pin_switch, INPUT);
  //pinMode(5, INPUT);
 // pinMode(4, OUTPUT);
	fps.Open();
  //attachInterrupt( digitalPinToInterrupt(pin_switch), blink, CHANGE );
}

void Enroll()
{
  fps.SetLED(true);
 //Fstatus == LOW;
  // Enroll test
  // find open enroll id
  if ( fps.IsPressFinger()) 
  {
    Serial.println("Finger toh hai lekin tu ADMin h kya nhi check krne de...");
    fps.CaptureFinger(false);
    int id1 = fps.Identify1_N();
    if( id1 ==  0  )
    {
      Serial.println("HI Admin...aaaj kisko add krega?");
      while(fps.IsPressFinger() == true) delay(100);
       fps.SetLED(false);
       delay(300);
  int enrollid = 0;
  bool okid = false;
  while ( okid == false )
  {
    okid = !fps.CheckEnrolled(enrollid);
    //Serial.println(okid);
    if ( okid == false )
      {
      enrollid++;
      //Serial.println(enrollid);
      }
  }
  fps.SetLED(true);
  fps.EnrollStart(enrollid);
  // enroll
  Serial.print("Press finger to Enroll #");
  Serial.println(enrollid);
  while(fps.IsPressFinger() == false) delay(100);
  bool bret = fps.CaptureFinger(true);
  int iret = 0;
  if (bret != false)
  {
    Serial.println("Remove finger");
    fps.Enroll1(); 
    while(fps.IsPressFinger() == true) delay(100);
    Serial.println("Press same finger again");
    while(fps.IsPressFinger() == false) delay(100);
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      Serial.println("Remove finger");
      fps.Enroll2();
      while(fps.IsPressFinger() == true) delay(100);
      Serial.println("Press same finger yet again");
      while(fps.IsPressFinger() == false) delay(100);
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
        Serial.println("Remove finger");
        iret = fps.Enroll3();
        if (iret == 0)
        {
          Serial.println("Enrolling Successfull");
          int i=0;
          for(i=0;i<=10;i++)
          {
            delay(10);
            fps.SetLED(false);
            delay(10);
            fps.SetLED(true);
            }
        }
        else
        {
          Serial.print("Enrolling Failed with error code:");
          Serial.println(iret);
        }
      }
      else Serial.println("Failed to capture third finger");
    }
    else Serial.println("Failed to capture second finger");
  }
  else Serial.println("Failed to capture first finger");
  }
  else
  {Serial.print("Tu admin nhi h chutiye");
   Serial.println(id1);}
  }
fps.SetLED(false);
}
void Verify()
{
  fps.SetLED(true);
//  Fstatus == HIGH;
  // Identify fingerprint test
  if ( fps.IsPressFinger() ) 
  {
    Serial.println("Please press finger");
    fps.CaptureFinger(false);
    int id = fps.Identify1_N();
    if (id <200)
    {
      Serial.print("Verified ID:");
      Serial.println(id);
      tog();
    }
    else
    {
      Serial.println("Finger not found");
     }
  }
 // while (  !fps.IsPressFinger() );
 // else
 // {
  //  Serial.println("Please press finger");
 //  }
  fps.SetLED(false);
 }
void loop()
{
   
  server.handleClient();
   while ( digitalRead(pin_switch) == HIGH) 
    {
      Serial.print("Verify Mode");
      Serial.println(digitalRead(pin_switch),BIN);
       //digitalWrite(pin_LED, HIGH);
       Verify();
    }
    //else 
    //{
       Serial.print("Enroll Mode");
       Serial.println(digitalRead(pin_switch),BIN);
       //digitalWrite(pin_LED, LOW);
       Enroll();
    //}
    
  fps.SetLED(false);
	delay(5000);
  Serial.flush();
}
