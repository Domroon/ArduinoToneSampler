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
float minVoltage;
float maxVoltage;
float gate;
uint8_t index;

float c5 = 4.87;
float d5 = 3.65;
float e5 = 2.43;
float f5 = 1.21;

float frequency[] = {
    349, // F4
    370, // F#4
    392, // G4
    415, // G#4
    440, // A4
    466, // Bb4
    493, // H4

    523, // C5
    554, // C#5
    587, // D5
    622, // D#5
    659, // E5
    699, // F5
    740, // F#5
    784, // G5
    831, // G#5
    880, // A5
    932, // Bb5
    988, // H5

    1047, // C6
    1109, // C#6
    1175, // D6
    1245, // D#6
    1319, // E6
    1397, // F6
    1480, // F#6
    1568, // G6
    1661, // G#6
    1760, // A6
    1865, // Bb6
    1976, // H6

    2093  // C7
};

float cvVoltages[] = {
    1.418,
    1.501,
    1.584,
    1.668,
    1.751,
    1.834,
    1.918,

    2,
    2.083,
    2.167,
    2.251,
    2.334,
    2.418,
    2.501,
    2.584,
    2.668,
    2.751,
    2.834,
    2.918,

    3,
    3.083,
    3.167,
    3.251,
    3.334,
    3.418,
    3.501,
    3.584,
    3.668,
    3.751,
    3.834,
    3.918,

    4
};

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

float readPitchVoltage(){
    // sensorValue = analogRead(A0);
    return analogRead(A0) * (4.87 / 1023.0);
}

float readGateVoltage(){
    // sensorValue = analogRead(A1);
    return analogRead(A1) * (4.87 / 1023.0);
}

void testToneLEDMapping(){
    voltage = readPitchVoltage();
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

uint8_t getFrequencyIndex(){
    voltage = readPitchVoltage();
    // Serial.print("voltage: ");
    // Serial.println(voltage);
    for(int i=0; i<32; i++){
        minVoltage = cvVoltages[i] - cvVoltages[i] * 0.02;
        maxVoltage = cvVoltages[i] + cvVoltages[i] * 0.02;
        if(voltage >= minVoltage && voltage <= maxVoltage){
            Serial.print("voltage: ");
            Serial.println(voltage);
            Serial.print("min voltage: ");
            Serial.println(minVoltage);
            Serial.print("max voltage: ");
            Serial.println(maxVoltage);
            Serial.print("index: ");
            Serial.println(i);
            return i;
        }
    }
    //return 0;
}

void sendByte(uint8_t byte){
    SPI.begin();
    digitalWrite(CS, LOW);
    SPI.transfer(byte);
    digitalWrite(CS, HIGH);
    SPI.endTransaction();
}

void testSPI(){
    // Write 2 x 8 Bit to Shift Reg, //ister
    sendByte(0b11001100);
    delay(100);
    sendByte(0b00110011);
    delay(100);
}

void testToneMapping2(){
    index = getFrequencyIndex();
    if(readGateVoltage() < 3){
        noTone(8);
        sendByte(LEDREG);
    } else {
        tone(8, frequency[index]);
        sendByte(LEDREG | (1<<index));
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(CS, OUTPUT);
    SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
}

void loop() {
    // testToneLEDMapping();
    testToneMapping2();
}
