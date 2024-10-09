#include <Windows.h>
#include <iostream>
#include <cstdlib>

namespace globals {
	static BYTE opacity = 255;
};

void SetWindowOpacity(HWND hwnd, BYTE opacity) {
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, opacity, LWA_ALPHA);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    SetWindowOpacity(hwnd, globals::opacity);
    return TRUE;
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Usage: windopac.exe <opacity> -> 1 - 100" << std::endl;
        return -1;
    }
    
    if (argc == 2) {
        int opacity{ std::atoi(argv[1]) };
        if (opacity != 0 && opacity > 0 && opacity < 99) {
			globals::opacity = static_cast<BYTE>(opacity / 100.0 * 256.0);
        }
    }

    EnumWindows(EnumWindowsProc, 0);
    return 0;
}