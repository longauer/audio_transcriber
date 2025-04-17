//
// Created by Samuel Longauer on 24/04/2024.
//

#ifndef PROJECT_TRANSCRIPT_GENERATION_H
#define PROJECT_TRANSCRIPT_GENERATION_H


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "note_classifier.h"

typedef std::pair<std::vector<NoteClassifier>, double> segment_chord;
typedef std::vector<segment_chord> channel_type;


std::string fixed_size_str(const std::string& st, int size){
    std::string result;
    for (int i = 0; i<size; i++){
        if (i<st.length()){
            result+=st[i];
        }else{
            result+=" ";
        }
    }
    return result;
}


// different rows represent different time segments
// columns represent sets of notes for each of the channels for the given time segment(row)
// the chords in each statement are ordered in an increasing order by their frequencies

void generateTranscript(const std::string& transcriptFilePath, std::vector<channel_type> channels){    // creating a separate transcript for each channel

    // transcript table proportions
    int column1_len = 15;
    int channel_col_len = 70;

    std::fstream output(transcriptFilePath, std::ios::out);
    output << "duration(s):   ";
    for (int i = 0; i<channels.size(); i++){
        std::ostringstream oss;
        oss << "channel " << i+1;
        std::string st = fixed_size_str(oss.str(), channel_col_len+column1_len);
        output << st;
    }
    output << std::endl;

    int rows = (int)channels[0].size();
    int columns = (int)channels.size();

    for (int i = 0; i<rows; i++){
        for (int j = 0; j<columns; j++){

            std::ostringstream oss;
            std::vector<NoteClassifier> chord = channels[j][i].first;
            double duration = channels[j][i].second;

            if (j == 0) oss << duration; else oss << " ";
            output << fixed_size_str(oss.str(), column1_len);
            oss.str(""); // resetting the oss object


            for (auto&& x : chord){
                oss << x.repr << " ";
            }
            output << fixed_size_str(oss.str(), channel_col_len);
            oss.str("");
            if (j<columns-1) output << "|";
        }
        output << std::endl;
    }
}


#endif //PROJECT_TRANSCRIPT_GENERATION_H
