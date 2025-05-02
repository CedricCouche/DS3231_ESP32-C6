/*
 * ESP32-C6 Test Code for DS3231 RTC Module
 * 
 * This code initializes the DS3231 RTC module, sets the time (if needed),
 * and continuously reads and displays the time from the module.
 * 
 * Connections:
 * DS3231 VCC -> ESP32-C6 3.3V
 * DS3231 GND -> ESP32-C6 GND
 * DS3231 SDA -> ESP32-C6 GPIO6 
 * DS3231 SCL -> ESP32-C6 GPIO5
 */

 #include <Wire.h>
 #include <RTClib.h>
 
 // Create an RTC object
 RTC_DS3231 rtc;
 
 void setup() {
   // Initialize serial communication
   Serial.begin(115200);
   
   // Wait for serial port to connect
   delay(3000);
   
   Serial.println("\nDS3231 RTC Test for ESP32-C6");
   
   // Initialize I2C communication
   Wire.begin(5, 6); // SDA = GPIO6, SCL = GPIO5
   
   // Check if the RTC is present and working
   if (!rtc.begin()) {
     Serial.println("Couldn't find RTC! Check your connections.");
     Serial.println("Make sure the I2C address is correct (0x68).");
     Serial.println("Halting program...");
     while (1);
   }
   
   Serial.println("RTC found!");
   
   // Check if the RTC lost power and the time needs to be set
   if (rtc.lostPower()) {
     Serial.println("RTC lost power, setting the time to compile time...");
     // Sets the RTC to the date & time this sketch was compiled
     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
     Serial.println("RTC time set to compile time.");
   }
 
 
   // Print temperature from DS3231 temperature sensor
   Serial.print("Temperature: ");
   Serial.print(rtc.getTemperature());
   Serial.println(" C");
   
   Serial.println("\nRTC Test Starting...");
 }
 
 void loop() {
   // Get the current date and time from the RTC
   DateTime now = rtc.now();
   
   // Format and print the date and time
   Serial.print("Date & Time: ");
   printDateTime(now);
   
   // Check for I2C errors
   checkI2CErrors();
   
   // Wait a second before reading again
   delay(1000);
 }
 
 // Function to print date and time in a readable format
 void printDateTime(DateTime dt) {
   Serial.print(dt.year(), DEC);
   Serial.print('/');
   Serial.print(dt.month(), DEC);
   Serial.print('/');
   Serial.print(dt.day(), DEC);
   Serial.print(" (");
   Serial.print(daysOfTheWeek(dt.dayOfTheWeek()));
   Serial.print(") ");
   
   Serial.print(dt.hour(), DEC);
   Serial.print(':');
   if (dt.minute() < 10) {
     Serial.print('0');
   }
   Serial.print(dt.minute(), DEC);
   Serial.print(':');
   if (dt.second() < 10) {
     Serial.print('0');
   }
   Serial.print(dt.second(), DEC);
   Serial.println();
 }
 
 // Function to return day of week as a string
 String daysOfTheWeek(uint8_t day) {
   switch (day) {
     case 0: return "Sunday";
     case 1: return "Monday";
     case 2: return "Tuesday";
     case 3: return "Wednesday";
     case 4: return "Thursday";
     case 5: return "Friday";
     case 6: return "Saturday";
     default: return "Unknown";
   }
 }
 
 // Check for I2C errors
 void checkI2CErrors() {
   uint8_t error = Wire.endTransmission(true);
   if (error) {
     Serial.print("I2C Error: ");
     switch (error) {
       case 1: Serial.println("Data too long to fit in transmit buffer"); break;
       case 2: Serial.println("Received NACK on transmit of address"); break;
       case 3: Serial.println("Received NACK on transmit of data"); break;
       case 4: Serial.println("Other error"); break;
       default: Serial.println("Unknown error"); break;
     }
   }
 }