#include "LibraryGUI.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {
std::vector<std::string> candidateFontPaths() {
    std::vector<std::string> candidates;

    if (const char* envFont = std::getenv("LIBRARY_GUI_FONT")) {
        candidates.emplace_back(envFont);
    }

#ifdef LIBRARY_GUI_DEFAULT_FONT
    if (std::string defaultFont = LIBRARY_GUI_DEFAULT_FONT; !defaultFont.empty()) {
        candidates.push_back(defaultFont);
    }
#endif

#ifdef _WIN32
    candidates.emplace_back("C:/Windows/Fonts/arial.ttf");
    candidates.emplace_back("C:/Windows/Fonts/segoeui.ttf");
#elif defined(__APPLE__)
    candidates.emplace_back("/System/Library/Fonts/Supplemental/Arial.ttf");
    candidates.emplace_back("/System/Library/Fonts/Supplemental/Helvetica.ttc");
#else
    candidates.emplace_back("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    candidates.emplace_back("/usr/share/fonts/dejavu/DejaVuSans.ttf");
    candidates.emplace_back("/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf");
#endif

    return candidates;
}
}  // namespace

int main() {
    LibraryGUI gui;

    for (const auto& fontPath : candidateFontPaths()) {
        if (!fontPath.empty() && std::filesystem::exists(fontPath) && gui.loadFont(fontPath)) {
            gui.run();
            return 0;
        }
    }

    std::cerr << "Error: Could not load a GUI font." << std::endl;
    std::cerr << "Set LIBRARY_GUI_FONT to a valid .ttf/.ttc file path or pass "
              << "-DLIBRARY_GUI_DEFAULT_FONT=/path/to/font when configuring CMake."
              << std::endl;
    return 1;
}
