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

void setup() {
    Serial.begin(9600);
    Serial.print("Start");
}

void loop() {
    testVoltageDivider();
}
