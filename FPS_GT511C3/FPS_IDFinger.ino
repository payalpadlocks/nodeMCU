#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"
// Hardware setup - FPS connected to:
//	  digital pin 4(arduino rx, fps tx)
//	  digital pin 5(arduino tx - 560ohm resistor - fps tx - 1000ohm resistor - ground)
//		this voltage divider brings the 5v tx line down to about 3.2v so we dont fry our fps
//long option = 0;
int pin_switch = 2;
//boolean Fstatus = LOW;
FPS_GT511C3 fps(4, 5);
void setup()
{
	Serial.begin(9600);
	delay(100);
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
  if ( fps.IsPressFinger() ) 
  {
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
