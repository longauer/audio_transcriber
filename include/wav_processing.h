//
// Created by Samuel Longauer on 26/02/2024.
//

#ifndef WAV_PROCESSING_H
#define WAV_PROCESSING_H

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <string>

class WaveFile{
public:
    // "RIFF" chunk
    std::string chunk_id;
    int chunk_size;
    std::string format;

    // fmt subchunk
    std::string subchunk1_id;
    int subchunk1_size;
    short audio_format;
    short num_channels;
    int sample_rate;
    int byte_rate;
    short block_align;
    short bits_per_sample;

    // data subchunk
    std::string subchunk2_id;
    int subchunk_size;

    // data
    std::vector<std::vector<int>> channels;

    explicit WaveFile(const std::string& filename);

private:
    std::string m_filename;
    void process_wav(const std::string&);

};

WaveFile::WaveFile(const std::string& filename){
    chunk_id.resize(4);
    format.resize(4);
    subchunk1_id.resize(4);
    subchunk2_id.resize(4);
    process_wav(filename);
}


void WaveFile::process_wav(const std::string& filename){
    std::fstream file;
    file.open(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()){
        std::cerr << "The file cannot be opened\n";
        std::exit(1);
    }

    //file.read(reinterpret_cast<char*>(&chunk_id), 4);
    file.read(reinterpret_cast<char*>(&chunk_id[0]),4);
    file.read(reinterpret_cast<char*>(&chunk_size), 4);
    file.read(reinterpret_cast<char*>(&format[0]), 4);

    file.read(reinterpret_cast<char*>(&subchunk1_id[0]), 4);
    file.read(reinterpret_cast<char*>(&subchunk1_size), 4);
    file.read(reinterpret_cast<char*>(&audio_format),2);
    file.read(reinterpret_cast<char*>(&num_channels), 2);
    file.read(reinterpret_cast<char*>(&sample_rate), 4);
    file.read(reinterpret_cast<char*>(&byte_rate), 4);
    file.read(reinterpret_cast<char*>(&block_align), 2);
    file.read(reinterpret_cast<char*>(&bits_per_sample), 2);

    file.read(reinterpret_cast<char*>(&subchunk2_id[0]), 4);
    file.read(reinterpret_cast<char*>(&subchunk_size), 4);

    // resize the channels vector for the number of channels
    channels.resize(num_channels, std::vector<int>());

    //reading the data into respective channels
    if (subchunk_size != chunk_size - 20 - subchunk1_size){
        subchunk_size = chunk_size - 20 - subchunk1_size;
    }

    for (int byte = 0; byte<subchunk_size/num_channels/(bits_per_sample/8); ++byte){
        for (int i = 0; i<num_channels; ++i){
            char tmp;
            int value = 0;
            for (int j = 0; j<bits_per_sample/8; ++j){
                file.read(reinterpret_cast<char*>(&tmp), 1);
                value |= (static_cast<uint8_t>(tmp) << (8*j)); // assuming little endian representation
            }
            channels[i].push_back(value);
        }
    }


    file.close();
}


#endif //WAV_PROCESSING_H
