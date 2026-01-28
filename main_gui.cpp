#include "LibraryGUI.h"
#include <iostream>

int main() {
    LibraryGUI gui;
    
    // Try to load font - you may need to adjust the path
    if (!gui.loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Error: Could not load font!" << std::endl;
        std::cerr << "Please ensure DejaVu Sans font is installed or update the font path." << std::endl;
        return 1;
    }
    
    gui.run();
    
    return 0;
}
