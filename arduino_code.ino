#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

const int waterSensorPin = A0;  
const int speakerPin = 5;       
const int pumpPin = 8;          

const int minSensorReading = 0;  
const int maxSensorReading = 672; 


const int lowWaterThreshold = 25;  


const int hysteresisBuffer = 5;  

const int numReadings = 10;      
int readings[numReadings];      
int readIndex = 0;             
long total = 0;              
int averageWaterLevel = 0;       

const String roasts[] = {
  "0001.mp3","0002.mp3","0003.mp3","0004.mp3","0005.mp3","0006.mp3","0007.mp3","0008.mp3","0009.mp3","0010.mp3","0011.mp3","0012.mp3","0013.mp3","0014.mp3","0015.mp3","0016.mp3","0017.mp3",
  
};

int roastIndex = 0;  
unsigned long lastDrinkingTime = 0;  
unsigned long lastStableTime = 0;    
bool isDrinking = false;  

SoftwareSerial mySoftwareSerial(10, 11);  
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  pinMode(speakerPin, OUTPUT);  
  pinMode(pumpPin, OUTPUT);    
  Serial.begin(9600);         
  mySoftwareSerial.begin(9600);


  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("DFPlayer Mini not found.");
    while (true);  
  }
  Serial.println("DFPlayer Mini found!");


  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void loop() {

  int rawWaterLevel = analogRead(waterSensorPin);


  int percentageWaterLevel = map(rawWaterLevel, 0, 1023, minSensorReading, maxSensorReading);


  total -= readings[readIndex];
  readings[readIndex] = percentageWaterLevel;
  total += readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  averageWaterLevel = total / numReadings;  

  Serial.print("Average Water Level: ");
  Serial.print(averageWaterLevel); 
  Serial.println("%");


  if (averageWaterLevel < (lowWaterThreshold - hysteresisBuffer)) {  
    if (!isDrinking) {
  
      isDrinking = true;
      roastIndex = (roastIndex + 1) % 5;  
      String roastMessage = roasts[roastIndex];
      Serial.println(roastMessage);  

     
      tone(speakerPin, 1000, 500); 
      
      myDFPlayer.play(1); 

      digitalWrite(pumpPin, HIGH); 
      delay(1000);  
      digitalWrite(pumpPin, LOW);  

      lastDrinkingTime = millis();
    }

    lastStableTime = millis();
  }
  else if (averageWaterLevel > (lowWaterThreshold + hysteresisBuffer)) {  
    if (isDrinking && (millis() - lastStableTime) >= 5000) {  
      isDrinking = false;
      Serial.println("Drinking stopped. No more roasts.");
      noTone(speakerPin);  
      
      lastStableTime = millis();
    }
  }

  delay(500); 
}
