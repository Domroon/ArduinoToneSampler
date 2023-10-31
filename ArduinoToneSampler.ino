#include <SPI.h>

#define PITCH A0
#define GATE A1
#define AUDIOOUT 8

#define CS 10

#define LEDREG 0b00000000 
#define LED0 0
#define LED1 1
#define LED2 2
#define LED3 3

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

float frequencies[] = {
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

class Audio {
    private:
        float* _frequencies;
        float* _cvVoltages;
        uint8_t _outputPin;
        float _minVoltage;
        float _maxVoltage;
        void _playTone(float pitch, float gate, uint8_t i){
            _minVoltage = _cvVoltages[i] - _cvVoltages[i] * 0.02;
            _maxVoltage = _cvVoltages[i] + _cvVoltages[i] * 0.02;
            if(pitch >= _minVoltage && pitch <= _maxVoltage){
                if(gate > 3.5){
                    tone(_outputPin, _frequencies[i]);
                    return;
                } else {
                    noTone(_outputPin);
                    return;
                }  
            }
        }
    public:
        Audio(float frequencies[], float cvVoltages[], uint8_t outputPin){
            _frequencies = frequencies;
            _cvVoltages = cvVoltages;
            _outputPin = outputPin;
        }
        void determineSound(float pitch, float gate){                  // execute in loop-function
            for(uint8_t i=0; i<32; i++){
                _playTone(pitch, gate, i);
            }
        }
};

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

float calculateVoltage(uint8_t analogInput){
    return analogRead(analogInput) * (4.87 / 1023.0);
}

Audio audio(frequencies, cvVoltages, AUDIOOUT);

void setup() {
    Serial.begin(9600);
    pinMode(CS, OUTPUT);
    SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
}

void loop() {
    float pitch = calculateVoltage(PITCH);
    float gate = calculateVoltage(GATE);
    audio.determineSound(pitch, gate);
}
