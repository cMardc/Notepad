#define UNICODE
#include "framework.h"
#include "Notepad.h"


#define MAX_LOADSTRING 100



HINSTANCE hInst;
HMENU hMenu;
HWND TextBox;
wchar_t szFile[100] = { NULL };

void loadMenu(HWND hWnd);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OpenDialog(HWND hwnd);
void loadInputBox(HWND hWnd);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {

    WNDCLASSW wc = { 0 };

    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"Window Class";
    wc.lpfnWndProc = WndProc;

    if (!RegisterClassW(&wc)) {
        return -1;
    }

    CreateWindowW(L"Window Class", L"Merdium Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 800, 800, NULL, NULL, NULL, NULL);

    MSG msg = { 0 };

    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}





LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        std::wifstream inFile(szFile); // Declaration and initialization here
        int wmId = LOWORD(wParam); // Extract command ID from wParam

        switch (wmId)
        {
        case 1: //New
            SetWindowTextW(hWnd, L"Untitled - Notepad");
            break;
        case 2: //Open
            OpenDialog(hWnd); 
            if (szFile[0] != L'\0') {
                std::wifstream inFile(szFile);
                if (inFile.is_open()) {
                    std::wstring fileContent((std::istreambuf_iterator<wchar_t>(inFile)), std::istreambuf_iterator<wchar_t>());

                    // Replace newline characters with \r\n for multiline display
                    std::wstring updatedContent;
                    for (wchar_t ch : fileContent) {
                        if (ch == L'\n') {
                            updatedContent += L"\r\n";
                        }
                        else {
                            updatedContent += ch;
                        }
                    }

                    SetWindowTextW(TextBox, updatedContent.c_str());

                    // Extract filename from path
                    std::wstring filename = szFile;
                    size_t lastBackslash = filename.find_last_of(L"\\/");
                    if (lastBackslash != std::wstring::npos) {
                        filename = filename.substr(lastBackslash + 1);
                    }

                    // Update the main window's title to include the filename
                    std::wstring windowTitle = filename + L" - Notepad";
                    SetWindowTextW(hWnd, windowTitle.c_str());

                    inFile.close();
                }
                else {
                    MessageBox(hWnd, L"Failed to open file for reading!", L"Error", MB_OK | MB_ICONERROR);
                }
            }
            else {
                MessageBox(hWnd, L"No file selected.", L"Info", MB_OK | MB_ICONINFORMATION);
            }
            break;


        case 3: //Save
        {
            OPENFILENAME ofn = { 0 };
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrDefExt = L"txt";
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

            if (GetSaveFileName(&ofn)) {
                wchar_t textBuffer[1024]; // Adjust the buffer size as needed
                GetWindowTextW(TextBox, textBuffer, sizeof(textBuffer) / sizeof(textBuffer[0]));

                std::wofstream outFile(szFile);
                if (outFile.is_open()) {
                    outFile << textBuffer;
                    outFile.close();
                    MessageBox(hWnd, L"File saved successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
                }
                else {
                    MessageBox(hWnd, L"Failed to save file!", L"Error", MB_OK | MB_ICONERROR);
                }
            }
        }
        break;
        case 4: //Exit
        {
            if (MessageBox(hWnd, L"Do you really want to exit? Any unsaved data will be lost.", L"Warning", MB_YESNO | MB_ICONWARNING) == IDYES) {
                PostQuitMessage(0);
            }
        }
        case 5: // Copy
        {
            SendMessage(TextBox, WM_COPY, 0, 0);
        }
        break;

        case 6: // Cut
        {
            SendMessage(TextBox, WM_CUT, 0, 0);
        }
        break;
        case 7: // Paste
        {
            SendMessage(TextBox, WM_PASTE, 0, 0);
        }
        break;
        case 8: // Select All
            SendMessage(TextBox, EM_SETSEL, 0, -1); // Select all text
            break;
        case 9: // About
            MessageBox(hWnd, L"This is the Notepad app.", L"About", MB_OK | MB_ICONINFORMATION);
            break;
        default:
            break;
        }
    }
    break;


    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        SetWindowTextW(hWnd, L"Notepad");
        loadMenu(hWnd);
        loadInputBox(hWnd);
        break;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
        break;
    }
    return 0;
}





void loadMenu(HWND hWnd) {
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();
    HMENU hEditMenu = CreateMenu();
    HMENU hHelpMenu = CreateMenu();

    AppendMenu(hFileMenu, MF_STRING, 1, L"New");
    AppendMenu(hFileMenu, MF_STRING, 2, L"Open");
    AppendMenu(hFileMenu, MF_STRING, 3, L"Save");
    AppendMenuW(hFileMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenu(hFileMenu, MF_STRING, 4, L"Exit");

    AppendMenu(hEditMenu, MF_STRING, 5, L"Copy");
    AppendMenu(hEditMenu, MF_STRING, 6, L"Cut");
    AppendMenu(hEditMenu, MF_STRING, 7, L"Paste");
    AppendMenu(hEditMenu, MF_STRING, 8, L"Select All");

    AppendMenu(hHelpMenu, MF_STRING, 9, L"About");

    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, L"Edit");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"Help");

    SetMenu(hWnd, hMenu);
}



void loadInputBox(HWND hWnd) {
    TextBox = CreateWindow(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
        0, 0, 800, 800, hWnd, NULL, NULL, NULL);
}




void OpenDialog(HWND hwnd)
{

    OPENFILENAME ofn;

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;


    // Display the Open dialog box.
    if (GetOpenFileName(&ofn) == TRUE) {
        wcscpy_s(szFile, ofn.lpstrFile);
    }



}