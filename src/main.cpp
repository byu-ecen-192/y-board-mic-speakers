#include "Arduino.h"
#include "coefficients.h"
#include "display.h"
#include "tone_gen.h"
#include "yboard.h"

std::string SONG_FILENAME = "/sd_card/ode_to_joy.wav";
std::string TONE_FILENAME = "/sd_card/audio_with_tone.wav";
std::string RECORDING_FILENAME = "/sd_card/recording.wav";
int tone_volume = 5;
int wav_volume = 5;

static Pipeline pipeline;
static File sound_file;
static WAVDecoder wav_decoder;
static EncodedAudioStream dec(&wav_decoder);
static FilteredStream<int16_t, float> filter_stream(dec, 1);
static VolumeStream volume;
static StreamCopy copier(pipeline, sound_file);

int16_t tremoloDuration = 200;
float tremoloDepth = 0.5;
const int sample_rate = 44100;

void play_wav_file(std::string filename);

void play_notes(std::string song);
void play_song();
void filter_tone();
void record();

void setup() {
    Serial.begin(115200);
    AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);
    Yboard.setup();
    tone_gen_setup(tone_volume);
}

void loop() {
    // tone_gen_loop();
    // play_notes(put something here);
    // play_song();
    // filter_tone();
    // record();
}

// Functions for playing notes and wav files
bool sd_check(std::string filename) {
    if (!SD.exists(filename.c_str())) {
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

void play_song() {
    if (Yboard.get_button(1)) {
        if (!sd_check(SONG_FILENAME)) {
            return;
        }
        Yboard.set_sound_file_volume(wav_volume);
        Yboard.set_all_leds_color(0, 255, 0);
        display_text("Playing song", 1);

        // Set up filter (none)
        volume.setVolume(0.5);
        filter_stream.setFilter(0, nullptr);

        play_wav_file(SONG_FILENAME);
        clear_display();
        Yboard.set_all_leds_color(0, 0, 0);
    }
}

void filter_tone() {
    if (Yboard.get_button(1)) {
        if (!sd_check(TONE_FILENAME)) {
            return;
        }
        Yboard.set_sound_file_volume(wav_volume);
        Yboard.set_all_leds_color(0, 255, 0);
        display_text("Playing unfiltered   file", 1);

        // Set up filter (none)
        volume.setVolume(0.5);
        filter_stream.setFilter(0, nullptr);

        play_wav_file(TONE_FILENAME);
        clear_display();
        Yboard.set_all_leds_color(0, 0, 0);
    }
    if (Yboard.get_button(2)) {
        if (!sd_check(TONE_FILENAME)) {
            return;
        }
        Yboard.set_sound_file_volume(wav_volume * 2);
        Yboard.set_all_leds_color(0, 0, 255);
        display_text("Playing filtered file", 1);

        // Set up filter (band reject)
        volume.setVolume(1.0);
        filter_stream.setFilter(0, new FIR<float>(coef));

        play_wav_file(TONE_FILENAME);
        Yboard.set_all_leds_color(0, 0, 0);
        clear_display();
    }
}

void record() {
    if (Yboard.get_button(1)) {
        bool started_recording = Yboard.start_recording(RECORDING_FILENAME);
        while (Yboard.get_button(1)) {
            if (started_recording) {
                Yboard.set_all_leds_color(255, 0, 0);
                display_text("Recording", 1);
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

        clear_display();
        Yboard.set_all_leds_color(0, 0, 0);
    }

    if (Yboard.get_button(2)) {
        if (!sd_check(RECORDING_FILENAME)) {
            return;
        }
        Yboard.set_sound_file_volume(wav_volume);
        Yboard.set_all_leds_color(0, 255, 0);

        if (Yboard.get_switch(1)) {
            display_text("Playing low pass     filtered recording", 1);
            // Setup filter (low pass)
            volume.setVolume(1.0);
            filter_stream.setFilter(0, new FIR<float>(low_pass_coef));
        } else if (Yboard.get_switch(2)) {
            display_text("Playing band pass    filtered recording", 1);
            // Setup filter (band pass)
            volume.setVolume(1.0);
            filter_stream.setFilter(0, new FIR<float>(band_pass_coef));
        } else {
            display_text("Playing unfiltered   recording", 1);
            // Setup filter (none)
            volume.setVolume(0.5);
            filter_stream.setFilter(0, nullptr);
        }

        play_wav_file(RECORDING_FILENAME);
        clear_display();
        Yboard.set_all_leds_color(0, 0, 0);
    }
}

void play_wav_file(std::string filename) {
    sound_file = SD.open(filename.c_str());

    // setup pipeline
    pipeline.add(dec);
    pipeline.add(filter_stream);
    pipeline.add(volume);
    pipeline.setOutput(Yboard.get_speaker_stream());
    // start all components with their default values
    pipeline.begin();

    while (sound_file.available()) {
        copier.copy();
    }

    pipeline.end();
}
