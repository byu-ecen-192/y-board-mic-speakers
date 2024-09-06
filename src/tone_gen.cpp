#include "tone_gen.h"
#include "Arduino.h"
#include "yboard.h"
#include "display.h"
#include <string>
#include <sstream>
#include <iomanip>
char current_note = 'A';
int octave = 4;
int volume = 5;

std::string fight_song = "O5 O5 F D. C#8 D. C#8 D8 B8- O4 G8 F F8 G8 G8- F O5 B- C B- B2- A2 E.- D8 E.- D8 E8- C8 A8 O4 F F8 G8 G8- F O5 A  B- C C2# D2 D. C8# D. C8# D8 B8- O4 G8 F F8 G8 G8- F O5 B- C B- B2- O4 G2 O5 B-. A8 C. B8- D2 O4 F8 G8 O5 B8- E2- O4 F8 G8 O5 B8- D2. B- B.- A8 C. B8- D8 B8- O4 G8 F F8 E8 F8 G8 O5 B- O4 G8 O5 D D B2.-";
std::string mario = "O5 E8 E8 R8 E8 R8 C8 E G R O4 G";
std::string star_spangled_banner = "O4 G8. E16 C E G O5 C2 E8. D16 C O4 E F# G2";
std::string star_wars = "O4 D12 D12 D12 G2 O5 D2 C12 B12 A12 G2 D C12 B12 A12 G2 D C12 B12 C12 A2";


double find_frequency() {
    double n = -1;
    switch (current_note) {
        case 'A':
            n = 1;
            break;
        case 'B':
            n = 3;
            break;
        case 'C':
            n = 4;
            break;
        case 'D':
            n = 6;
            break;
        case 'E':
            n = 8;
            break;
        case 'F':
            n = 9;
            break;
        case 'G':
            n = 11;
            break;
    }
    n = 12 * (octave) + n;
    return 440 * pow(2, (n - 49) / 12);
}
void write_to_display() {
    std::stringstream ss;
    ss << std::left << std::setw(21) << ("Note: " + std::string(1, current_note) + std::to_string(octave));
    std::stringstream freq_stream;
    freq_stream << "Frequency: " << std::fixed << std::setprecision(2) << find_frequency() << "Hz";
    ss << std::left << std::setw(21) << freq_stream.str();
    ss << std::left << std::setw(21) << ("Volume: " + std::to_string(volume));
    clear_display();
    display_text(ss.str(), 1);
}
void octave_change(int amount) {
    octave = 4 + amount;
    std::string input_string = 'O' + std::to_string(octave);
    Yboard.play_notes(input_string);
    
}
std::string secret = "T150 O4 C8 D8 F8 D8 O5 A8. A8. O4 G2 R8 C8 D8 F8 D8 G8. G8. F2";
void note_control() {
    static int last_pot_val = 0;
    static bool noisy = false;
    int pot_val = map(Yboard.get_knob(),0,100,0,14);
    int average = 0;


    int num_samples = 5;
    for (int i = 0; i < num_samples; i++) {
        int current_reading = map(Yboard.get_knob(),0,100,0,14);
        average += current_reading;
    }
    if (average % num_samples != 0) {
        pot_val = average / num_samples;
        noisy = true;
    }
    else {
        noisy = false;
    }

    
    if (pot_val != last_pot_val && !noisy) {
        
        current_note = 'A' + pot_val;
        if (current_note > 'A' + 13) {
            octave_change(2);
            current_note -= 14;
        }
        else if (current_note > 'A' + 6) {
            octave_change(1);
            current_note -= 7;
        }
        else {
            octave_change(0);
        }
        write_to_display();
        last_pot_val = pot_val;

    }
}

void tone_gen_setup(int new_volume)
{
    volume = new_volume;
    if (volume > 10) {
        volume = 10;
    }
    else if (volume < 1) {
        volume = 1;
    }
    setup_display(0x3C);
    octave_change(0);
    std::string input_string = 'V' + std::to_string(volume);
    Yboard.play_notes(input_string);
}

void tone_gen_loop()
{   
    static bool first_loop = false;
    if (!first_loop) {
        write_to_display();
        first_loop = true;
    }
    note_control();   
    if (Yboard.get_button(1)) {
        std::string input_string;
        input_string += current_note;

        Yboard.set_all_leds_color(0, 255, 0);
        while (Yboard.get_button(1)) {
            delay(10);
            Yboard.play_notes(input_string);
        }
        Yboard.set_all_leds_color(0, 0, 0);
    }
}
