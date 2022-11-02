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
    HWND hwnd;  // 窗口句柄
    MSG msg;
    WNDCLASS wndclass = { 0 };
    HACCEL hAccel = NULL;
    HMENU hMenu;     // 菜单的句柄。

    wndclass.style = CS_HREDRAW | CS_VREDRAW;                           // 调整宽度高度重新绘制窗口
    wndclass.lpfnWndProc = WndProc;                                     // 指定窗口过程（必须是回调函数
    wndclass.cbClsExtra = 0;                                            // 额外空间，一般为0
    wndclass.cbWndExtra = 0;                                            // 额外空间，一般为0
    wndclass.hInstance = hInstance;                                     // 应用程序的实例句柄
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));    // 所有基于该窗口类的窗口设定的图标
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);                     // 所有基于该窗口类的窗口设定的鼠标指针
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);       // 背景
    wndclass.lpszMenuName = szMenuName;                                 // 菜单
    wndclass.lpszClassName = szAppName;                                 // 窗口类名字

    // Unicode字符码
    if (!RegisterClass(&wndclass))  // 使用IsWindowUnicode判断窗口是否为Unicode窗口
    {
        MessageBox(NULL, TEXT("这个程序需要在Windows NT上运行！"), szAppName, MB_ICONERROR);
        return 0;
    }

    hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
    hwnd = CreateWindow(
        szAppName,
        TEXT("Windows Test!"),
        WS_OVERLAPPEDWINDOW,  // WS_VSCROLL | WS_HSCROLL 垂直、水平滚动条
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
            TranslateMessage(&msg); // 产生字符消息，并将消息投递到消息队列中
            DispatchMessage(&msg);  // 额
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
    static int cxBuffer, cyBuffer;  // 整个窗口作为输入界面，计算出能存放多少个字符
    static int xCaret, yCaret;
    static int cxIcon, cyIcon;      // 图标的大小
    
    static TCHAR* pBuffer = NULL;
    static TCHAR szFileText[MAX_PATH];
    static TCHAR szFileName[MAX_PATH];
    static TCHAR szBuffer[MAX_PATH];
    HDC hdc;
    HMENU hMenu;
    int x, y, i;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;
    //HINSTANCE hInstance;    // 实例的句柄。 这是内存中模块的基址。

    switch (message)
    {
    case WM_CREATE:
        hInst = ((LPCREATESTRUCT)lParam) -> hInstance;
        cxIcon = GetSystemMetrics(SM_CXICON);
        cyIcon = GetSystemMetrics(SM_CXICON);
        hwndEdit = CreateWindow(
            TEXT("EDIT"),
            TEXT("///请输入内容///"),
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
            DestroyWindow(hwndEdit); // 销毁指定窗口
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
            DestroyWindow(hwndEdit); // 销毁指定窗口
            break;
        case IDM_EXIT:
            SendMessage(hwnd, WM_CLOSE, NULL, 0);
        default:
            break;
        }
        return 0;

    case WM_PAINT:
        return 0;
        

    case WM_CLOSE:      // 发送为窗口或应用程序应终止的信号
        if (MessageBox(hwnd, TEXT("是否退出"), TEXT("退出"), MB_YESNO) == IDYES)
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