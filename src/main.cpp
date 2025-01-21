#include "Arduino.h"
#include "display.h"
#include "filters.h"
#include "tone_gen.h"
#include "yboard.h"

std::string FILENAME = "/sd_card/ode_to_joy.wav";
std::string FILTERED_FILENAME = "/sd_card/filtered.wav";
int tone_volume = 5;
int wav_volume = 5;

void play_notes(std::string song);
void play_wav();
void filters(int frequency);

void setup() {
    Serial.begin(9600);
    Yboard.setup();
    tone_gen_setup(tone_volume);
}

void loop() {
    tone_gen_loop();
    // play_notes(put something here);
    // play_wav();
    // filters(put something here);
}

// Functions for playing notes and wav files
bool sd_check() {
    if (!SD.exists(FILENAME.c_str())) {
        display_text("Insert SD card and reset", 1);
        for (int i = 0; i < 5; i++) {
            Yboard.set_all_leds_color(255, 0, 0);
            delay(250);
            Yboard.set_all_leds_color(0, 0, 0);
            delay(250);
        }
        clear_display();
        return false;
    }
    return true;
}
void play_notes(std::string song) {
    if (Yboard.get_button(1)) {
        Yboard.set_all_leds_color(0, 255, 0);
        std::string display_string = "Playing notes";
        display_text(display_string, 1);
        Yboard.play_notes(song);
        clear_display();
        Yboard.set_all_leds_color(0, 0, 0);
    }
}
void play_wav() {
    if (Yboard.get_button(1)) {
        if (!sd_check()) {
            return;
        }
        Yboard.set_sound_file_volume(wav_volume);
        Yboard.set_all_leds_color(0, 255, 0);
        display_text("Playing wave file", 1);
        Yboard.play_sound_file(FILENAME);
        clear_display();
        Yboard.set_all_leds_color(0, 0, 0);
    }
}
void filters(int frequency) {
    if (Yboard.get_button(1)) {
        if (!sd_check()) {
            return;
        }
        Yboard.set_sound_file_volume(wav_volume);
        Yboard.set_all_leds_color(0, 255, 0);
        display_text("Playing unfiltered   file", 1);
        Yboard.play_sound_file(FILENAME);
        clear_display();
        Yboard.set_all_leds_color(0, 0, 0);
    }
    if (Yboard.get_button(2)) {
        if (!sd_check()) {
            return;
        }
        Yboard.set_sound_file_volume(wav_volume * 2);
        Yboard.set_all_leds_color(0, 0, 255);
        display_text("Filtering", 1);
        bandRejectFilter(FILENAME, FILTERED_FILENAME, frequency);
        Yboard.set_all_leds_color(0, 255, 0);
        display_text("Playing filtered file", 1);
        Yboard.play_sound_file(FILTERED_FILENAME);
        Yboard.set_all_leds_color(0, 0, 0);
        clear_display();
    }
}
