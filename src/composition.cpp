#include <stdexcept>
#include <string>
#include <memory>
#include <cstddef>
#include "composition.h"
#include <vector>
#include "pattern.h"
#include "instruments.h"

int Composition::GetBPM() {
    return bpm_;
}

int Composition::GetInstrumentsNumber() {
    return instruments_.size();
}

Instrument* Composition::GetInstrument(size_t index) {
    return instruments_[index].get();
}

bool Composition::SetBPM(int bpm) {
    if (bpm < 1) { 
        return false;
    }
    bpm_ = bpm;
    return true;
}

void Composition::AddInstrument(std::unique_ptr<Instrument>& instrument) {
    instruments_.push_back(std::move(instrument));
}

void Composition::AddPattern(Pattern& pattern) {
    patters_.push_back(pattern);
}

bool Composition::GetPattern(int& index, Pattern& pattern) {
    if (index >= patters_.size()) {
        return false;
    }
    pattern = patters_[index];
    return true;
}

size_t Composition::GetInstrumentIndex(std::string& name) {
    for (int i = 0; i < instruments_.size(); i++) {
        if (name == instruments_[i]->GetName()) {
            return i;
        }
    }
    throw std::runtime_error("Used instrument is missing!");
}