#pragma once
#include <string>
#include <fstream>
#include <vector>
#include "composition.h"

class ItmoLoopsParser {
public:
    ItmoLoopsParser(Composition& composition, std::string file_path);
    ~ItmoLoopsParser();
    void Parse();

private:
    std::string& Peek(size_t offset = 1);
    bool Advance(size_t offset = 1);

    bool MakeShift();
    bool GetToken(std::string& token);
    void ProcessToken();

    void ProcessBPM();

    void ProcessInstrument();
    void ProcessInstrumentEffect(std::unique_ptr<Instrument>& instrument);
    void ProcessInstrumentValue(
        std::unique_ptr<Instrument>& instrument, 
        std::string& str);

    void ProcessPattern();
    void GetNotesAndLinks(Pattern& new_pattern);
    void ProcessCall(Pattern& new_pattern);
    void ProcessNote(Pattern& new_pattern);

    std::string file_path_;
    std::ifstream input_;
    Composition& composition_;
    std::vector<std::string> tokens_;
    size_t index_ = 0;
};