#include <iostream>

// Noclip module
class Noclip {
public:
    Noclip() {
        std::cout << "Noclip initialized" << std::endl;
    }
    
    void enable() {
        std::cout << "Noclip enabled - collision disabled" << std::endl;
    }
    
    void disable() {
        std::cout << "Noclip disabled - collision enabled" << std::endl;
    }
};

int main() {
    Noclip clip;
    clip.enable();
    clip.disable();
    return 0;
}
