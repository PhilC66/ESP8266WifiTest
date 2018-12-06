/*
    Creation d'une fonction de connexion Wifi à partir d'une liste SSID/PWD connu
		fichier "credentials_xxxx.h" selon format:
		const char ssid[][13]  = {"sssid1","ssid2","ssid3","ssid4","ssid5" };
		const char pwd [][21]  = {"pwd1","pwd2","pwd3","pwd4","pwd5"};
		byte nssid = 5;	// nombre de reseaux
*/
#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <credentials_home.h>

long timer ;
#define LedBleue 2

String adressemac;
String adresseip;
String ssidconnected;							// apres connxion wifi contient le ssid+nbr de tentative de cnx

void setup() {
	Serial.begin(115200);
	pinMode(BUILTIN_LED,OUTPUT);
	pinMode(LedBleue	 ,OUTPUT);
	digitalWrite(BUILTIN_LED,HIGH);
	digitalWrite(LedBleue,HIGH);

	// Set WiFi to station mode and disconnect from an AP if it was previously connected
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);

	Serial.println(F("Setup done"));
}

void loop() {

	if(!connectWIFI()){	// connexion Wifi, si pas de Wifi dispo retour deepsleep
	Serial.println(F("pas de connexion Wifi retour sleep"));
	}
	else{
		Serial.print(F("connexion Wifi ok ")),Serial.println(ssidconnected);
		digitalWrite(BUILTIN_LED,LOW);
	}

	for(int i=0; i<5;i++){// Wait a bit before scanning again
		digitalWrite(LedBleue,HIGH);
		delay(500);
		digitalWrite(LedBleue,LOW);
		delay(500);
	}
	digitalWrite(LedBleue,HIGH);

	WiFi.disconnect();
	digitalWrite(BUILTIN_LED,HIGH);
}

boolean connectWIFI(){
	Serial.println(F("scan start"));
	int ssidnbr = 0;
	int ssiddispo[nssid];
	int n = WiFi.scanNetworks();
	Serial.print(F("scan done : "));
	if (n == 0) {
		Serial.println(F("no networks found"));
		return false;
	} else {
		Serial.print(n);
		Serial.println(F(" networks found"));
		for (int i = 0; i < n; ++i) {
			// Print SSID and RSSI for each network found
			Serial.print(i);
			Serial.print(": ");
			Serial.print(WiFi.SSID(i));
			Serial.print(" (");
			Serial.print(WiFi.RSSI(i));
			Serial.print(")");
			Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
			delay(10);
		}
	}
	for(byte i = 0; i < nssid; i ++){	// cherche reseau connu parmi present
		for(byte j = 0; j < n; j ++){
			int rc = strcmp(WiFi.SSID(j).c_str(), ssid[i]);
			if (rc == 0){
				ssiddispo[ssidnbr] = i;	// liste reseau connu
				ssidnbr ++;							// Qte reseau connu
				Serial.print(F("ssidnbr = ")),Serial.println(ssidnbr);
			}
		}
	}
	for(int i = 0; i < ssidnbr; i ++){
		Serial.print(F("SSID dispo  ")),Serial.println(ssid[ssiddispo[i]]);
	}
	byte retries = 0;
	byte i = 0;
	do{
		WiFi.begin(ssid[ssiddispo[i]], pwd[ssiddispo[i]]);
		Serial.print(F("connexion à ")),Serial.println(ssid[ssiddispo[i]]);
		while (WiFi.status() != WL_CONNECTED) {
			delay(1000);
			Serial.print(F("."));
			retries++;
			if(retries > 25){
				retries = 0;
				//break;
				if(i < ssidnbr){
					i++;
					Serial.println(i);
					break;
				}
				else{
					Serial.println(F("tous les reseaux balayés sans connexion reussie"));
					return false;	// tous les reseau balayés sans connexion reussie
				}
			}
		}
		
	}while (WiFi.status() != WL_CONNECTED);

	adressemac     = WiFi.macAddress();
	adresseip      = WiFi.localIP().toString();
	ssidconnected  = String(WiFi.SSID());
	ssidconnected += "-";
	ssidconnected += String(retries);

	Serial.print(F("WiFi connected, ")),Serial.println(ssidconnected);
	Serial.print(F("IP  address: ")),   Serial.println(adresseip);
	Serial.print(F("MAC address: ")),   Serial.println(adressemac);
	return true;		
}