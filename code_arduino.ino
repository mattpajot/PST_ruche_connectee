// include the SoftwareSerial library so you can use its functions:
#include <SoftwareSerial.h>

//Include pour les capteurs d'humidité et de température
#include "DHT.h"


//température
#define DHTPIN 4
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);
//masse
#include "HX711.h" //lib pour la masse
//Valeurs obtenues avec le code de calibrage
#define rapport3 107800 //Valeur etablie pour la tare a 0. donc ne pas mettre de poids sur la ruche sinon tare faussee
#define rapport1 107600
#define rapport2 107500
#define rapport4 108650
#define data1  3
#define data2  5
#define data3  6
#define data4  9
#define horloge  2
// identification des capteurs
HX711 capteur1;
HX711 capteur2;
HX711 capteur3;
HX711 capteur4;
//fin masse

//pin pour module sigfox
#define rxPin 10
#define txPin 11



// set up a new serial port
SoftwareSerial Sigfox =  SoftwareSerial(rxPin, txPin);

//Set to 0 if you don't need to see the messages in the console
#define DEBUG 1

//Messages buffer
uint8_t msg[12];

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  //masse:
  capteur1.begin(data1, horloge);
  capteur1.set_scale(rapport1);  //on set le rapport de calibration
  capteur1.tare(); //On tare au premier demarrage
  
  capteur2.set_scale(rapport2);
  capteur2.begin(data2, horloge);
  capteur2.tare();

  capteur3.set_scale(rapport3);
  capteur3.begin(data2, horloge);
  capteur3.tare();
  
  capteur4.set_scale(rapport4);
  capteur4.begin(data2, horloge);
  capteur4.tare();

  //finmasse
  if(DEBUG){
    Serial.begin(9600);
  }
  //Humidite et temperature
  dht.begin();
  // open Wisol communication
   // define pin modes for tx, rx:
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Sigfox.begin(9600);
  delay(100);
  Sigfox.print("AT\r");
  getID();
  delay(100);
  getPAC();
}

// the loop function runs over and over again forever
void loop() {
  
  // masse
  
  float massefinale = mesure(); //on collecte la masse après
  int m_entier;
  int m_deci;
  afficher(massefinale); // on affiche les donnees dans le moniteur serie
  for(int x=0; x<3;x++) //on effetue 3 mesures pour avoir une bonne valeur moyenne
  {
    massefinale = mesure(); // on appelle la fonction qui retourne la valeur de 10 mesures de masse
    m_entier = ENTIER(massefinale); // on separe la partie entiere de la masse pour ensuite l'envoyer par sigfox
    m_deci = DECIMAL(massefinale);  // on separe la partie decimale de la masse pour ensuite l'envoyer par sigfox
    afficher(massefinale);  //affichage dans le moniteur serie
    delay(1000); // pause pour les variations des capteurs
  }
  
  int tab_hutemp; //tableau de temperature
  float Temperature = temperature();  // on recupere la temperature
  int temp_e = ENTIER(Temperature); // on separe la partie entiere de la temperature pour ensuite l'envoyer par sigfox
  int temp_d = DECIMAL(Temperature); // on separe la partie decimale de la temperature pour ensuite l'envoyer par sigfox
  float humi = humidite(); //fonction qui retourne l'humidite
  int humi_e = ENTIER(humi); // on separe la partie entiere de l'humidite pour ensuite l'envoyer par sigfox
  int humi_d = DECIMAL(humi); // on separe la partie decimale de l'humidite pour ensuite l'envoyer par sigfox
    
 
  
  //finmasse
  //messages a envoyer.
  msg[0] = m_entier;
  msg[1] = m_deci;
  msg[2] = temp_e;
  msg[3] = temp_d;
  msg[4] = humi_e;
  msg[5] = humi_d;
  sendMessage(msg, 6);

  // In the ETSI zone, due to the reglementation, an object cannot emit more than 1% of the time hourly
  // So, 1 hour = 3600 sec
  // 1% of 3600 sec = 36 sec
  // A Sigfox message takes 6 seconds to emit
  // 36 sec / 6 sec = 6 messages per hours -> 1 every 10 minutes
  delay(900000); // tempo de 15 minutes (faire attention de ne pas dépasser le nombre de messages maximums à envoyer par jour)
}

void blink(){
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);
}

//Get Sigfox ID
String getID(){
  String id = "";
  char output;

  Sigfox.print("AT$I=10\r");
  while (!Sigfox.available()){
     blink();
  }

  while(Sigfox.available()){
    output = Sigfox.read();
    id += output;
    delay(10);
  }

  if(DEBUG){
    Serial.println("Sigfox Device ID: ");
    Serial.println(id);
  }

  return id;
}


//Get PAC number
String getPAC(){
  String pac = "";
  char output;

  Sigfox.print("AT$I=11\r");
  while (!Sigfox.available()){
     blink();
  }

  while(Sigfox.available()){
    output = Sigfox.read();
    pac += output;
    delay(10);
  }

  if(DEBUG){
    Serial.println("PAC number: ");
    Serial.println(pac);
  }

  return pac;
}


//Send Sigfox Message
void sendMessage(uint8_t msg[], int size)
{
  if(DEBUG){
    Serial.println("Inside sendMessage");
  }
  

  String status = "";
  String hexChar = "";
  String sigfoxCommand = "";
  char output;

  sigfoxCommand += "AT$SF=";
  
  for (int i = 0; i < size; i++)
  {
    hexChar = String(msg[i], HEX);
    Serial.println(hexChar);

    //padding
    if (hexChar.length() == 1)
    {
      hexChar = "0" + hexChar;
    }

    sigfoxCommand += hexChar;
  }

  if(DEBUG){
    Serial.println("Sending...");
    Serial.println(sigfoxCommand);

  }

  Sigfox.println(sigfoxCommand);


  while (!Sigfox.available())
  {
    if(DEBUG){
      Serial.println("Waiting for response");
    }
    
    delay(1000);
  }

  while (Sigfox.available())
  {
    output = (char)Sigfox.read();
    status += output;
    delay(10);
  }

  if(DEBUG){
    Serial.println();
    Serial.print("Status \t");
    Serial.println(status);
  }
  
}

  float mesure() //mesure de la masse totale
{
  float r1=0, r2=0, r3=0, r4=0, masse=0;

  for(int x=0; x<10;x++)
  {
    r1 += capteur1.get_units();
    r2 += capteur2.get_units();
    r3 += capteur3.get_units();
    r4 += capteur4.get_units();
  }
  
  r1 = r1/10;
  r2 = r2/10;
  r3 = r3/10;
  r4 = r4/10;
  masse = (r1+r2+r3+r4);
  Serial.print(masse);
  return(masse);
}
void afficher(float masse)
{
  Serial.print("\nmasse :");
  Serial.print(masse);
  Serial.print("kg");
}

float humidite()
{
  delay(2000);
  return dht.readHumidity();
}
float temperature()
{
  delay(2000);
  return dht.readTemperature();
}

int ENTIER(float valeur)
{
  int entier;
  entier = valeur;
  return entier;
}
int DECIMAL(float valeur)
{
  int entier;
  entier = valeur;
  int decimal;
  decimal = valeur*100 - entier*100;
  return decimal;
}
