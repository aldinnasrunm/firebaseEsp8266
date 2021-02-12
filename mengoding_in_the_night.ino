#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>


#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>  

#define FIREBASE_HOST "eggcubator-d446f-default-rtdb.firebaseio.com" //Without http:// or https:// schemes
#define FIREBASE_AUTH "4KYOs3lhfrbQdWKqAu9P5JkMgTtn6nwUV1WqC07i"
#define WIFI_SSID "WIFI_HOME"
#define WIFI_PASSWORD "sidorejo"


#define DHTPIN 2  //pin GPIO 2 aka D4
#define RELAY 0 //pin GPIO 3 aka D3
#define DHTTYPE    DHT11 //define seri DHT

DHT dht(DHTPIN, DHTTYPE);

FirebaseData fbdo; //Firebase data untuk menyimpan data
FirebaseData firebaseData;


void setup()
{

  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY,OUTPUT); 
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


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //Enable auto reconnect the WiFi when connection lost
  Firebase.reconnectWiFi(true);

}

void sensorUpdate()
{
// Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("C  ,"));
  Serial.print(f);
  Serial.println(F("F  "));

  if (Firebase.setFloat(firebaseData, "/FirebaseIOT/temperature", t))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  if (Firebase.setFloat(firebaseData, "/FirebaseIOT/humidity", h))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }  
}

void loop()
{
  sensorUpdate();
  initRelay();
  initLed();
}


//inisialisasi relay
void initRelay(){
    if(Firebase.getInt(fbdo, "/LED_Status"))
  {
    //Success
    if(fbdo.intData() == 1){
      digitalWrite(RELAY,LOW);
      Serial.println("Relay On");
      }else{
        digitalWrite(RELAY,HIGH);
       Serial.println("Relay off");
      }

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in getInt, ");
    Serial.println(fbdo.errorReason());
  }
  
  }

void initLed(){
  //alien
   if(Firebase.getInt(fbdo, "/LED_Status"))
  {
    //Success
    if(fbdo.intData() == 1){
      digitalWrite(LED_BUILTIN,LOW);
      }else{
        digitalWrite(LED_BUILTIN,HIGH);
      }
    Serial.print("Get int data success, int = ");
    Serial.println(fbdo.intData());

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in getInt, ");
    Serial.println(fbdo.errorReason());
  }
}
