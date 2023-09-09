#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RE 8
#define DE 7
#define sensor A0
#define wet 360
#define dry 640
#define ONE_WIRE_BUS 5
#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


const byte nitro_inquiry_frame[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos_inquiry_frame[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota_inquiry_frame[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};
byte values[11];
SoftwareSerial modbus(2,3);
int serval=4800;


RF24 radio(9, 10); // CE, CSN on Blue Pill
const byte address[6] = "zulas";

struct MyVariable
{
  byte soilmoisturepercent;
  byte nitrogen;
  byte phosphorous;
  byte potassium;
  byte  temperature;
};
MyVariable variable;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  radio.begin();                  //Starting the Wireless communication
  radio.openWritingPipe(address); //Setting the address where we will send the data
  radio.setPALevel(RF24_PA_MIN);  //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.stopListening();          //This sets the module as transmitter

  modbus.begin(serval);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  sensors.begin();
  variable.temperature=0;

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  delay(2000);
  
  

}

void loop() {

  sensors.requestTemperatures();
  variable.temperature = sensors.getTempCByIndex(0);
  int val=analogRead(sensor);
  Serial.println(val);
  variable.soilmoisturepercent = map(val, wet, dry, 100, 0);
 
  variable.nitrogen = nitrogen();
  delay(250);
  variable.phosphorous = phosphorous();
  delay(250);
  variable.potassium = potassium();
  delay(250);



  Serial.print("Soil Moisture: ");
  Serial.print(variable.soilmoisturepercent);
  Serial.println("%");
 
  Serial.print("Nitrogen: ");
  Serial.print(variable.nitrogen);
  Serial.println(" mg/kg");
  Serial.print("Phosphorous: ");
  Serial.print(variable.phosphorous);
  Serial.println(" mg/kg");
  Serial.print("Potassium: ");
  Serial.print(variable.potassium);
  Serial.println(" mg/kg");
 
  Serial.print("Temperature: ");
  Serial.print(variable.temperature);
  Serial.println("*C");
 
  Serial.println();
  radio.write(&variable, sizeof(MyVariable));
  Serial.println("Data Packet Sent");
  Serial.println("");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("N: ");
  display.print(variable.nitrogen);
  display.print("  P: ");
  display.println(variable.phosphorous);
  display.print("K: ");
  display.print(variable.potassium);
  display.println(" (mg/kg)");
  display.print("T: ");
  display.print(variable.temperature);
  display.println(" *C");
  display.print("M: ");
  display.print(variable.soilmoisturepercent);
  display.print(" %");
  //display.println("N: %f mg/kg \nP: %f mg/kg\nK: %f mg/kg\nT: %f C\nM: %f %%",variable.nitrogen,variable.phosphorous,variable.potassium,variable.temperature,variable.soilmoisturepercent);
  display.display();

  delay(2000);
  display.clearDisplay();
}



byte nitrogen(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(modbus.write(nitro_inquiry_frame,sizeof(nitro_inquiry_frame))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    // When we send the inquiry frame to the NPK sensor, then it replies with the response frame
    // now we will read the response frame, and store the values in the values[] arrary, we will be using a for loop.
    for(byte i=0;i<7;i++){
    //Serial.print(modbus.read(),HEX);
    values[i] = modbus.read();
    //Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4]; // returns the Nigtrogen value only, which is stored at location 4 in the array
}
 
byte phosphorous(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(modbus.write(phos_inquiry_frame,sizeof(phos_inquiry_frame))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(modbus.read(),HEX);
    values[i] = modbus.read();
   // Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
 
byte potassium(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(10);
  if(modbus.write(pota_inquiry_frame,sizeof(pota_inquiry_frame))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
    for(byte i=0;i<7;i++){
    //Serial.print(modbus.read(),HEX);
    values[i] = modbus.read();
    //Serial.print(values[i],HEX);
    }
    Serial.println();
  }
  return values[4];
}
