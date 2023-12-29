#include <MqttClient.h>

// Import the necessary libraries
#include <SoftwareSerial.h>


// Define the pins for the SIM800C module
const int gsm_reset = 8;
const int gsm_pwr_key = 9;
const int gsm_tx = 10;
const int gsm_rx = 11;

// Define the MQTT broker connection details
const char *mqtt_broker = "localhost";
const int mqtt_port = 1883;
const char *mqtt_username = "user";
const char *mqtt_password = "password";

// Define the MQTT topic to publish to
const char *mqtt_topic = "test";

// Create a SoftwareSerial object for the SIM800C module
SoftwareSerial gsmSerial(gsm_rx, gsm_tx);

// Create an MQTT client object
MQTT client(gsmSerial);

// Initialize the SIM800C module
void setup() {
  // Set the pins for the SIM800C module
  pinMode(gsm_reset, OUTPUT);
  pinMode(gsm_pwr_key, OUTPUT);

  // Reset the SIM800C module
  digitalWrite(gsm_reset, LOW);
  delay(1000);
  digitalWrite(gsm_reset, HIGH);
  delay(10000);

  // Initialize the SoftwareSerial object for the SIM800C module
  gsmSerial.begin(9600);

  // Initialize the MQTT client object
  client.setServer(mqtt_broker, mqtt_port);
  client.setUsername(mqtt_username);
  client.setPassword(mqtt_password);
}

// The loop() function runs repeatedly
void loop() {
  // Connect to the MQTT broker
  if (!client.connect()) {
    // If the connection fails, retry
    delay(5000);
    continue;
  }

  // Publish a message to the MQTT topic
  client.publish(mqtt_topic, "Hello world!");

  // Disconnect from the MQTT broker
  client.disconnect();

  // Wait for 5 seconds before reconnecting
  delay(5000);
}
