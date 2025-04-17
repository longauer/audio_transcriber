//
// Created by Samuel Longauer on 21/04/2024.
//

#ifndef PROJECT_WAV_CREATION_H
#define PROJECT_WAV_CREATION_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <utility>
#include "note_classifier.h"

using namespace std;
typedef std::pair<std::vector<NoteClassifier>, double> segment_chord;
typedef std::vector<segment_chord> channel_type;

class WaveGener {
private:
    // RIFF chunk
    string chunk_id = "RIFF";
    string chunk_size = "____";
    string format = "WAVE";

    // fmt sub-chunk
    string subchunk1_id = "fmt ";
    int subchunk1_size = 16;
    int audio_format = 1;
    int sample_rate = 44100;
    int bits_per_sample = 16;
    int num_channels;
    int byte_rate;
    int block_align;

    // Data sub-chunk
    const string subchunk2_id = "data";
    const string subchunk2_size = "____";

    static inline void write_as_bytes(ofstream& file, int value, int byte_size){
        file.write(reinterpret_cast<const char*>(&value), byte_size);
    }

    std::vector<std::vector<double>> recreate_pcm();

public:
    std::vector<channel_type> channels;
    explicit WaveGener(std::vector<channel_type> channels) : channels(std::move(channels)){
        this->num_channels = (int)(this->channels.size());
        this->byte_rate = this->sample_rate*this->num_channels*(this->subchunk1_size/8);
        this->block_align = this->num_channels*(this->subchunk1_size/8);
    }
    void write_to_file(const string& filePath);
};

std::vector<std::vector<double>> WaveGener::recreate_pcm() {
    std::vector<std::vector<double>> channels_values;

    const double max_amplitude = pow(2, bits_per_sample-1) - 5;

    for (auto&& chords : channels){
        std::vector<double> values;
        for (auto&& chord : chords){
            std::vector<NoteClassifier>& frequencies = chord.first;
            int num_freq = (int)frequencies.size();
            double duration = chord.second;

            int fade_out_length = min((int)(sample_rate*duration/20), 175);
            int fade_out_start = (int)((sample_rate)*duration - fade_out_length);
            int j = 1;

            for (int i = 0; i<sample_rate*duration; i++){
                double value = 0;
                for (const auto& note : frequencies){
                    double freq = note.freq;
                    value+=sin(2*M_PI*i*freq/sample_rate);
                }
                if (i >= fade_out_start){
                    values.push_back((value*max_amplitude/num_freq)*(1-static_cast<double>(j++)/fade_out_length));
                }else if (i <= fade_out_length){
                    values.push_back((value*max_amplitude/num_freq)*(static_cast<double>(i)/fade_out_length));
                }else{
                    values.push_back((value*max_amplitude/num_freq));
                }
            }
        }
        channels_values.push_back(values);
    }

    return channels_values;
}

void WaveGener::write_to_file(const string& filePath){
    ofstream wav;
    wav.open(filePath, ios::binary);
    if (wav.is_open()){
        wav << chunk_id;
        wav << chunk_size;
        wav << format;

        wav << subchunk1_id;
        write_as_bytes(wav, subchunk1_size, 4);
        write_as_bytes(wav, audio_format,2 );
        write_as_bytes(wav, num_channels, 2);
        write_as_bytes(wav, sample_rate, 4);
        write_as_bytes(wav, byte_rate, 4);
        write_as_bytes(wav, block_align, 2);
        write_as_bytes(wav, bits_per_sample, 2);

        wav << subchunk2_id;
        wav << subchunk2_size;


        // reconstructing the pcm wave form based on the generated representation
        // introducing fade-in and fade-out around the segment connections to increase fluency in the transitions
        int start_audio = (int)wav.tellp();

        std::vector<std::vector<double>> channels_values = recreate_pcm();

        // writing the data for each channel in an interleaved fashion (respecting the .wav file format)
        for (int i = 0; i<channels_values[0].size(); i++){
            for (int j = 0; j<channels_values.size(); j++){
                write_as_bytes(wav, channels_values[j][i], bits_per_sample/8);
            }
        }


        int end_audio = (int)wav.tellp();
        int diff = end_audio - start_audio;
        wav.seekp(start_audio-4);
        write_as_bytes(wav, end_audio - start_audio, 4);
        wav.seekp(4, ios::beg);
        write_as_bytes(wav, 36 + diff, 4);

    }

    wav.close();

}

#endif //PROJECT_WAV_CREATION_H
