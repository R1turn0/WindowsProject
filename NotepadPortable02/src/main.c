#include <windows.h>
#include <strsafe.h>
#include "resource.h"
#include "NoteFile.h"
#include "NoteFont.h"
//#include "NotePrint.h"

#define CXCREAT 0
#define CYCREAT 0
#define NUMLINES 100
#define EDITID   1
#define UNHEADER TEXT("New File")
#define BUFFER(x, y) *(pBuffer + y * cxBuffer + x)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int AskAboutSave(HWND, TCHAR*);

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

    // 从与应用程序实例关联的可执行 (.exe) 文件中加载指定的菜单资源。
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

    // 加载指定的加速器表（即快捷键
    hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));   
    
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
    static HINSTANCE hInst = NULL;
   
    static int cxBuffer, cyBuffer;      // 整个窗口作为输入界面，计算出能存放多少个字符
    static int cxIcon, cyIcon;          // 图标的大小
    static TCHAR* pBuffer = NULL;       // 缓冲区指针
    static TCHAR szFileName[MAX_PATH];  // 文件名
    static TCHAR szTitleName[MAX_PATH]; // 
    static TCHAR szFileText[MAX_PATH];  // 文件内容
    static TCHAR szBuffer[MAX_PATH];    // 设置缓冲区

    HDC hdc;
    HMENU hMenu;
    int x, y, i;
    int id;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;
    static BOOL isSave = TRUE;
    static int szlParam = 0;
    //HINSTANCE hInstance;    // 实例的句柄。 这是内存中模块的基址。

    switch (message)
    {
    case WM_CREATE:
        if (hInst == NULL)
            hInst = ((LPCREATESTRUCT)lParam) -> hInstance;
        cxIcon = GetSystemMetrics(SM_CXICON);
        cyIcon = GetSystemMetrics(SM_CXICON);
        FileInit(hwnd);
        FontInit(hwndEdit);
        DoCaption(hwnd, szFileName);
        hwndEdit = CreateWindow(
            TEXT("EDIT"),
            TEXT("New File."),
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
            CXCREAT,
            CYCREAT,
            LOWORD(szlParam), // 因为取到lParam为00 00 00 00，在重新发送WM_SIZE后获取到lParam，so进程重新绘制窗口
            HIWORD(szlParam),
            hwnd,
            (HMENU)EDITID,
            hInst,
            NULL
        ); 
        id = GetWindowLong(hwndEdit, GWL_ID);
        SendMessage(hwndEdit, EM_LIMITTEXT, 32000, 0L);
        return 0;

    case WM_SIZE:
        szlParam = lParam;
        MoveWindow(hwndEdit, 0, 0, LOWORD(szlParam), HIWORD(szlParam), TRUE);   // 更改指定窗口的位置和尺寸。
        //UpdateWindow(hwnd);
        //InvalidateRect(hwnd, NULL, TRUE);
        //RedrawWindow(hwnd, );
        return 0;

    case WM_SETFOCUS:
        SetFocus(hwndEdit);
        return 0;

    case WM_COMMAND:
        hMenu = GetMenu(hwnd);
        if (lParam && LOWORD(wParam) == EDITID)
        {
            switch (HIWORD(wParam))
            {
            case EN_UPDATE:
                isSave = TRUE;
                break;
            case EN_ERRSPACE:   // 当编辑控件无法分配足够的内存以满足特定请求时，将发送EN_ERRSPACE通知消息
            case EN_MAXTEXT:    // 当前文本插入超过编辑控件的指定字符数时，将发送EN_MAXTEXT通知消息。文本插入已被截断。
                MessageBox(hwnd, TEXT("编辑控件内存不足。"),
                    szAppName, MB_OK | MB_ICONSTOP);
                break;
            }
            break;
            return 0;
        }

        switch (LOWORD(wParam))
        {
        case IDM_FILE_NEW:
            DestroyWindow(hwndEdit); // 销毁指定窗口
            SendMessage(hwnd, WM_CREATE, 0, NULL);
            SendMessage(hwnd, WM_SETFOCUS, 0, NULL);
            break;

        case IDM_FILE_OPEN:
            if (!isSave && IDCANCEL == AskAboutSave(hwnd, szFileName))
                break;
            DestroyWindow(hwndEdit); // 销毁指定窗口
            SendMessage(hwnd, WM_CREATE, 0, NULL);
            if (FileOpenDlg(hwnd, szFileName, szTitleName))
            {
                if (FALSE == MapFileRead(hwndEdit, szTitleName))
                {
                    wsprintf(szBuffer, TEXT("无法读取文件"), szTitleName[0] ? szTitleName : UNHEADER);
                    MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
                    szFileName[0] = '\0';
                    szTitleName[0] = '\0';
                }
            }
            DoCaption(hwnd, szTitleName);
            isSave = TRUE;
            break;

        case IDM_FILE_SAVE:
            if (FileWrite(hwndEdit, szFileName))
            {
                break;
            }
            
        case IDM_FILE_SAVEAS:
            if (FileSaveDlg(hwnd, szFileName, szTitleName))
            {
                DoCaption(hwnd, szTitleName);
                if (FileWrite(hwndEdit, szFileName))
                {
                    isSave = TRUE;
                    break;
                }
                else
                {
                    wsprintf(szBuffer, TEXT("无法另存为..."), szTitleName[0] ? szTitleName : UNHEADER);
                    MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
                    break;
                }
            }
            break;
        case IDM_FILE_PRINT:
            //if (!PrintFile(hInst, hwnd, hwndEdit, szTitleName))
            //{
            //    wsprintf(szBuffer, TEXT("打印失败"), szTitleName[0] ? szTitleName : UNHEADER);
            //    MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
            //}
            break;

        case IDM_FILE_CLOSE:
            if (!isSave && MessageBox(hwnd, TEXT("是否保存？"), TEXT("退出"), MB_YESNO) == IDYES)
            {
                SendMessage(hwndEdit, WM_COMMAND, IDM_FILE_SAVE, 0);
            }
            DestroyWindow(hwndEdit); // 销毁指定窗口
            break;

        case IDM_EXIT:
            SendMessage(hwnd, WM_CLOSE, NULL, 0);
            break;

        case ID_FORMAT_FONT:
            if (FontChooseFont(hwnd))
                FontSetFont(hwndEdit);
            break;

        default:
            break;
        }
        return 0;

    case WM_CLOSE:      // 发送为窗口或应用程序应终止的信号
        if (!isSave && MessageBox(hwnd, TEXT("是否保存？"), TEXT("退出"), MB_YESNO) == IDYES)
        {
            SendMessage(hwndEdit, WM_COMMAND, IDM_FILE_SAVE, 0);
        }
        if (MessageBox(hwnd, TEXT("是否退出"), TEXT("退出"), MB_YESNO) == IDYES)
            DestroyWindow(hwnd);
        else
            return 0;

    case WM_DESTROY:
        FontDeinitialize();
        PostQuitMessage(0);
        return 0;
    }// message END

    return DefWindowProc(hwnd, message, wParam, lParam);
}

int DoCaption(HWND hwnd, TCHAR* szFileName)
{
    TCHAR szCaption[1 + MAX_PATH];
    wsprintf(szCaption, TEXT("%s - %s"), szAppName, szFileName[0] ? szFileName : UNHEADER);
    SetWindowText(hwnd, szCaption);
    return 0;
}

int AskAboutSave(HWND hwnd, TCHAR* szFileName)
{
    TCHAR szBuffer[MAX_PATH + 64];
    int ret;

    wsprintf(szBuffer, TEXT("是否保存当前文件%s?"), szFileName[0] ? szFileName : UNHEADER);
    ret = MessageBox(hwnd, szBuffer, szFileName, MB_YESNOCANCEL | MB_ICONQUESTION);
    if (ret == IDYES)
    {
        if (!SendMessage(hwnd, WM_COMMAND, IDM_FILE_SAVE, 0))
        {
            ret = IDCANCEL;
        }
    }
    return ret;
}


/*
Creating a File View
https://learn.microsoft.com/en-us/windows/win32/memory/creating-a-file-view

MapViewOfFile function (memoryapi.h)
https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile

*/