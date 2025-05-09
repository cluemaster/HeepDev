/**
 * Heep Client for ESP32/ESP8266
 * 
 * This firmware connects an ESP32 or ESP8266 to the Heep server
 * and provides device capabilities through configurable controls.
 * 
 * Features:
 * - WiFi connectivity with auto reconnect
 * - OTA updates support
 * - WebSocket communication with Heep server
 * - MQTT integration
 * - Device configuration through web interface
 * - Secure communication with TLS
 */

#if defined(ESP32)
  #include <WiFi.h>
  #include <ESPmDNS.h>
  #include <WebServer.h>
  #include <Update.h>
  WebServer server(80);
  #define LED_BUILTIN 2
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  #include <ESP8266WebServer.h>
  ESP8266WebServer server(80);
#endif

#include <WiFiClient.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <LittleFS.h>

// WiFi credentials
// These should be stored in flash and configurable via web interface
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

// Heep server details
const char* heepServer = "192.168.1.100"; // Replace with your Heep server IP
const int heepPort = 8088;
const bool useSecureConnection = false;

// MQTT broker details (optional)
const char* mqttBroker = "192.168.1.100";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

// Device details
String deviceId;
String deviceName = "ESP32 Heep Device";
bool deviceConfigured = false;

// WebSocket and MQTT clients
WebSocketsClient webSocket;
WiFiClient espClient;
PubSubClient mqtt(espClient);

// Intervals for different operations (in milliseconds)
const long heartbeatInterval = 10000;  // 10 seconds
const long reconnectInterval = 5000;   // 5 seconds

// Timestamps for interval management
unsigned long lastHeartbeatTime = 0;
unsigned long lastReconnectAttempt = 0;

// Device controls
struct Control {
  String id;
  String name;
  String type;
  String value;
  String min;
  String max;
  String options;
};

// Maximum number of controls
const int MAX_CONTROLS = 10;
Control controls[MAX_CONTROLS];
int controlCount = 0;

// Forward declarations
void setupWiFi();
void setupWebSocketClient();
void setupMQTT();
void handleWebSocketEvent(WStype_t type, uint8_t * payload, size_t length);
void sendHeartbeat();
void registerDevice();
void updateControl(String controlId, String value);
void saveConfiguration();
void loadConfiguration();
void setupWebServer();
void generateDeviceId();

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("\nHeep Client Starting...");
  
  // Initialize the LED pin as an output
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialize EEPROM for storing configuration
  EEPROM.begin(512);
  
  // Initialize the file system
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");
  }
  
  // Generate or load device ID
  loadConfiguration();
  if (deviceId.length() == 0) {
    generateDeviceId();
    saveConfiguration();
  }
  
  // Setup WiFi connection
  setupWiFi();
  
  // Setup the web server for OTA updates and configuration
  setupWebServer();
  
  // Setup WebSocket client for Heep communication
  setupWebSocketClient();
  
  // Setup MQTT client (optional)
  setupMQTT();
  
  // Add some default controls for demonstration
  if (controlCount == 0) {
    // Add an LED control
    controls[controlCount].id = "led1";
    controls[controlCount].name = "LED";
    controls[controlCount].type = "toggle";
    controls[controlCount].value = "false";
    controlCount++;
    
    // Add a slider control
    controls[controlCount].id = "slider1";
    controls[controlCount].name = "Brightness";
    controls[controlCount].type = "slider";
    controls[controlCount].value = "50";
    controls[controlCount].min = "0";
    controls[controlCount].max = "100";
    controlCount++;
  }
  
  Serial.println("Setup complete");
}

void loop() {
  // Handle WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    // Attempt to reconnect
    if (millis() - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = millis();
      Serial.println("Reconnecting to WiFi...");
      WiFi.reconnect();
    }
  } else {
    // Handle WebSocket connection
    webSocket.loop();
    
    // Handle MQTT connection
    if (mqtt.connected()) {
      mqtt.loop();
    } else {
      // Attempt to reconnect to MQTT
      if (millis() - lastReconnectAttempt > reconnectInterval) {
        lastReconnectAttempt = millis();
        if (mqtt.connect(deviceId.c_str(), mqttUser, mqttPassword)) {
          Serial.println("Connected to MQTT broker");
          // Subscribe to device-specific topics
          mqtt.subscribe(("heep/device/" + deviceId + "/commands").c_str());
        }
      }
    }
    
    // Send heartbeat message to server
    if (millis() - lastHeartbeatTime > heartbeatInterval) {
      lastHeartbeatTime = millis();
      sendHeartbeat();
    }
  }
  
  // Handle web server requests
  server.handleClient();
}

void setupWiFi() {
  Serial.printf("Connecting to %s ", ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection for up to 20 seconds
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 20000) {
    delay(500);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
    
    // Setup mDNS responder
    if (MDNS.begin(deviceId.c_str())) {
      MDNS.addService("http", "tcp", 80);
      Serial.printf("mDNS responder started: %s.local\n", deviceId.c_str());
    }
  } else {
    Serial.println("\nWiFi connection failed");
  }
}

void setupWebSocketClient() {
  // Configure WebSocket client
  if (useSecureConnection) {
    webSocket.beginSSL(heepServer, heepPort);
  } else {
    webSocket.begin(heepServer, heepPort, "/");
  }
  
  // Set event handler
  webSocket.onEvent(handleWebSocketEvent);
  
  // Try to reconnect every 5 seconds if connection is lost
  webSocket.setReconnectInterval(5000);
  
  Serial.println("WebSocket client configured");
}

void setupMQTT() {
  mqtt.setServer(mqttBroker, mqttPort);
  mqtt.setCallback([](char* topic, byte* payload, unsigned int length) {
    // Handle incoming MQTT messages
    payload[length] = 0; // Null terminate the payload
    String message = String((char*)payload);
    String topicStr = String(topic);
    
    Serial.printf("MQTT message received [%s]: %s\n", topic, message.c_str());
    
    // Example of handling a command message
    if (topicStr == "heep/device/" + deviceId + "/commands") {
      // Parse the JSON payload
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, message);
      
      if (!error) {
        String controlId = doc["control_id"];
        String value = doc["value"];
        
        // Update the control
        updateControl(controlId, value);
      }
    }
  });
}

void handleWebSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      // Register device with the server
      registerDevice();
      break;
    
    case WStype_TEXT:
      // Handle incoming WebSocket messages
      {
        String message = String((char*)payload);
        Serial.printf("WebSocket message received: %s\n", message.c_str());
        
        // Parse the JSON message
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, message);
        
        if (!error) {
          String messageType = doc["message_type"];
          
          // Handle different message types
          if (messageType == "control_update") {
            String controlId = doc["control_id"];
            String value = doc["value"];
            
            // Update the control
            updateControl(controlId, value);
          }
          else if (messageType == "get_devices") {
            // Server is requesting device information
            registerDevice();
          }
        }
      }
      break;
    
    case WStype_BIN:
      // Binary data not supported yet
      break;
  }
}

void sendHeartbeat() {
  DynamicJsonDocument doc(256);
  doc["message_type"] = "heartbeat";
  doc["device_id"] = deviceId;
  
  String message;
  serializeJson(doc, message);
  
  webSocket.sendTXT(message);
  Serial.println("Heartbeat sent");
}

void registerDevice() {
  DynamicJsonDocument doc(1024);
  doc["message_type"] = "register_device";
  doc["device_id"] = deviceId;
  doc["name"] = deviceName;
  doc["ip_address"] = WiFi.localIP().toString();
  doc["port"] = 80;
  
  // Add controls
  JsonArray controlsArray = doc.createNestedArray("controls");
  for (int i = 0; i < controlCount; i++) {
    JsonObject control = controlsArray.createNestedObject();
    control["id"] = controls[i].id;
    control["name"] = controls[i].name;
    control["type"] = controls[i].type;
    control["value"] = controls[i].value;
    
    if (controls[i].min.length() > 0) {
      control["min"] = controls[i].min;
    }
    
    if (controls[i].max.length() > 0) {
      control["max"] = controls[i].max;
    }
    
    if (controls[i].options.length() > 0) {
      control["options"] = controls[i].options;
    }
  }
  
  String message;
  serializeJson(doc, message);
  
  webSocket.sendTXT(message);
  Serial.println("Device registered");
}

void updateControl(String controlId, String value) {
  // Find the control with the given ID
  for (int i = 0; i < controlCount; i++) {
    if (controls[i].id == controlId) {
      // Update the control value
      controls[i].value = value;
      
      // Handle control actions
      if (controlId == "led1") {
        // Handle LED control
        if (value == "true") {
          digitalWrite(LED_BUILTIN, HIGH);
        } else {
          digitalWrite(LED_BUILTIN, LOW);
        }
      }
      else if (controlId == "slider1") {
        // Handle brightness control (if PWM is supported)
        #if defined(ESP32)
          int brightness = value.toInt();
          // Map from 0-100 to 0-255
          brightness = map(brightness, 0, 100, 0, 255);
          // Use PWM to control LED brightness
          analogWrite(LED_BUILTIN, brightness);
        #endif
      }
      
      // Send control update to MQTT if connected
      if (mqtt.connected()) {
        DynamicJsonDocument doc(256);
        doc["control_id"] = controlId;
        doc["value"] = value;
        
        String message;
        serializeJson(doc, message);
        
        mqtt.publish(("heep/device/" + deviceId + "/status").c_str(), message.c_str());
      }
      
      // Send control update to WebSocket server
      DynamicJsonDocument doc(256);
      doc["message_type"] = "control_update";
      doc["device_id"] = deviceId;
      doc["control_id"] = controlId;
      doc["value"] = value;
      
      String message;
      serializeJson(doc, message);
      
      webSocket.sendTXT(message);
      
      Serial.printf("Control %s updated to %s\n", controlId.c_str(), value.c_str());
      return;
    }
  }
  
  Serial.printf("Control %s not found\n", controlId.c_str());
}

void saveConfiguration() {
  // Save configuration to EEPROM
  EEPROM.writeString(0, deviceId);
  EEPROM.writeString(50, deviceName);
  EEPROM.commit();
  
  // Save controls to a file
  File file = LittleFS.open("/controls.json", "w");
  if (file) {
    DynamicJsonDocument doc(1024);
    JsonArray controlsArray = doc.createNestedArray("controls");
    
    for (int i = 0; i < controlCount; i++) {
      JsonObject control = controlsArray.createNestedObject();
      control["id"] = controls[i].id;
      control["name"] = controls[i].name;
      control["type"] = controls[i].type;
      control["value"] = controls[i].value;
      control["min"] = controls[i].min;
      control["max"] = controls[i].max;
      control["options"] = controls[i].options;
    }
    
    serializeJson(doc, file);
    file.close();
    Serial.println("Configuration saved");
  } else {
    Serial.println("Failed to open controls.json for writing");
  }
}

void loadConfiguration() {
  // Load configuration from EEPROM
  deviceId = EEPROM.readString(0);
  deviceName = EEPROM.readString(50);
  
  // Load controls from a file
  File file = LittleFS.open("/controls.json", "r");
  if (file) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, file);
    
    if (!error) {
      JsonArray controlsArray = doc["controls"];
      controlCount = 0;
      
      for (JsonObject control : controlsArray) {
        if (controlCount < MAX_CONTROLS) {
          controls[controlCount].id = control["id"].as<String>();
          controls[controlCount].name = control["name"].as<String>();
          controls[controlCount].type = control["type"].as<String>();
          controls[controlCount].value = control["value"].as<String>();
          controls[controlCount].min = control["min"].as<String>();
          controls[controlCount].max = control["max"].as<String>();
          controls[controlCount].options = control["options"].as<String>();
          
          controlCount++;
        }
      }
    }
    
    file.close();
    Serial.println("Configuration loaded");
  } else {
    Serial.println("No saved controls found");
  }
}

void setupWebServer() {
  // Set up the web server for OTA updates and configuration
  server.on("/", HTTP_GET, []() {
    // Serve the web interface from LittleFS
    if (LittleFS.exists("/index.html")) {
      File file = LittleFS.open("/index.html", "r");
      server.streamFile(file, "text/html");
      file.close();
    } else {
      // Fallback if no index.html exists
      String html = "<html><head><title>" + deviceName + "</title>";
      html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
      html += "<style>body{font-family:Arial;margin:0;padding:20px;}</style></head>";
      html += "<body><h1>" + deviceName + "</h1>";
      html += "<p>Device ID: " + deviceId + "</p>";
      html += "<p>IP Address: " + WiFi.localIP().toString() + "</p>";
      html += "<h2>Controls</h2><ul>";
      
      for (int i = 0; i < controlCount; i++) {
        html += "<li>" + controls[i].name + " (" + controls[i].type + "): " + controls[i].value + "</li>";
      }
      
      html += "</ul></body></html>";
      server.send(200, "text/html", html);
    }
  });
  
  // API endpoint for getting device information
  server.on("/api/device", HTTP_GET, []() {
    DynamicJsonDocument doc(1024);
    doc["device_id"] = deviceId;
    doc["name"] = deviceName;
    doc["ip_address"] = WiFi.localIP().toString();
    
    JsonArray controlsArray = doc.createNestedArray("controls");
    for (int i = 0; i < controlCount; i++) {
      JsonObject control = controlsArray.createNestedObject();
      control["id"] = controls[i].id;
      control["name"] = controls[i].name;
      control["type"] = controls[i].type;
      control["value"] = controls[i].value;
      
      if (controls[i].min.length() > 0) {
        control["min"] = controls[i].min;
      }
      
      if (controls[i].max.length() > 0) {
        control["max"] = controls[i].max;
      }
      
      if (controls[i].options.length() > 0) {
        control["options"] = controls[i].options;
      }
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  // API endpoint for updating a control
  server.on("/api/control", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      
      DynamicJsonDocument doc(256);
      DeserializationError error = deserializeJson(doc, body);
      
      if (!error) {
        String controlId = doc["control_id"];
        String value = doc["value"];
        
        updateControl(controlId, value);
        server.send(200, "application/json", "{\"status\":\"success\"}");
      } else {
        server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
      }
    } else {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"No body\"}");
    }
  });
  
  // Setup OTA update endpoint
  #if defined(ESP32)
    server.on("/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "Update failed" : "Update successful");
      ESP.restart();
    }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
      }
    });
  #elif defined(ESP8266)
    server.on("/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "Update failed" : "Update successful");
      ESP.restart();
    }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace)) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });
  #endif
  
  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void generateDeviceId() {
  // Generate a unique device ID based on MAC address
  uint8_t mac[6];
  WiFi.macAddress(mac);
  
  deviceId = "heep-";
  for (int i = 0; i < 6; i++) {
    char buf[3];
    sprintf(buf, "%02x", mac[i]);
    deviceId += buf;
  }
  
  Serial.printf("Generated device ID: %s\n", deviceId.c_str());
}
