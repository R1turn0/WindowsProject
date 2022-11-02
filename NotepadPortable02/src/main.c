#include <windows.h>
#include <strsafe.h>
#include "resource.h"
#include "NoteFile.h"
#include "NoteFont.h"

#define NUMLINES 100
#define EDITID   1
#define BUFFER(x, y) *(pBuffer + y * cxBuffer + x) 
#define UNTITLED TEXT("New File")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static TCHAR szAppName[] = TEXT("Notepad#");
static TCHAR szMenuName[] = TEXT("NotepadMenu");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szFileName[MAX_PATH];
    HWND hwnd;  // ���ھ��
    MSG msg;
    WNDCLASS wndclass = { 0 };
    HACCEL hAccel = NULL;
    HMENU hMenu;     // �˵��ľ����

    wndclass.style = CS_HREDRAW | CS_VREDRAW;                           // ������ȸ߶����»��ƴ���
    wndclass.lpfnWndProc = WndProc;                                     // ָ�����ڹ��̣������ǻص�����
    wndclass.cbClsExtra = 0;                                            // ����ռ䣬һ��Ϊ0
    wndclass.cbWndExtra = 0;                                            // ����ռ䣬һ��Ϊ0
    wndclass.hInstance = hInstance;                                     // Ӧ�ó����ʵ�����
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));    // ���л��ڸô�����Ĵ����趨��ͼ��
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);                     // ���л��ڸô�����Ĵ����趨�����ָ��
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);       // ����
    wndclass.lpszMenuName = szMenuName;                                 // �˵�
    wndclass.lpszClassName = szAppName;                                 // ����������

    // Unicode�ַ���
    if (!RegisterClass(&wndclass))  // ʹ��IsWindowUnicode�жϴ����Ƿ�ΪUnicode����
    {
        MessageBox(NULL, TEXT("���������Ҫ��Windows NT�����У�"), szAppName, MB_ICONERROR);
        return 0;
    }

    hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
    hwnd = CreateWindow(
        szAppName,
        TEXT("Windows Test!"),
        WS_OVERLAPPEDWINDOW,  // WS_VSCROLL | WS_HSCROLL ��ֱ��ˮƽ������
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        hMenu,
        hInstance,
        NULL
    );

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    hAccel = LoadAccelerators(hInstance, szMenuName);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(hwnd, hAccel, &msg))
        {
            TranslateMessage(&msg); // �����ַ���Ϣ��������ϢͶ�ݵ���Ϣ������
            DispatchMessage(&msg);  // ��
        }
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static DWORD dwCharSet = DEFAULT_CHARSET;
    static HWND hwndEdit;
    static HINSTANCE hInst;
    
    static int cxChar, cyChar;
    static int cxClient, cyClient;
    static int cxBuffer, cyBuffer;  // ����������Ϊ������棬������ܴ�Ŷ��ٸ��ַ�
    static int xCaret, yCaret;
    static int cxIcon, cyIcon;      // ͼ��Ĵ�С
    
    static TCHAR* pBuffer = NULL;
    static TCHAR szFileText[MAX_PATH];
    static TCHAR szFileName[MAX_PATH];
    static TCHAR szBuffer[MAX_PATH];
    HDC hdc;
    HMENU hMenu;
    int x, y, i;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;
    //HINSTANCE hInstance;    // ʵ���ľ���� �����ڴ���ģ��Ļ�ַ��

    switch (message)
    {
    case WM_CREATE:
        hInst = ((LPCREATESTRUCT)lParam) -> hInstance;
        cxIcon = GetSystemMetrics(SM_CXICON);
        cyIcon = GetSystemMetrics(SM_CXICON);
        hwndEdit = CreateWindow(
            TEXT("EDIT"),
            TEXT("///����������///"),
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
            0,
            10,
            1000,
            500,
            hwnd,
            (HMENU)EDITID,
            hInst,
            NULL
        );
        NotepadFileInitialize(hwnd);
        NotepadFontInitialize(hwndEdit);
        DoCaption(hwnd, szFileName);
        //return 0;

    case WM_SETFOCUS:
        SetFocus(hwndEdit);
        return 0;

    case WM_SIZE:
        MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;

    case WM_COMMAND:
        hMenu = GetMenu(hwnd);
        switch (LOWORD(wParam))
        {
        case IDM_FILE_NEW:
            DestroyWindow(hwndEdit); // ����ָ������
            SendMessage(hwnd, WM_CREATE, NULL, 0);
            break;
        case IDM_FILE_OPEN:
        case IDM_FILE_SAVE:
            if (szFileText[0])
            {
                if (1)
                {
                    return 0;
                }
                else
                {
                    wsprintf(szBuffer, TEXT("Can't Save the File!"), szFileName[0] ? szFileName : UNTITLED);
                    MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
                    return 0;
                }
            }
        case IDM_FILE_SAVEAS:
        case IDM_FILE_PRINT:
        case IDM_FILE_CLOSE:
            DestroyWindow(hwndEdit); // ����ָ������
            break;
        case IDM_EXIT:
            SendMessage(hwnd, WM_CLOSE, NULL, 0);
        default:
            break;
        }
        return 0;

    case WM_PAINT:
        return 0;
        

    case WM_CLOSE:      // ����Ϊ���ڻ�Ӧ�ó���Ӧ��ֹ���ź�
        if (MessageBox(hwnd, TEXT("�Ƿ��˳�"), TEXT("�˳�"), MB_YESNO) == IDYES)
            DestroyWindow(hwnd);
        else
            return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }// message END

    return DefWindowProc(hwnd, message, wParam, lParam);
}

int DoCaption(HWND hwnd, TCHAR* szTitleName)
{
    TCHAR szCaption[1 + MAX_PATH];
    wsprintf(szCaption, TEXT("%s - %s"), szAppName, szTitleName[0] ? szTitleName : UNTITLED);
    SetWindowText(hwnd, szCaption);
    return 0;
}

/*
Creating a File View
https://learn.microsoft.com/en-us/windows/win32/memory/creating-a-file-view

MapViewOfFile function (memoryapi.h)
https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile

*/