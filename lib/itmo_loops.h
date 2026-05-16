#pragma once
#include <string>

class ItmoLoops {
public:
    ItmoLoops(int argc, char* argv[]);
    void Run();
private:
    bool CheckFiles();
    bool CheckFormat();
    std::string input_;
    std::string output_;
};