#include <stdexcept>
#include <string>
#include <cstddef>
#include "event.h"

Event::Event() {
    type_ = Type::kEmpty;
}

Event::Event(int& start, std::string& call) : start_(start), event_(call) {
    type_ = Type::kCall;
};

Event::Event(
    int& start, 
    std::string& instrument, 
    std::string& pitch, 
    double& duration, 
    double& velocity) :
    start_(start),
    event_(Note{instrument, pitch, duration, velocity}) {
        type_ = Type::kNote;
    };

Event::Type Event::GetType() {
    return type_;
}

std::string Event::GetName() {
    if (type_ == Type::kEmpty) {
        throw std::runtime_error("Event is empty!");
    }
    return std::get<std::string>(event_);
}

int Event::GetStart() {
    if (type_ == Type::kEmpty) {
        throw std::runtime_error("Event is empty!");
    }
    return start_;
}

std::string Event::GetInstrument() {
    if (type_ != Type::kNote) {
        throw std::runtime_error("Event is not a Note!");
    }
    return std::get<Note>(event_).instrument;
}

std::string Event::GetPitch() {
    if (type_ != Type::kNote) {
        throw std::runtime_error("Event is not a Note!");
    }
    return std::get<Note>(event_).pitch;
}

double Event::GetDuration() {
    if (type_ != Type::kNote) {
        throw std::runtime_error("Event is not a Note!");
    }
    return std::get<Note>(event_).duration;
}

double Event::GetVelocity() {
    if (type_ != Type::kNote) {
        throw std::runtime_error("Event is not a Note!");
    }
    return std::get<Note>(event_).velocity;
}