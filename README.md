# eCumez
This is an IoT-based smart chicken coop system that enables automatic door control, temperature and humidity monitoring, and light detection. The project utilizes NodeMCU, DHT11 sensor, LDR sensor, and a servo motor to control the door via Firebase. 

## Overview  
This is an IoT-based smart chicken coop system that enables automatic door control, temperature and humidity monitoring, and light detection. The project utilizes **NodeMCU**, **DHT11 sensor**, **LDR sensor**, and a **servo motor** to control the door via **Firebase**.  

## Features  
- **Automated Door Control** – The coop door opens and closes automatically based on sensor data 
- **Temperature & Humidity Monitoring** – Real-time environmental tracking ensures optimal conditions   
- **Light Detection** – Adjusts the coop’s lighting according to external brightness 
- **Firebase Integration** – Enables remote monitoring and control 

## Technologies Used  
- **NodeMCU (ESP8266)**  
- **DHT11 (Temperature & Humidity Sensor)**  
- **LDR (Light Sensor)**  
- **Servo Motor (Door Control)**  
- **Firebase (Real-time Database & Remote Control)**  

## Installation & Setup  
1. **Hardware Setup:**  
   - Connect the **DHT11 sensor**, **LDR sensor**, and **servo motor** to the **NodeMCU**.  
   - Ensure proper wiring and power supply.  

2. **Software Setup:**  
   - Install the required Arduino libraries:  
     ```sh  
     ArduinoJson  
     ESP8266WiFi  
     FirebaseESP8266  
     DHT  
     ```  
   - Flash the NodeMCU with the provided code.  
   - Configure **Firebase** for real-time data handling.  

3. **Usage:**  
   - Monitor the coop conditions remotely via Firebase.  
   - The system will automatically open/close the door and adjust settings based on environmental data.  

