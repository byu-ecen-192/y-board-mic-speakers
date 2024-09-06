#ifndef TONE_GEN_H
#define TONE_GEN_H
#include <string>

extern std::string secret;
extern std::string fight_song;
extern std::string mario;
extern std::string star_spangled_banner;
extern std::string star_wars;

void tone_gen_setup(int volume);
void tone_gen_loop();

#endif /* TONE_GEN_H */