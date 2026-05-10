#include <windows.h>
#include <stdio.h>
#include <string.h>

#define IDC_USERNAME  1001
#define IDC_GENERATE  1002
#define IDC_OUTPUT    1003
#define IDC_COPY      1004
#define IDC_ABOUT     1005
#define IDC_EXIT      1006

char gSerial[64];
HFONT hFont;

void CenterWindow(HWND hwnd)
{
    RECT rc;
    GetWindowRect(hwnd, &rc);

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    SetWindowPos(hwnd, NULL,
        (screenW - width) / 2,
        (screenH - height) / 2,
        0, 0,
        SWP_NOZORDER | SWP_NOSIZE);
}

void SetControlFont(HWND hwnd)
{
    SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
}

void GenerateKey(const char* username, char* output)
{
    char buffer[32] = "_r <()<1-Z2[l5,^";

    int userLen = strlen(username);
    int seedLen = strlen(buffer);
    int maxLen = (userLen > seedLen) ? userLen : seedLen;

    for (int i = 0; i < maxLen; i++)
    {
        unsigned char userChar = username[i % userLen];
        unsigned char seedChar = buffer[i % seedLen];

        buffer[i % seedLen] = ((userChar ^ seedChar) % 25) + 'A';
    }

    sprintf(output, "%.4s-%.4s-%.4s-%.4s",
        &buffer[0], &buffer[4], &buffer[8], &buffer[12]);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hUsername, hOutput;

    switch (msg)
    {
        case WM_CREATE:
        {
            hFont = CreateFontA(-11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma");

            HWND hLabel1 = CreateWindowExA(0, "STATIC", "Username:",
                WS_CHILD | WS_VISIBLE, 10, 10, 70, 18, hwnd, NULL, NULL, NULL);
            SetControlFont(hLabel1);

            hUsername = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                76, 8, 210, 21, hwnd, (HMENU)IDC_USERNAME, NULL, NULL);
            SetControlFont(hUsername);

            HWND hLabel2 = CreateWindowExA(0, "STATIC", "Reg. Code:",
                WS_CHILD | WS_VISIBLE, 10, 36, 70, 18, hwnd, NULL, NULL, NULL);
            SetControlFont(hLabel2);

            hOutput = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
                WS_CHILD | WS_VISIBLE | ES_READONLY | ES_AUTOHSCROLL,
                76, 34, 210, 21, hwnd, (HMENU)IDC_OUTPUT, NULL, NULL);
            SetControlFont(hOutput);

            const int btnW = 60, btnH = 22;
            const int startX = 14, spacing = 8, y = 60;

            HWND hGenerate = CreateWindowExA(0, "BUTTON", "Generate",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                startX, y, btnW, btnH, hwnd, (HMENU)IDC_GENERATE, NULL, NULL);
            SetControlFont(hGenerate);

            HWND hAbout = CreateWindowExA(0, "BUTTON", "About",
                WS_VISIBLE | WS_CHILD,
                startX + (btnW + spacing), y, btnW, btnH,
                hwnd, (HMENU)IDC_ABOUT, NULL, NULL);
            SetControlFont(hAbout);

            HWND hCopy = CreateWindowExA(0, "BUTTON", "Copy",
                WS_VISIBLE | WS_CHILD,
                startX + ((btnW + spacing) * 2), y, btnW, btnH,
                hwnd, (HMENU)IDC_COPY, NULL, NULL);
            SetControlFont(hCopy);

            HWND hExit = CreateWindowExA(0, "BUTTON", "Exit",
                WS_VISIBLE | WS_CHILD,
                startX + ((btnW + spacing) * 3), y, btnW, btnH,
                hwnd, (HMENU)IDC_EXIT, NULL, NULL);
            SetControlFont(hExit);

            return 0;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDC_GENERATE:
                {
                    char username[64];
                    GetWindowTextA(hUsername, username, sizeof(username));

                    if (strlen(username) < 4)
                    {
                        MessageBoxA(hwnd, "Username must be at least 4 characters.", "Error", MB_OK);
                        return 0;
                    }

                    GenerateKey(username, gSerial);
                    SetWindowTextA(hOutput, gSerial);
                    return 0;
                }

                case IDC_COPY:
                {
                    char text[64];
                    GetWindowTextA(hOutput, text, sizeof(text));

                    if (!strlen(text))
                    {
                        MessageBoxA(hwnd, "Nothing to copy.", "Error", MB_OK);
                        return 0;
                    }

                    if (OpenClipboard(hwnd))
                    {
                        EmptyClipboard();

                        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, strlen(text) + 1);
                        char* ptr = (char*)GlobalLock(hMem);

                        strcpy(ptr, text);
                        GlobalUnlock(hMem);

                        SetClipboardData(CF_TEXT, hMem);
                        CloseClipboard();
                    }

                    MessageBoxA(hwnd, "Reg. Code copied.", "Copied", MB_OK);
                    return 0;
                }

                case IDC_ABOUT:
                    MessageBoxA(hwnd,
                        "KeyGen by balktusse\nWin32 API in C\n2026-05-10",
                        "About", MB_OK);
                    return 0;

                case IDC_EXIT:
                    DestroyWindow(hwnd);
                    return 0;
            }
            return 0;
        }

        case WM_DESTROY:
            DeleteObject(hFont);
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "KeygenWindow";

    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(0, CLASS_NAME, "KeyGen by balktusse",
        WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        300, 116,
        NULL, NULL, hInstance, NULL
    );

    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(NULL, IDI_APPLICATION));
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL, IDI_APPLICATION));

    CenterWindow(hwnd);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}