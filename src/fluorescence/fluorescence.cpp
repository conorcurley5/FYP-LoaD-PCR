#include <Adafruit_AS7341.h>
#include <Arduino_JSON.h>
#include <websocket.h>

Adafruit_AS7341 as7341;

volatile int f_atime = 100;
volatile int f_astep = 999;
volatile as7341_gain_t f_gain = AS7341_GAIN_256X;

void setup_fluorescence () {
    if (!as7341.begin()){
        Serial.println("Could not find AS7341");
        while (1) { delay(10); }
    }

    as7341.setATIME(f_atime);
    as7341.setASTEP(f_astep);
    as7341.setGain(f_gain);
}

void fetch_fluorescence () {
    if (!as7341.readAllChannels()){
        Serial.println("Error reading all channels!");
        return;
    }

    JSONVar fluorescence_obj;

    fluorescence_obj["type"] = "FLUORESCENCE_READINGS";

    fluorescence_obj["F1_415"] = as7341.getChannel(AS7341_CHANNEL_415nm_F1);
    fluorescence_obj["F2_445"] = as7341.getChannel(AS7341_CHANNEL_445nm_F2);
    fluorescence_obj["F3_480"] = as7341.getChannel(AS7341_CHANNEL_480nm_F3);
    fluorescence_obj["F4_515"] = as7341.getChannel(AS7341_CHANNEL_515nm_F4);
    fluorescence_obj["F5_555"] = as7341.getChannel(AS7341_CHANNEL_555nm_F5);
    fluorescence_obj["F6_590"] = as7341.getChannel(AS7341_CHANNEL_590nm_F6);
    fluorescence_obj["F7_630"] = as7341.getChannel(AS7341_CHANNEL_630nm_F7);
    fluorescence_obj["F8_680"] = as7341.getChannel(AS7341_CHANNEL_680nm_F8);
    fluorescence_obj["CLEAR"] = as7341.getChannel(AS7341_CHANNEL_CLEAR);
    fluorescence_obj["NIR"] = as7341.getChannel(AS7341_CHANNEL_NIR);

    String f = JSON.stringify(fluorescence_obj);

    notify_clients(f);
}

void set_gain (int gain) {
    switch (gain) {
        case 1:
            as7341.setGain(AS7341_GAIN_1X);
            break;
        case 2:
            as7341.setGain(AS7341_GAIN_2X);
            break;
        case 4:
            as7341.setGain(AS7341_GAIN_4X);
            break;
        case 8:
            as7341.setGain(AS7341_GAIN_8X);
            break;
        case 16:
            as7341.setGain(AS7341_GAIN_16X);
            break;
        case 32:
            as7341.setGain(AS7341_GAIN_32X);
            break;
        case 64:
            as7341.setGain(AS7341_GAIN_64X);
            break;
        case 128:
            as7341.setGain(AS7341_GAIN_128X);
            break;
        case 256:
            as7341.setGain(AS7341_GAIN_256X);
            break;
        case 512:
            as7341.setGain(AS7341_GAIN_512X);
            break;
        default:
            Serial.println("Unsupported Value Detected");
    }

    return;
}

void set_atime (int atime) {
    as7341.setATIME(atime);
    return;
}

void set_astep (int astep) {
    as7341.setASTEP(astep);
    return;
}