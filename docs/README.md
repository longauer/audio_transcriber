#  Audio Transcriber
My goal for this project was to create a tool that can help you analyze audio recordings and create a transcription to a simplified musical notation.
This application is intended to be used mainly for analysis of musical recordings from the point of view of standard western 12-tone equal temperament given that it classifies notes with reference to this specific standardized temperament.

# Description
- the applications takes an audio file on the input (.wav is the only accepted file type)
- the input audio file can have multiple channels
- the app can generate a transcript of the musical notation
- the app can generate an audio file based on the generated transcript (for testing of the accuracy of the transcript)

### Lower level modules
- *wav_processing.h* is used to parse the .wav input file (details from the header of the file, raw data in the PCM format)
- *wav_creation.h* serves to rebuild the transformed version of the original recording as captured by the created representation
- *dft.h* contains implementation of FFT algorithm as well as the Hann windowing function for reducing spectral leakage after transforming the time-domain sample by DFT
- *note_classifier.h* contains a class that encapsulates a musical note in the final musical representation. It is able to decide the note's name and assignment to an octave.

### Higher level modules
These are the modules directly used in the main.cpp file of the *Audio Transcriber*
- *audio_analysis.h* produces a list - progression of chords in analyzed segments for each of the channels of the recording. It directly depends on lower level modules: *wav_processing.h*, *note_classifier.h*
- *audio_generation.h* encapsulates lower level module *wav_creation.h*
- *transcript_generation.h* simply produces a transcript written in a .txt file. It directly depends on lower level module: *note_classifier.h*


# How to use
- there are five different command line arguments that you can specify: *--input_audio*, *--output_audio*, *--transcript* respectively for the location of the input audio file, location of the output audio file and the transcript. If *--output_audio* or *--transcript* is not specified the respective file will not be generated. Specifying the remaining two command line arguments is voluntary with their defaults settings being *--num_frequencies 1* and *--segment_size 0*. Even though not technically required, specifying the last two arguments is recommended in most cases in order to get better result.
- the *--num_frequencies* parameter dictates the maximum number of concurrent notes per segment in the final transcription
- the *--segment_size* specifies the length of one segment (subdivisions of the recording) in seconds (adjusting to the tempo leads to better results)

# How to build and run the application
### prerequisites:
- CMake (version 3.23 or higher)
- C++ Compiler that supports C++20 standard (e.g., GCC, Clang, MSVC)

### steps to building and running the application:
- After downloading the package navigate to the root directory of the project
- create a "build" directory within the root project directory and navigate inside the newly created directory
- run CMake to generate the build files with the command: *cmake ..*
- use CMake to build the project with the following command: *cmake --build .*
- the last command creates an executable in the src subdirectory within the build directory. Run *./src/audio_transcriber* (on Unix-based systems) or *./src/audio_transcriber.exe* (on Windows) (specifying at least the mandatory commandline arguments)

# Example
In package there are directories *audio_output*, *audio_transcripts* with outputs for two of the sample recordings from the *audio_samples* directory. The ./audio_output/progression.wav and ./audio_transcripts/piano_progression.txt can be generated with the following command run from the root directory of the package (assuming the project is already built):
*./build/src/audio_transcriber --input_audio ./audio_samples/piano_progression.wav --output_audio ./audio_output/progression.wav --transcript ./audio_transcripts/piano_progression.txt --num_frequencies 8 --segment_size 3.4*