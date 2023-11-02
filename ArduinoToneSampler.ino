#include <SPI.h>

#define PITCH A0
#define GATE A1
#define AUDIOOUT 8

#define CS_TONE_LIGHT 10
#define CS_STEP_LIGHT 9
#define CS_MODE_LIGHT 7

#define LEDREG 0b00000000 
#define LEDREG16 0b0000000000000000 
#define LED0 0
#define LED1 1
#define LED2 2
#define LED3 34

#define TOLERANCE 0.01

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
            _minVoltage = _cvVoltages[i] - _cvVoltages[i] * TOLERANCE;
            _maxVoltage = _cvVoltages[i] + _cvVoltages[i] * TOLERANCE;
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

class Light {
    private:
        float* _frequencies;
        float* _cvVoltages;
        uint8_t _csToneLight;
        uint8_t _csStepLight;
        uint8_t _csModeLight;
        float _minVoltage;
        float _maxVoltage;
        uint8_t _toneByte;
        uint8_t _lightByte;
        uint16_t _lightWord;
        void _sendByte(uint8_t byte, uint8_t cs){
            SPI.begin();
            digitalWrite(cs, LOW);
            SPI.transfer(byte);
            digitalWrite(cs, HIGH);
            SPI.endTransaction();
        }
        void _sendWord(uint16_t word, uint8_t cs){
            SPI.begin();
            digitalWrite(cs, LOW);
            SPI.transfer16(word);
            digitalWrite(cs, HIGH);
            SPI.endTransaction();
        }
        void _createToneData(float pitch, float gate, uint8_t i){
            _minVoltage = _cvVoltages[i] - _cvVoltages[i] * TOLERANCE;
            _maxVoltage = _cvVoltages[i] + _cvVoltages[i] * TOLERANCE;
            if(pitch >= _minVoltage && pitch <= _maxVoltage){
                if(gate > 3.5){
                    _toneByte = LEDREG | (1 << i);
                    return;
                } else {
                    _toneByte = LEDREG;
                    return;
                }  
            }
        }
        void _createStepData(){
            for(int i=15; i>=0; i--){
                _lightWord = LEDREG16 | (1 << i);
                _sendWord(_lightWord, _csStepLight);
                delay(50);
            }
        }
    public:
        Light(float frequencies[], float cvVoltages[], uint8_t csToneLight, uint8_t csStepLight, uint8_t csModeLight){
            _frequencies = frequencies;
            _cvVoltages = cvVoltages;
            _csToneLight = csToneLight;
            _csStepLight = csStepLight;
            _csModeLight = csModeLight;

            _toneByte = LEDREG;

            pinMode(_csToneLight, OUTPUT);
            pinMode(_csStepLight, OUTPUT);
            pinMode(_csModeLight, OUTPUT);
        }
        void determineToneLight(float pitch, float gate){
            for(uint8_t i=0; i<32; i++){
                _createToneData(pitch, gate, i);
                _sendByte(_toneByte, _csToneLight);
            }
        }
        void showStartup(){
            for(int i=15; i>=0; i--){
                _lightWord = LEDREG16 | (1 << i);
                _sendWord(_lightWord, _csStepLight);
                delay(50);
            }
            _sendWord(LEDREG16, _csStepLight);
        }
};

float calculateVoltage(uint8_t analogInput){
    return analogRead(analogInput) * (4.87 / 1023.0);
}

Audio audio(frequencies, cvVoltages, AUDIOOUT);
Light light(frequencies, cvVoltages, CS_TONE_LIGHT, CS_STEP_LIGHT, CS_MODE_LIGHT);

void setup() {
    Serial.begin(9600);
    Serial.println("Start Device");
    SPI.beginTransaction(SPISettings(16000000, LSBFIRST, SPI_MODE0));
    light.showStartup();
}

void loop() {
    float pitch = calculateVoltage(PITCH);
    float gate = calculateVoltage(GATE);
    audio.determineSound(pitch, gate);
    light.determineToneLight(pitch, gate);
}
