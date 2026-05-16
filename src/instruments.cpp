#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cmath>
#include <cstdint>
#include <memory>
#include <utility>
#include "instruments.h"
#include "renderer.h"
#include "effects.h"

void Instrument::SetName(std::string& name) {
    name_ = name;
}

std::string Instrument::GetName() {
    return name_;
}

double Instrument::Envelope(uint64_t time, uint64_t duration) {
    if (time < attack_) {
        return time / attack_;
    }
    if (time > duration - release_) {
        return (duration - time) / release_;
    }
    return 1.0f;
}

void Instrument::AddEffect(std::unique_ptr<Effect>& effect) {
    effects_.push_back(std::move(effect));
}

void Sampler::LoadSamples() {
    std::ifstream samples(sample_, std::ios::binary);
    if (!samples.is_open()) {
        throw std::runtime_error("Cannot open sampler file!");
    }
    samples.seekg(44);
    uint16_t s;
    while (samples.read((char*)&s, sizeof(int16_t))) {
        samples_.push_back(s / 32768.0);
    }
}

void Sampler::AddSetting(std::string& name, std::string& value) {
    if (name == "sample") {
        sample_ = value;
        LoadSamples();
    }
    else if (name == "root") {
        root_ = value;
    }
    else if (name == "attack") {
        attack_ = std::stod(value);
    }
    else if (name == "release") {
        release_ = std::stod(value);
    }
    else if (name == "loop") {
        int comma = value.find(",");
        if (comma < 1) {
            throw std::runtime_error("Wrong loop value for Sampler!");
        }
        loop1_ = std::stoi(value.substr(0, value.size() - comma - 1));
        loop2_ = std::stoi(value.substr(comma + 1, value.size() - comma - 1));
    }
    else {
        throw std::runtime_error("Unidentified setting for Sampler!");
    }
}

double Sampler::Sample(
    uint64_t time, 
    uint64_t duration, 
    double frequency, 
    double velocity) {
    double rate = frequency / kFrequences.at(root_);
    uint64_t pos = static_cast<uint64_t>(time * rate);
    if (loop1_ < 0 || loop2_ <= loop1_) {
        if (pos >= samples_.size()) {
            return 0.0;
        }
    } else {
        if (pos >= static_cast<uint64_t>(loop2_)) {
            uint64_t loop_len = loop2_ - loop1_;
            pos = loop1_ + (pos - loop1_) % loop_len;
        }
    }
    double raw = samples_[pos];
    double env = Envelope(time, duration);
    return raw * env * velocity;
}

void Square::AddSetting(std::string& name, std::string& value)  {
    if (name == "attack") {
        attack_ = std::stod(value);
    }
    else if (name == "release") {
        release_ = std::stod(value);
    }
    else if (name == "duty") {
        duty_ = std::stoi(value);
    }
    else {
        throw std::runtime_error("Unidentified setting for Sampler!");
    }
}

double Square::Sample(
    uint64_t time, 
    uint64_t duration, 
    double frequency, 
    double velocity) { 
    double t = (float)time / kSampleRate;
    double phase = frequency * t - std::floor(frequency * t);
    double duty_ratio = duty_ / 100.0;
    double raw;
    if (phase < duty_ratio) {
        raw = 1.0;
    }
    else {
        raw = -1.0;
    }
    double env = Envelope(time, duration);
    return raw * env * velocity;
}

void Sine::AddSetting(std::string& name, std::string& value) {
    if (name == "attack") {
        attack_ = std::stod(value);
    }
    else if (name == "release") {
        release_ = std::stod(value);
    }
    else {
        throw std::runtime_error("Unidentified setting for Sampler!");
    }
}

double Sine::Sample(
    uint64_t time, 
    uint64_t duration, 
    double frequency, 
    double velocity) { 
    double t = (double)time / kSampleRate;
    double phase = 2.0f * 3.14 * frequency * t;
    double raw = sin(phase);
    double env = Envelope(time, duration);
    return raw * env * velocity;
}

void Triangle::AddSetting(std::string& name, std::string& value) {
    if (name == "attack") {
        attack_ = std::stod(value);
    }
    else if (name == "release") {
        release_ = std::stod(value);
    }
    else {
        throw std::runtime_error("Unidentified setting for Sampler!");
    }
}

double Triangle::Sample(
    uint64_t time, 
    uint64_t duration, 
    double frequency, 
    double velocity) {
    double t = (double)time / kSampleRate;
    double phase = frequency * t - std::floor(frequency * t);
    double raw = 1.0 - 4.0 * std::abs(phase - 0.5);
    double env = Envelope(time, duration);
    return raw * env * velocity;
}

std::unique_ptr<Instrument> InstrumentMaker::Create(const std::string& type) {
    if (type == "sampler") {
        return std::make_unique<Sampler>();
    }
    else if (type == "square") {
        return std::make_unique<Square>();
    }
    else if (type == "sine") {
        return std::make_unique<Sine>();
    }
    else if (type == "triangle") {
        return std::make_unique<Triangle>();
    }
    else {
        throw std::runtime_error("Unkkown instrument!");
    }
}