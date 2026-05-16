#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>
#include "renderer.h"
#include "pattern.h"
#include "event.h"
#include "composition.h"

const std::unordered_map<std::string, double> kFrequences = {
    {"C0",  16.35},
    {"C#0", 17.32},
    {"D0",  18.35},
    {"D#0", 19.45},
    {"E0",  20.60},
    {"F0",  21.83},
    {"F#0", 23.12},
    {"G0",  24.50},
    {"G#0", 25.96},
    {"A0",  27.50},
    {"A#0", 29.14},
    {"B0",  30.87},
    {"C1",  32.70},
    {"C#1", 34.65},
    {"D1",  36.71},
    {"D#1", 38.89},
    {"E1",  41.20},
    {"F1",  43.65},
    {"F#1", 46.25},
    {"G1",  49.00},
    {"G#1", 51.91},
    {"A1",  55.00},
    {"A#1", 58.27},
    {"B1",  61.74},
    {"C2",  65.41},
    {"C#2", 69.30},
    {"D2",  73.42},
    {"D#2", 77.78},
    {"E2",  82.41},
    {"F2",  87.31},
    {"F#2", 92.50},
    {"G2",  98.00},
    {"G#2", 103.83},
    {"A2",  110.00},
    {"A#2", 116.54},
    {"B2",  123.47},
    {"C3",  130.81},
    {"C#3", 138.59},
    {"D3",  146.83},
    {"D#3", 155.56},
    {"E3",  164.81},
    {"F3",  174.61},
    {"F#3", 185.00},
    {"G3",  196.00},
    {"G#3", 207.65},
    {"A3",  220.00},
    {"A#3", 233.08},
    {"B3",  246.94},
    {"C4",  261.63},
    {"C#4", 277.18},
    {"D4",  293.66},
    {"D#4", 311.13},
    {"E4",  329.63},
    {"F4",  349.23},
    {"F#4", 369.99},
    {"G4",  392.00},
    {"G#4", 415.30},
    {"A4",  440.00},
    {"A#4", 466.16},
    {"B4",  493.88},
    {"C5",  523.25},
    {"C#5", 554.37},
    {"D5",  587.33},
    {"D#5", 622.25},
    {"E5",  659.26},
    {"F5",  698.46},
    {"F#5", 739.99},
    {"G5",  783.99},
    {"G#5", 830.61},
    {"A5",  880.00},
    {"A#5", 932.33},
    {"B5",  987.77},
    {"C6",  1046.50},
    {"C#6", 1108.73},
    {"D6",  1174.66},
    {"D#6", 1244.51},
    {"E6",  1318.51},
    {"F6",  1396.91},
    {"F#6", 1479.98},
    {"G6",  1567.98},
    {"G#6", 1661.22},
    {"A6",  1760.00},
    {"A#6", 1864.66},
    {"B6",  1975.53},
    {"C7",  2093.00},
    {"C#7", 2217.46},
    {"D7",  2349.32},
    {"D#7", 2489.02},
    {"E7",  2637.02},
    {"F7",  2793.83},
    {"F#7", 2959.96},
    {"G7",  3135.96},
    {"G#7", 3322.44},
    {"A7",  3520.00},
    {"A#7", 3729.31},
    {"B7",  3951.07},
    {"C8",  4186.01},
    {"C#8", 4434.92},
    {"D8",  4698.64},
    {"D#8", 4978.03},
    {"E8",  5274.04},
    {"F8",  5587.65},
    {"F#8", 5919.91},
    {"G8",  6271.93},
    {"G#8", 6644.88},
    {"A8",  7040.00},
    {"A#8", 7458.62},
    {"B8",  7902.13}
};

const int kSampleRate = 44100;
const int kBits = 16;

WavRenderer::WavRenderer(Composition& composition, std::string& file_path) :
    composition_(composition),  file_path_(file_path) {
    output_.open(file_path_);
    BuildTimeline();
    comp_ = std::vector<double>(DefineCompLen(),0.0);
}

WavRenderer::~WavRenderer() {
    if (output_.is_open()) {
        output_.close();
    }
}

Pattern WavRenderer::CallPattern(std::string name) {
    Pattern pattern;
    for (int i = 0; composition_.GetPattern(i, pattern); i++) {
        if (pattern.GetName() == name) {
            return pattern;
        }
    }
    throw std::runtime_error("Calling pattern is missing!");
}

uint64_t WavRenderer::CountStart(
    Event& event, 
    Pattern& pattern, 
    int& units_shift) {
    uint64_t samples_per_unit = kSampleRate * 60 
    / (composition_.GetBPM() * pattern.GetResolution());
    return samples_per_unit * (units_shift + event.GetStart());
}

uint64_t WavRenderer::CountDuration(Event& event, Pattern& pattern) {
    uint64_t samples_per_unit = kSampleRate * 60 
    / (composition_.GetBPM() * pattern.GetResolution());
    return samples_per_unit * event.GetDuration();
}

double WavRenderer::CountVelocity(Event& event, Pattern& pattern) {
    return event.GetVelocity() / 100.0;
}

double WavRenderer::CountFrequency(Event& event, Pattern& pattern) {
    return kFrequences.at(event.GetPitch());
}

size_t WavRenderer::CountInstrument(Event& event, Pattern& pattern) {
    std::string instrument_name = event.GetInstrument();
    size_t instrument = composition_.GetInstrumentIndex(instrument_name);
    return instrument;
}

void WavRenderer::ProccessNote(
    Event& event, 
    Pattern& pattern, 
    int& units_shift) {
    timeline_.push_back({
        CountStart(event, pattern, units_shift), 
        CountDuration(event, pattern), 
        CountVelocity(event, pattern), 
        CountFrequency(event, pattern), 
        CountInstrument(event, pattern)
    });
}

void WavRenderer::ProcessPattern(Pattern pattern, int units_shift) {
    Event event;
    for (int i = 0; pattern.GetEvent(i, event); i++) {
        if (event.GetType() == Event::Type::kNote) {
            ProccessNote(event, pattern, units_shift);
            continue;
        }
        int new_units_shift = event.GetStart();
        ProcessPattern(CallPattern(event.GetName()), new_units_shift);
    }
}

bool WavRenderer::Comp(Note& first, Note& second) {
    return first.start < second.start;
}

void WavRenderer::BuildTimeline() {
    ProcessPattern(CallPattern("main"), 0);
    std::sort(timeline_.begin(), timeline_.end(), Comp);
}

uint64_t WavRenderer::DefineCompLen() {
    uint64_t res = 0;
    for (int i = 0; i < timeline_.size(); i++) {
        res = std::max(res, timeline_[i].start + timeline_[i].duration);
    }
    return res;
}

void WavRenderer::RenderSound(
    size_t& InstrumentIndex, 
    std::vector<double>&inst_buffer) {
    for (int j = 0; j < timeline_.size(); j++) {
        if (InstrumentIndex == timeline_[j].instrument) {
            for (int s = 0; s < timeline_[j].duration; s++) {
                uint64_t global = s + timeline_[j].start;
                double value = composition_.GetInstrument(InstrumentIndex)
                    ->Sample(
                        s, 
                        timeline_[j].duration, 
                        timeline_[j].frequency, 
                        timeline_[j].velocity);
                inst_buffer[global] += value;
            }
        }
    }
}

void WavRenderer::WriteHeader() {
    Header header;
    header.num_channels = 1;
    header.subchunk_size = 16;
    header.audio_format = 1;
    header.sample_rate = kSampleRate;
    header.bits_per_sample = kBits;
    header.block_align = header.num_channels * header.bits_per_sample / 8;
    header.byte_rate = header.sample_rate * header.block_align;
    header.data_size = comp_.size() * header.block_align;
    header.chunk_size = 36 + header.data_size;
    output_.write(reinterpret_cast<const char*>(&header), sizeof(Header));
}

void WavRenderer::WriteData() {
    for (size_t i = 0; i < comp_.size(); ++i) {
        double v = std::max(-1.0, std::min(1.0, comp_[i]));
        int16_t sample = static_cast<int16_t>(v * 32767);
        output_.write(reinterpret_cast<const char*>(&sample), sizeof(sample));
    }
}

void WavRenderer::Normalize() {
    int coef = -1;
    for (int i = 0; i < comp_.size(); i++) {
        if (comp_[i] > 1 && comp_[i] > coef) {
            coef = comp_[i];
        }
    }
    if (coef == -1) {
        return;
    }
    for (int i = 0; i < comp_.size(); i++) {
        comp_[i] = comp_[i] / coef;
    }
}

void WavRenderer::Render() {
    for (size_t i = 0; i < composition_.GetInstrumentsNumber(); i++) {
        std::vector<double> inst_buffer(comp_.size(), 0.0);
        RenderSound(i, inst_buffer);
        composition_.GetInstrument(i)->ApplyEffects(inst_buffer);
        for (size_t k = 0; k < comp_.size(); k++) {
            comp_[k] += inst_buffer[k];
        }
    }
    WriteHeader();
    Normalize();
    WriteData();
}