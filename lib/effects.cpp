#include <stdexcept>
#include <vector>
#include <memory>
#include <cmath>
#include <string>
#include <cstddef>
#include "renderer.h"
#include "effects.h"

bool Effect::Validate(std::string& value) {
    return std::stod(value) >= 0;
}

void Gain::AddSetting(std::string& name, std::string& value) {
    if (name == "gain" && Validate(value)) {
        gain_ = std::stod(value);
    }
    else {
        throw std::runtime_error("Unknown effect setting!");
    }
}

void Gain::Proccess(std::vector<double>&buffer) {
    for (int i = 0; i < buffer.size(); i++) {
        buffer[i] = buffer[i] * gain_;
    }
}

void Echo::AddSetting(std::string& name, std::string& value) {
    if (name == "delay" && Validate(value)) {
        delay_samples_ = static_cast<size_t>(std::stod(value) * 44100);    
    }
    else if (name == "decay" && Validate(value)) {
        decay_ = std::stod(value);
    }
    else {
        throw std::runtime_error("!");
    }
}

void Echo::Proccess(std::vector<double>&buffer) {
    for (size_t i = delay_samples_; i < buffer.size(); ++i) {
        buffer[i] += decay_ * buffer[i - delay_samples_];
    }
}

void Tremolo::AddSetting(std::string& name, std::string& value) {
    if (name == "freq" && Validate(value)) {
        freq_ = std::stod(value);
    }
    else if (name == "depth" && Validate(value)) {
        depth_ = std::stod(value);
    }
    else {
        throw std::runtime_error("Unknown effect setting!");
    }
}

void Tremolo::Proccess(std::vector<double>&buffer) {
    for (int i = 0; i < buffer.size(); i++) {
        double t = static_cast<double>(i) / kSampleRate;
        double mod = 1.0 - depth_ + depth_ * std::sin(2.0 * 3.14 * freq_ * t);
        buffer[i] = buffer[i] * mod;
    }
}

std::unique_ptr<Effect> EffectMaker::Create(std::string& type) {
    if (type == "gain") {
        return std::make_unique<Gain>();
    }
    else if (type == "echo") {
        return std::make_unique<Echo>();
    }
    else if (type == "tremolo") {
        return std::make_unique<Tremolo>();
    }
    else {
        throw std::runtime_error("Unknown instrument effect!");
    }
}