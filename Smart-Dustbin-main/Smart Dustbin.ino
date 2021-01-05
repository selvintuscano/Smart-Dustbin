#include<Servo.h>
Servo servo;
const int trigPin = 12;
const int echoPin = 11;
int servoPin = 7;
const int TrigPin = 6;
const int EchoPin = 5;
float initialdistance = 40; // insert initial distance or distance between sensor and empty base tank
float surfacearea = 10; // enter value of surface area of the rectangular tank base by multiplying width and length
long duration1, dist, average;   
long aver[3];   //array for average
float duration, distance, heightofwater;

String apiKey = "9ROBZXLN16A2RSXH";  //API key

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  servo.attach(servoPin); 
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  Serial.begin(9600);
  Serial1.begin(115200);   // Arduino to ESP Communication
  servo.write(0);         //close cap on power on
  delay(100);
  servo.detach(); 
  connectWiFi();           // To connect to Wifi
}

void measure()
{


digitalWrite(10,HIGH);
digitalWrite(TrigPin, LOW);
delayMicroseconds(5);
digitalWrite(TrigPin, HIGH);
delayMicroseconds(15);
digitalWrite(TrigPin, LOW);
pinMode(EchoPin, INPUT);
duration = pulseIn(EchoPin, HIGH);
dist = (duration1/2) / 29.1;    //obtain distance
}
void loop() { 
  for (int i=0;i<=2;i++) {   //average distance
    measure();               
   aver[i]=dist;            
    delay(10);              //delay between measurements
  }
 dist=(aver[0]+aver[1]+aver[2])/3;    

if ( dist<50 ) {
//Change distance as per your need
 servo.attach(servoPin);
  delay(1);
 servo.write(0);  
 delay(3000);       
 servo.write(150);    
 delay(1000);
 servo.detach();      
}





  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;
  //heightofwater = initialdistance-distance;
  //volume = heightofwater*surfacearea;
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(1000);
  //Serial.print("Volume: ");
  //Serial.println(volume);
  //delay(1000);





  Serial1.println("AT+CIPMUX=0\r\n");      // To Set MUX = 0
  delay(2000);                             // Wait for 2 sec

  // TCP connection 
  String cmd = "AT+CIPSTART=\"TCP\",\"";   // TCP connection with thingspeak server
  cmd += "184.106.153.149";                // IP addr of api.thingspeak.com
  cmd += "\",80\r\n\r\n";                  // Port No. = 80

  Serial1.println(cmd);                    // Display above Command
  Serial.println(cmd);                     // Send above command to Rx1, Tx1

  delay(20000);                            // Wait for 20 Sec

  if(Serial1.find("ERROR"))                // If returns error in TCP connection
  { 
    Serial.println("AT+CIPSTART error");   // Display error msg
    //return; 
  }

  // prepare GET string 
  String getStr = "GET /update?api_key=";   
  getStr += apiKey;
  getStr +="&field1=";
  getStr += distance; 
  getStr += "\r\n\r\n"; 

  Serial.println(getStr);                 // Display GET String 

  cmd = "AT+CIPSEND=";                    // send data length 
  cmd += String(getStr.length());
  cmd+="\r\n";

  Serial.println(cmd);                   // Display Data length 
  Serial1.println(cmd);                  // Send Data length command to Tx1, Rx1
  
  delay(20000);                          // wait for 20sec

  if(Serial1.find(">"))                    // If prompt opens verify connection with cloud
  {
    Serial.println("connected to Cloud");  // Display confirmation msg 
    Serial1.print(getStr);                 // Send GET String to Rx1, Tx1
  }
  else
  { 
    Serial1.println("AT+CIPCLOSE\r\n");    // Send Close Connection command to Rx1, Tx1
    Serial.println("AT+CIPCLOSE");         // Display Connection closed 
  } 
  
 
  delay(16000);                            // wait for 16sec

 
}
boolean connectWiFi() {               // Connect to Wifi Function
 
  Serial1.println("AT+CWMODE=1\r\n"); // Setting Mode = 1 
  delay(100);                         // wait for 100 mSec

  String cmd = "AT+CWJAP=\"";         // Connect to WiFi
  cmd += "BESR";                     // ssid_name
  cmd += "\",\"";
  cmd += "12345678";                  // password
  cmd += "\"\r\n";              
  
  Serial.println(cmd);                // Display Connect Wifi 
  Serial1.println(cmd);               // send Connect WiFi command to Rx1, Tx1 
  
  delay(10000);                       // wait for 10 sec

  Serial1.println("AT+CWJAP?");       // Verify Connected WiFi

  if(Serial1.find("+CWJAP"))        
  {
    Serial.println("OK, Connected to WiFi.");         // Display Confirmation msg ......first time
    return true;
  }
  else
  {
    Serial.println("Can not connect to the WiFi.");   // Display Error msg
    return false;
  }
}
