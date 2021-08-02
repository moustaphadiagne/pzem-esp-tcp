#include<WiFiClient.h>
#include<ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>
#include <InfluxDb.h>


PZEM004Tv30 pzem(D5, D6);	//initialisation objet premier pzem GPIO 14 GPIO 12
PZEM004Tv30 pzemm(D7, D4); 	// deuxieme pzem GPIO 13 GPIO 2
PZEM004Tv30 pzemmm(D1, D2);	//troisieme pzem GPIO 5 GPIO 4
const int buttonPin = 15;    //selection gpio 15 ou pin D8 pour le conatct de defaut
int etatdefaut = 0;          //initialisation de la variable qui contient l etat du contact
int defaut = 0;   //pour RAS OU DEFAUT THERMIQUE 
int running = 0;   //pour marche/arret
int defaut_now= 0;   //Variable temoin pour verifier changement d etat defaut
int running_now= 0;	//Variable temoin pour verifier changement d etat marche/arret
int debit=0;     //debit en m3/heures
float debits=0;  //debit en m3/secondes
unsigned int timer=0;  //conteur du temps de marche de la pompe
int h, m, s;
String nom_pompe="KMASSAR2";

#define INFLUXDB_HOST "185.98.128.237"   //adresse ip serveur grafana/influxdb
#define INFLUXDB_PORT 48086    //port du serveur
#define WIFI_SSID "RUT955_TENEZIS"              //ssid rtu
#define WIFI_PASS "1234567890"  //mot de passe rtu
Influxdb influx(INFLUXDB_HOST,INFLUXDB_PORT);  //requete de connexion

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);  //configuration pin en mode entree
  Serial.print("Connexio a .... ");
  Serial.println(WIFI_SSID);  //affichage du nom du ssid sur la console
  WiFi.begin(WIFI_SSID,WIFI_PASS ); //connexion au rtu
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); //affichage de l adresse ip de l esp


   long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);  //affichage de la puissance du signal en dbm
    Serial.println(" dBm");
  
influx.setDb("bnsp");  //connexion a la base de donnees 

  Serial.println("configuration terminee.");

}
int loopCount = 0;  //compteur qui ne depasse pas 40 secondes sans envoyer les donnees
float volume=0;  //volume en m3
void loop() {

etatdefaut = digitalRead(buttonPin);  //lecture de l etat du contact de defaut et recuperation de la variable
if (etatdefaut == HIGH) {
    // il y a defaut thermique
 Serial.println("defaut");
 defaut = 0;  
  } else {
    // tout est normal
 Serial.println("normal");
  defaut = 1;
  }
Serial.print("Volume: "); Serial.print(volume); Serial.println("m3");  //affichage du volume dans la boucle

    float voltage = pzem.voltage();  //tension1
    if( !isnan(voltage) ){
        Serial.print("Voltage1: "); Serial.print(voltage); Serial.println("V");
    } else {
        Serial.println("Error reading voltage");
    }

 float voltage2 = pzemm.voltage();  //tension2
    if( !isnan(voltage2) ){
        Serial.print("Voltage2: "); Serial.print(voltage2); Serial.println("V");
    } else {
        Serial.println("Error reading voltage");
    }
    float voltage3 = pzemmm.voltage();  //tension3
    if( !isnan(voltage2) ){
        Serial.print("Voltage3: "); Serial.print(voltage3); Serial.println("V");
    } else {
        Serial.println("Error reading voltage");
    }

    float current = pzem.current();  //intensite 1
    if( !isnan(current) ){
        Serial.print("Current: "); Serial.print(current); Serial.println("A");
    } else {
        Serial.println("Error reading current");
    }

 float current2 = pzemm.current();  //intensite 2
    if( !isnan(current2) ){
        Serial.print("Current2: "); Serial.print(current2); Serial.println("A");
    } else {
        Serial.println("Error reading current");
    }
 float current3 = pzemmm.current();  //intensite 3
    if( !isnan(current3) ){
        Serial.print("Current3: "); Serial.print(current3); Serial.println("A");
    } else {
        Serial.println("Error reading current");
    }


    float power = pzem.power();  //puissance1
    if( !isnan(power) ){
        Serial.print("Power: "); Serial.print(power); Serial.println("W");
    } else {
        Serial.println("Error reading power");
    }

 float power2 = pzemm.power();  //puissance2
    if( !isnan(power2) ){
        Serial.print("Power2: "); Serial.print(power2); Serial.println("W");
    } else {
        Serial.println("Error reading power");
    }
     float power3 = pzemmm.power();  //puissance3
    if( !isnan(power3) ){
        Serial.print("Power3: "); Serial.print(power3); Serial.println("W");
    } else {
        Serial.println("Error reading power");
    }

    float energy = pzem.energy();  //energie1
    if( !isnan(energy) ){
        Serial.print("Energy: "); Serial.print(energy,3); Serial.println("kWh");
    } else {
        Serial.println("Error reading energy");
    }
    float energy2 = pzemm.energy();  //energie2
    if( !isnan(energy2) ){
        Serial.print("Energy2: "); Serial.print(energy2,3); Serial.println("kWh");
    } else {
        Serial.println("Error reading energy");
    }
float energy3 = pzemmm.energy();  //energie3
    if( !isnan(energy3) ){
        Serial.print("Energy3: "); Serial.print(energy3,3); Serial.println("kWh");
    } else {
        Serial.println("Error reading energy");
    }


    float frequency = pzem.frequency();  //frequence1
    if( !isnan(frequency) ){
        Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
    } else {
        Serial.println("Error reading frequency");
    }
    float frequency2 = pzemm.frequency();  //frequence2
    if( !isnan(frequency2) ){
        Serial.print("Frequency2: "); Serial.print(frequency2, 1); Serial.println("Hz");
    } else {
        Serial.println("Error reading frequency");
    }
     float frequency3 = pzemmm.frequency();  //frequence3
    if( !isnan(frequency3) ){
        Serial.print("Frequency3: "); Serial.print(frequency3, 1); Serial.println("Hz");
    } else {
        Serial.println("Error reading frequency");
    }


    float pf = pzem.pf();  //facteur de puissance 1
    if( !isnan(pf) ){
        Serial.print("PF: "); Serial.println(pf);
    } else {
        Serial.println("Error reading power factor");
    }
     float pf2 = pzemm.pf();  //facteur de puissance 2
    if( !isnan(pf2) ){
        Serial.print("PF2: "); Serial.println(pf2);
    } else {
        Serial.println("Error reading power factor");
    }
 float pf3 = pzemmm.pf();    //facteur de puissance 3
    if( !isnan(pf3) ){
        Serial.print("PF3: "); Serial.println(pf3);
    } else {
        Serial.println("Error reading power factor");
    }

    


  if (current > 11){  //si l intensite 1 est superieur a 11 ampere donc pompe en marche.
    Serial.println("marche");
    running =1;
    timer++;
    
    }else{
      Serial.println("arret");
      running =0;
      }

//si changement etat  defaut/ras ou changement etat marche/arret ou 40 secondes ecoules on envoie au serveur grafane
if (loopCount >= 40 || defaut != defaut_now || running != running_now ){
 InfluxData row("pompage");
  row.addTag("nom_pompe", "KMASSAR1");
  row.addValue("voltage", voltage);
  row.addValue("voltage2", voltage2);
  row.addValue("voltage3", voltage3);
  row.addValue("Intensite", current);
  row.addValue("Intensite2", current2);
  row.addValue("Intensite3", current3);
  row.addValue("vpuissance", power);
  row.addValue("energie", energy);
  row.addValue("frequence", frequency);
  row.addValue("facteur", pf);
  row.addValue("defaut", defaut);
  row.addValue("running", running);
  row.addValue("volume", volume);
  row.addValue("timer", timer);
influx.write(row);    //ecriture dans la base de donnees

if (running == 0){  //si la machine s est arrete on reinitialise le temps de marche et le volume
    timer=0;
    volume=0;
    }
  Serial.println(defaut);
  Serial.println(running);
  defaut_now=defaut;  //variable temoins
  running_now=running;  //variable temoins
  loopCount=0;  //initialisation compteur temps apres envoie
  }
//  debit en fonction de l intensite
if (current >= 10 && current < 11) {
  debit=0;
  debits=0.0;
}
else if (current >= 11 && current < 12) {
  debit=69;
  debits=0.01916;
}
else if (current >= 12 && current < 13) {
  debit=138;
  debits=0.0383;
}
else if (current >= 13 && current < 14) {
  debit=207;
  debits=0.0575;
}
else if (current >= 14 && current < 15) {
  debit=278;
  debits=0.0772;
}
else if (current >= 15 && current < 16) {
  debit=347;
  debits=0.0963;
}
else if (current >= 16 && current < 17) {
  debit=416;
  debits=0.1155;
}
else if (current >= 17 && current < 18) {
  debit=485;
  debits=0.1347;
}
else if (current >= 18 && current < 19) {
  debit=554;
  debits=0.1538;
}
else if (current >= 19 && current < 23) {
  Serial.println("surintensite");
  debit=554;
  debits=0.1538;
}
else {
  Serial.println("arret");
  debit=0;
  debits=0;
}
volume+=debits;   //integration des debits m3/s

    delay(1000);  //pause d une seconde
    loopCount++;  //incrementation boucle
    
}