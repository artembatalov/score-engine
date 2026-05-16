#include <string>
#include <fstream>
#include <utility>
#include <memory>
#include <cctype>
#include <cstddef>
#include <stdexcept>
#include "parser.h"
#include "composition.h"
#include "event.h"
#include "effects.h"
#include "instruments.h"
#include "pattern.h"

ItmoLoopsParser::ItmoLoopsParser(Composition& composition, 
    std::string file_path) : 
    composition_(composition), 
    file_path_(file_path) {
    input_.open(file_path_);
}

ItmoLoopsParser::~ItmoLoopsParser() {
    if (input_.is_open()) {
        input_.close();
    }
}

std::string& ItmoLoopsParser::Peek(size_t offset) {
    if (index_ + offset < tokens_.size()) {
        return tokens_[index_ + offset ];
    }
    throw std::runtime_error("Syntax error in file!");
}
    
bool ItmoLoopsParser::Advance(size_t offset) {
    if (index_ + offset < tokens_.size()) {
        index_ = index_ + offset;
        return true;
    }
    return false;
}

void ItmoLoopsParser::ProcessBPM() {
    composition_.SetBPM(std::stoi(Peek()));
    Advance();
}

std::pair<std::string, std::string> ExtractValues(std::string& input) {
    int pos = input.find("=");
    if (pos == -1) {
        throw std::runtime_error("Syntax error in file!");
    }
    return std::make_pair(input.substr(0, pos), 
            input.substr(pos + 1, input.size() - pos - 1));
}

void ItmoLoopsParser::ProcessInstrumentValue(
    std::unique_ptr<Instrument>& instrument, 
    std::string& str) {
    std::pair<std::string, std::string> setting = ExtractValues(str);
    instrument->AddSetting(setting.first, setting.second);
}

void ItmoLoopsParser::ProcessInstrumentEffect(
    std::unique_ptr<Instrument>& instrument) {
    std::unique_ptr<Effect> effect 
        = EffectMaker::Create(Peek(2));
    Advance(2);
    while (Peek() != "end" && Peek() != "effect") {
        std::pair<std::string, std::string> setting = ExtractValues(Peek());
        effect->AddSetting(setting.first, setting.second);
        Advance();
    }
    instrument->AddEffect(effect);
}

void ItmoLoopsParser::ProcessInstrument() {
    std::unique_ptr<Instrument> instrument 
        = InstrumentMaker::Create(Peek(2));
    instrument->SetName(Peek());
    Advance(2);
    while (Peek() != "end") {
        if (Peek() == "effect") {
            ProcessInstrumentEffect(instrument);
        }
        else {
            ProcessInstrumentValue(instrument, Peek());
            Advance();
        }
        
    }
    Advance();
    composition_.AddInstrument(instrument);
}

void ItmoLoopsParser::ProcessCall(Pattern& new_pattern) {
    int start = std::stoi(Peek());
    std::string call =  Peek(2).substr(1);
    Event new_event(start, call);
    new_pattern.AddEvent(new_event);
    Advance(2);
}

void ItmoLoopsParser::ProcessNote(Pattern& new_pattern) {
    int start = std::stoi(Peek());
    std::string instrument = Peek(2);
    std::string pitch = Peek(3);
    double duration = std::stod(Peek(4));
    double velocity = std::stod(Peek(5));
    Event new_event(start, instrument, pitch, duration, velocity);
    new_pattern.AddEvent(new_event);
    Advance(5);
}

void ItmoLoopsParser::GetNotesAndLinks(Pattern& new_pattern) {
    while (Peek() != "end") {
        if (Peek(2)[0] == '@') {
            ProcessCall(new_pattern);
        }
        else {
            ProcessNote(new_pattern);
        }
    }
}

void ItmoLoopsParser::ProcessPattern() {
    std::string name = Peek();
    int resolution = std::stoi(Peek(3));
    Advance(3);
    Pattern new_pattern(name, resolution);
    GetNotesAndLinks(new_pattern);
    composition_.AddPattern(new_pattern);
    Advance();
}

void ItmoLoopsParser::ProcessToken() {
    if (tokens_[index_] == "bpm") {
        ProcessBPM();
    }
    else if (tokens_[index_] == "instrument") {
        ProcessInstrument();
    }
    else if (tokens_[index_] == "pattern") {
        ProcessPattern();
    }
    else {
        throw std::runtime_error("Syntax error in file!");
    }
}

bool ItmoLoopsParser::MakeShift() {
    while (true) {
        char current = input_.peek();
        if (current == -1) {
            return false;
        }
        if (std::isspace(static_cast<unsigned char>(current))) {
            input_.get();
            continue;
        }
        if (current == '#') {
            input_.get();
            std::string comment = "";
            getline(input_, comment);
            continue;
        }
        return true;
    }
}

bool ItmoLoopsParser::GetToken(std::string& token) {
    if (!MakeShift()) {
        return false;
    }
    char current;
    while (input_.get(current)) {
        if (std::isspace(static_cast<unsigned char>(current))) {
            break;
        }
        token += current;
    }
    return true;
}

void ItmoLoopsParser::Parse() {
    std::string token;
    while (GetToken(token)) {
        tokens_.push_back(token);
        token.clear();
    }
    do {
        ProcessToken();
    } while (Advance());
}