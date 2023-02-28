/* STOCKBRIDGE SPECIAL PROJECTS LAB CAPETOWN LAUNCH CODE */
/* Designed for the CWV Module with Gas - SGP30 */
/* IMU - KXTJ3-1057, and Altitude - SPL06 */

/* --- Headers --- */
#include <Arduino.h>
#include <Wire.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

/* --- Sensors --- */
#include <SPL06-007.h> // Altitude (you may have to modify the I2C address in the library to 0x77)
#include "kxtj3-1057.h" // IMU (Using new xChips IMU)
#include "SparkFun_SGP30_Arduino_Library.h" // Gas Sensor

/* --- SD Card/SPI pins --- */
#define SPI_MISO 12
#define SPI_MOSI 13
#define SPI_SCK 14
#define SD_CS 5

/* --- I2C Lines for CWV Module --- */
#define I2C_SDA 26
#define I2C_SCL 27

#define HIGH_RESOLUTION

String defaultDataFileName = "data";
String defaultLogPath = "/log.txt";
int threshold = 0; // (6Gs of force) Threshold for the accelerometer to initiate the rest of the script (to stop from reading before the rocket is launched) m/s^2
bool hasLaunched = false;
double localPressure = 1013.3; // Local pressure in hPa (Cape Town South Africa)
String header = "Time,Temperature C,Pressure mb,Altitude M,Acell X,Acell Y,Acell Z,Magnitude,CO2 ppm,TVOC ppb";

SPIClass spi = SPIClass(HSPI); //It's important this is outside of the setup function for some reason
KXTJ3 accel(0x0F);
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
  // Magnitude vector of the acceleration
  float x = accel.axisAccel(X);
  float y = accel.axisAccel(Y);
  float z = accel.axisAccel(Z);
  float magnitude = sqrt(x*x + y*y + z*z);
  String data = "";
  data += 
    String(millis()) + "," +
    String(get_temp_c()) + "," +
    String(get_pressure()) + "," +
    String(get_altitude(get_pressure(),localPressure)) + "," +
    String(x) + "," +
    String(y) + "," +
    String(z) + "," +
    String(magnitude) + "," +
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

  if(accel.begin(12.5, 8) != 0 ) { // 12.5Hz sample rate, 16g range. anything over 8g is innacurate
    log("IMU not found");
  } else {
    log("IMU found");
  }

  SPL_init(0x77);
  if (get_spl_id() != 16) {
    log("SPL06-007 not found");
    return;
  }
  log("SPL06-007 found");

  accel.intConf(123, 1, 10, HIGH);
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
    accel.standby(false);
    if (hasLaunched == true) {
      takeReadings();
    }
    // Check if the accelerometer is moving and take readings if it is to save power and space on the SD card.
    // The threshold must be more than the force of gravity (9810) to prevent false positives
    if ((accel.axisAccel(X) > threshold or accel.axisAccel(X) < -threshold or accel.axisAccel(Y) > threshold or accel.axisAccel(Y) < -threshold or accel.axisAccel(Z) > threshold or accel.axisAccel(Z) < -threshold) and hasLaunched == false) {
      log("Rocket Launch Detected at " + String(millis()/60000) + " minutes");
      hasLaunched = true;
    }
    accel.standby(true);
  }
}