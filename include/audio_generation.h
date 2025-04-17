//
// Created by Samuel Longauer on 17/02/2024.
//

#ifndef AUDIO_TRANSCRIBER_AUDIO_GENERATION_H
#define AUDIO_TRANSCRIBER_AUDIO_GENERATION_H

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <string>
#include "wav_creation.h"


void generateAudio(const std::string& outputFilePath, std::vector<channel_type> chords){
    WaveGener wave_generator(std::move(chords));
    wave_generator.write_to_file(outputFilePath);
}

#endif //AUDIO_TRANSCRIBER_AUDIO_GENERATION_H


