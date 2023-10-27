#include <SPI.h>

#define C5 523
#define Db5 554
#define D5 587
#define Eb5 622
#define E5 659
#define F5 699
#define Gb5 740
#define G5 784
#define Ab5 831
#define A5 880
#define Bb5 932
#define H5 988

#define CS 10

#define LEDREG 0b00000000 
#define LED0 0
#define LED1 1
#define LED2 2
#define LED3 3

int sensorValue;
float voltage;

float c5 = 4.87;
float d5 = 3.65;
float e5 = 2.43;
float f5 = 1.21;

void testToneScale(){
    tone(8, C5);
    delay(250);
    noTone(8); 
    
    tone(8, Db5); 
    delay(250); 
    noTone(8);

    tone(8, D5);
    delay(250);
    noTone(8); 

    tone(8, Eb5);
    delay(250);
    noTone(8); 

    tone(8, E5);
    delay(250);
    noTone(8); 

    tone(8, F5);
    delay(250);
    noTone(8); 

    tone(8, Gb5);
    delay(250);
    noTone(8); 

    tone(8, G5);
    delay(250);
    noTone(8); 

    tone(8, Ab5);
    delay(250);
    noTone(8); 

    tone(8, A5);
    delay(250);
    noTone(8); 

    tone(8, Bb5);
    delay(250);
    noTone(8); 

    tone(8, H5);
    delay(250);
    noTone(8); 
}

void testVoltageDivider(){
    sensorValue = analogRead(A0);
    voltage = sensorValue * (4.87 / 1023.0);
    Serial.println(voltage);
    delay(500);
}

float readVoltage(){
    sensorValue = analogRead(A0);
    return sensorValue * (4.87 / 1023.0);
}

void testToneLEDMapping(){
    voltage = readVoltage();
    if(voltage >= c5-c5*0.1 && voltage <= c5+c5*0.1){
        tone(8, C5);
        sendByte(LEDREG | (1<<LED0));
    } else if(voltage >= d5-d5*0.1 && voltage <= d5+d5*0.1){
        tone(8, D5);
        sendByte(LEDREG | (1<<LED1));
    } else if(voltage >= e5-e5*0.1 && voltage <= e5+e5*0.1){
        tone(8, E5);
        sendByte(LEDREG | (1<<LED2));
    } else if(voltage >= f5-f5*0.1 && voltage <= f5+f5*0.1){
        tone(8, F5);
        sendByte(LEDREG | (1<<LED3));
    } else if(voltage < 0.2){
        sendByte(LEDREG);
        noTone(8);
    } 
}

void sendByte(uint8_t byte){
    SPI.begin();
    digitalWrite(CS, LOW);
    SPI.transfer(byte);
    digitalWrite(CS, HIGH);
    SPI.endTransaction();
}

void testSPI(){
    // Write 2 x 8 Bit to Shift Register
    sendByte(0b11001100);
    delay(100);
    sendByte(0b00110011);
    delay(100);
}

void setup() {
    pinMode(CS, OUTPUT);
    SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
}

void loop() {
    testToneLEDMapping();
}
