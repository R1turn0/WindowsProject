#include <windows.h>
#include <strsafe.h>

#define NUMLINES 100

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

// wParam: -WM_KEYDOWN:虚拟键代码 / -WM_CHAR:ANSI或Unicode字符码
//lParam：指定重复计数、OEM 扫描码、扩展键标记、内容代码、先前状态和转换状态，具体含义和表示方法如下：
// =============================================================================================
//  位            含义
// 0 - 15        1. 当前消息被重复的次数
//               2. 当用户按下按键不松开时，Windows 会将该键产生的所有 WM_KEYDOWN 消息合称为一个消息，并增加该字段的值
//               3. 重复计数大于 1 表明此时连续击键的速度快于程序的处理能力
// 16 - 23	     OEM 扫描码（键盘硬件产生的代码）
// 24	         1. 指定该按键是否扩展键
//               2. 比如 IBM 加强型键盘有 101 或 102 个键，右边的 ALT 和 CTRL 就是扩展键
// 25 - 28	     系统保留
// 29	         内容代码：对于 WM_KEYDOWN 消息来说，这个值总是 0
// 30	         先前状态：在消息发送前如果按键是被按下的话该值是 1，否则是 0
// 31	         转换状态：对于 WM_KEYDOWN 消息来说，这个值总是 0
// =============================================================================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;        // 设备环境句柄
    PAINTSTRUCT ps;
    TEXTMETRIC tm;
    SCROLLINFO si;
    RECT rect;
    TCHAR szBuffer[128];
    size_t iTarget;                 // 字符串的长度

    static int cxChar, cyChar;      // 获取文本宽高
    static int cxClient, cyClient;  // 获取客户区窗口高度
    static int cxScreen, cyScreen;  // 获取屏幕高度
    static int iVscrollPos;    

    static int xClient;     // 客户区的宽度
    static int yClient;     // 客户区的高度
    static int xClientMax;  // 客户区内容显示的最大宽度（超过此宽度则加入滚动条）

    static int xChar;       // 字体的平均水平宽度
    static int yChar;       // 字体的垂直高度
    static int xUpper;      // 字体的大写字符的水平宽度

    static int xPos;        // 当前水平滚动条滑块的位置
    static int yPos;        // 当前垂直滚动条滑块的位置

    int i;                  // 循环计数器
    int x, y;               // 水平和垂直的坐标

    int FirstLine;          // 失效区域的第一行（需重绘的第一行）
    int LastLine;           // 失效区域的最后一行（（需重绘的最后一行））
    HRESULT hr;
    size_t abcLength;       // 用于存放 abc 数组的长度

    switch (message)
    {
    case WM_CREATE: //？非队列消息
        hdc = GetDC(hwnd);
        GetTextMetrics(hdc, &tm);
        cxChar = tm.tmAveCharWidth;                     // 获取文本字符宽
        cyChar = tm.tmHeight + tm.tmExternalLeading;    // 获取文本行高

        xChar = tm.tmAveCharWidth;
        xUpper = (tm.tmPitchAndFamily & 1 ? 3 : 2) * xChar / 2;
        yChar = tm.tmHeight + tm.tmExternalLeading;
        ReleaseDC(hwnd, hdc);

        //SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE); // 设置指定滚动条的最小和最大滚动框位置
        //SetScrollPos(hwnd, SB_VERT, 0, TRUE);                  // 设置滚动条位置

        // 设置客户区的最大宽度 
        // （我们这里设置为 48 个小写字符的宽度 + 12 个大写字符的宽度）
        xClientMax = 48 * xChar + 12 * xUpper;
        return 0;

    case WM_PAINT:  // 所有绘制工作都在相应WM_PAINT消息时处理
        //hdc = BeginPaint(hwnd, &ps);    // 绘画的初始化
        //GetClientRect(hwnd, &rect);
        //DrawText(hdc, TEXT("This is my first Windows! YES!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);   // 显示在一行内，垂直居中，水平居中
        
        //TextOut(hdc, 400, 300, TEXT("勾股定理"),4); // 字符串对其模式取决于当前的文本对其模式，通过GetTextAlign可获取，通过SetTextAlign可修改
        //EndPaint(hwnd, &ps);

        //for (i = 0; i < 10; i++)
        //{
        //    //wsprintf(szBuffer, TEXT("%2d: %s"), i + 1, TEXT("A String!"));  // 不安全（缓冲区溢出攻击
        //    StringCchPrintf(szBuffer, 128, TEXT("%2d: %s"), i + 1, TEXT("A String!"));
        //    StringCchLength(szBuffer, sizeof(szBuffer), &iTarget);
        //    TextOut(hdc, cxChar, i * cyChar, szBuffer, iTarget);
        //}

        hdc = BeginPaint(hwnd, &ps);    // 绘画的初始化
        GetClientRect(hwnd, &rect);
        cxScreen = GetSystemMetrics(SM_CXSCREEN);       // GetSystemMetrics函数返回Windows中各种图形项（图标、鼠标指针、标题栏和滚动条等）的尺寸信息
        cyScreen = GetSystemMetrics(SM_CYSCREEN);

        // 获得垂直滚动条的位置
        si.cbSize = sizeof(si);
        si.fMask = SIF_POS;
        GetScrollInfo(hwnd, SB_VERT, &si);
        yPos = si.nPos;

        // 获得水平滚动条的位置
        GetScrollInfo(hwnd, SB_HORZ, &si);
        xPos = si.nPos;

        // 计算需要重绘的区域
        FirstLine = max(0, yPos + ps.rcPaint.top / yChar);
        LastLine = min(NUMLINES - 1, yPos + ps.rcPaint.bottom / yChar);

        for (i = FirstLine; i < LastLine; i++)
        {
            x = xChar * (1 - xPos);
            y = yChar * (i - yPos);

            // 使用安全字符串函数获得字符串长度
            StringCchPrintf(szBuffer, 128, TEXT("客户当前的分辨率是：%d * %d px"), cxClient, cyClient);
            hr = StringCchLength(szBuffer, 128, &abcLength);
            if ((FAILED(hr)) || (abcLength == NULL))
            {
                // 获取字符串长度失败的提示...
                TextOut(hdc, x, y, TEXT(""), 10);
            }

            StringCchPrintf(szBuffer, 10, TEXT("%5d"), i + 1);
            StringCchLength(szBuffer, 10, &iTarget);
            TextOut(hdc, x, y, szBuffer, iTarget);
        }

        //StringCchPrintf(szBuffer, 128, TEXT("当前显示器的分辨率是:%d * %d px"), cxScreen, cyScreen);
        //DrawText(hdc, szBuffer, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);   // 显示在一行内，垂直居中，水平居中

        //StringCchPrintf(szBuffer, 128, TEXT("客户当前的分辨率是：%d * %d px"), cxClient, cyClient);
        //StringCchLength(szBuffer, 128, &iTarget);
        //TextOut(hdc, cxChar * 5, 10, szBuffer, iTarget);

        //for (i = 0; i < NUMLINES; i++)
        //{
        //    //y = cyChar * (i - iVscrollPos);                     // 使得行号与滚动条匹配
        //    StringCchPrintf(szBuffer, 10, TEXT("%5d"), i + 1);
        //    StringCchLength(szBuffer, 10, &iTarget);
        //    TextOut(hdc, 0, y, szBuffer, iTarget);
        //}
        return 0;

    //case WM_LBUTTONDOWN:    // WM_LBUTTONUP
    //    MessageBox(hwnd, TEXT("客户区"), TEXT("这是哪？"), MB_OK);
    //    return 0;
    
    //case WM_NCLBUTTONDOWN:    // WM_NCLBUTTONUP
    //    MessageBox(hwnd, TEXT("非客户区"), TEXT("这是哪？"), MB_OK);
    //    return 0;

    case WM_SIZE:
        // 获得客户区的尺寸 
        yClient = HIWORD(lParam);
        xClient = LOWORD(lParam);
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);

        // 设置垂直滚动条范围和页面大小（设置页面大小将决定滑块的粗细）
        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE;
        si.nMin = 0;
        si.nMax = NUMLINES - 1;
        si.nPage = yClient / yChar;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

        // 设置水平滚动条范围和页面大小（设置页面大小将决定滑块的粗细） 
        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE;
        si.nMin = 0;
        si.nMax = 2 + xClientMax / xChar;
        si.nPage = xClient / xChar;
        SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

        return 0;

    case WM_HSCROLL:
        // 获得水平滚动条的所有信息
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_HORZ, &si);

        // 保存当前滑块位置，迟些进行比较
        xPos = si.nPos;
        switch (LOWORD(wParam))
        {
            // 用户点击滚动条左边的三角形
        case SB_LINELEFT:
            si.nPos -= 1;
            break;

            // 用户点击滚动条右边的三角形
        case SB_LINERIGHT:
            si.nPos += 1;
            break;

            // 用户点击滑块左边的滚动条轴
        case SB_PAGELEFT:
            si.nPos -= si.nPage;
            break;

            // 用户点击滑块右边的滚动条轴
        case SB_PAGERIGHT:
            si.nPos += si.nPage;
            break;

            // 用户拖动滚动条
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;

        default:
            break;
        }

        // 设置滚动条滑块的新位置
        si.fMask = SIF_POS;
        SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

        // 获得滚动条滑块的位置，由于窗口调整，它可能不是同一个值
        GetScrollInfo(hwnd, SB_HORZ, &si);

        // 与此前的保存的值进行比较，如果不同则滚动窗口
        if (si.nPos != xPos)
        {
            ScrollWindow(hwnd, xChar * (xPos - si.nPos), 0, NULL, NULL);
        }

        return 0;

    case WM_VSCROLL:
        // 获得垂直滚动条的所有信息
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);
        
        yPos = si.nPos; // 保存当前滑块位置，迟些进行比较

        switch (LOWORD(wParam))
        {
            // 用户点击键盘 Home 按键
        case SB_TOP:
            si.nPos = si.nMin;
            break;

            // 用户点击键盘 End 按键
        case SB_BOTTOM:
            si.nPos = si.nMax;
            break;

            // 用户点击滚动条上边的三角形
        case SB_LINEUP:
            si.nPos -= 1;
            break;

            // 用户点击滚动条下边的三角形
        case SB_LINEDOWN:
            si.nPos += 1;
            break;

            // 用户点击滑块上边的滚动条轴
        case SB_PAGEUP:
            si.nPos -= si.nPage;
            break;

            // 用户点击滑块下边的滚动条轴
        case SB_PAGEDOWN:
            si.nPos += si.nPage;
            break;

            // 用户拖动滚动条
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;

        default:
            break;
        }

        // 设置滚动条滑块的新位置
        si.fMask = SIF_POS;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

        // 获得滚动条滑块的位置，由于窗口调整，它可能不是同一个值
        GetScrollInfo(hwnd, SB_VERT, &si);

        // 与此前的保存的值进行比较，如果不同则滚动窗口
        if (si.nPos != yPos)
        {
            ScrollWindow(hwnd, 0, yChar * (yPos - si.nPos), NULL, NULL);
            UpdateWindow(hwnd);
        }

        //switch (LOWORD(wParam))
        //{
        //case SB_LINEUP:     // 向上滚动一行
        //    iVscrollPos -= 1;
        //    break;

        //case SB_LINEDOWN:   // 向下滚动一行
        //    iVscrollPos += 1;
        //    break;

        //case SB_PAGEUP:     // 向上滚动一页
        //    iVscrollPos -= cyClient / cyChar;
        //    break;

        //case SB_PAGEDOWN:   // 向下滚动一页
        //    iVscrollPos += cyClient / cyChar;
        //    break;

        //case SB_THUMBTRACK: // 别抓着我不放
        //    iVscrollPos = HIWORD(wParam);
        //    break;
        //}
        //
        //iVscrollPos = max(0, min(iVscrollPos, NUMLINES - 1));   // 设置滚动条位置
        //if (iVscrollPos != GetScrollPos(hwnd, SB_VERT)) // 当滚动条位置不变时
        //{
        //    SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
        //    InvalidateRect(hwnd, NULL, TRUE);       // 滑动滚动条后重绘窗口，使得客户区失效，Windows在消息队列为空时才发送WM_PAINT消息
        //    UpdateWindow(hwnd);                     // 为防止程序繁忙Windows未能及时发送WM_PAINT消息，手动发送WM_PAINT消息
        //}
        return 0;

        // 当用户按下A键并释放（CapsLock关闭状态）时，窗口回接收到WM_KEYDOWN(0x41)、WM_CHAR(0x61)、WM_KEYUP(0x41)三个消息
        // 当用户按下A键不释放时将重复发送WM_KEYDOWN(0x41)、WM_CHAR(0x61)两个消息
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_UP:     // 响应键盘↑键
            SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
            break;

        case VK_DOWN:   // 响应键盘↓键
            SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
            break;

        case VK_HOME:   // 响应键盘HOME键
            SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
            break;

        case VK_END:    // 响应键盘END键
            SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
            break;

        case VK_PRIOR:  // 响应键盘PgUp键
            SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
            break;

        case VK_NEXT:   // 响应键盘PgDn键
            SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
            break;
        }
        return 0;

    case WM_CLOSE:      // 发送为窗口或应用程序应终止的信号
        if (MessageBox(hwnd, TEXT("是否退出"), TEXT("退出"), MB_YESNO) == IDYES)
            DestroyWindow(hwnd);
        else
            return 0;

    case WM_DESTROY:    // 当窗口被销毁时
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}
