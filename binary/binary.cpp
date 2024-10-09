#include <Windows.h>
#include <iostream>

void SetWindowOpacity(HWND hwnd, BYTE opacity) {
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, opacity, LWA_ALPHA);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    SetWindowOpacity(hwnd, 230);
    return TRUE;
}

int main() {
    EnumWindows(EnumWindowsProc, 0);

    std::cout << "Transparency applied to all windows!" << std::endl;
    return 0;
}