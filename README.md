## compile and upload

    arduino-cli compile --fqbn arduino:avr:uno ArduinoToneSampler.ino
    arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:uno ArduinoToneSampler.ino && arduino-cli monitor -p /dev/ttyACM0

## Links

    http://www.mv-spoeck-wissenswert.de/html/Toene_und_Notennamen.htm

    https://www.mathematik.uni-marburg.de/~thormae/lectures/mmk/mmk_5_1_ger_web.html#1

    https://mido.readthedocs.io/en/stable/

    https://github.com/users/Domroon/projects/11

    https://www.arduino.cc/reference/en/language/functions/communication/spi/

    https://github.com/khoih-prog/TimerInterrupt