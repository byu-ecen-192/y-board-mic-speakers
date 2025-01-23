#include "Arduino.h"
#include "yboard.h"
#include "tone_gen.h"
#include "filters.h"
#include "display.h"

std::string FILENAME = "/sd_card/ode_to_joy.wav";
std::string FILTERED_FILENAME = "/sd_card/filtered.wav";

static const std::string FILE_NAME_RECORDER = "/audio_recorder_demo.wav";
static const float smoothness_pts = 500;    // larger=slower change in brightness
static const float smoothness_gamma = 0.14; // affects the width of peak (more or less darkness)
static const float smoothness_beta = 0.5;   // shifts the gaussian to be symmetric

unsigned long previousMillis = 0; // Stores last time the action was performed
unsigned int smoothness_index = 0;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define NUM_BARS 16
#define BAR_WIDTH 6
#define MAX_BAR_HEIGHT (SCREEN_HEIGHT - 5)

void draw_animation();

void play_notes(std::string song);
void play_wav();
void filters_file(int frequency);
void microphone();

int tone_volume = 5;
int wav_volume = 5;

void setup() {
    Serial.begin(9600);
    Yboard.setup();
    
    tone_gen_setup(tone_volume);
}

void loop() {
    tone_gen_loop();
    // play_notes(star_wars);  
    // play_wav();
    // microphone();
    // filters_file(440); 
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
        if (!sd_check()) return;
        Yboard.set_sound_file_volume(wav_volume);
        Yboard.set_all_leds_color(0, 255, 0);
        display_text("Playing wave file", 1);
        Yboard.play_sound_file(FILE_NAME_RECORDER);
        clear_display();
        Yboard.set_all_leds_color(0, 0, 0);
    }
}
void microphone() {
    unsigned long currentMillis = millis();

    if (Yboard.get_button(1)) {
        bool started_recording = Yboard.start_recording(FILE_NAME_RECORDER);
        while (Yboard.get_button(1)) {
            if (started_recording) {
                Yboard.set_all_leds_color(255, 0, 0);
                draw_animation();
                delay(100);
            } else {
                Yboard.set_all_leds_color(255, 0, 0);
                delay(100);
                Yboard.set_all_leds_color(0, 0, 0);
                delay(100);
            }
        }

        if (started_recording) {
            delay(200); // Don't stop the recording immediately
            Yboard.stop_recording();
        }

        Yboard.display.clearDisplay();
        Yboard.display.display();
        Yboard.set_all_leds_color(0, 0, 0);
    }

    if (Yboard.get_button(2)) {
        Yboard.set_all_leds_color(0, 0, 255);
        if (!Yboard.play_sound_file(FILE_NAME_RECORDER)) {
            for (int i = 0; i < 3; i++) {
                Yboard.set_all_leds_color(255, 0, 0);
                delay(100);
                Yboard.set_all_leds_color(0, 0, 0);
                delay(100);
            }
        }
    }

    if (currentMillis - previousMillis >= 5) {
        // Save the last time you performed the action
        previousMillis = currentMillis;

        float pwm_val =
            255.0 *
            (exp(-(pow(((smoothness_index / smoothness_pts) - smoothness_beta) / smoothness_gamma,
                       2.0)) /
                 2.0));
        Yboard.set_all_leds_color(int(pwm_val), int(pwm_val), int(pwm_val));

        // Increment the index and reset if it exceeds the number of points
        smoothness_index++;
        if (smoothness_index >= smoothness_pts) {
            smoothness_index = 0;
        }
    }
}
void draw_animation() {
    Yboard.display.clearDisplay();

    Yboard.display.setTextSize(1);
    Yboard.display.fillCircle(3, 3, 2, WHITE);
    Yboard.display.drawChar(8, 0, 'R', WHITE, BLACK, 1);
    Yboard.display.drawChar(16, 0, 'E', WHITE, BLACK, 1);
    Yboard.display.drawChar(24, 0, 'C', WHITE, BLACK, 1);

    // Draw equalizer bars with random heights to simulate sound levels
    for (int i = 0; i < NUM_BARS; i++) {
        int barHeight = random(5, MAX_BAR_HEIGHT); // Random height between 5 and max height
        int x = i * (BAR_WIDTH + 2);               // Calculate x position of the bar

        // Draw the bar (bottom-up)
        Yboard.display.fillRect(x, SCREEN_HEIGHT - barHeight, BAR_WIDTH, barHeight, WHITE);
    }

    Yboard.display.display();
}
void filters_file(int frequency) {
    if (Yboard.get_button(1)) {
        if (!sd_check()) return;
        Yboard.set_sound_file_volume(wav_volume);
        Yboard.set_all_leds_color(0, 255, 0);
        display_text("Playing unfiltered   file", 1);
        Yboard.play_sound_file(FILENAME);
        clear_display();
        Yboard.set_all_leds_color(0, 0, 0);
    }
    if (Yboard.get_button(2)) {
        if (!sd_check()) return;
        Yboard.set_sound_file_volume(wav_volume*2);
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