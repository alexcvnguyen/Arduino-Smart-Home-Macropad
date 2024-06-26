#include <WiFi.h>
#include <PubSubClient.h>
#include <Keypad.h>

const char* ssid = ""; // Wifi name
const char* password = ""; // Wifi password
const char* mqtt_server = ""; // Replace with your MQTT broker IP address (could be your Home Assistant IP address)
const char* mqtt_user = ""; // MQTT username
const char* mqtt_password = ""; // MQTT password

WiFiClient espClient;
PubSubClient client(espClient);

const byte ROWS = 3;
const byte COLS = 3;

  //Define a matrix of topics
const char* topics[ROWS * COLS] = {
  "homeassistant/macropad/button1", "homeassistant/macropad/button2", "homeassistant/macropad/button3",
  "homeassistant/macropad/button4", "homeassistant/macropad/button5", "homeassistant/macropad/button6",
  "homeassistant/macropad/button7", "homeassistant/macropad/button8", "homeassistant/macropad/button9"
};

    //Define a matrix of keys
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'}
};

  //The GPIO pins used for rows and columns
byte rowPins[ROWS] = {13, 12, 14};
byte colPins[COLS] = {32, 33, 26};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  
    // Initialize MQTT client
  client.setServer(mqtt_server, 1883); // MQTT broker IP and port
}

    //Set up wifi

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

      //Check for keypresses
  char key = keypad.getKey();
  if (key != NO_KEY) {
    Serial.println(key);

      //Define payload for MQTT
    char payload[2];
    payload[0] = key;
    payload[1] = '\0';


      //Publish topic to MQTT
    client.publish("homeassistant/macropad/button", payload);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
