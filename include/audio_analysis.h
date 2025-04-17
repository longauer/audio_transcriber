//
// Created by Samuel Longauer on 17/02/2024.
//

#ifndef AUDIO_TRANSCRIBER_AUDIO_ANALYSIS_H
#define AUDIO_TRANSCRIBER_AUDIO_ANALYSIS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <complex>
#include <algorithm>
#include "wav_processing.h"
#include "dft.h"
#include "note_classifier.h"


using namespace std;
typedef std::pair<std::vector<NoteClassifier>, double> segment_chord;
typedef std::vector<std::vector<segment_chord>> channel_field;

class AudioAnalyzer {
private:
    double frequency;
    int num_dominant;

    static std::vector<int> strip_leading_zeros(std::vector<int>& vect);
    static std::vector<std::complex<double>> time_domain_preprocessing(std::vector<double>& pcm, int num_samples, int sample_rate);
    segment_chord analyzeSegment(std::vector<int>& segment, int sample_rate) const;
    std::vector<segment_chord> analyzeChannel(std::vector<int>& channel, int sample_rate);

public:
    explicit AudioAnalyzer(double frequency=0, int num_dominant=1) : frequency(frequency), num_dominant(num_dominant){};
    channel_field analyzeAudio(const std::string& filePath);
};

std::vector<int> AudioAnalyzer::strip_leading_zeros(std::vector<int>& vect){
    std::vector<int> result;
    for (auto&& x : vect){
        if (x == 0) continue;
        result.push_back(x);
    }

    return result;
}

std::vector<std::complex<double>> AudioAnalyzer::time_domain_preprocessing(std::vector<double>& pcm, int num_samples, int sample_rate){
    // potentially padding time_domain_data with zeroes to ensure at lest 1 hz resolution after applying dft
    std::vector<std::complex<double>> time_domain_data(max(num_samples, 4*sample_rate));
    int i = 0;
    for (; i<num_samples; i++){
        time_domain_data[i] = complex<double>(static_cast<double>(pcm[i]), 0);
    }
    for(; i<4*sample_rate; i++){
        time_domain_data[i] = complex<double>(pcm[i%num_samples], 0);
    }

    return time_domain_data;
}

segment_chord AudioAnalyzer::analyzeSegment(std::vector<int>& segment, int sample_rate) const{
    int num_samples = static_cast<int>(segment.size());
    double duration = static_cast<double>(num_samples)/sample_rate; // duration of the recording in seconds

    std::vector<double> dsegment;
    dsegment.reserve(num_samples);
    for (int i : segment){
        dsegment.emplace_back(static_cast<double>(i));
    }
    applyHannWindow(dsegment);

    // construct a complex vector - input for the fft
    std::vector<std::complex<double>> time_domain_data = time_domain_preprocessing(dsegment, num_samples, sample_rate);
    num_samples = static_cast<int>(time_domain_data.size());

    // convert time domain data input to the frequency domain using fft + scaling
    vector<complex<double>> frequency_domain_data;
    frequency_domain_data = FFT(time_domain_data);

    // compute the power spectral density of the transformed data
    vector<double> power_spectral_density = powerSpectralDensity(frequency_domain_data, num_samples, sample_rate);

    double maxPSD = 0.0;
    for (auto&& x : power_spectral_density){
        maxPSD = max(maxPSD, x);
    }

    // localizing the peaks in the graph of power spectral density
    vector<pair<double, double>> power_peaks;
    for (int i = 1; i<power_spectral_density.size()-1; i++){
        if (power_spectral_density[i-1]<power_spectral_density[i] && power_spectral_density[i] > power_spectral_density[i+1]){
            power_peaks.emplace_back(power_spectral_density[i]/maxPSD*1000, static_cast<double>(i)*sample_rate/num_samples);
        }
    }

    // sorting the peaks in decreasing order (which corresponds to the significance of the given frequencies in the original recording)
    sort(power_peaks.begin(), power_peaks.end(), [](const std::pair<double,double>& a, const std::pair<double,double>& b){return a.first > b.first;});

    // converting the most dominant concurrent frequencies (within the analyzed segment) to their musical representation
    vector<NoteClassifier> result;
    set<string> notes;
    int cnt = 0;
    for (int i = 0; i<power_peaks.size(); i++){
        NoteClassifier note(power_peaks[i].second);
        if (cnt>num_dominant-1) break;
        if (note.repr.empty()) continue;
        if (notes.find(note.repr) == notes.end()){
            notes.insert(note.repr);
            result.push_back(note);
            cnt++;
        }

    }
    sort(result.begin(), result.end());
    return make_pair(result, duration);
}

std::vector<segment_chord> AudioAnalyzer::analyzeChannel(std::vector<int>& channel, int sample_rate){
    vector<segment_chord> result;
    channel = strip_leading_zeros(channel); // leaving out the silent part at the beginning of the recording from the analysis
    int num_samples = static_cast<int>(channel.size());
    vector<int> segment;
    double window_size = frequency*sample_rate;
    auto num_samples_left = (double)num_samples;
    int curr_sample = 0;

    while(window_size<=num_samples_left){
        for (int i = 0; i<window_size; i++){
            segment.push_back(channel[curr_sample++]);
        }
        num_samples_left -= window_size;
        result.push_back(analyzeSegment(segment, sample_rate));
        segment.clear();
    }
    for (; curr_sample<num_samples; curr_sample++){
        segment.push_back(channel[curr_sample]);
    }
    if (!segment.empty()) result.push_back(analyzeSegment(segment, sample_rate));

    return result;
}

channel_field AudioAnalyzer::analyzeAudio(const std::string& filePath){  // frequency determines the bin width of the separately analyzed partitions of the original recording

    WaveFile file_object(filePath);

    int num_channels = file_object.num_channels;
    double sample_rate = file_object.sample_rate;
    int num_samples = static_cast<int>(file_object.channels[0].size());  // all channels contain the same number of samples
    double duration = static_cast<double>(num_samples)/sample_rate;

    if (frequency == 0) frequency = duration;

    channel_field channel_outputs;
    for (int i=0; i<num_channels; i++){
        channel_outputs.push_back(analyzeChannel(file_object.channels[i], (int)sample_rate));
    }
    return channel_outputs;

}


#endif //AUDIO_TRANSCRIBER_AUDIO_ANALYSIS_H
