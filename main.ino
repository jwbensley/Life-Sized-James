//
// GLOBAL VARS
//

// PIR output is on digital pin...
int pir_pin = 3;
// PIR calibration delay
int pir_cali_delay = 10;    /////// What is in the data sheet?
// Set true when PIR first detects motion
/////boolean pir_active = true;
// PIR delay before next going active
long unsigned int pir_delay = 3000;


/*
 * SD card attached to SPI bus as follows:
 * MOSI - pin 11
 * MISO - pin 12
 * CLK -  pin 13
 * CS -   pin 8
*/    

#include <SD.h>

// Set up SD utility library variables
Sd2Card card;
SdVolume volume;
SdFile root;
// CS pin
const int chipSelect = 8;


#include <TMRpcm.h>
TMRpcm tmrpcm;


// Unused analog pin to seed random()
int rnd_pin = 0;
int seed;

//
// END
//


void setup() {
  
  Serial.begin(9600);

  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  pinMode(pir_pin, INPUT);
  digitalWrite(pir_pin, LOW);

  // PIR calibration time
  Serial.print("Calibrating sensor (");
  Serial.print(pir_cali_delay);
  Serial.print(" seconds) ");
  for(int i = 0; i < pir_cali_delay; i++){
    Serial.print(".");
    delay(1000);
  }
  
  Serial.println(" done");

  pinMode(10, OUTPUT); ///// What is this for?
  SD.begin(chipSelect);

  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("SD card initialisation failed!");
    return;
  } else {
   Serial.println("SD card initialised"); 
  }

  // Open partition - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition!");
    return;
  }
  root.openRoot(volume);

  tmrpcm.speakerPin = 9;
  tmrpcm.setVolume(5);

  seed = analogRead(rnd_pin);
  randomSeed(seed);
  Serial.print("Random seed is ");
  Serial.println(seed);

}



void loop(){

  //if(digitalRead(pir_pin) == HIGH) {
    
    Serial.print("motion detected at ");
    Serial.print(millis()/1000);
    Serial.println(" sec, pausing...");

    int rand = random(1, 6); // Min is 0 if unspecified, max is 6-1?
    String filename = String(rand);
    filename = filename + ".wav";
    Serial.println(filename);
    
    if (SD.exists(filename)) {
      char charFN[filename.length()+1];
      filename.toCharArray(charFN, filename.length()+1);
      Serial.print("Playing ");
      Serial.println(charFN);
      tmrpcm.play(charFN); //the sound file "1" will play each time the arduino powers up, or is reset
    /*  
    File myFile = SD.open(filename);
      if (myFile) {
        //while (myFile.available()) {
        //  Serial.write(myFile.read());
        //}
        myFile.close();
      } else {
        Serial.print("Error opening ");
        Serial.println(filename);
      }
    */
    } else {
      Serial.print(filename);
      Serial.println(" not found!");
    }

    seed = analogRead(rnd_pin);
    randomSeed(seed);

    delay(pir_delay);

    Serial.println("Ready.");
    
  //} //digitalRead(pir_pin) == HIGH

}
