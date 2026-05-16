#include <iostream>
#include <itmo_loops.h>

int main(int argc, char* argv[]) {
    ItmoLoops generator(argc, argv);
    generator.Run();
    std::cout << "File was saved to:\n" << argv[2] << "\n";
    return 0;
}