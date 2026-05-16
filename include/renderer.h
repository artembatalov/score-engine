#pragma once
#include <fstream>
#include <vector>
#include <unordered_map>
#include "composition.h"
#include "pattern.h"

extern const std::unordered_map<std::string, double> kFrequences;
extern const int kSampleRate;
extern const int kBits;

class WavRenderer {
public:
    WavRenderer(Composition& composition, std::string& file_path_);
    ~WavRenderer();
    void Render();
private:
    std::string file_path_;
    std::ofstream output_;
    Composition& composition_;

    struct Note {
        uint64_t start;
        uint64_t duration;
        double velocity;
        double frequency;
        size_t instrument;
    };
    std::vector<Note> timeline_;
    std::vector<double> comp_;

    #pragma pack(push, 1)
    struct Header {
        char riff[4] = {'R','I','F','F'};
        int32_t chunk_size;
        char wave[4] = {'W','A','V','E'};
        char fmt[4] = {'f','m','t',' '};
        int32_t subchunk_size = 16;
        int16_t audio_format = 1;
        int16_t num_channels;
        int32_t sample_rate;
        int32_t byte_rate;
        int16_t block_align;
        int16_t bits_per_sample;
        char data[4] = {'d','a','t','a'};
        int32_t data_size;
    };
    #pragma pack(pop)

    void BuildTimeline();
    Pattern CallPattern(std::string name);
    void ProcessPattern(Pattern pattern, int units_shift);
    void ProccessNote(Event& event, Pattern& pattern, int& units_shift);
    static bool Comp(Note& first, Note& second);

    uint64_t CountStart(Event& event, Pattern& pattern, int& units_shift);
    uint64_t CountDuration(Event& event, Pattern& pattern);
    double CountVelocity(Event& event, Pattern& pattern);
    double CountFrequency(Event& event, Pattern& pattern);
    size_t CountInstrument(Event& event, Pattern& pattern);
    uint64_t DefineCompLen();

    void RenderSound(
        size_t& InstrumentIndex, 
        std::vector<double>&inst_buffer);
    void Normalize();
    void WriteHeader();
    void WriteData();
};