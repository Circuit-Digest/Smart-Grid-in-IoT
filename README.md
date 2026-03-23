# Smart-Grid-in-IoT

A smart grid is not a complicated buzzword. It simply means electrical infrastructure is monitored continuously, data is collected in real time, and decisions are made based on measured values instead of guesswork.
This project builds a practical IoT smart energy monitoring node using an Arduino UNO R4 WiFi and the PZEM-004T energy meter. It reads electrical parameters, displays them locally on an OLED screen, and uploads data to ThingSpeak for remote monitoring and historical analysis.

💡 The system behaves like a small smart grid endpoint — not just a standalone meter.


🎯 Features

✅ Measures Voltage, Current, Power, Energy, Frequency, and Power Factor in real time
✅ Displays live readings on a local 128×64 OLED screen
✅ Streams data to ThingSpeak cloud for remote logging and graph visualization
✅ Non-blocking firmware — sensing, display, and WiFi run simultaneously using millis()
✅ Rolling display mode — press a button to cycle through each parameter individually
✅ 3D printed enclosure for a clean, professional, and safe physical build
✅ WiFi auto-reconnect with state machine handling
✅ Clean Modbus RTU communication via the PZEM004Tv40_R4 library


🧰 Hardware Components
ComponentQuantityNotesArduino UNO R4 WiFi1Main controller — built-in WiFi + hardware Serial1 for ModbusPZEM-004T Energy Meter1Measures all 6 AC electrical parameters via Modbus RTUCurrent Transformer Coil1Included with PZEM — clamps around live wire for safe current sensingOLED Display (128×64)1I2C display, address 0x3CPush Button1Toggles rolling display modeBreadboard / PCB1For assembly and prototypingJumper WiresSeveralMale-to-male and male-to-femaleUSB Cable1Programming and powering the ArduinoAC Load1Test load to verify measurements


📐 Electrical Parameters Monitored
ParameterUnitDescription⚡ VoltageVRMS AC mains voltage — deviation causes device failure or motor overheating🔌 CurrentmARMS load current — primary cause of overheating and fire risk when excessive💡 PowerWReal power (useful work) — what appears on your electricity bill🔋 EnergykWhAccumulated power over time — total electricity consumed📡 FrequencyHzGrid stability indicator — deviation signals system stress or imbalance📊 Power Factor—Efficiency ratio — poor PF results in wasted capacity and higher losses

📦 Required Libraries
Install all via the Arduino Library Manager:
LibraryPurposePZEM004Tv40_R4Modbus RTU communication with PZEM-004T (optimized for UNO R4 WiFi)Adafruit_SSD1306OLED display driverAdafruit_GFXGraphics primitives for the OLEDThingSpeakCloud data upload and channel managementWiFiS3WiFi connectivity for Arduino UNO R4 WiFi

⚠️ Important: Use PZEM004Tv40_R4 specifically — it communicates directly through Serial1 and avoids timing issues found in generic PZEM implementations.


☁️ ThingSpeak Cloud Setup

Create a free account at thingspeak.com and log in
Click New Channel and enable 6 fields, naming them clearly:

Field 1 → Voltage
Field 2 → Current
Field 3 → Power
Field 4 → Energy
Field 5 → Frequency
Field 6 → Power Factor


Open the API Keys tab — copy your Channel ID and Write API Key
Paste both values into the firmware (see Configuration below)
Upload firmware — ThingSpeak auto-generates graphs and your dashboard goes live


⚙️ Configuration
Before uploading, update these values in the sketch:
cpp// ── WiFi Credentials ──────────────────────────────────────
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ── ThingSpeak Settings ───────────────────────────────────
unsigned long myChannelNumber = YOUR_CHANNEL_ID;
const char*   myWriteAPIKey   = "YOUR_WRITE_API_KEY";

🔐 Security: Never commit real credentials to a public repo. Use a secrets.h file and add it to .gitignore.


🔌 Wiring Guide
FromToPZEM-004T TXArduino RX1 (Serial1)PZEM-004T RXArduino TX1 (Serial1)PZEM-004T GNDArduino GNDOLED SDAArduino SDA (A4)OLED SCLArduino SCL (A5)OLED VCCArduino 3.3VPush ButtonArduino D4 (internal pull-up enabled)Current TransformerClamp around live wire ONLY — never both live + neutral

⚠️ Safety Warning: The PZEM-004T connects directly to mains AC voltage. Exercise extreme caution. Consult a qualified electrician before wiring to live mains if unsure.


📲 Display Modes
ModeTriggerDescriptionAll-in-oneDefault (button OFF)All 6 parameters shown simultaneously on one screenRollingPress button (ON)Cycles through each parameter individually every 5 seconds

💻 Source Code — Key Sections Explained
Object Initialization
cppPZEM004Tv40_R4 pzem(&Serial1);   // Modbus on hardware serial — isolated from USB debug
WiFiClient client;                // Manages all network communication
Non-Blocking Timing
cppunsigned long lastReadTime = 0;
const unsigned long readInterval = 1000;   // Read every 1 second

if (currentMillis - lastReadTime >= readInterval) {
    lastReadTime = currentMillis;
    readPZEMData();
}

This prevents delay() from blocking — the processor stays free for display updates and WiFi simultaneously.

Reading All Parameters
cppif (pzem.readAll()) {
    voltage     = pzem.getVoltage();
    current     = pzem.getCurrent() * 1000;   // Convert A → mA
    power       = pzem.getPower();
    energy      = pzem.getEnergy();
    frequency   = pzem.getFrequency();
    powerFactor = pzem.getPowerFactor();
    dataValid   = true;
}
Uploading to ThingSpeak
cppThingSpeak.setField(1, voltage);
ThingSpeak.setField(2, current);
ThingSpeak.setField(3, power);
ThingSpeak.setField(4, energy);
ThingSpeak.setField(5, frequency);
ThingSpeak.setField(6, powerFactor);

int statusCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
if (statusCode == 200) {
    Serial.println("Upload successful!");
}
Separated Responsibilities
cppupdateDisplay();         // Local OLED visualization
uploadToThingSpeak();    // Cloud logging

Each function is independent — easy to test, debug, and extend individually.


🗂️ Project Structure
smart-grid-iot/
├── src/
│   └── smart_grid_monitor.ino       # Main Arduino sketch
├── docs/
│   ├── circuit_diagram.png          # Full wiring schematic
│   ├── hardware_setup.jpg           # Physical assembly photo
│   └── thingspeak_dashboard.png     # Cloud dashboard screenshot
├── enclosure/
│   └── enclosure_model.stl          # 3D printable enclosure (PETG recommended)
└── README.md

🔧 Troubleshooting
IssueSymptomFixNo readings from meterNo response / blank valuesCheck TX↔RX wiring; verify Serial1; confirm shared GNDCommunication failureError code 1 (CRC mismatch)Shorten wires; tighten connections; route away from mainsCurrent always 0 AShows 0 mA constantlyClamp CT around one live wire only — not both conductorsOLED blank screenNothing displayedVerify SDA/SCL; confirm I2C address is 0x3CRandom resetsBoard restarts unexpectedlyUse stable 5V adapter; avoid USB power when load is active

🚀 Future Enhancements

 SD card logging for offline data storage when WiFi is unavailable
 Multi-circuit monitoring using additional PZEM-004T modules
 Local web dashboard served over LAN (no internet required)
 Over-current and over-voltage real-time alerts
 Mobile push notifications for abnormal readings
 Daily and weekly energy usage analytics and PDF reports


📚 Related Projects

🔗 IoT Electricity Energy Meter using ESP12 & Arduino
🔗 IoT Smart Energy Meter with SMS Alert
🔗 Real-Time Energy Monitoring Device using ESP32
🔗 Precision Digital Micro Current Meter
🔗 Mobile-Operated Home Automation with Energy Meter (NodeMCU)


📄 License
This project is open source. Feel free to use, modify, and distribute with proper attribution.

<div align="center">
Built with ❤️ using Arduino UNO R4 WiFi + PZEM-004T + ThingSpeak
⭐ Star this repo if it helped you  |  🐛 Open an issue for bugs  |  🔀 Pull requests welcome
</div>
