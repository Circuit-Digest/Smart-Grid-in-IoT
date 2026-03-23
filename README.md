Smart Grid Monitoring System Using IoT
🚀 Project Overview

This project demonstrates a Smart Grid Monitoring System that measures and monitors electrical parameters in real time using an Arduino UNO R4 WiFi and PZEM-004T Energy Meter.

The system continuously tracks voltage, current, power, energy, frequency, and power factor, and sends the data to the cloud using ThingSpeak. It also displays real-time values locally on an OLED display, making it a complete IoT-based energy monitoring solution.

🎯 Features

✔ Real-time electrical parameter monitoring
✔ Cloud data logging using ThingSpeak
✔ OLED display for live readings
✔ WiFi-enabled remote monitoring
✔ Non-blocking code for smooth operation
✔ Rolling display using push button
✔ Accurate measurement using PZEM module

🛠️ Components Required
S.No	Component	Quantity	Purpose
1	Arduino UNO R4 WiFi	1	Main controller with WiFi
2	PZEM-004T Energy Meter	1	Measures electrical parameters
3	Current Transformer (CT)	1	Current sensing
4	OLED Display (SSD1306)	1	Displays readings
5	Push Button	1	Controls display mode
6	Breadboard / PCB	1	Circuit setup
7	Jumper Wires	As required	Connections
8	USB Cable	1	Programming & power
9	AC Load	1	Testing
⚙️ Working Principle
PZEM-004T measures voltage and current from the AC line.
It internally calculates power, energy, frequency, and power factor.
Arduino reads data using Modbus RTU via UART.
Data is processed and:
Displayed on OLED
Printed on Serial Monitor
Uploaded to ThingSpeak
The cycle repeats continuously for real-time monitoring.
📡 System Flow
AC Supply → PZEM-004T → Arduino UNO R4 WiFi → OLED Display
                                      ↓
                                 ThingSpeak Cloud
🌐 ThingSpeak Setup
Create a ThingSpeak account
Create a new channel
Add fields:
Voltage
Current
Power
Energy
Frequency
Power Factor
Copy:
Channel ID
Write API Key
Paste them into the Arduino code
💻 Important Code Snippets
Include Required Libraries
#include <WiFiS3.h>
#include <ThingSpeak.h>
#include <PZEM004Tv40_R4.h>
Initialize PZEM
PZEM004Tv40_R4 pzem(&Serial1);
Read Sensor Data
if (pzem.readAll()) {
  voltage = pzem.getVoltage();
  current = pzem.getCurrent();
}
Upload to ThingSpeak
ThingSpeak.setField(1, voltage);
ThingSpeak.writeFields(channelNumber, apiKey);
📊 Parameters Measured
Voltage (V)
Current (mA)
Power (W)
Energy (kWh)
Frequency (Hz)
Power Factor
🖥️ Output
OLED Display → Real-time values
Serial Monitor → Debug data
ThingSpeak Dashboard → Graphs and history
📡 Applications
Smart Energy Monitoring
Home Power Management
Industrial Load Monitoring
Electrical Lab Analysis
Preventive Maintenance Systems
🔧 Troubleshooting
No Data from PZEM
Check TX/RX connections
Ensure common ground
Verify Serial1 usage
Constant Zero Current
Clamp CT only on live wire
Do not clamp both wires
OLED Not Working
Check I2C address (0x3C)
Verify SDA and SCL connections
WiFi Not Connecting
Check SSID and password
Ensure proper signal strength
Random Reset
Use stable power supply
Avoid powering from weak USB
🔮 Future Enhancements
Add SD card for offline logging
Multi-meter monitoring system
SMS/email alert system
Mobile app integration
Advanced energy analytics
📚 Learning Outcomes

After completing this project, you will understand:

IoT-based energy monitoring
Modbus RTU communication
PZEM-004T working
OLED interfacing with Arduino
WiFi and cloud integration
Real-time data handling
📌 Conclusion

This project provides a simple, reliable, and scalable smart grid monitoring solution using IoT. It combines real-time sensing, local display, and cloud logging, making it ideal for both learning and practical applications in energy management.

Author :
Vedhathiri K
