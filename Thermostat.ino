
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal.h>

DHT dht(13,DHT11);
LiquidCrystal lcd(8,9,4,5,6,7);

byte f[] = {0x08,0x14,0x08,0x07,0x04,0x07,0x04,0x04}; //Degrees F character
byte rh[] = {0x12,0x04,0x08,0x12,0x00,0x05,0x07,0x05}; //Humidity character
byte l[] = {0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04}; //Divider character

int TempF;             //Temperature in Fahrenheit (Nearest Degree)
int HumR;              //Relative Humidity (Nearest 5%)
int fixT = 1.3;          //Temperature value offset for inaccurate sensor

int setPoint = 73;     //Target Temperature
int cycleTime = 60;    //How long the heat runs when called.
int senseTime = 550;   //How long to wait between heat cycles.

int cycleCount;        //Data recording - number of heat cycles since last restart

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
  lcdPrintOff();
}

void loop() {
  readSensors();
  if(TempF <= (setPoint - 2)){
    cycleHeat();
  }
 int x;
 x = analogRead (0);
  if (x > 60 && x < 200) {
    setPoint = 75;
    if (setPoint > 75){
      setPoint = 75;
      lcd.print ("Go Fuck Yourself "); 
    }
  }
  else if (x < 400){
    setPoint = 68;
  }
  else if (x < 600){
    setPoint = 73;
  }
  else if (x < 800){
    cycleHeat();
  }
  delay(5000);
}


/* Heating Cycle --------------------------------------------------------------------------------*/
void cycleHeat(){
  digitalWrite(12, HIGH); 
  for(int i = cycleTime; i > 0; i=i-5) {
    lcdPrintOn(i);
    delay(5000);
  }
  cycleCount = cycleCount + 1;
  digitalWrite(12, LOW); 
  lcdPrintOff();
  senseHeat(senseTime);
}
void senseHeat(int t){
  for(int i = t; i > 0; i--) {    
    readSensors();
    lcdPrintSense(i);
    delay(1000);
  }
  lcdPrintOff();
} 

/* Sensor Reading -------------------------------------------------------------------------------*/

void readSensors(){
  lcdPrintColon();
  TempF = checkTemperature();
  HumR = checkHumidity();
  delay(200);
  lcdPrintTop();
}
int checkHumidity(){
  int i = dht.readHumidity();
  int m = i % 5;
  if (m > 2){
    return (i - m + 5);
  }
  else{
    return (i - m);
  }
}
int checkTemperature(){
  return dht.readTemperature()*1.8 + 32 - fixT;
}

/* LCD Writing ----------------------------------------------------------------------------------*/

void lcdPrintTop(){ 
  lcd.setCursor(0, 0);
  lcd.print(TempF);
  lcd.write(byte(0));
  lcd.print(" ");
  lcd.print(HumR);
  lcd.write(byte(1));
  lcd.write(byte(2));
  lcd.print("Heat ");
  lcd.print(setPoint);
  lcd.write(byte(0));
}
void lcdPrintOn(int t){
  lcd.setCursor(0, 1);
  lcd.print("Heat On - ");
  lcd.print(t);
  lcd.print("s  ");
}
void lcdPrintSense(int t){
  lcd.setCursor(0, 1);
  lcd.print("Sensing... ");
  lcd.print(t);
  lcd.print("s  ");
}
void lcdPrintOff(){
  lcd.setCursor(0, 1);
  lcd.print("Idle - ");
  lcd.print(int(cycleCount));
  lcd.print(" cycles");
}
void lcdPrintColon(){
  lcd.setCursor(3,0);
    lcd.print(":");
}