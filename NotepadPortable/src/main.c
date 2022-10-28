#include <windows.h>
#include <strsafe.h>

#define NUMLINES 100
#define BUFFER(x, y) *(pBuffer + y * cxBuffer + x) 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("MyWindows");
    HWND hwnd;  // 窗口句柄
    MSG msg;
    WNDCLASS wndclass = { 0 };

    wndclass.style = CS_HREDRAW | CS_VREDRAW;                       // 调整宽度高度重新绘制窗口
    wndclass.lpfnWndProc = WndProc;                                 // 指定窗口过程（必须是回调函数
    wndclass.cbClsExtra = 0;                                        // 额外空间，一般为0
    wndclass.cbWndExtra = 0;                                        // 额外空间，一般为0
    wndclass.hInstance = hInstance;                                 // 应用程序的实例句柄
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);               // 所有基于该窗口类的窗口设定的图标
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);                 // 所有基于该窗口类的窗口设定的鼠标指针
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);   // 背景
    wndclass.lpszMenuName = NULL;                                   // 菜单
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

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg); // 产生字符消息，并将消息投递到消息队列中
        DispatchMessage(&msg);  // 额
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static DWORD dwCharSet = DEFAULT_CHARSET;
    
    static int cxChar, cyChar;
    static int cxClient, cyClient;
    static int cxBuffer, cyBuffer;  // 整个窗口作为输入界面，计算出能存放多少个字符
    static int xCaret, yCaret;
    
    static TCHAR* pBuffer = NULL;
    HDC hdc;
    int x, y, i;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;

    switch (message)
    {
    case WM_CREATE:
        hdc = GetDC(hwnd);

        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
        GetTextMetrics(hdc, &tm);
        cxChar = tm.tmAveCharWidth;
        cyChar = tm.tmHeight;

        ReleaseDC(hwnd, hdc);
        // 此处无需返回

    case WM_SIZE:
        if (message == WM_SIZE)
        {
            cxClient = LOWORD(lParam);
            cyClient = HIWORD(lParam);
        }

        cxBuffer = max(1, cxClient / cxChar);
        cyBuffer = max(1, cyClient / cyChar);

        if (pBuffer != NULL)
        {
            free(pBuffer);
        }
        pBuffer = (TCHAR*)malloc(cxBuffer * cyBuffer * sizeof(TCHAR));

        // 将输入框内填充空格
        for (y = 0; y < cyBuffer; y++)
        {
            for (x = 0; x < cxBuffer; x++)
            {
                BUFFER(x, y) = ' ';
            }
        }

        // 将符号插入左上角
        xCaret = 0; 
        yCaret = 0;

        if (hwnd == GetFocus())
        {
            SetCaretPos(xCaret * cxChar, yCaret * cyChar);
        }

        InvalidateRect(hwnd, NULL, TRUE);
        return 0;

    case WM_SETFOCUS:
        CreateCaret(hwnd, NULL, cxChar, cyChar);    // 创建插入符号
        SetCaretPos(xCaret * cxChar, yCaret * cxChar);
        ShowCaret(hwnd);
        return 0;

    case WM_KILLFOCUS:
        HideCaret(hwnd);
        DestroyCaret();
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_HOME:
            xCaret = 0;
            break;
            
        case VK_END:
            xCaret = cxBuffer - 1;
            break;

        case VK_PRIOR:
            yCaret = 0;
            break;

        case VK_NEXT:
            yCaret = cyBuffer - 1;
            break;

        case VK_LEFT:
            xCaret = max(xCaret - 1, 0);
            break;

        case VK_RIGHT:
            xCaret = min(xCaret + 1, cxBuffer - 1);
            break;

        case VK_UP:
            yCaret = max(yCaret - 1, 0);
            break;

        case VK_DOWN:
            yCaret = min(yCaret + 1, cyBuffer - 1);
            break;

        case VK_DELETE:
            for (x = xCaret; x < cxBuffer - 1; x++)
            {
                BUFFER(x, yCaret) = BUFFER(x + 1, yCaret);
            }
            BUFFER(cxBuffer - 1, yCaret) = ' ';

            HideCaret(hwnd);
            hdc = GetDC(hwnd);
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
            TextOut(hdc, xCaret * cxChar, yCaret * cyChar, &BUFFER(xCaret, yCaret), cxBuffer - xCaret);
            ReleaseDC(hwnd, hdc);
            ShowCaret(hwnd);
            break;
        }// WM_KEYDOWN END
        SetCaretPos(xCaret * cxChar, yCaret * cyChar);
        return 0;

    case WM_CHAR:
        for (i = 0; i < (int)LOWORD(lParam); i++)
        {
            switch (wParam)
            {
            case '\b':
                if (xCaret > 0)
                {
                    xCaret--;
                    SendMessage(hwnd, WM_KEYDOWN, VK_DELETE, 1);
                }
                break;

            case '\t':
                do
                {
                    SendMessage(hwnd, WM_CHAR, ' ', 1);
                } while (xCaret % 8 != 0);
                break;

            case '\n':
                if (++yCaret == cyBuffer)
                    yCaret = 0;
                break;

            case '\r':
                xCaret = 0;
                if (++yCaret == cyBuffer)
                {
                    yCaret = 0;
                }
                break;

            case'\x18':
                for (y = 0; y < cyBuffer; x++)
                {
                    for (x = 0; x < cxBuffer; x++)
                    {
                        BUFFER(x, y) = ' ';
                    }
                }

                xCaret = 0;
                yCaret = 0;

                InvalidateRect(hwnd, NULL, FALSE);
                break;

            default:
                BUFFER(xCaret, yCaret) = (TCHAR)wParam;

                HideCaret(hwnd);
                hdc = GetDC(hwnd);
                SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
                TextOut(hdc, xCaret* cxChar, yCaret* cyChar, &BUFFER(xCaret, yCaret), 1);
                ReleaseDC(hwnd, hdc);
                ShowCaret(hwnd);

                if (++xCaret == cxBuffer)
                {
                    xCaret = 0;
                    if (++yCaret == cyBuffer)
                    {
                        yCaret = 0;
                    }
                }
                break;
            }
        }

        SetCaretPos(xCaret * cxChar, yCaret * cyChar);
        return 0;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);

        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
        for (y = 0; y < cyBuffer; y++)
        {
            TextOut(hdc, 0, y * cyChar, &BUFFER(0, y), cxBuffer);
        }

        EndPaint(hwnd, &ps);
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