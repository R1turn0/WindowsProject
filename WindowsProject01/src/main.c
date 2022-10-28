#include <windows.h>
#include <strsafe.h>

#define NUMLINES 100

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

// wParam: -WM_KEYDOWN:��������� / -WM_CHAR:ANSI��Unicode�ַ���
//lParam��ָ���ظ�������OEM ɨ���롢��չ����ǡ����ݴ��롢��ǰ״̬��ת��״̬�����庬��ͱ�ʾ�������£�
// =============================================================================================
//  λ            ����
// 0 - 15        1. ��ǰ��Ϣ���ظ��Ĵ���
//               2. ���û����°������ɿ�ʱ��Windows �Ὣ�ü����������� WM_KEYDOWN ��Ϣ�ϳ�Ϊһ����Ϣ�������Ӹ��ֶε�ֵ
//               3. �ظ��������� 1 ������ʱ�����������ٶȿ��ڳ���Ĵ�������
// 16 - 23	     OEM ɨ���루����Ӳ�������Ĵ��룩
// 24	         1. ָ���ð����Ƿ���չ��
//               2. ���� IBM ��ǿ�ͼ����� 101 �� 102 �������ұߵ� ALT �� CTRL ������չ��
// 25 - 28	     ϵͳ����
// 29	         ���ݴ��룺���� WM_KEYDOWN ��Ϣ��˵�����ֵ���� 0
// 30	         ��ǰ״̬������Ϣ����ǰ��������Ǳ����µĻ���ֵ�� 1�������� 0
// 31	         ת��״̬������ WM_KEYDOWN ��Ϣ��˵�����ֵ���� 0
// =============================================================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;        // �豸�������
    PAINTSTRUCT ps;
    TEXTMETRIC tm;
    SCROLLINFO si;
    RECT rect;
    TCHAR szBuffer[128];
    size_t iTarget;                 // �ַ����ĳ���

    static int cxChar, cyChar;      // ��ȡ�ı����
    static int cxClient, cyClient;  // ��ȡ�ͻ������ڸ߶�
    static int cxScreen, cyScreen;  // ��ȡ��Ļ�߶�
    static int iVscrollPos;    

    static int xClient;     // �ͻ����Ŀ��
    static int yClient;     // �ͻ����ĸ߶�
    static int xClientMax;  // �ͻ���������ʾ������ȣ������˿��������������

    static int xChar;       // �����ƽ��ˮƽ���
    static int yChar;       // ����Ĵ�ֱ�߶�
    static int xUpper;      // ����Ĵ�д�ַ���ˮƽ���

    static int xPos;        // ��ǰˮƽ�����������λ��
    static int yPos;        // ��ǰ��ֱ�����������λ��

    int i;                  // ѭ��������
    int x, y;               // ˮƽ�ʹ�ֱ������

    int FirstLine;          // ʧЧ����ĵ�һ�У����ػ�ĵ�һ�У�
    int LastLine;           // ʧЧ��������һ�У������ػ�����һ�У���
    HRESULT hr;
    size_t abcLength;       // ���ڴ�� abc ����ĳ���

    switch (message)
    {
    case WM_CREATE: //���Ƕ�����Ϣ
        hdc = GetDC(hwnd);
        GetTextMetrics(hdc, &tm);
        cxChar = tm.tmAveCharWidth;                     // ��ȡ�ı��ַ���
        cyChar = tm.tmHeight + tm.tmExternalLeading;    // ��ȡ�ı��и�

        xChar = tm.tmAveCharWidth;
        xUpper = (tm.tmPitchAndFamily & 1 ? 3 : 2) * xChar / 2;
        yChar = tm.tmHeight + tm.tmExternalLeading;
        ReleaseDC(hwnd, hdc);

        //SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE); // ����ָ������������С����������λ��
        //SetScrollPos(hwnd, SB_VERT, 0, TRUE);                  // ���ù�����λ��

        // ���ÿͻ���������� 
        // ��������������Ϊ 48 ��Сд�ַ��Ŀ�� + 12 ����д�ַ��Ŀ�ȣ�
        xClientMax = 48 * xChar + 12 * xUpper;
        return 0;

    case WM_PAINT:  // ���л��ƹ���������ӦWM_PAINT��Ϣʱ����
        //hdc = BeginPaint(hwnd, &ps);    // �滭�ĳ�ʼ��
        //GetClientRect(hwnd, &rect);
        //DrawText(hdc, TEXT("This is my first Windows! YES!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);   // ��ʾ��һ���ڣ���ֱ���У�ˮƽ����
        
        //TextOut(hdc, 400, 300, TEXT("���ɶ���"),4); // �ַ�������ģʽȡ���ڵ�ǰ���ı�����ģʽ��ͨ��GetTextAlign�ɻ�ȡ��ͨ��SetTextAlign���޸�
        //EndPaint(hwnd, &ps);

        //for (i = 0; i < 10; i++)
        //{
        //    //wsprintf(szBuffer, TEXT("%2d: %s"), i + 1, TEXT("A String!"));  // ����ȫ���������������
        //    StringCchPrintf(szBuffer, 128, TEXT("%2d: %s"), i + 1, TEXT("A String!"));
        //    StringCchLength(szBuffer, sizeof(szBuffer), &iTarget);
        //    TextOut(hdc, cxChar, i * cyChar, szBuffer, iTarget);
        //}

        hdc = BeginPaint(hwnd, &ps);    // �滭�ĳ�ʼ��
        GetClientRect(hwnd, &rect);
        cxScreen = GetSystemMetrics(SM_CXSCREEN);       // GetSystemMetrics��������Windows�и���ͼ���ͼ�ꡢ���ָ�롢�������͹������ȣ��ĳߴ���Ϣ
        cyScreen = GetSystemMetrics(SM_CYSCREEN);

        // ��ô�ֱ��������λ��
        si.cbSize = sizeof(si);
        si.fMask = SIF_POS;
        GetScrollInfo(hwnd, SB_VERT, &si);
        yPos = si.nPos;

        // ���ˮƽ��������λ��
        GetScrollInfo(hwnd, SB_HORZ, &si);
        xPos = si.nPos;

        // ������Ҫ�ػ������
        FirstLine = max(0, yPos + ps.rcPaint.top / yChar);
        LastLine = min(NUMLINES - 1, yPos + ps.rcPaint.bottom / yChar);

        for (i = FirstLine; i < LastLine; i++)
        {
            x = xChar * (1 - xPos);
            y = yChar * (i - yPos);

            // ʹ�ð�ȫ�ַ�����������ַ�������
            StringCchPrintf(szBuffer, 128, TEXT("�ͻ���ǰ�ķֱ����ǣ�%d * %d px"), cxClient, cyClient);
            hr = StringCchLength(szBuffer, 128, &abcLength);
            if ((FAILED(hr)) || (abcLength == NULL))
            {
                // ��ȡ�ַ�������ʧ�ܵ���ʾ...
                TextOut(hdc, x, y, TEXT(""), 10);
            }

            StringCchPrintf(szBuffer, 10, TEXT("%5d"), i + 1);
            StringCchLength(szBuffer, 10, &iTarget);
            TextOut(hdc, x, y, szBuffer, iTarget);
        }

        //StringCchPrintf(szBuffer, 128, TEXT("��ǰ��ʾ���ķֱ�����:%d * %d px"), cxScreen, cyScreen);
        //DrawText(hdc, szBuffer, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);   // ��ʾ��һ���ڣ���ֱ���У�ˮƽ����

        //StringCchPrintf(szBuffer, 128, TEXT("�ͻ���ǰ�ķֱ����ǣ�%d * %d px"), cxClient, cyClient);
        //StringCchLength(szBuffer, 128, &iTarget);
        //TextOut(hdc, cxChar * 5, 10, szBuffer, iTarget);

        //for (i = 0; i < NUMLINES; i++)
        //{
        //    //y = cyChar * (i - iVscrollPos);                     // ʹ���к��������ƥ��
        //    StringCchPrintf(szBuffer, 10, TEXT("%5d"), i + 1);
        //    StringCchLength(szBuffer, 10, &iTarget);
        //    TextOut(hdc, 0, y, szBuffer, iTarget);
        //}
        return 0;

    //case WM_LBUTTONDOWN:    // WM_LBUTTONUP
    //    MessageBox(hwnd, TEXT("�ͻ���"), TEXT("�����ģ�"), MB_OK);
    //    return 0;
    
    //case WM_NCLBUTTONDOWN:    // WM_NCLBUTTONUP
    //    MessageBox(hwnd, TEXT("�ǿͻ���"), TEXT("�����ģ�"), MB_OK);
    //    return 0;

    case WM_SIZE:
        // ��ÿͻ����ĳߴ� 
        yClient = HIWORD(lParam);
        xClient = LOWORD(lParam);
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);

        // ���ô�ֱ��������Χ��ҳ���С������ҳ���С����������Ĵ�ϸ��
        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE;
        si.nMin = 0;
        si.nMax = NUMLINES - 1;
        si.nPage = yClient / yChar;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

        // ����ˮƽ��������Χ��ҳ���С������ҳ���С����������Ĵ�ϸ�� 
        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE;
        si.nMin = 0;
        si.nMax = 2 + xClientMax / xChar;
        si.nPage = xClient / xChar;
        SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

        return 0;

    case WM_HSCROLL:
        // ���ˮƽ��������������Ϣ
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_HORZ, &si);

        // ���浱ǰ����λ�ã���Щ���бȽ�
        xPos = si.nPos;
        switch (LOWORD(wParam))
        {
            // �û������������ߵ�������
        case SB_LINELEFT:
            si.nPos -= 1;
            break;

            // �û�����������ұߵ�������
        case SB_LINERIGHT:
            si.nPos += 1;
            break;

            // �û����������ߵĹ�������
        case SB_PAGELEFT:
            si.nPos -= si.nPage;
            break;

            // �û���������ұߵĹ�������
        case SB_PAGERIGHT:
            si.nPos += si.nPage;
            break;

            // �û��϶�������
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;

        default:
            break;
        }

        // ���ù������������λ��
        si.fMask = SIF_POS;
        SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

        // ��ù����������λ�ã����ڴ��ڵ����������ܲ���ͬһ��ֵ
        GetScrollInfo(hwnd, SB_HORZ, &si);

        // ���ǰ�ı����ֵ���бȽϣ������ͬ���������
        if (si.nPos != xPos)
        {
            ScrollWindow(hwnd, xChar * (xPos - si.nPos), 0, NULL, NULL);
        }

        return 0;

    case WM_VSCROLL:
        // ��ô�ֱ��������������Ϣ
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);
        
        yPos = si.nPos; // ���浱ǰ����λ�ã���Щ���бȽ�

        switch (LOWORD(wParam))
        {
            // �û�������� Home ����
        case SB_TOP:
            si.nPos = si.nMin;
            break;

            // �û�������� End ����
        case SB_BOTTOM:
            si.nPos = si.nMax;
            break;

            // �û�����������ϱߵ�������
        case SB_LINEUP:
            si.nPos -= 1;
            break;

            // �û�����������±ߵ�������
        case SB_LINEDOWN:
            si.nPos += 1;
            break;

            // �û���������ϱߵĹ�������
        case SB_PAGEUP:
            si.nPos -= si.nPage;
            break;

            // �û���������±ߵĹ�������
        case SB_PAGEDOWN:
            si.nPos += si.nPage;
            break;

            // �û��϶�������
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;

        default:
            break;
        }

        // ���ù������������λ��
        si.fMask = SIF_POS;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

        // ��ù����������λ�ã����ڴ��ڵ����������ܲ���ͬһ��ֵ
        GetScrollInfo(hwnd, SB_VERT, &si);

        // ���ǰ�ı����ֵ���бȽϣ������ͬ���������
        if (si.nPos != yPos)
        {
            ScrollWindow(hwnd, 0, yChar * (yPos - si.nPos), NULL, NULL);
            UpdateWindow(hwnd);
        }

        //switch (LOWORD(wParam))
        //{
        //case SB_LINEUP:     // ���Ϲ���һ��
        //    iVscrollPos -= 1;
        //    break;

        //case SB_LINEDOWN:   // ���¹���һ��
        //    iVscrollPos += 1;
        //    break;

        //case SB_PAGEUP:     // ���Ϲ���һҳ
        //    iVscrollPos -= cyClient / cyChar;
        //    break;

        //case SB_PAGEDOWN:   // ���¹���һҳ
        //    iVscrollPos += cyClient / cyChar;
        //    break;

        //case SB_THUMBTRACK: // ��ץ���Ҳ���
        //    iVscrollPos = HIWORD(wParam);
        //    break;
        //}
        //
        //iVscrollPos = max(0, min(iVscrollPos, NUMLINES - 1));   // ���ù�����λ��
        //if (iVscrollPos != GetScrollPos(hwnd, SB_VERT)) // ��������λ�ò���ʱ
        //{
        //    SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
        //    InvalidateRect(hwnd, NULL, TRUE);       // �������������ػ洰�ڣ�ʹ�ÿͻ���ʧЧ��Windows����Ϣ����Ϊ��ʱ�ŷ���WM_PAINT��Ϣ
        //    UpdateWindow(hwnd);                     // Ϊ��ֹ����æWindowsδ�ܼ�ʱ����WM_PAINT��Ϣ���ֶ�����WM_PAINT��Ϣ
        //}
        return 0;

        // ���û�����A�����ͷţ�CapsLock�ر�״̬��ʱ�����ڻؽ��յ�WM_KEYDOWN(0x41)��WM_CHAR(0x61)��WM_KEYUP(0x41)������Ϣ
        // ���û�����A�����ͷ�ʱ���ظ�����WM_KEYDOWN(0x41)��WM_CHAR(0x61)������Ϣ
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_UP:     // ��Ӧ���̡���
            SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
            break;

        case VK_DOWN:   // ��Ӧ���̡���
            SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
            break;

        case VK_HOME:   // ��Ӧ����HOME��
            SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
            break;

        case VK_END:    // ��Ӧ����END��
            SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
            break;

        case VK_PRIOR:  // ��Ӧ����PgUp��
            SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
            break;

        case VK_NEXT:   // ��Ӧ����PgDn��
            SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
            break;
        }
        return 0;

    case WM_CLOSE:      // ����Ϊ���ڻ�Ӧ�ó���Ӧ��ֹ���ź�
        if (MessageBox(hwnd, TEXT("�Ƿ��˳�"), TEXT("�˳�"), MB_YESNO) == IDYES)
            DestroyWindow(hwnd);
        else
            return 0;

    case WM_DESTROY:    // �����ڱ�����ʱ
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}
