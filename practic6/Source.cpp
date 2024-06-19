#include <windows.h>
#include <commctrl.h>
#include <wincrypt.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <strsafe.h>

#pragma comment(lib, "comctl32.lib") // ����������� ���������� comctl32.lib ��� ��������
#pragma comment(lib, "crypt32.lib")  // ����������� ���������� crypt32.lib ��� ����������������� �������
#pragma comment(lib, "shell32.lib")  // ����������� ���������� shell32.lib ��� ������ ������
#pragma comment(lib, "shlwapi.lib")  // ����������� ���������� shlwapi.lib ��� ������ �� ��������

// ���������� ����������:
HINSTANCE hInst; // ������� ���������
HWND hwndButtonBrowse, hwndEditFilePath, hwndProgress, hwndEditFileInfo, hwndEditHashOutput, hwndEditHashInput, hwndButtonVerify, hwndButtonWebsite, hwndButtonAbout, hwndButtonExit, hwndProgressText, hwndLabelFilePath, hwndLabelHashOutput, hwndLabelHashInput, hwndLabelFileInfo;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ComputeFileHash(LPCTSTR filePath, HWND hwndProgress, HWND hwndEditHashOutput, HWND hwndEditFileInfo, HWND hwndProgressText);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT("��� ����������");
    HWND         hwnd;
    MSG          msg;
    WNDCLASS     wndclass;

    // ������������� ����� ��������� ����������
    InitCommonControls();

    // ����������� ������ ����
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT("��������� Windows NT"), szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(szAppName, TEXT("��� ���������� ��� Windows"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        650, 450, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        // �������� ����� ��� ���� ����� ���� � �����
        hwndLabelFilePath = CreateWindowEx(0, L"STATIC", L"���� � �����:",
            WS_CHILD | WS_VISIBLE,
            20, 20, 120, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        // �������� ���������� ���� ��� ���� � �����
        hwndEditFilePath = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            110, 20, 380, 30, hwnd, (HMENU)2, GetModuleHandle(NULL), NULL);

        // �������� ������ "Browse..."
        hwndButtonBrowse = CreateWindowEx(0, L"BUTTON", L"Browse...",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            500, 20, 100, 30, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

        // �������� ���������� ���������
        hwndProgress = CreateWindowEx(0, PROGRESS_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
            20, 60, 580, 30, hwnd, (HMENU)3, GetModuleHandle(NULL), NULL);
        SendMessage(hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // ��������� ���������
        SendMessage(hwndProgress, PBM_SETSTEP, (WPARAM)1, 0); // ��� ���������

        // �������� ���� ��� ����������� �������� ����������� (�������)
        hwndProgressText = CreateWindowEx(0, L"STATIC", L"",
            WS_CHILD | WS_VISIBLE,
            20, 100, 580, 20, hwnd, (HMENU)11, GetModuleHandle(NULL), NULL);

        // �������� ����� ��� ���������� � ����� � ������ �����������
        hwndLabelFileInfo = CreateWindowEx(0, L"STATIC", L"���������� � �����:",
            WS_CHILD | WS_VISIBLE,
            20, 130, 200, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        // �������� ���������� ���� ��� ���������� � ����� � ������ �����������
        hwndEditFileInfo = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            20, 150, 580, 50, hwnd, (HMENU)4, GetModuleHandle(NULL), NULL);

        // �������� ����� ��� ������ ���� �����
        hwndLabelHashOutput = CreateWindowEx(0, L"STATIC", L"������� ��� �����:",
            WS_CHILD | WS_VISIBLE,
            20, 210, 200, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        // �������� ���������� ���� ��� ������ ���� �����
        hwndEditHashOutput = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY,
            20, 230, 580, 30, hwnd, (HMENU)5, GetModuleHandle(NULL), NULL);

        // �������� ����� ��� ����� ���������� ����
        hwndLabelHashInput = CreateWindowEx(0, L"STATIC", L"��� ��������� �����:",
            WS_CHILD | WS_VISIBLE,
            20, 270, 200, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        // �������� ���������� ���� ��� ����� ���������� ����
        hwndEditHashInput = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            20, 290, 580, 30, hwnd, (HMENU)6, GetModuleHandle(NULL), NULL);

        // �������� ������ "Verify"
        hwndButtonVerify = CreateWindowEx(0, L"BUTTON", L"Verify",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            20, 330, 100, 30, hwnd, (HMENU)7, GetModuleHandle(NULL), NULL);

        // �������� ������ "Website"
        hwndButtonWebsite = CreateWindowEx(0, L"BUTTON", L"Website",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            130, 330, 100, 30, hwnd, (HMENU)8, GetModuleHandle(NULL), NULL);

        // �������� ������ "About"
        hwndButtonAbout = CreateWindowEx(0, L"BUTTON", L"About",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            240, 330, 100, 30, hwnd, (HMENU)9, GetModuleHandle(NULL), NULL);

        // �������� ������ "Exit"
        hwndButtonExit = CreateWindowEx(0, L"BUTTON", L"Exit",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            350, 330, 100, 30, hwnd, (HMENU)10, GetModuleHandle(NULL), NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1: { // ������ "Browse..."
            OPENFILENAME ofn;       // ��������� ��� ������� �������� �����
            TCHAR szFilePath[MAX_PATH] = { 0 };
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = szFilePath;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFilePath);
            ofn.lpstrFilter = TEXT("��� �����\0*.*\0");
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn) == TRUE) {
                SetWindowText(hwndEditFilePath, ofn.lpstrFile);
                ComputeFileHash(ofn.lpstrFile, hwndProgress, hwndEditHashOutput, hwndEditFileInfo, hwndProgressText);
            }
            break;
        }
        case 7: { // ������ "Verify"
            TCHAR szComputedHash[41];
            TCHAR szInputHash[41];
            GetWindowText(hwndEditHashOutput, szComputedHash, 41);
            GetWindowText(hwndEditHashInput, szInputHash, 41);

            if (lstrcmpi(szComputedHash, szInputHash) == 0) {
                TCHAR szMessage[128];
                wsprintf(szMessage, TEXT("��������: %s\n�������: %s\n\n�������������!"), szInputHash, szComputedHash);
                MessageBox(hwnd, szMessage, TEXT("��������� ��������"), MB_OK | MB_ICONINFORMATION);
            }
            else {
                TCHAR szMessage[128];
                wsprintf(szMessage, TEXT("��������: %s\n�������: %s\n\n�� �������������!"), szInputHash, szComputedHash);
                MessageBox(hwnd, szMessage, TEXT("��������� ��������"), MB_OK | MB_ICONERROR);
            }
            break;
        }
        case 8: // ������ "Website"
            ShellExecute(NULL, TEXT("open"), TEXT("https://www.strerr.com/ru/sha1_file.html"), NULL, NULL, SW_SHOWNORMAL);
            break;
        case 9: // ������ "About"
            MessageBox(hwnd, TEXT("��������� ��� ����������� ������.\n�����: axm"), TEXT("� ���������"), MB_OK | MB_ICONINFORMATION);
            break;
        case 10: // ������ "Exit"
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

void ComputeFileHash(LPCTSTR filePath, HWND hwndProgress, HWND hwndEditHashOutput, HWND hwndEditFileInfo, HWND hwndProgressText) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    BYTE rgbFile[1024];
    DWORD cbRead = 0;
    BYTE rgbHash[20];
    DWORD cbHash = 20;
    CHAR rgbDigits[] = "0123456789abcdef";
    TCHAR szHash[41];
    LARGE_INTEGER fileSize;
    LARGE_INTEGER totalBytesRead = { 0 };
    int progress = 0;

    hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        MessageBox(NULL, TEXT("�� ������� ������� ����"), TEXT("������"), MB_OK | MB_ICONERROR);
        return;
    }

    GetFileSizeEx(hFile, &fileSize);

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        MessageBox(NULL, TEXT("CryptAcquireContext �� �������"), TEXT("������"), MB_OK | MB_ICONERROR);
        CloseHandle(hFile);
        return;
    }

    if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)) {
        MessageBox(NULL, TEXT("������ CryptGetHashParam"), TEXT("������"), MB_OK | MB_ICONERROR);
        CloseHandle(hFile);
        CryptReleaseContext(hProv, 0);
        return;
    }

    while (ReadFile(hFile, rgbFile, sizeof(rgbFile), &cbRead, NULL) && cbRead > 0) {
        if (!CryptHashData(hHash, rgbFile, cbRead, 0)) {
            MessageBox(NULL, TEXT("������ CryptGetHashParam"), TEXT("������"), MB_OK | MB_ICONERROR);
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            CloseHandle(hFile);
            return;
        }
        totalBytesRead.QuadPart += cbRead;
        progress = (int)((totalBytesRead.QuadPart * 100) / fileSize.QuadPart);
        SendMessage(hwndProgress, PBM_SETPOS, progress, 0);

        TCHAR szProgressText[50];
        wsprintf(szProgressText, TEXT("������� �����������: %d%%"), progress);
        SetWindowText(hwndProgressText, szProgressText);
    }

    cbHash = sizeof(rgbHash);
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
        for (DWORD i = 0; i < cbHash; i++) {
            szHash[i * 2] = rgbDigits[rgbHash[i] >> 4];
            szHash[i * 2 + 1] = rgbDigits[rgbHash[i] & 0xf];
        }
        szHash[cbHash * 2] = 0;

        SetWindowText(hwndEditHashOutput, szHash);

        // ���������� ���������� � ����� � ������ �����������
        TCHAR szFileInfo[512];
        wsprintf(szFileInfo, TEXT("����: %s\n ������: %lld ����\n ����� �����������: SHA-1"), filePath, fileSize.QuadPart);
        SetWindowText(hwndEditFileInfo, szFileInfo);
    }
    else {
        MessageBox(NULL, TEXT("������ CryptGetHashParam"), TEXT("������"), MB_OK | MB_ICONERROR);
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);
}
