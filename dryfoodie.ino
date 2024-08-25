// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------

#include <NewPing.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define pinbutton 13
#define TURN_TIME 175
#define TRIGGER_PIN  5  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     18  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 18 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

double maximum = 21;
const char* ssid = "UNDEFINED";
const char* password = "meng2003";
const char* mqtt_server = "d43ff0ab6db24ba296b1aed5d48bd702.s1.eu.hivemq.cloud";
static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

LiquidCrystal_I2C lcd(0x27,16,2); 
Servo myServo;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
WiFiClientSecure espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

void onPour(String pour = "") {
  int pourDuration = 500;
  if (pour != "") {
    Serial.print("BUTTON => ");
    Serial.println(digitalRead(pinbutton));
    if (pour == "low") {
      pourDuration = 400;
    } else if (pour == "high") {
      pourDuration = 600;
    }
      myServo.write(0);
      delay(pourDuration);
      myServo.write(90);
      delay(pourDuration);
      myServo.write(180);
      delay(pourDuration);
      myServo.write(90);
  }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String response = "";
  
  for (int i = 0; i < length; i++) {
    char data = (char)payload[i];
    response += String(data);
  }

  if (String(topic) == "pour_value") {
    Serial.println(response);
    onPour(response);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "dryfoodie123";
    if (client.connect(clientId.c_str(), "hasyimfaris", "FARfar110800")) {
      Serial.println("connected");
      client.subscribe("pour_value");
      // client.subscribe("left");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

double toPercent(double current) {
  double percent = ((17 - current) / 15) * 100;
  return percent;
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  espClient.setCACert(root_ca);
  myServo.attach(16);
  pinMode(pinbutton, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
  if (digitalRead(pinbutton) == 1) {
    onPour("med");
  }
  lcd.setCursor(0,0);
  double current = sonar.ping() / 57.0;
  int percent = 100;
  if (current > 2) {
    // Serial.println(current);
    percent = round(toPercent(current));
    if (percent > 0) {
    lcd.print(percent);
    lcd.print("%");
    Serial.print(percent);
    Serial.println("%");
    client.publish("left", "20");

    } else {
    percent = 0;
    lcd.print("0%");
    Serial.println("0%");
    }
  } else {
    lcd.print("100%");
    Serial.println("100%");
  }
  String toStr = String(percent, DEC);
  client.publish("left", toStr.c_str());
  delay(3000);
  lcd.clear();
  delay(1000);
}

