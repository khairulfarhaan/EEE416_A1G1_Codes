#define BLYNK_TEMPLATE_ID "TMPL6DGvl5oob"
#define BLYNK_DEVICE_NAME "Precision Agriculture"
#define BLYNK_TEMPLATE_NAME "Precision Agriculture"
#define BLYNK_AUTH_TOKEN "memKhat0TEcNEy67mmakg524nIWuXHUI"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const char* ssid = "Farhaan";
const char* password = "12345678";

RF24 radio(4, 5); 
const byte address[6] = "zulas";

BlynkTimer timer;
 
struct MyVariable
{
  byte soilmoisturepercent;
  byte nitrogen;
  byte phosphorous;
  byte potassium;
  byte temperature;
};
MyVariable variable;
 
WiFiClient client;


void readData()
{

  if(recvData())
  {
 
  Serial.println("Data Received:");
  
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

  Blynk.virtualWrite(V0, variable.nitrogen);
  Blynk.virtualWrite(V1, variable.phosphorous);
  Blynk.virtualWrite(V2, variable.potassium);
  Blynk.virtualWrite(V3, variable.temperature);
  Blynk.virtualWrite(V4, variable.soilmoisturepercent);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  radio.begin();                  //Starting the Wireless communication
  radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.startListening();              //This sets the module as receiver
  
  Serial.println("Receiver Started....");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.println();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  timer.setInterval(1000L, readData);
}


int recvData()
{
  if ( radio.available() ) 
  {
    radio.read(&variable, sizeof(MyVariable));
    return 1;
    }
    return 0;
}

void loop() {
  // put your main code here, to run repeatedly:

  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  
  
}
