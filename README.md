⚡ Smart Grid Monitoring System Using IoT
🚀 Project Overview

This project demonstrates a Smart Grid Monitoring System using an Arduino UNO R4 WiFi and the PZEM-004T Energy Meter.

It enables real-time monitoring of electrical parameters such as voltage, current, power, energy, frequency, and power factor. The data is displayed locally on an OLED display and simultaneously uploaded to the cloud using ThingSpeak for remote access and analysis.

🎯 Features
✔ Real-time energy monitoring
✔ Cloud integration using ThingSpeak
✔ OLED display for instant readings
✔ WiFi-enabled remote monitoring
✔ Non-blocking code execution
✔ Push-button controlled display modes
✔ Accurate measurements using PZEM module

---
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

---
⚙️ Working Principle
The PZEM-004T measures voltage and current from the AC line
It internally calculates power, energy, frequency, and power factor
Arduino reads data using Modbus RTU via UART
Data is displayed on OLED and sent to ThingSpeak
The process repeats continuously

---
🌐 ThingSpeak Setup
Create a ThingSpeak account
Create a new channel
Add fields: Voltage, Current, Power, Energy, Frequency, Power Factor
Copy Channel ID and Write API Key
Paste them into the Arduino code

---

📊 Parameters Measured
Parameter	Unit
Voltage	V
Current	mA
Power	W
Energy	kWh
Frequency	Hz
Power Factor	—

---

🖥️ Output
OLED Display → Real-time readings
Serial Monitor → Debug data
ThingSpeak Dashboard → Graphs and logs
📡 Applications
Smart Energy Monitoring
Home Power Management
Industrial Load Monitoring
Electrical Labs
Preventive Maintenance Systems

---

🔧 Troubleshooting
Issue	Cause	Solution
No readings	Wiring issue	Check TX/RX and ground
Error code 1	CRC mismatch	Reduce noise and wiring length
0 Current	CT issue	Clamp only live wire
OLED blank	I2C issue	Check address (0x3C)
WiFi not connecting	Network issue	Verify SSID and password

---

🔮 Future Enhancements
SD card logging
Multi-device monitoring
SMS/email alerts
Mobile app integration
Energy analytics

---
📌 Conclusion

This project provides a simple and reliable smart grid monitoring solution using IoT. It combines real-time sensing, local display, and cloud logging, making it suitable for both learning and real-world applications.

---
