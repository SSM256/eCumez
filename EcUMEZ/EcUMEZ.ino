#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <Servo.h>

// Wi-Fi postavke
const char* ssid = "Xiaomi 11T";
const char* password = "ananas73";

// Firebase postavke
#define API_KEY "AIzaSyBnAyceu8ya2BEGbuGZBSYlZRUWjZtZ9sk"
#define DATABASE_URL "https://ecumez-default-rtdb.europe-west1.firebasedatabase.app/"

// Firebase objekti
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// LED i LDR pinovi
#define LED_PIN 14 // GPIO14
#define LDR_PIN A0 // Analogni pin za LDR

// DHT senzor
#define DHTPIN 12 // GPIO12
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Servo motor
Servo servo;
#define SERVO_PIN D0 // GPIO16 (D0)

// IR senzor
#define IR_SENSOR_PIN 5 // GPIO5 (D1 na NodeMCU)

// Promjenjive za senzore i stanje
float temperature;
float humidity;
String doorControl = "";
bool doorState = false;  // false = zatvorena, true = otvorena
bool sensorTriggered = false;

void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    now = time(nullptr);
  }
  Serial.println("Vrijeme postavljeno!");
}

void setup() {
  setClock();
  Serial.begin(115200);

  // Povezivanje na WiFi
  Serial.print("Povezivanje na WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi povezan!");
  Serial.println("IP adresa: " + WiFi.localIP().toString());

  // Postavljanje Firebase postavki
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Firebase autentifikacija (anonimna)
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase prijava uspješna!");
  } else {
    Serial.println("Greška u prijavi: " + String(config.signer.signupError.message.c_str()));
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Inicijalizacija senzora
  dht.begin();

  // Inicijalizacija servo motora
  servo.attach(SERVO_PIN);
  servo.write(180); // Početna pozicija (zatvoreno)

  // Postavljanje pinova
  pinMode(LED_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
}

void openDoor() {
  Serial.println("Otvaranje vrata...");
  servo.write(0); // Otvaranje vrata
  doorState = true;
  updateFirebaseDoorState("otvori");
}

void closeDoor() {
  Serial.println("Zatvaranje vrata...");
  servo.write(180); // Zatvaranje vrata
  doorState = false;
  updateFirebaseDoorState("zatvori");
}

void updateFirebaseDoorState(String state) {
  if (Firebase.RTDB.setString(&fbdo, "/KontrolaVrata", state)) {
    Serial.println("Stanje vrata ažurirano u Firebase: " + state);
  } else {
    Serial.println("Greška pri ažuriranju stanja vrata: " + fbdo.errorReason());
  }
}




 void loop() {
  // Provjera IR senzora
  if (digitalRead(IR_SENSOR_PIN) == LOW && !sensorTriggered) {
    sensorTriggered = true; // Sprečavanje višestruke detekcije
    openDoor();
    delay(5000); // Vrijeme dok su vrata otvorena
    closeDoor();
    sensorTriggered = false;
  }

  // Čitanje vrijednosti "KontrolaVrata" iz Firebase
  if (Firebase.RTDB.getString(&fbdo, "/KontrolaVrata")) {
    doorControl = fbdo.stringData();
    Serial.println("Kontrola vrata: " + doorControl);

    // Upravljanje vratima putem Firebase-a
    // Ovdje provjeravaš stanje i samo mijenjaš vrata ako je potrebno
    if (doorControl == "otvori" && !doorState) {
      openDoor();
    }
    if (doorControl == "zatvori" && doorState) {
      closeDoor();
    }
  } else {
    Serial.println("Greška pri čitanju KontrolaVrata: " + fbdo.errorReason());
  }

  // Očitavanje LDR-a
  int ldrValue = analogRead(LDR_PIN);
  Serial.print("LDR vrijednost: ");
  Serial.println(ldrValue);

  // Očitavanje temperature i vlažnosti
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Greška pri očitavanju sa DHT senzora!");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Vlažnost: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Slanje temperature i vlažnosti na Firebase
    if (Firebase.RTDB.setFloat(&fbdo, "/senzori/temperatura", temperature)) {
      Serial.println("Temperatura poslana na Firebase!");
    } else {
      Serial.println("Greška pri slanju temperature: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "/senzori/vlaznost", humidity)) {
      Serial.println("Vlažnost poslana na Firebase!");
    } else {
      Serial.println("Greška pri slanju vlažnosti: " + fbdo.errorReason());
    }
  }

  // Logika za LED kontrolu
  if (ldrValue < 50) {
    digitalWrite(LED_PIN, HIGH); // Upali LED ako je mrak
  } else {
    digitalWrite(LED_PIN, LOW);  // Ugasi LED ako je svjetlo
  }

  delay(1000); // Pauza između iteracija

}
