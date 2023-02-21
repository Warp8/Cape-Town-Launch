/* STOCKBRIDGE SPECIAL PROJECTS LAB CAPETOWN LAUNCH CODE */
/* Designed for the CWV Module with Gas - SGP30 */
/* IMU - LIS2DH12, and Altitude - SPL06 */

/* --- Headers --- */
#include <Arduino.h>
#include <Wire.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

/* --- Sensors --- */
#include <SPL06-007.h> // Altitude (you may have to modify the I2C address in the library to 0x77)
#include "SparkFun_LIS2DH12.h" // IMU
#include "SparkFun_SGP30_Arduino_Library.h" // Gas Sensor

/* --- SD Card/SPI pins --- */
#define SPI_MISO 12
#define SPI_MOSI 13
#define SPI_SCK 14
#define SD_CS 5

/* --- I2C Lines for CWV Module --- */
#define I2C_SDA 26
#define I2C_SCL 27

String defaultDataFileName = "data";
String defaultLogPath = "/log.txt";
int threshold = 2500; // (2.5Gs of force) Threshold for the accelerometer to initiate the rest of the script (to stop from reading before the rocket is launched) mm/s^2
bool hasLaunched = false;
double localPressure = 1013.3; // Local pressure in hPa (Cape Town South Africa)
String header = "Time,Temperature C,Pressure mb,Altitude M,Acell X,Acell Y,Acell Z,CO2 ppm,TVOC ppb";

SPIClass spi = SPIClass(HSPI); //It's important this is outside of the setup function for some reason
SPARKFUN_LIS2DH12 accel;
SGP30 sgp30;

String dataPath = "/" + defaultDataFileName + ".csv";

void log(String message) { // Logs a message to the serial monitor and to the log file
  Serial.println(String(millis()) + " " + message);
  write(defaultLogPath, String(millis()) + " " + message);
}

void write(String path, String data) {
  File file = SD.open(path, FILE_APPEND);
  if(!file) { return; }
  if(file.println(data)) { } else { }
  file.close();
}

void takeReadings() {
  Serial.println(String(millis()) + " Measuring Sensors");
  sgp30.measureAirQuality();
  String data = "";
  data += 
    String(millis()) + "," +
    String(get_temp_c()) + "," +
    String(get_pressure()) + "," +
    String(get_altitude(get_pressure(),localPressure)) + "," +
    String(accel.getX()) + "," +
    String(accel.getY()) + "," +
    String(accel.getZ()) + "," +
    String(sgp30.CO2) + "," +
    String(sgp30.TVOC);
  Serial.println(String(millis()) + " Finished measuring sensors");
  // Write the data to the CSV file
  write(dataPath, data);
  Serial.println(String(millis()) + " Data written to SD card");
}

void initializeSensors() {
  if (!sgp30.begin()) {
    log("SGP30 not found");
    return;
  } else {
    log("SGP30 found");
    sgp30.initAirQuality();
  }

  if (!accel.begin()) {
    log("LIS2DH12 not found");
    return;
  } else {
    log("LIS2DH12 found");
  }

  try {
    SPL_init();
  } catch (int e) {
    log("SPL06-007 not found");
    return;
  }
  log("SPL06-007 found");
}

void initializeSD() {
  spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS); 
  if (!SD.begin(SD_CS, spi)) { //Initialize SD Card on SPI bus
    Serial.println(String(millis()) + " SD Card initialization failed");
    return;
  } else { 
    log("SD Card initialized");
  }

  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){ //Check if SD Card is present
    Serial.println(String(millis()) + " No SD Card present");
    return; 
  } else { 
    log("SD Card present");
  }

  if(SD.exists(dataPath)) { //Check if the data file already exists and create a new one if it does
    log("Data file already exists, creating a new one");
    // add a number to the end of the file name and check again
    int i = 1;
    while (SD.exists("/" + defaultDataFileName + String(i) + ".csv")) {
      i++;
    }
    dataPath = "/" + defaultDataFileName + String(i) + ".csv";
  }

  // Write the header to the CSV file
  write(dataPath, header);
  log("Header written to SD card");
}

void setup() {
  delay(5000);
  // Initialize I2C, SD, Sensors, and Serial
  Serial.begin(115200);
  Serial.println(String(millis()) + " Starting...");

  // Initialize I2C
  Wire.setPins(I2C_SDA, I2C_SCL);
  Wire.begin();

  // Initialize SD Card
  initializeSD();

  // Initialize Sensors
  initializeSensors();

  log("Finished setup");
}

void loop() {
  if (millis() % 150 == 0) {
    if (hasLaunched == true) {
      //log("Taking readings");
      takeReadings();
      //log("Finished taking readings");
    }
    // Check if the accelerometer is moving and take readings if it is to save power and space on the SD card.
    // The threshold must be more than the force of gravity (9810) to prevent false positives
    if ((accel.getX() > threshold or accel.getX() < -threshold or accel.getY() > threshold or accel.getY() < -threshold or accel.getZ() > threshold or accel.getZ() < -threshold) and hasLaunched == false) {
      log("Rocket Launch Detected at " + String(millis()/60000) + " minutes");
      hasLaunched = true;
    }
  }
}