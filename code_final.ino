#include<ESP8266WiFi.h>  //importation de bibliotheques
#include<WiFiClient.h>
#include <WiFiUDP.h>  //pour udp
#include<ESP8266WebServer.h> //serveur webs
#include <SoftwareSerial.h>

#include <PZEM004Tv30.h> //pour capteur pzem
#include <InfluxDb.h>  //transferer influx db via tcp

/* Use software serial for the PZEM
 * Pin 11 Rx (Connects to the Tx pin on the PZEM)
 * Pin 12 Tx (Connects to the Rx pin on the PZEM)
*/
PZEM004Tv30 pzem(D5, D6);  //instanciation objet pzem avec d5 et d6
const int buttonPin = 4;  // gpio 4 
const int marchePin = 5; //gpio 5
int etatdefaut = 0;
int etatmarche = 0;
int defaut = 0;  //variable retour du defaut
int running = 0;  //variable retour de la position
int defaut_now= 0;  //derniere valeur avant boucle
int running_now= 0;  //idem
String nom_pompe="KMASSAR2";
String line;

//const char* ssid = "RUT955_TENEZIS";//Replace with your network SSID
//const char* password = "1234567890";//Replace with your network password
//funbox KH7dSUAT9fKb4HTjpY
//const char* ssid = "FUNBOX_D2F3";
//const char* password = "KH7dSUAT9fKb4HTjpY";

#define INFLUXDB_HOST "185.98.128.237"   //adresse ip raspberry public
#define INFLUXDB_PORT 48086  //numero de port serveur raspberry public
#define WIFI_SSID "RUT955_TENEZIS"              //ssid rtu
#define WIFI_PASS "1234567890"  //mot de passe rtu
Influxdb influx(INFLUXDB_HOST,INFLUXDB_PORT);
WiFiServer server(80);
//byte host[] = {192, 168, 1, 5};
//byte host[] = {185, 98, 128, 237};
//int port = 48086;
//int port = 8086;
//WiFiUDP udp;

void setup() {
  Serial.begin(9600); //monitaur avec 9600 baud
  pinMode(buttonPin, INPUT);  //d1 en entree
  pinMode(marchePin, INPUT);  //d2 en entree
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID,WIFI_PASS );  // connection avec wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  //affiche adresse ip esp
  

   long rssi = WiFi.RSSI();  //puissance signal
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
  //server.begin();

  influx.setDb("bnsp");  //base de donnees influx db

  Serial.println("Setup Complete.");

}
int loopCount = 0;  //compteur d initialisation
void loop() {
 
etatdefaut = digitalRead(buttonPin);  //lecture etat d2 ou gpio 4
if (etatdefaut == HIGH) {
    // turn LED on
 Serial.println("defaut");
 defaut = 0;  //variable de retour
  } else {
    // turn LED off
 Serial.println("normal");
  defaut = 1;  //variable de retour
  }


etatmarche = digitalRead(marchePin);   //lecture etat d1 ou gpio 5
if (etatmarche == HIGH) {
    // turn LED on
 Serial.println("marche");
 running = 1;  //variable de retour
  } else {
    // turn LED off
 Serial.println("arret");
  running = 0;  //variable de retour
  }

  
    float voltage = pzem.voltage();
    if( !isnan(voltage) ){
        Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
    } else {
        Serial.println("Error reading voltage");
    }

    float current = pzem.current();
    if( !isnan(current) ){
        Serial.print("Current: "); Serial.print(current); Serial.println("A");
    } else {
        Serial.println("Error reading current");
    }

    float power = pzem.power();
    if( !isnan(power) ){
        Serial.print("Power: "); Serial.print(power); Serial.println("W");
    } else {
        Serial.println("Error reading power");
    }

    float energy = pzem.energy();
    if( !isnan(energy) ){
        Serial.print("Energy: "); Serial.print(energy,3); Serial.println("kWh");
    } else {
        Serial.println("Error reading energy");
    }

    float frequency = pzem.frequency();
    if( !isnan(frequency) ){
        Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
    } else {
        Serial.println("Error reading frequency");
    }

    float pf = pzem.pf();
    if( !isnan(pf) ){
        Serial.print("PF: "); Serial.println(pf);
    } else {
        Serial.println("Error reading power factor");
    }

    Serial.println();

//line = String("pompage,nom_pompe="+nom_pompe+" voltage="+voltage+",current="+current+",power="+power+",energy="+energy+",frequency="+frequency+",pf="+pf );
//Serial.println(line);
  //udp.beginPacket(host, port);
  //udp.print(line);
  //udp.endPacket();

//on envoie au serveur  soit a chaque 2 minutes ou a chaque changement d etat de la position ou du defaut
if (loopCount >= 60 || defaut != defaut_now || running != running_now ){
 InfluxData row("pompage");
  row.addTag("nom_pompe", "KMASSAR1");
  //row.addTag("Sensor", "Temp");
  //row.addTag("Unit", "Celsius");
  row.addValue("voltage", voltage);
  row.addValue("Intensite", current);
  row.addValue("vpuissance", power);
  row.addValue("energie", energy);
  row.addValue("frequence", frequency);
  row.addValue("facteur", pf);
  row.addValue("defaut", defaut);
  row.addValue("running", running);
 

  influx.write(row);  //ecriture dans la base

  Serial.println(defaut);
  Serial.println(running);
  defaut_now=defaut;  //stockage des dernieres valeurs avant la boucle
  running_now=running; //idem
  loopCount=0;
  }
 // loopCount=0;
   // }
    delay(2000);
    loopCount++;
    //defaut_now=defaut;
    //running_now=running;
}
