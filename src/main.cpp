#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include "audio_analysis.h"
#include "audio_generation.h"
#include "transcript_generation.h"


struct Args{
    std::string input_audio_file_path;
    std::string output_audio_file_path;
    std::string transcript_file_path;
    int num_frequencies;
    double segment_size;

    Args(){
        num_frequencies = 1; // only the most dominant frequency will be extracted from each sample
        segment_size = 0; // the recording is going to be analyzed as a whole
    }
};


Args parse_args(int argc, char *argv[]){

    std::string freq_flag = "--num_frequencies";
    std::string segment_size_flag = "--segment_size";
    std::string input_audio_flag = "--input_audio";
    std::string output_audio_flag = "--output_audio";
    std::string transcript_flag = "--transcript";

    Args parsed_args;

    std::vector<std::string> args(argv+1, argv+argc);

    try {
        for (int i = 0; i<args.size(); i++){
            if (args[i] == freq_flag){
                parsed_args.num_frequencies = std::stoi(args[i+1]);
            }
            if (args[i] == segment_size_flag){
                parsed_args.segment_size = std::stod(args[i+1]);
            }
            if (args[i] == input_audio_flag){
                parsed_args.input_audio_file_path = args[i+1];
            }
            if (args[i] == output_audio_flag){
                parsed_args.output_audio_file_path = args[i+1];
            }
            if (args[i] == transcript_flag){
                parsed_args.transcript_file_path = args[i+1];
            }
        }
        // it is mandatory to set input_audio
        if (parsed_args.input_audio_file_path.empty()){
            throw std::exception();
        }
    }catch(...){
        std::cerr << "invalid command line arguments\n" << std::endl;
        std::exit(1);
    }

    return parsed_args;
}

int main(int argc, char *argv[]) {

    auto [ inputAudioFilePath, outputAudioFilePath, transcriptFilePath, num_frequencies, segment_size ] = parse_args(argc, argv);
    AudioAnalyzer analyzer(segment_size, num_frequencies);
    channel_field data = analyzer.analyzeAudio(inputAudioFilePath);

    // reconstructing the audio from extracted dominant frequencies
    if (!outputAudioFilePath.empty()){
        generateAudio(outputAudioFilePath, data);
    }

    // generating a transcript based on the input audio file
    if (!transcriptFilePath.empty()){
        generateTranscript(transcriptFilePath, data);
    }

    return 0;
}
