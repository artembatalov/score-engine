#pragma once
#include <string>
#include <vector>
#include "event.h"

class Pattern {
public:
    Pattern();
    Pattern(std::string name, int resolution);
    void AddEvent(Event& event);
    std::string GetName();
    int GetResolution();
    bool GetEvent(int& index, Event& event);
private:
    std::string name_;
    int resolution_;
    std::vector<Event> events_;
};