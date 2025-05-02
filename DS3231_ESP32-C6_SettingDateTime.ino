/*
 * ESP32-C6 DS3231 Time Setter Tool
 * 
 * This code is specifically designed to set the time on your DS3231 RTC module
 * through serial commands. Once the time is set, the RTC will maintain the time
 * using its battery backup, even when the ESP32 is powered off or reset.
 *
 * Connections:
 * DS3231 VCC -> ESP32-C6 3.3V
 * DS3231 GND -> ESP32-C6 GND
 * DS3231 SDA -> ESP32-C6 GPIO9 (default I2C SDA pin for ESP32-C6)
 * DS3231 SCL -> ESP32-C6 GPIO8 (default I2C SCL pin for ESP32-C6)
 */

 #include <Wire.h>
 #include <RTClib.h>
 
 // Create RTC object
 RTC_DS3231 rtc;
 
 // Buffer for serial input
 String inputBuffer = "";
 bool inputComplete = false;
 
 void setup() {
   // Initialize serial communication
   Serial.begin(115200);
   delay(1000);
   
   Serial.println("\n===== DS3231 RTC Time Setter Tool =====");
   
   // Initialize I2C
   Wire.begin(5, 6);  // SDA = GPIO6, SCL = GPIO5 (default for ESP32-C6)
   
   // Check if RTC module is available
   if (!rtc.begin()) {
     Serial.println("ERROR: Couldn't find RTC module!");
     Serial.println("Please check your wiring connections.");
     while (1) delay(10); // Stop execution
   }
   
   Serial.println("RTC module found and connected successfully!");
   
   // Get and display current time
   displayCurrentTime();
   
   // Show usage instructions
   printInstructions();
 }
 
 void loop() {
   // Check if data is available to read
   while (Serial.available()) {
     char inChar = (char)Serial.read();
     
     // Process commands
     if (inChar == '\n' || inChar == '\r') {
       if (inputBuffer.length() > 0) {
         processCommand(inputBuffer);
         inputBuffer = "";
       }
     } else {
       // Add character to input buffer
       inputBuffer += inChar;
     }
   }
   
   // Display current time every second
   static unsigned long lastDisplayTime = 0;
   if (millis() - lastDisplayTime > 1000) {
     lastDisplayTime = millis();
     displayCurrentTime();
   }
 }
 
 void printInstructions() {
   Serial.println("\n-------------------------------------------");
   Serial.println("COMMAND MENU:");
   Serial.println("-------------------------------------------");
   Serial.println("1. Set time using format: set YYYY-MM-DD HH:MM:SS");
   Serial.println("   Example: set 2025-04-30 14:30:00");
   Serial.println("2. Show these instructions: help");
   Serial.println("3. Show current time: time");
   Serial.println("4. Show temperature: temp\n");
   Serial.println("NOTE: Once time is set, it will be maintained by");
   Serial.println("the RTC module's battery even after power cycles.");
   Serial.println("-------------------------------------------");
 }
 
 void processCommand(String command) {
   command.trim();
   command.toLowerCase();
   
   if (command == "help") {
     printInstructions();
   }
   else if (command == "time") {
     displayCurrentTime();
   }
   else if (command == "temp") {
     Serial.print("RTC Temperature: ");
     Serial.print(rtc.getTemperature());
     Serial.println(" °C");
   }
   else if (command.startsWith("set ")) {
     setRtcTime(command.substring(4));
   }
   else {
     Serial.println("ERROR: Unknown command. Type 'help' for instructions.");
   }
 }
 
 void setRtcTime(String timeString) {
   // Expected format: YYYY-MM-DD HH:MM:SS
   timeString.trim();
   
   // Check minimum length requirement
   if (timeString.length() < 19) {
     Serial.println("ERROR: Invalid time format. Use: YYYY-MM-DD HH:MM:SS");
     return;
   }
   
   // Parse date components
   int year = timeString.substring(0, 4).toInt();
   int month = timeString.substring(5, 7).toInt();
   int day = timeString.substring(8, 10).toInt();
   
   // Parse time components
   int hour = timeString.substring(11, 13).toInt();
   int minute = timeString.substring(14, 16).toInt();
   int second = timeString.substring(17, 19).toInt();
   
   // Validate components
   if (year < 2000 || year > 2099 || month < 1 || month > 12 || day < 1 || day > 31 ||
       hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
     Serial.println("ERROR: Time values out of valid range!");
     return;
   }
   
   // Set the RTC time
   rtc.adjust(DateTime(year, month, day, hour, minute, second));
   
   Serial.println("SUCCESS: Time has been set!");
   Serial.println("The RTC will maintain this time using its battery backup.");
   
   // Show the newly set time
   displayCurrentTime();
 }
 
 void displayCurrentTime() {
   DateTime now = rtc.now();
   
   Serial.print("Current time: ");
   Serial.print(now.year(), DEC);
   Serial.print('-');
   if (now.month() < 10) Serial.print('0');
   Serial.print(now.month(), DEC);
   Serial.print('-');
   if (now.day() < 10) Serial.print('0');
   Serial.print(now.day(), DEC);
   
   Serial.print(' ');
   
   if (now.hour() < 10) Serial.print('0');
   Serial.print(now.hour(), DEC);
   Serial.print(':');
   if (now.minute() < 10) Serial.print('0');
   Serial.print(now.minute(), DEC);
   Serial.print(':');
   if (now.second() < 10) Serial.print('0');
   Serial.print(now.second(), DEC);
   
   Serial.print(" (");
   printDayOfWeek(now.dayOfTheWeek());
   Serial.println(")");
 }
 
 void printDayOfWeek(uint8_t day) {
   const char* daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
   if (day < 7) {
     Serial.print(daysOfWeek[day]);
   } else {
     Serial.print("Unknown");
   }
 }