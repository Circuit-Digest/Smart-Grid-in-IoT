⚡ SMART GRID MONITORING SYSTEM USING IoT
🚀 PROJECT OVERVIEW

This project demonstrates a Smart Grid Monitoring System using an Arduino UNO R4 WiFi and the PZEM-004T Energy Meter.

It measures electrical parameters in real time and displays them locally on an OLED display while also sending data to ThingSpeak cloud for remote monitoring.

🎯 FEATURES

• Real-time electrical parameter monitoring
• Cloud logging using ThingSpeak
• OLED display for live readings
• WiFi-enabled remote monitoring
• Non-blocking program execution
• Push-button controlled display

🛠️ COMPONENTS REQUIRED
S.No | Component | Quantity | Purpose

1 | Arduino UNO R4 WiFi | 1 | Main controller
2 | PZEM-004T Energy Meter | 1 | Measures parameters
3 | Current Transformer (CT) | 1 | Current sensing
4 | OLED Display (SSD1306) | 1 | Display output
5 | Push Button | 1 | Control display
6 | Breadboard / PCB | 1 | Circuit setup
7 | Jumper Wires | As req. | Connections
8 | USB Cable | 1 | Power & programming
9 | AC Load | 1 | Testing

⚙️ WORKING PRINCIPLE
PZEM-004T measures voltage and current
It calculates power, energy, frequency, and power factor
Arduino reads data using UART (Modbus RTU)
Data is:
Displayed on OLED
Sent to ThingSpeak cloud
Process repeats continuously
📡 SYSTEM FLOW

AC Supply → PZEM → Arduino → OLED
↓
ThingSpeak Cloud

🌐 THINGSPEAK SETUP
Create ThingSpeak account
Create a new channel
Add fields:
Voltage
Current
Power
Energy
Frequency
Power Factor
Copy Channel ID & API Key
Paste into code
💻 IMPORTANT CODE SNIPPETS

Include Libraries:
#include <WiFiS3.h>
#include <ThingSpeak.h>
#include <PZEM004Tv40_R4.h>

Initialize:
PZEM004Tv40_R4 pzem(&Serial1);

Read Data:
if (pzem.readAll()) {
voltage = pzem.getVoltage();
}

Upload:
ThingSpeak.setField(1, voltage);

📊 PARAMETERS MEASURED

• Voltage (V)
• Current (mA)
• Power (W)
• Energy (kWh)
• Frequency (Hz)
• Power Factor

🖥️ OUTPUT

• OLED Display → Live data
• Serial Monitor → Debug
• ThingSpeak → Graphs

📡 APPLICATIONS

• Smart energy monitoring
• Home power management
• Industrial load monitoring
• Electrical labs

🔧 TROUBLESHOOTING

Problem: No readings
Solution: Check TX/RX wiring

Problem: 0 Current
Solution: Check CT placement

Problem: OLED not working
Solution: Check I2C address

Problem: WiFi not connecting
Solution: Check credentials

🔮 FUTURE ENHANCEMENTS

• SD card logging
• SMS alerts
• Mobile app
• Multi-meter system

📌 CONCLUSION

This project provides a simple and reliable smart grid monitoring solution with both local and cloud-based tracking.
