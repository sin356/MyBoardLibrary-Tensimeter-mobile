/*************************************************************************************
 *  Created By: Tauseef Ahmad
 *  Created On: 15 April, 2023
 *  
 *  Tutorial: https://youtu.be/lYN8hqNAKbU
 *  My Channel: https://www.youtube.com/channel/UCOXYfOHgu-C-UfGyDcu5sYw/
 ***********************************************************************************/
 
 #include <SIM800L.h>
#include <DHT.h>

SIM800L sim800l(2, 3); //Rx, Tx

//Enter the phone number of the person whom you want to send sms
String PHONE = "+923001234567";

#define DHTPIN 4     	// DHT11 data pin
#define DHTTYPE DHT11   // DHT11 sensor type
DHT dht(DHTPIN, DHTTYPE);

void handleSMS(String number, String message) {
  Serial.println("number: " + number + "\nMessage: " + message);
}

void handleCall(String number) {
  Serial.println("New call from " + number);
}

void setup() {
  Serial.begin(9600);
  
  sim800l.begin(9600);
  
  dht.begin();
  
  sim800l.setSMSCallback(handleSMS);
  sim800l.setCallCallback(handleCall);
}

void loop() {
  sim800l.listen();
  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read data from DHT11 sensor");
    return;
  }
  
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  
  String Message = "Temperature: " +temperature+ " °C, Humidity: " +humidity+ " %";
  sim800l.sendSMS(PHONE, Message);
  
  delay(10000);
}