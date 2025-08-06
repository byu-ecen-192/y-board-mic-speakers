#include "tone_gen.h"
#include "Arduino.h"
#include "yboard.h"
#include "display.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

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
    ss << "Note: " << current_note << octave << "\n";
    ss << "Frequency: " << std::fixed << std::setprecision(2) << find_frequency() << "Hz\n";
    ss << "Volume: " << volume << "\n";
    std::string display_str = ss.str();
    const char* display_cstr = display_str.c_str();
    Yboard.display.clearDisplay();
    Yboard.display.setCursor(0, 0);
    Yboard.display.write(display_cstr);
    Yboard.display.display();
}
void octave_change(int amount) {
    octave = 4 + amount;
    std::string input_string = 'O' + std::to_string(octave);
    Yboard.play_notes(input_string);
    
}
std::string secret = "T150 O4 C8 D8 F8 D8 O5 A8. A8. O4 G2 R8 C8 D8 F8 D8 G8. G8. F2";
void note_control() {
    static int last_pot_val = 0;
    int pot_val = Yboard.get_knob() % 28;
    if( pot_val < 0) pot_val = 0;
    
    if (pot_val != last_pot_val) {
        
        current_note = 'A' + pot_val;
        if (current_note > 'A' + 6) {
            octave_change((int)(current_note - 'A') / 7);
            current_note -= 7 * ((int)(current_note - 'A') / 7);
        } else {
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
    octave_change(0);
    std::string input_string = 'V' + std::to_string(volume);
    Yboard.play_notes(input_string);
}

void tone_gen_loop()
{   
    static bool first_loop = true;
    if (first_loop) {
        write_to_display();
        first_loop = false;
    }
    note_control();   
    if (Yboard.get_button(1)) {
        std::string input_string;
        input_string += current_note;

        Yboard.set_all_leds_color(0, 255, 0);
        while (Yboard.get_button(1)) {
            Serial.println(("Playing note: " + input_string).c_str());
            delay(10);
            Yboard.play_notes(input_string);
        }
        Yboard.set_all_leds_color(0, 0, 0);
    }
}
