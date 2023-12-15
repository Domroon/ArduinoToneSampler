#include <SPI.h>

#define PITCH A0
#define GATE A1
#define AUDIOOUT 8

#define CS_TONE_LIGHT 10
#define CS_STEP_LIGHT 9
#define CS_OCTAVE_LIGHT 6
#define CS_MODE_LIGHT 5
#define CS_SITE_LIGHT 7

// CLK 13
// MOSI 11

#define LEDREG 0b00000000 
#define LEDREG16 0b0000000000000000
#define LEDREG8  0b00000000 
#define LED0 0
#define LED1 1
#define LED2 2
#define LED3 34

#define TOLERANCE 0.01

#define DIRECT_MODE 0
#define SAMPLE_MODE 1
#define PLAY_MODE 2

// Tones
#define C       0
#define Csharp  1
#define D       2
#define Dsharp  3
#define E       4
#define F       5
#define Fsharp  6
#define G       7
#define Gsharp  8
#define A       9
#define Bb      10
#define H       11

#define F4 349
#define Fsharp4 370
#define G4 392
#define Gsharp4 415
#define A4 440
#define Bb4 466
#define H4 493
#define C5 523
#define Csharp5 554
#define D5 587
#define Dsharp5 622
#define E5 659
#define F5 699
#define Fsharp5 740
#define G5 784
#define Gsharp5 831
#define A5 880
#define Bb5 932
#define H5 988
#define C6 1047
#define Csharp6 1109
#define D6 1175
#define Dsharp6 1245
#define E6 1319
#define F6 1397
#define Fsharp6 1480
#define G6 1568
#define Gsharp6 1661
#define A6 1760
#define Bb6 1865
#define H6 1976
#define C7 2093

uint16_t testMelody[] = {C5, Csharp5, D5, Dsharp5, E5, F5, Fsharp5, G5, Gsharp5, A5, Bb5, H5};
// uint16_t testMelody[] = {C5, E5, G5};

volatile byte state = LOW;

uint16_t stepNum = 0;

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
        void playToneMS(uint16_t freq, uint16_t timeInMS){             // Improve: It must be unblocked
            tone(_outputPin, freq);
            delay(timeInMS);
            noTone(_outputPin);
        }
};

class Light {
    private:
        float* _frequencies;
        float* _cvVoltages;
        uint8_t _csToneLight;
        uint8_t _csStepLight;
        uint8_t _csModeLight;
        uint8_t _csOctaveLight;
        uint8_t _csSiteLight;
        float _minVoltage;
        float _maxVoltage;
        uint8_t _toneByte;
        uint8_t _toneNum;
        uint8_t _octaveByte;
        uint8_t _octave;
        uint8_t _lightByte;
        uint16_t _lightWord;
        uint8_t _stepNum;
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
        Light(float frequencies[], float cvVoltages[], uint8_t csToneLight, uint8_t csStepLight, uint8_t csModeLight, uint8_t csOctaveLight, uint8_t csSiteLight){
            _frequencies = frequencies;
            _cvVoltages = cvVoltages;
            _csToneLight = csToneLight;
            _csStepLight = csStepLight;
            _csModeLight = csModeLight;
            _csOctaveLight = csOctaveLight;
            _csSiteLight = csSiteLight;

            _toneByte = LEDREG;
            _octaveByte = LEDREG16;

            pinMode(_csToneLight, OUTPUT);
            pinMode(_csStepLight, OUTPUT);
            pinMode(_csModeLight, OUTPUT);
            pinMode(_csOctaveLight, OUTPUT);
            pinMode(_csSiteLight, OUTPUT);

            changeStepQty(16);
        }

        void determineToneLight(float pitch, float gate){
            for(uint8_t i=0; i<32; i++){
                _createToneData(pitch, gate, i);
                _sendByte(_toneByte, _csToneLight);
            }
        }
        
        void showNote(uint16_t tone){
            if(tone >= C5 && tone < C6){
                _octave = 5;
            } else if(tone >= C6 && tone < C7){
                _octave = 6;
            }

            if(tone == C5 || tone == C6){
                _toneNum = 0;
            }

            if(tone == C5 || tone == C6){
                _toneNum = C;
            } else if(tone == Csharp5 || tone == Csharp6){
                _toneNum = Csharp;
            } else if(tone == D5 || tone == D6){
                _toneNum = D;
            } else if(tone == Dsharp5 || tone == Dsharp6){
                _toneNum = Dsharp;
            } else if(tone == E5 || tone == E6){
                _toneNum = E;
            } else if(tone == F5 || tone == F6){
                _toneNum = F;
            } else if(tone == Fsharp5 || tone == Fsharp6){
                _toneNum = Fsharp;
            } else if(tone == G5 || tone == G6){
                _toneNum = G;
            } else if(tone == Gsharp5 || tone == Gsharp6){
                _toneNum = Gsharp;
            } else if(tone == A5 || tone == A6){
                _toneNum = A;
            } else if(tone == Bb5|| tone == Bb6){
                _toneNum = Bb;
            } else if(tone == H5|| tone == H6){
                _toneNum = H;
            }
            
            _toneByte = LEDREG16 |  (1 << _toneNum);
            _sendByte(_toneByte, _csToneLight);

            _octaveByte = LEDREG16 | (1 << _octave);
            _sendByte(_octaveByte, _csOctaveLight);
        }

        void showStartup(uint8_t delay_time){
            _sendWord(LEDREG16, _csStepLight);      // Turn off all Step LEDS
            _sendWord(LEDREG16, _csToneLight);      // Turn off all Tone LEDS
            _sendWord(LEDREG16, _csOctaveLight);    // Turn off all Octave LEDS
            _sendWord(LEDREG16, _csModeLight);      // Turn off all Mode LEDS
            _sendWord(LEDREG16, _csSiteLight);      // Turn off all Site LEDs

            for(int i=0; i<12; i++){
                _lightWord = LEDREG16 | (1 << i);
                _sendWord(_lightWord, _csToneLight);
                delay(delay_time);
            }
            _sendWord(LEDREG16, _csToneLight);      // Turn off all Tone LEDS

            for(int i=0; i<8; i++){
                _lightByte = LEDREG8 | (1 << i);
                _sendByte(_lightByte, _csOctaveLight);
                delay(delay_time);
            }
            _sendWord(LEDREG8, _csOctaveLight);      // Turn off all Octave LEDS

            for(int i=0; i<6; i++){
                _lightByte = LEDREG8 | (1 << i);
                _sendByte(_lightByte, _csModeLight);
                delay(delay_time);
            }
            _sendWord(LEDREG8, _csModeLight);      // Turn off all Mode LEDS

            for(int i=0; i<4; i++){
                _lightByte = LEDREG8 | (1 << i);
                _sendByte(_lightByte, _csSiteLight);
                delay(delay_time);
            }
            _sendWord(LEDREG8, _csSiteLight);      // Turn off all Site LEDS

            for(int i=0; i<16; i++){
                _lightWord = LEDREG16 | (1 << i);
                _sendWord(_lightWord, _csStepLight);
                delay(delay_time);
            }
            _sendWord(LEDREG16, _csStepLight);      // Turn off all Step LEDS            
        }

        void changeStepQty(uint8_t stepNum){
            _stepNum = stepNum;
            _lightWord = LEDREG16;
            for(int i=0; i<_stepNum; i++){
                _lightWord |= (1 << i);
            }
            _sendWord(_lightWord, _csStepLight);
        }

        void showStep(uint8_t step){
            // changeStepQty(_stepNum);
            _lightWord ^= (1 << step);
            _sendWord(_lightWord, _csStepLight);
        }
};



class Device {
    public:
        uint8_t mode;
        
};

float calculateVoltage(uint8_t analogInput){
    return analogRead(analogInput) * (4.87 / 1023.0);
}

Audio audio(frequencies, cvVoltages, AUDIOOUT);
Light light(frequencies, cvVoltages, CS_TONE_LIGHT, CS_STEP_LIGHT, CS_MODE_LIGHT, CS_OCTAVE_LIGHT, CS_SITE_LIGHT);

// void exe_direct_mode() {
//     float pitch = calculateVoltage(PITCH);
//     float gate = calculateVoltage(GATE);
//     audio.determineSound(pitch, gate);
//     light.determineToneLight(pitch, gate);
// }

// void exe_play_mode(uint16_t stepNum) { // parameter: uint16_t tones[]
//     stepNum = 0;
//     for(uint16_t tone: testMelody){
//         audio.playToneMS(tone, 100);
//         light.showNote(tone);
//         stepNum += 1;
//         delay(250);
//     }
    
// }

void stop_and_start(){
    state = !state;
}

void sendByte(uint8_t byte, uint8_t cs){
    SPI.begin();
    digitalWrite(cs, LOW);
    SPI.transfer(byte);
    digitalWrite(cs, HIGH);
    SPI.endTransaction();
}

void sendWord(uint16_t word, uint8_t cs){
    SPI.begin();
    digitalWrite(cs, LOW);
    SPI.transfer16(word);
    digitalWrite(cs, HIGH);
    SPI.endTransaction();
}

class StepLight {
    private:
        uint16_t _stepWord;
        uint8_t _stepQty;
        uint8_t _step;
    public:
        StepLight(){
            pinMode(CS_STEP_LIGHT, OUTPUT);
            _stepWord = LEDREG16;
            _stepQty = 16;
            _step = 0;
        }
        void changeStepQty(uint8_t stepQty){
            _stepQty = stepQty;
            _stepWord = LEDREG16;
            for(int i=0; i<_stepQty; i++){
                _stepWord |= (1 << i);
            }
            _stepWord &= ~(1 << _step);
            sendWord(_stepWord, CS_STEP_LIGHT);
        }
        void nextStep(){            
            _step += 1;
            _stepWord &= ~(1 << _step);
            if(_step != 0){
                _stepWord |= (1 << _step-1);
            }
            if(_step == _stepQty){
                _stepWord |= (1 << _stepQty-1);
                _stepWord &= ~(1 << 0);
                _step = 0;
            }
        }
        void showStep(){
            sendWord(_stepWord, CS_STEP_LIGHT);
        }
        void showStartup(uint8_t delayTime){
            for(int i=0; i<16; i++){
                _stepWord = LEDREG16 | (1 << i);
                sendWord(_stepWord, CS_STEP_LIGHT);
                delay(delayTime);
            }
            sendWord(LEDREG16, CS_STEP_LIGHT);      // Turn off all Step LEDS  
        }
};

class ToneLight {
    private:
        uint8_t _octave;
        uint8_t _toneNum;
        uint16_t _toneWord;
        uint8_t _octaveByte;
    public:
        ToneLight() {
            pinMode(CS_TONE_LIGHT, OUTPUT);
            pinMode(CS_OCTAVE_LIGHT, OUTPUT);
        }
        void showNote(uint16_t tone){
            if(tone >= C5 && tone < C6){
                _octave = 5;
            } else if(tone >= C6 && tone < C7){
                _octave = 6;
            }

            if(tone == C5 || tone == C6){
                _toneNum = 0;
            }

            // if(tone == C5 || tone == C6){
            //     _toneNum = C;
            // } else if(tone == E5 || tone == E6){
            //     _toneNum = E;
            // } else if(tone == G5 || tone == G6){
            //     _toneNum = G;
            // }

            if(tone == C5 || tone == C6){
                _toneNum = C;
            } else if(tone == Csharp5 || tone == Csharp6){
                _toneNum = Csharp;
            } else if(tone == D5 || tone == D6){
                _toneNum = D;
            } else if(tone == Dsharp5 || tone == Dsharp6){
                _toneNum = Dsharp;
            } else if(tone == E5 || tone == E6){
                _toneNum = E;
            } else if(tone == F5 || tone == F6){
                _toneNum = F;
            } else if(tone == Fsharp5 || tone == Fsharp6){
                _toneNum = Fsharp;
            } else if(tone == G5 || tone == G6){
                _toneNum = G;
            } else if(tone == Gsharp5 || tone == Gsharp6){
                _toneNum = Gsharp;
            } else if(tone == A5 || tone == A6){
                _toneNum = A;
            } else if(tone == Bb5|| tone == Bb6){
                _toneNum = Bb;
            } else if(tone == H5|| tone == H6){
                _toneNum = H;
            }

            _toneWord = LEDREG16 |  (1 << _toneNum);
            sendWord(_toneWord, CS_TONE_LIGHT);

            _octaveByte = LEDREG8 | (1 << _octave);
            sendByte(_octaveByte, CS_OCTAVE_LIGHT);
        }
};

StepLight stepLight;
ToneLight toneLight;

void setup() {
    Serial.begin(9600);
    Serial.println("Start Device");
    
    SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
    // light.showStartup(20);
    // light.changeStepQty(6);
    attachInterrupt(digitalPinToInterrupt(2), stop_and_start, RISING) ;
    // pinMode(CS_OCTAVE_LIGHT, OUTPUT);
    // pinMode(CS_TONE_LIGHT, OUTPUT);
    pinMode(CS_MODE_LIGHT, OUTPUT);
    pinMode(CS_SITE_LIGHT, OUTPUT);
    // pinMode(CS_STEP_LIGHT, OUTPUT);

    stepLight.showStartup(50);
    stepLight.changeStepQty(12);
}

void loop() {
    for(uint16_t tone: testMelody){
        toneLight.showNote(tone);
        stepLight.showStep();
        stepLight.nextStep();
        audio.playToneMS(tone, 200);
    }
    // // sendWord(0b1010101010101010, CS_STEP_LIGHT);
    // sendByte(0b10101010, CS_SITE_LIGHT);
    // delay(500);
    // //sendWord(0b0101010101010101, CS_STEP_LIGHT);
    // sendByte(0b01010101, CS_SITE_LIGHT);
    // delay(500);
    // // exe_play_mode(stepNum);
    // // if(state){
    // //     light.showStartup();
    // // }
}
