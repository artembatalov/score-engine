#pragma once
#include <string>
#include <variant>

class Event {
public:
    Event();
    Event(int& start, std::string& call);
    Event(
        int& start, 
        std::string& instrument, 
        std::string& pitch, 
        double& duration, 
        double& velocity
    );

    enum class Type {kEmpty, kCall, kNote};
    Type GetType();
    std::string GetName();
    int GetStart();
    std::string GetInstrument();
    std::string GetPitch();
    double GetDuration();
    double GetVelocity();
private:
    struct Note {
        std::string instrument;
        std::string pitch;
        double duration;
        double velocity;
    };
    int start_;
    std::variant<Note, std::string> event_;
    Type type_;
};