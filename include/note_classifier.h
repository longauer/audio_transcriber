//
// Created by Samuel Longauer on 21/04/2024.
//

#ifndef PROJECT_NOTE_CLASSIFIER_H
#define PROJECT_NOTE_CLASSIFIER_H

#include <cmath>
#include <string>
#include <vector>

double HALF_STEP = std::pow(2, 1.0/12);
double QUARTER_STEP = std::pow(2, 1.0/24);

class NoteClassifier{
public:
    explicit NoteClassifier(double);
    double freq;
    std::string repr;
    bool out_of_range = true;
    bool operator<(const NoteClassifier& other) const {
        return this->freq < other.freq;
    }
private:
    static const int octaves_ = 9;
    double C_Hz[octaves_] = {16.352, 32.703, 65.406, 130.813, 261.626, 523.251, 1046.502, 2093.005, 4186.009};
    std::vector<std::string> notation_ = {"C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B"};
    std::vector<double> noteFrequencies_;
    void calculate_frequencies_();
    void classify_freq_(); // assigning a label describing the closest note and octave
};


NoteClassifier::NoteClassifier(double freq){
    this->freq = freq;
    calculate_frequencies_();
    classify_freq_();
}

void NoteClassifier::calculate_frequencies_(){
    for (int i = 0; i<octaves_; i++){
        double base_note = C_Hz[i];
        for (int j = 0; j<12; j++){
            this->noteFrequencies_.push_back(base_note);
            base_note*=HALF_STEP;
        }
    }
}

void NoteClassifier::classify_freq_(){

    int note_index = -1; // sentinel value

    // testing bounds
    int lower_bound = 0;
    int upper_bound = octaves_*12 - 1;

    if (freq <=noteFrequencies_[lower_bound]){
        double tolerance = noteFrequencies_[lower_bound]/QUARTER_STEP;
        if (freq > tolerance){
            note_index = lower_bound;
        }
    }else if (freq >= noteFrequencies_[upper_bound]){
        double tolerance = noteFrequencies_[upper_bound]*QUARTER_STEP;
        if (freq < tolerance){
            note_index = upper_bound;
        }
    }else{
        for (int note = 0; note<noteFrequencies_.size(); note++){
            if (freq >= noteFrequencies_[note] && freq <=noteFrequencies_[note+1]){
                double midway = noteFrequencies_[note]*QUARTER_STEP;
                note_index = (freq<midway ? note : note+1);
                break;
            }
        }
    }

    if (note_index >=0){
        int octave_ord = note_index/12;
        int note = note_index%12;
        this->out_of_range = false;
        this->repr = notation_[note] + "(" + std::to_string(octave_ord) + ")";

    }
}

#endif //PROJECT_NOTE_CLASSIFIER_H
