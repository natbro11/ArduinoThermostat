
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal.h>

DHT dht(13, DHT11);
LiquidCrystal lcd(8,9,4,5,6,7);

byte charDegF[] = {B01000, B10100, B01000, B00111, B00100, B00110, B00100, B00100};
byte charRelH[] = {B10010, B00100, B01000, B10010, B00000, B00101, B00111, B00101};

unsigned long lastTime;
unsigned long currentTime;
unsigned long deltaTime;

long heatTime;
long senseTime;

float tempAvg2Min;
float tempAvg10Min;
float tempAvg60Min;

float humAvg2Min;
float humAvg10Min;
float humAvg60Min;

long heatCycleTime;
long senseCycleTime;
float setTemperature;

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16, 2);

  lcd.createChar(0,charDegF);
  lcd.createChar(1,charRelH);

  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);  
  digitalWrite(12, HIGH);

  heatCycleTime = 90000;
  senseCycleTime = 300000;

  setTemperature = 72.0;
  senseTime = 0;
  heatTime = 0;

  tempAvg2Min = 76.0; 
  tempAvg10Min = 76.0;
  tempAvg60Min = 76.0;
  humAvg2Min = 50.0; 
  humAvg10Min = 50.0;
  humAvg60Min = 50.0;
}

void loop() {
  lastTime = currentTime;
  currentTime = millis();
  deltaTime = currentTime - lastTime;
  
  if(heatTime > 0){
    heatTime -= deltaTime;
  }
  if(senseTime > 0){
    senseTime -= deltaTime;
  }

  printMain();


  if(heatTime > 0){
    digitalWrite(11, HIGH);
  }
  else{
    digitalWrite(11, LOW);
  }

  if((tempAvg2Min < (setTemperature - 0.5)) && (senseTime <= 0)){
    heatTime += heatCycleTime;
    senseTime += senseCycleTime + heatCycleTime;
  }

  if((currentTime % 5000) < (lastTime % 5000)){
    updateTemperature();
    updateHumidity();
  }
  readInput();

}

// Operation Functions ----------------------------------------------------------------------------

void readInput(){
  if((currentTime % 200) < (lastTime % 200)){
    if(analogRead(A0)>50 && analogRead(A0)<200){
      setTemperature += 0.1;
    }
    if(analogRead(A0)>200 && analogRead(A0)<400){
      setTemperature -= 0.1;
    }
    if(analogRead(A0)>-1 && analogRead(A0)<50){
      setTemperature = 67.5;
    }
    if(analogRead(A0)>400 && analogRead(A0)<600){
      setTemperature = 72.5;
    }
    if(setTemperature > 79.5){
      setTemperature = 79.5;
    }
    if(setTemperature < 60.5){
      setTemperature = 60.5;
    }
  }
}

// DHT Functions ----------------------------------------------------------------------------------
void updateTemperature(){
  float temp = dht.readTemperature(true);
  tempAvg2Min = ((tempAvg2Min * 11.0) + (temp)) / 12.0;
  tempAvg10Min = ((tempAvg10Min * 59.0) + (temp)) / 60.0;
  tempAvg60Min = ((tempAvg60Min * 359.0) + (temp)) / 360.0;
}

void updateHumidity(){
  float hum = dht.readHumidity();
  humAvg2Min = ((humAvg2Min * 11.0) + (hum)) / 12.0;
  humAvg10Min = ((humAvg10Min * 59.0) + (hum)) / 60.0;
  humAvg60Min = ((humAvg60Min * 359.0) + (hum)) / 360.0;
}

// Display Functions ------------------------------------------------------------------------------

void printMain(){
  lcd.setCursor(0,0);
  lcd.print("Heat-");
  lcd.print(setTemperature,1);
  lcd.write(byte(0));
  lcd.print("|");
  lcd.print(tempAvg2Min,1);
  lcd.write(byte(0));
  lcd.setCursor(0,1);
  lcd.print("H:");
  lcd.print(heatTime/1000);
  lcd.print(" - S:");
  lcd.print(senseTime/1000);
  lcd.print("                ");
}
