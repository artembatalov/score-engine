#include <string>
#include "pattern.h"
#include "event.h"

Pattern::Pattern() {};

Pattern::Pattern(
    std::string name, 
    int resolution) : 
    name_(name), 
    resolution_(resolution) {};

void Pattern::AddEvent(Event& event) {
    events_.push_back(event);
}

std::string Pattern::GetName() {
    return name_;
}

int Pattern::GetResolution() {
    return resolution_;
}

bool Pattern::GetEvent(int& index, Event& event) {
    if (index >= events_.size()) {
        return false;
    }
    event = events_[index];
    return true;
}