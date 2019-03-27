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
int pin_switch =  4;
int pin_servo  =  5;
FPS_GT511C3 fps(13,2);
void handleRoot() 
{
  server.send(200, "text/html", "<h1><CENTER>You Are Connected</CENTER></h1><a href='/toggle'><button type='button' class='button' style='background-color:Tomato'>Click Me!</button></a>");
}
void tog(){
  fps.Close();
  myservo.attach(pin_servo);
  delay(20);
  myservo.write(30);
  server.send(200, "text/html", "<h1><CENTER>You Are Connected2</CENTER></h1>");
  delay(5000);
  myservo.write(0);
  delay(5000);
  myservo.detach();
  fps.Open();        
}
void setup()
{
	Serial.begin(9600);
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
	fps.Open();
}

void Enroll()
{
  fps.SetLED(true);
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
    {
      Serial.print("Not an Admin");
      Serial.println(id1);
    }
  }
  fps.SetLED(false);
}
void Verify()
{
  fps.SetLED(true);
  // Identify fingerprint test
  if ( fps.IsPressFinger() ) 
  {
    Serial.println("Please press finger");
    fps.CaptureFinger(false);
    int id = fps.Identify1_N();
    if (id <200)
    {
      fps.SetLED(false);
      Serial.print("Verified ID:");
      Serial.println(id);
      tog();
      fps.SetLED(true);
    }
    else
    {
      Serial.println("Finger not found");
     }
  }
  fps.SetLED(false);
}
void loop()
{
  server.handleClient();
  while ( digitalRead(pin_switch) == HIGH) 
  {
    Serial.print("Verify Mode");
    Serial.println(digitalRead(pin_switch),BIN);
    Verify();
  }
  Serial.print("Enroll Mode");
  Serial.println(digitalRead(pin_switch),BIN);
  Enroll();
  fps.SetLED(false);
	delay(5000);
  Serial.flush();
}
