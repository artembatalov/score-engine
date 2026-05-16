#include <fstream>
#include <string>
#include <stdexcept>
#include "itmo_loops.h"
#include "composition.h"
#include "parser.h"
#include "renderer.h"

void ItmoLoops::Run() {
    Composition composition;
    ItmoLoopsParser parser(composition, input_);
    parser.Parse();
    WavRenderer renderer(composition, output_);
    renderer.Render();
}

ItmoLoops::ItmoLoops(int argc, char* argv[]) {
    if (argc != 3) {
        throw std::invalid_argument("Invalid number of arguments!");
    }
    input_ = argv[1];
    output_ = argv[2];
    if (!CheckFormat() || !CheckFiles()) {
        throw std::invalid_argument("Invalid pathes!");
    }
}

bool ItmoLoops::CheckFormat() {
    if (input_.size() < 5 
    || input_.substr(input_.size() - 4, 4) != ".txt") {
        return false;
    }
    if (output_.size() < 5
    || output_.substr(output_.size() - 4, 4) != ".wav") {
        return false;
    }
    return true;
}

bool ItmoLoops::CheckFiles() {
    std::fstream input(input_, std::ios::in);
    if (!input.is_open()) {
        return false;
    }
    input.close();
    return true;
}