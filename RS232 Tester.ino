
/*
 Name:		Serial_to_HTTP.ino
 Created:	8/9/2020 8:59:28 PM
 Author:	Lucas Lego Andrade
 The project is to make a tester for RS232 serial cables and systens, with pre made strings as aditional features as loopback, Null
 Additionaly will provide RS485 with all teh same functions as rs232 and analog reading and 4 - 20mA sensors reading 
 There is a option to log the data on a SD card with all functions
 */

#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "FS.h"
#include "SD.h"



 //Oled Display settings
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     14 // Reset pin #
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SD_CS 5

String timeStamp;

String data = "";

String dataMessage;

RTC_DATA_ATTR int readingID = 0;

/*Swtiches to select the function
//const int paro = 2;
//const int tss = 3;
//const int json = 4;
//const int xml = 5;*/

const int sw1 = 16;
const int sw2 = 17;
const int sw3 = 27;
const int sw4 = 25;
const int sw5 = 5;

DynamicJsonDocument dataJson(1000);

void setup() {


    Serial.begin(115200);


    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay(); // Clear the buffer
    display.setTextSize(2);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
  

    pinMode(sw1, INPUT);
    pinMode(sw2, INPUT);
    pinMode(sw3, INPUT);
    pinMode(sw4, INPUT);
    pinMode(sw5, INPUT);
       
    SD.begin(SD_CS);
    if (!SD.begin(SD_CS)) {
        Serial.println("Card Mount Failed");
        data = "Card Mount Failed";
        return;
    }
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        data = "No SD card attached";
        return;
    }
    Serial.println("Initializing SD card...");
    data = "Initializing SD card...";
    if (!SD.begin(SD_CS)) {
        Serial.println("ERROR - SD card initialization failed!");
        data = "ERROR - SD card initialization failed!";
        return;    // init failed
    }

    // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
    File file = SD.open("/data.txt");
    if (!file) {
        Serial.println("File doens't exist");
        data = "File doens't exist";
        Serial.println("Creating file...");
        data = "File doens't exist";
        writeFile(SD, "/data.txt", "Reading ID, Value \r\n");
    }
    else {
        Serial.println("File already exists");
        data = "File already exists";
    }
    file.close();

}


int paroSignal() {

    Serial.flush();
    Serial.begin(9600);
    data = "P";
    data += String(random(0, 3000));
    Serial.println(data);
}

int tssSignal() {

    Serial.flush();
    Serial.begin(9600);
    data = "H";
    data += String(random(0, 360));
    Serial.println(data);
}

int jsonSignal() {

    dataJson["RHeading"] = random(0, 360);
    dataJson["RDepth"] = random(0, 3000);
    dataJson["RAlt"] = random(0, 30);
    dataJson["CHeading"] = random(0, 360);
    dataJson["CDepth"] = random(0, 3000);
    dataJson["RPitch"] = random(-15, 15);
    dataJson["RRoll"] = random(-15, 15);
    dataJson["RTurns"] = random(-3, 3);
    dataJson["CTurns"] = random(-3, 3);
    dataJson["CP1"] = random(-1300, -600);
    dataJson["CP2"] = random(-1300, -600);
    dataJson["RBty"] = random(0, 3000);
    dataJson["North"] = random(0, 700000000);
    dataJson["East"] = random(0, 700000000);
    dataJson["KP1"] = random(0, 7000);
    dataJson["KP2"] = random(0, 7000);

    Serial.flush();
    Serial.begin(115200);
    data = "";
    Serial.print("[");
    serializeJson(dataJson, Serial);
    serializeJson(dataJson, data);
    Serial.println("]");
}

int xmlSignal() {

    Serial.flush();
    Serial.begin(115200);

    data = "/*KAANSATQRO,2051,2,5,26,10,101.26,27,32,1001,21.1619,86.8515,10,4,1.23,9.81,0.23,0,0,0*/";

    Serial.println(data);
}

void logSDCard() {
    dataMessage = String(readingID) + ","  +
        String(data) + "\r\n";
    Serial.print("Save data: ");
    Serial.println(dataMessage);
    appendFile(SD, "/data.txt", dataMessage.c_str());
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS& fs, const char* path, const char* message) {
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("File written");
    }
    else {
        Serial.println("Write failed");
    }
    file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS& fs, const char* path, const char* message) {
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message)) {
        Serial.println("Message appended");
    }
    else {
        Serial.println("Append failed");
    }
    file.close();
}


void loop() {

    if (digitalRead(sw1) == LOW) {
        tssSignal();
    }


    if (digitalRead(sw2) == LOW) {
        paroSignal();
    }

    if (digitalRead(sw3) == LOW) {
        jsonSignal();
    }

    if (digitalRead(sw4) == LOW) {
        xmlSignal();
    }

    if (digitalRead(sw5) == LOW) {
        logSDCard();
        readingID++;
        delay(500);
    }

    display.clearDisplay();
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setCursor(0, 0);
    display.println(data);
    display.display();

    delay(500);

}