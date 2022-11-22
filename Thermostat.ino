
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal.h>

DHT dht(13,DHT11);
LiquidCrystal lcd(8,9,4,5,6,7);

byte f[] = {0x08,0x14,0x08,0x07,0x04,0x07,0x04,0x04}; //Degrees F character
byte rh[] = {0x12,0x04,0x08,0x12,0x00,0x05,0x07,0x05}; //Humidity character
byte l[] = {0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04}; //Divider character
byte a[] = {0x10,0x10,0x10,0x1C,0x02,0x05,0x07,0x05}; //Long Average character
byte g[] = {0x04,0x04,0x04,0x0F,0x1F,0x1F,0x1F,0x0E}; // GFY

float tempdata[10] = {72.0, 72.0, 72.0, 72.0, 72.0, 72.0, 72.0, 72.0, 72.0, 72.0};

float longAverage = 72.000;
int longCount = 1;
float setAverage = 72.000;
int setCount = 1;

int x;
int fixT = 1.8;          //Temperature value offset for inaccurate sensor
int cycleCount = 0;

float setPoint = 72.0;     //Target Temperature
int cycleTime = 60;    //How long the heat runs when called.
int senseTime = 550;   //How long to wait between heat cycles.

void setup() {
  delay(2000);
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);

  dht.begin();
  lcd.begin(16, 2);
  lcd.createChar(0, f);
  lcd.createChar(1, rh);
  lcd.createChar(2, l);
  lcd.createChar(3, a);
  lcd.createChar(4, g);
  lcd.setCursor(0, 0);
  lcd.print(dht.readTemperature());
  lcd.print(" ");
  lcd.print(getAverage());
}



void loop() {
  
  for(int i = 0; i < 99; i++){
    x = analogRead (0);
    if (x < 60) {
      setPoint = 72.0;
    }
    else if (x < 200) {
      setPoint = setPoint + 0.1;
      if(setPoint > 74.0){
        setPoint = 74.0;
      }
      lcdPrintTop();
      lcd.setCursor(0,0);
      lcd.write(byte(4));
      lcd.write("Fuck Yourself");
      lcd.write(byte(4));
      lcd.write(" ");
      delay(250);
    }
    else if (x < 400){
      setPoint = setPoint - 0.1;
      if(setPoint < 68.0){
        setPoint = 68.0;
      }
      lcdPrintTop();
      delay(250);
    }
    else if (x < 600){
      cycleHeat();
    }
    else if (x < 800){
      lcdPrintData();
      delay(5000);
    }
    delay(50);
  } 
  updateTemp();
  lcdPrintTop();
  lcdPrintIdle();
  delay(50);

  if(getAverage() < (setPoint - 0.2)){
    cycleHeat();
  }
}



void cycleHeat(){
  digitalWrite(12, HIGH);
  cycleCount = cycleCount + 1;
  for(int i = cycleTime; i > 0; i=i-5){
    updateTemp();
    lcdPrintTop();
    lcdPrintHeat(i);
    delay(5000);
  }
  digitalWrite(12, LOW);
  for(int i = senseTime; i > 0; i=i-5){
  updateTemp();
  lcdPrintTop();
  lcdPrintSense(i);
  delay(5000);
  }
  updateSetAverage();
  updateLongAverage();
  if(getAverage() < (setPoint - 0.2)){
    senseTime = senseTime - 5;
  }
  else{
    senseTime = senseTime + 5;
  }
}




//-Temp-------------------------------------------------------------------

float readTemp(){
  return dht.readTemperature()*1.8 + 32 - fixT;
}
int readHumidity(){
  int i = dht.readHumidity();
  int m = i % 5;
  if (m > 2){
    return (i - m + 5);
  }
  else{
    return (i - m);
  }
}

//-Queue------------------------------------------------------------------

float getAverage(){
  float s = 0;
  for(int i = 0; i < 10; i = i + 1){
    s = s + tempdata[i];
  }
  return s / 10;
}
void updateTemp(){
  for(int i = 9; i > 0 ; i = i - 1){
    tempdata[i] = tempdata[i-1];
  }
  tempdata[0] = readTemp();
}


void updateLongAverage(){
  longCount = longCount + 1;
  longAverage = (((longCount-1) * longAverage) + getAverage())/longCount;
}

void updateSetAverage(){
  setCount = setCount + 1;
  setAverage = (((setCount-1) * setAverage) + setPoint)/setCount;
}

//-Display----------------------------------------------------------------

void lcdPrintTop(){ 
  lcd.setCursor(0, 0);
  lcd.print(getAverage());
  lcd.setCursor(4, 0);
  lcd.write(byte(0));
  lcd.print(" ");
  lcd.print(readHumidity());
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.print("H-");
  lcd.print(setPoint);
}
void lcdPrintIdle(){
  lcd.setCursor(0, 1);
  lcd.write("Idle                  ");
}
void lcdPrintHeat(int t){
  lcd.setCursor(0, 1);
  lcd.print("Heating - ");
  lcd.print(t);
  lcd.print("s  ");
}
void lcdPrintSense(int t){
  lcd.setCursor(0, 1);
  lcd.print("Sensing... ");
  lcd.print(t);
  lcd.print("s  ");
}
void lcdPrintData(){
  lcd.setCursor(0, 0);
  lcd.print(cycleCount);
  lcd.print("cycles - ");
  lcd.print(senseTime);
  lcd.print("s    ");
  lcd.setCursor(0, 1);
  lcd.print("SA:");
  lcd.print(setAverage);
  lcd.setCursor(7, 1);
  lcd.print(" ");
  lcd.print("LA:");
  lcd.print(longAverage);
  lcd.setCursor(15, 1);
  lcd.print(" ");
}