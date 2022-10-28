#include <windows.h>
#include <strsafe.h>

#define NUMLINES 100
#define BUFFER(x, y) *(pBuffer + y * cxBuffer + x) 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("MyWindows");
    HWND hwnd;  // ���ھ��
    MSG msg;
    WNDCLASS wndclass = { 0 };

    wndclass.style = CS_HREDRAW | CS_VREDRAW;                       // ������ȸ߶����»��ƴ���
    wndclass.lpfnWndProc = WndProc;                                 // ָ�����ڹ��̣������ǻص�����
    wndclass.cbClsExtra = 0;                                        // ����ռ䣬һ��Ϊ0
    wndclass.cbWndExtra = 0;                                        // ����ռ䣬һ��Ϊ0
    wndclass.hInstance = hInstance;                                 // Ӧ�ó����ʵ�����
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);               // ���л��ڸô�����Ĵ����趨��ͼ��
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);                 // ���л��ڸô�����Ĵ����趨�����ָ��
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);   // ����
    wndclass.lpszMenuName = NULL;                                   // �˵�
    wndclass.lpszClassName = szAppName;                             // ����������

    // Unicode�ַ���
    if (!RegisterClass(&wndclass))  // ʹ��IsWindowUnicode�жϴ����Ƿ�ΪUnicode����
    {
        MessageBox(NULL, TEXT("���������Ҫ��Windows NT�����У�"), szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(
        szAppName,
        TEXT("Windows Test!"),
        WS_OVERLAPPEDWINDOW,  // WS_VSCROLL | WS_HSCROLL ��ֱ��ˮƽ������
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
        TranslateMessage(&msg); // �����ַ���Ϣ��������ϢͶ�ݵ���Ϣ������
        DispatchMessage(&msg);  // ��
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static DWORD dwCharSet = DEFAULT_CHARSET;
    
    static int cxChar, cyChar;
    static int cxClient, cyClient;
    static int cxBuffer, cyBuffer;  // ����������Ϊ������棬������ܴ�Ŷ��ٸ��ַ�
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
        // �˴����践��

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

        // ������������ո�
        for (y = 0; y < cyBuffer; y++)
        {
            for (x = 0; x < cxBuffer; x++)
            {
                BUFFER(x, y) = ' ';
            }
        }

        // �����Ų������Ͻ�
        xCaret = 0; 
        yCaret = 0;

        if (hwnd == GetFocus())
        {
            SetCaretPos(xCaret * cxChar, yCaret * cyChar);
        }

        InvalidateRect(hwnd, NULL, TRUE);
        return 0;

    case WM_SETFOCUS:
        CreateCaret(hwnd, NULL, cxChar, cyChar);    // �����������
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