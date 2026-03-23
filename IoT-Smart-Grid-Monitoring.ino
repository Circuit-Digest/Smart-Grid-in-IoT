#include <WiFiS3.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ThingSpeak.h>
#include <PZEM004Tv40_R4.h>

// WiFi credentials
const char* ssid = "Semicon Media";
const char* password = "cracksen1605";

// ThingSpeak settings
unsigned long myChannelNumber = 3238257;
const char* myWriteAPIKey = "CRXGU545EOA6Z8R9";

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Button settings
#define BUTTON_PIN 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
PZEM004Tv40_R4 pzem(&Serial1);
WiFiClient client;

// Timing variables (non-blocking)
unsigned long lastReadTime = 0;
unsigned long lastUploadTime = 0;
unsigned long lastSerialPrintTime = 0;
unsigned long lastDisplayUpdateTime = 0;
unsigned long lastDisplaySwitchTime = 0;
unsigned long wifiConnectStartTime = 0;
unsigned long lastButtonPressTime = 0;

const unsigned long readInterval = 1000;
const unsigned long serialPrintInterval = 1000;
const unsigned long displayUpdateInterval = 500;
const unsigned long displaySwitchInterval = 5000;
const unsigned long uploadInterval = 16000;
const unsigned long wifiTimeout = 10000;
const unsigned long debounceDelay = 200;

// Display mode (0 = all parameters, 1-6 = individual parameters)
int displayMode = 0;

// Rolling display control
bool rollingEnabled = false;
bool lastButtonState = HIGH;
bool buttonState = HIGH;

// State machine for WiFi
enum WiFiState {
  WIFI_IDLE,
  WIFI_CONNECTING,
  WIFI_CONNECTED,
  WIFI_FAILED
};

WiFiState wifiState = WIFI_IDLE;

// Data storage
float voltage = 0.0;
float current = 0.0;
float power = 0.0;
float energy = 0.0;
float frequency = 0.0;
float powerFactor = 0.0;
bool dataValid = false;

void setup() {
  Serial.begin(115200);

  Serial.println("PZEM-004T Energy Monitor with ThingSpeak");
  Serial.println("==========================================");

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("PZEM Monitor");
  display.println("Initializing...");
  display.display();

  // Initialize button pin with internal pullup
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize PZEM
  pzem.begin();

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Start WiFi connection
  wifiState = WIFI_CONNECTING;
  WiFi.begin(ssid, password);
  wifiConnectStartTime = millis();

  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
}

void loop() {
  unsigned long currentMillis = millis();

  // Handle button press
  handleButton(currentMillis);

  // Handle WiFi state machine
  handleWiFi(currentMillis);

  // Read PZEM data (non-blocking interval)
  if (currentMillis - lastReadTime >= readInterval) {
    lastReadTime = currentMillis;
    readPZEMData();
  }

  // Update Serial Monitor (non-blocking interval)
  if (currentMillis - lastSerialPrintTime >= serialPrintInterval && dataValid) {
    lastSerialPrintTime = currentMillis;
    printToSerial();
  }

  // Switch display mode every 5 seconds (only if rolling is enabled)
  if (rollingEnabled && (currentMillis - lastDisplaySwitchTime >= displaySwitchInterval)) {
    lastDisplaySwitchTime = currentMillis;
    displayMode++;
    if (displayMode > 6) {
      displayMode = 1;  // Reset to first individual parameter (Voltage), skip all-in-one
    }
  }

  // Update OLED Display (non-blocking interval)
  if (currentMillis - lastDisplayUpdateTime >= displayUpdateInterval) {
    lastDisplayUpdateTime = currentMillis;
    updateDisplay();
  }

  // Upload to ThingSpeak (non-blocking interval)
  if (currentMillis - lastUploadTime >= uploadInterval && dataValid) {
    if (wifiState == WIFI_CONNECTED) {
      lastUploadTime = currentMillis;
      uploadToThingSpeak();
    }
  }
}

void handleWiFi(unsigned long currentMillis) {
  switch (wifiState) {
    case WIFI_IDLE:
      break;

    case WIFI_CONNECTING:
      if (WiFi.status() == WL_CONNECTED) {
        wifiState = WIFI_CONNECTED;
        Serial.println("\nWiFi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("WiFi Connected!");
        display.print("IP: ");
        display.println(WiFi.localIP());
        display.display();
      } else if (currentMillis - wifiConnectStartTime >= wifiTimeout) {
        wifiState = WIFI_FAILED;
        Serial.println("\nWiFi Connection Failed!");

        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("WiFi Failed!");
        display.println("Running offline");
        display.display();
      }
      break;

    case WIFI_CONNECTED:
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Disconnected! Reconnecting...");
        wifiState = WIFI_CONNECTING;
        WiFi.begin(ssid, password);
        wifiConnectStartTime = currentMillis;
      }
      break;

    case WIFI_FAILED:
      if (currentMillis - wifiConnectStartTime >= 30000) {
        Serial.println("Retrying WiFi connection...");
        wifiState = WIFI_CONNECTING;
        WiFi.begin(ssid, password);
        wifiConnectStartTime = currentMillis;
      }
      break;
  }
}

void handleButton(unsigned long currentMillis) {
  // Read the button state
  int reading = digitalRead(BUTTON_PIN);

  // Check if button state has changed and debounce
  if (reading != lastButtonState) {
    lastButtonPressTime = currentMillis;
  }

  if ((currentMillis - lastButtonPressTime) > debounceDelay) {
    // If the button state has changed
    if (reading != buttonState) {
      buttonState = reading;

      // Button pressed (LOW because of pullup)
      if (buttonState == LOW) {
        // Toggle rolling mode
        rollingEnabled = !rollingEnabled;

        if (rollingEnabled) {
          Serial.println("Rolling display ON");
          displayMode = 1;  // Start from first individual parameter (Voltage)
          lastDisplaySwitchTime = currentMillis;  // Reset timer
        } else {
          Serial.println("Rolling display OFF");
          displayMode = 0;  // Go to all-in-one display immediately
        }
      }
    }
  }

  lastButtonState = reading;
}

void readPZEMData() {
  if (pzem.readAll()) {
    voltage = pzem.getVoltage();
    current = pzem.getCurrent() * 1000;
    power = pzem.getPower();
    energy = pzem.getEnergy();
    frequency = pzem.getFrequency();
    powerFactor = pzem.getPowerFactor();
    dataValid = true;
  } else {
    dataValid = false;
    Serial.print("Error reading PZEM! Code: ");
    Serial.println(pzem.getLastError());
  }
}

void printToSerial() {
  Serial.print("Voltage:      ");
  Serial.print(voltage, 1);
  Serial.println(" V");

  Serial.print("Current:      ");
  Serial.print(current, 0);
  Serial.println(" mA");

  Serial.print("Power:        ");
  Serial.print(power, 1);
  Serial.println(" W");

  Serial.print("Energy:       ");
  Serial.print(energy, 3);
  Serial.println(" kWh");

  Serial.print("Frequency:    ");
  Serial.print(frequency, 1);
  Serial.println(" Hz");

  Serial.print("Power Factor: ");
  Serial.println(powerFactor, 2);
  Serial.println("------------------------");
}

void updateDisplay() {
  display.clearDisplay();

  if (!dataValid) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("ERROR!");
    display.println("");
    display.print("Code: ");
    display.println(pzem.getLastError());
    display.println("");
    display.println("Check connections");
    display.display();
    return;
  }

  // Display based on current mode
  switch (displayMode) {
    case 0:
      displayAllParameters();
      break;
    case 1:
      displaySingleParameter("Voltage", voltage, 1, "V");
      break;
    case 2:
      displaySingleParameter("Current", current, 0, "mA");
      break;
    case 3:
      displaySingleParameter("Power", power, 1, "W");
      break;
    case 4:
      displaySingleParameter("Energy", energy, 3, "kWh");
      break;
    case 5:
      displaySingleParameter("Frequency", frequency, 1, "Hz");
      break;
    case 6:
      displaySingleParameter("Power Factor", powerFactor, 2, "");
      break;
  }

  display.display();
}

void displayAllParameters() {
  display.setTextSize(1);

  // Title
  display.setCursor(0, 0);
  display.println("Energy Monitor");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  // Voltage
  display.setCursor(0, 14);
  display.print("Voltage : ");
  display.print(voltage, 1);
  display.println(" V");

  // Current (in mA)
  display.setCursor(0, 24);
  display.print("Current : ");
  display.print(current, 0);
  display.println(" mA");

  // Power
  display.setCursor(0, 34);
  display.print("Power   : ");
  display.print(power, 1);
  display.println(" W");

  // Energy
  display.setCursor(0, 44);
  display.print("Energy  : ");
  display.print(energy, 3);
  display.println(" kWh");

  // Frequency and Power Factor
  display.setCursor(0, 54);
  display.print("Freq:");
  display.print(frequency, 1);
  display.print("Hz");

  display.setCursor(75, 54);
  display.print("PF:");
  display.print(powerFactor, 2);

  //WiFi status indicator
  if (wifiState == WIFI_CONNECTED) {
    display.fillRect(118, 6, 2, 2, SSD1306_WHITE);
    display.fillRect(121, 4, 2, 4, SSD1306_WHITE);
    display.fillRect(124, 2, 2, 6, SSD1306_WHITE);
  }
}

void displaySingleParameter(const char* paramName, float value, int decimals, const char* unit) {
  // Display parameter name at top
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(paramName);
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  // Display value in large font (size 2)
  display.setTextSize(2);
  
  // Convert value to string with proper decimal places
  char valueStr[16];
  dtostrf(value, 0, decimals, valueStr);
  
  // Calculate position to center the value and unit
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(valueStr, 0, 0, &x1, &y1, &w, &h);
  
  // Position value in the middle of screen
  int valueX = (SCREEN_WIDTH - w) / 2;
  int valueY = 28;
  
  display.setCursor(valueX, valueY);
  display.print(valueStr);
  
  // Display unit in smaller font next to value
  if (strlen(unit) > 0) {
    display.setTextSize(1);
    // Position unit right after the value
    display.setCursor(valueX + w + 4, valueY + 8);
    display.print(unit);
  }

  // WiFi status indicator
  if (wifiState == WIFI_CONNECTED) {
    display.fillRect(118, 6, 2, 2, SSD1306_WHITE);
    display.fillRect(121, 4, 2, 4, SSD1306_WHITE);
    display.fillRect(124, 2, 2, 6, SSD1306_WHITE);
  }
}

void uploadToThingSpeak() {
  Serial.println("\nUploading to ThingSpeak...");

  // Set the fields
  ThingSpeak.setField(1, voltage);
  ThingSpeak.setField(2, current);
  ThingSpeak.setField(3, power);
  ThingSpeak.setField(4, energy);
  ThingSpeak.setField(5, frequency);
  ThingSpeak.setField(6, powerFactor);

  // Write to ThingSpeak channel
  int statusCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (statusCode == 200) {
    Serial.println("Channel update successful!");
    Serial.print("  Voltage: ");
    Serial.print(voltage, 1);
    Serial.println(" V");
    Serial.print("  Current: ");
    Serial.print(current, 0);
    Serial.println(" mA");
    Serial.print("  Power: ");
    Serial.print(power, 1);
    Serial.println(" W");
    Serial.print("  Energy: ");
    Serial.print(energy, 3);
    Serial.println(" kWh");
    Serial.print("  Frequency: ");
    Serial.print(frequency, 1);
    Serial.println(" Hz");
    Serial.print("  Power Factor: ");
    Serial.println(powerFactor, 2);
  } else {
    Serial.print("Problem updating channel. HTTP error code: ");
    Serial.println(statusCode);
  }

  Serial.println();
}