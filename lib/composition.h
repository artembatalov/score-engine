#pragma once
#include <vector>
#include <string>
#include "instruments.h"
#include "pattern.h"

class Composition {
public:
    int GetBPM();
    int GetInstrumentsNumber();
    bool SetBPM(int bpm);
    void AddInstrument(std::unique_ptr<Instrument>& instrument);
    void AddPattern(Pattern& pattern);
    bool GetPattern(int& index, Pattern& pattern);
    size_t GetInstrumentIndex(std::string& name);
    Instrument* GetInstrument(size_t index);
private:
    int bpm_;
    std::vector<std::unique_ptr<Instrument> > instruments_;
    std::vector<Pattern> patters_;
};