#include <iostream>

// Freecam module
class Freecam {
public:
    Freecam() {
        std::cout << "Freecam initialized" << std::endl;
    }
    
    void enable() {
        std::cout << "Freecam enabled" << std::endl;
    }
    
    void disable() {
        std::cout << "Freecam disabled" << std::endl;
    }
};

int main() {
    Freecam cam;
    cam.enable();
    cam.disable();
    return 0;
}
