#include <windows.h>
#include <strsafe.h>
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

    wndclass.style = CS_HREDRAW | CS_VREDRAW;                       // 调整宽度高度重新绘制窗口
    wndclass.lpfnWndProc = WndProc;                                 // 指定窗口过程（必须是回调函数
    wndclass.cbClsExtra = 0;                                        // 额外空间，一般为0
    wndclass.cbWndExtra = 0;                                        // 额外空间，一般为0
    wndclass.hInstance = hInstance;                                 // 应用程序的实例句柄
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);               // 所有基于该窗口类的窗口设定的图标
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);                 // 所有基于该窗口类的窗口设定的鼠标指针
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);   // 背景
    wndclass.lpszMenuName = szMenuName;                             // 菜单
    wndclass.lpszClassName = szAppName;                             // 窗口类名字

    // Unicode字符码
    if (!RegisterClass(&wndclass))  // 使用IsWindowUnicode判断窗口是否为Unicode窗口
    {
        MessageBox(NULL, TEXT("这个程序需要在Windows NT上运行！"), szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(
        szAppName,
        TEXT("Windows Test!"),
        WS_OVERLAPPEDWINDOW,  // WS_VSCROLL | WS_HSCROLL 垂直、水平滚动条
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
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
    
    static TCHAR* pBuffer = NULL;
    static TCHAR szFileName[MAX_PATH];
    HDC hdc;
    int x, y, i;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;

    switch (message)
    {
    case WM_CREATE:
        hInst = ((LPCREATESTRUCT)lParam) -> hInstance;
        hwndEdit = CreateWindow(
            TEXT("EDIT"),
            TEXT("**我是编辑框 请在这里输入内容**"),
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
        DoCaption(hwndEdit, szFileName);
        return 0;

        //就是为了编辑框获取焦点后产生的消息
    case WM_SETFOCUS:

        SetFocus(hwndEdit);
        return 0;

        //窗口的大小发生变化后，会收到这个消息。

    case WM_SIZE:
        MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
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