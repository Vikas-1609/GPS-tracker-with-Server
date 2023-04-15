#include <ThingSpeak.h>

#include <FirebaseESP8266.h>  
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h> 
WiFiClient client;
#define FIREBASE_HOST "ENTER FIREBASE REALTIME DATABASE URL"
#define FIREBASE_AUTH "ENTER FIREBASE SECRETS KEY"
#define WIFI_SSID "ENTER WIFI NAME"
#define WIFI_PASSWORD "ENTER WIFI PASSWORD"

unsigned long myChannelNumber = ENTER CHANNEL NUMBER;
const char * myWriteAPIKey = "ENTER THINKSPEAK API KEY";

//-----------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------
//Define FirebaseESP8266 data object
FirebaseData firebaseData;

FirebaseJson json;
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//GPS Module RX pin to NodeMCU D1
//GPS Module TX pin to NodeMCU D2
const int RXPin = 3, TXPin = 1;
SoftwareSerial neo6m(RXPin, TXPin);
TinyGPSPlus gps;
//-----------------------------------------------------------------------------------


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void setup()
{

  Serial.begin(115200);

  neo6m.begin(9600);
  
  wifiConnect();

  Serial.println("Connecting Firebase.....");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Serial.println("Firebase OK.");
   ThingSpeak.begin(client);
  
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void loop() {
  
  smartdelay_gps(500);

  if(gps.location.isValid()) 
  {
    float latitude = gps.location.lat();
    float longitude = gps.location.lng();
    int speed = gps.speed.kmph();
    
    if(Firebase.setFloat(firebaseData, "ENTER DATABASE PATH", latitude))
      {print_ok();}
    else
      {print_fail();}
    //-------------------------------------------------------------
    if(Firebase.setFloat(firebaseData, "ENTER DATABASE PATH", longitude))
      {print_ok();}
    else
      {print_fail();}

       if(Firebase.setFloat(firebaseData, "ENTER DATABASE PATH", speed))
       {print_ok();}
    else
      {print_fail();}
   //-------------------------------------------------------------
    ThingSpeak.setField(1, latitude);
    ThingSpeak.setField(2, longitude);

    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
   
  }
  else
  {
    Serial.println("No valid GPS data found.");
  }
  
  delay(1000);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
static void smartdelay_gps(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (neo6m.available())
      gps.encode(neo6m.read());
  } while (millis() - start < ms);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void print_ok()
{
    Serial.println("------------------------------------");
    Serial.println("OK");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void print_fail()
{
    Serial.println("------------------------------------");
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

void firebaseReconnect()
{
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
