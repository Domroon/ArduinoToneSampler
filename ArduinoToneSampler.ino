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

void testToneMapping(){
    voltage = readVoltage();
    if(voltage >= c5-c5*0.1 && voltage <= c5+c5*0.1){
        tone(8, C5);
    } else if(voltage >= d5-d5*0.1 && voltage <= d5+d5*0.1){
        tone(8, D5);
    } else if(voltage >= e5-e5*0.1 && voltage <= e5+e5*0.1){
        tone(8, E5);
    } else if(voltage >= f5-f5*0.1 && voltage <= f5+f5*0.1){
        tone(8, F5);
    } else if(voltage < 0.2){
        noTone(8);
    } 
}

void setup() {
    Serial.begin(9600);
    Serial.print("Start");
}

void loop() {
    testToneMapping();
}
