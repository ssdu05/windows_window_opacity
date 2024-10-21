#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include <TlHelp32.h>
#include <vector>
#include <cwctype>
#include <array>

namespace globals {
	static BYTE opacity = 255;
	static std::vector<PROCESSENTRY32> processes(70);
    static std::vector<const wchar_t*> excludesW{ L"explorer"};
    static std::vector<const char*> excludesWindowText{ "Program Manager", "MSCTFIME UI", "Default IME"};
};


void setPE32s() {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32W pe32 = { 0 };
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    
    if (!Process32FirstW(hSnap, &pe32)) {
        CloseHandle(hSnap);
        return;
    }
    
    do {
        globals::processes.push_back(pe32);
    } while (Process32NextW(hSnap, &pe32));

    CloseHandle(hSnap);
}

void SetWindowOpacity(HWND hwnd, BYTE opacity) {
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, opacity, LWA_ALPHA);
}

bool getPE32ByID(std::vector<PROCESSENTRY32W>& pe32s, DWORD procID, OUT PROCESSENTRY32W& out) {
    for (PROCESSENTRY32W& pe32 : pe32s) {
        if (pe32.th32ProcessID == procID) {
            out = pe32;
            return true;
        }
    }
    return false;
}

// case is ignored
bool contains(const wchar_t* str, const wchar_t* find) {
    int lenStr{ lstrlenW(str) };
    int lenFind{ lstrlenW(find) };
    unsigned int found{ 0 };
    for (unsigned int i{ 0 }; i < lenStr; ++i) {
        for (unsigned int j{ 0 }; j < lenFind; ++j) {
            //std::wcout << str[i + j] << L" == " << find[j] << std::endl;
            if ((i + j) >= lenStr)
                break;

			if (std::towlower(str[i + j]) == std::towlower(find[j])) {
				++found;
			}
			else {
				found = 0;
                break;
			}
			if (found == lenFind) {
				return true;
			}
        }
    }
    return false;
}


bool contains(const char* str, const char* find) {
    size_t lenStr{ strlen(str) };
    size_t lenFind{ strlen(find) };
    unsigned int found{ 0 };
    for (unsigned int i{ 0 }; i < lenStr; ++i) {
        for (unsigned int j{ 0 }; j < lenFind; ++j) {
            //std::wcout << str[i + j] << L" == " << find[j] << std::endl;
            if ((i + j) >= lenStr)
                break;

			if (std::tolower(str[i + j]) == std::tolower(find[j])) {
				++found;
			}
			else {
				found = 0;
                break;
			}
			if (found == lenFind) {
				return true;
			}
        }
    }
    return false;
}

bool containAny(const wchar_t* str, const std::vector<const wchar_t*> any) {
    for (auto find : any) {
        if (contains(str, find)) {
            return true;
        }
    }
    return false;
}

bool containAny(const char* str, const std::vector<const char*> any) {
    for (auto find : any) {
        if (contains(str, find)) {
            return true;
        }
    }
    return false;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD procID{ 0 };
    constexpr unsigned int sizeTextWindow{ 1024 };
    char textWindow[sizeTextWindow];
    memset(textWindow, 0, sizeTextWindow);
    GetWindowTextA(hWnd, textWindow, sizeTextWindow);

    if (GetWindowThreadProcessId(hWnd, &procID)) {
        PROCESSENTRY32W pe32;
        if (getPE32ByID(globals::processes, procID, pe32)) {
			if (!containAny(pe32.szExeFile, globals::excludesW) && !containAny(textWindow, globals::excludesWindowText) && strlen(textWindow) != 0) {
				//std::cout << "procid is: " << procID << std::endl;
				//std::wcout << pe32.szExeFile << std::endl;
				//std::cin.get();
				SetWindowOpacity(hWnd, globals::opacity);
				
			}
			else {
                // a window of lively paper has textWindow strlen of 0
                if (1) {
                    pe32.th32ModuleID;
					//std::cout << "strlen is: " << strlen(textWindow) << std::endl;
					//std::cout << "hWnd is: " << hWnd << std::endl;
					//std::cout << "window textWindow is: " << textWindow << std::endl;
					//std::cout << "procid is: " << procID << std::endl;
					//std::wcout << pe32.szExeFile << std::endl;
					//std::cin.get();
					//SetWindowOpacity(hWnd, globals::opacity);
                }
			}
        }
    }
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

    setPE32s();
    EnumWindows(EnumWindowsProc, 0);
    return 0;
}