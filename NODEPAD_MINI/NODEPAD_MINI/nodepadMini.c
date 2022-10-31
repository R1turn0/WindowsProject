#include <windows.h>
#include "resource.h"


#define EDITID   1
#define UNTITLED TEXT ("(记事本)")


LRESULT CALLBACK WndProc  (HWND, UINT, WPARAM, LPARAM) ;
BOOL    CALLBACK AboutDlgProc (HWND, UINT, WPARAM, LPARAM) ;


BOOL NodepanFileOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
short AskAboutSave (HWND hwnd, TCHAR * szTitleName);
void DoCaption (HWND hwnd, TCHAR * szTitleName);


void DisplayMyWnd( HWND hWnd );
void doMessage();
HWND CreateMyWindow( LPCTSTR lpClassName, HINSTANCE hInstance );
void OkMessage (HWND hwnd, TCHAR * szMessage, TCHAR * szTitleName);
//
// 定义在nodeFile.c里
void NodepadeFileInitialize (HWND) ;
BOOL NodepadFileRead (HWND, PTSTR) ;
BOOL NodepadFileWrite (HWND hwndEdit, PTSTR pstrFileName);
BOOL NodepadeFileOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
BOOL NodepadeFileSaveDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);


// 定义在nodepadePrnt.c里
BOOL NodepadePrntPrintFile (HINSTANCE hInst, HWND hwnd, HWND hwndEdit,  PTSTR szTitleName);

// 定义在nodepadeFont.c里

void NodepadeFontInitialize   (HWND) ;
BOOL NodepadeFontChooseFont (HWND hwnd);
void NodepadeFontSetFont (HWND hwndEdit);



// 全局变量
static HWND  hDlgModeless ;
//可以给菜单指定一个和程序名相同的字符串ID，这样很方便我们调用菜单，
static  TCHAR lpClassName[] = TEXT("NODEPAD_MINI");  

// 1、 入口函数
int WINAPI WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,int nShowCmd ){

    HWND hWnd = NULL;
    MSG       msg ;//消息结构
    HACCEL    hAccel ;

    // 注册窗口
    RegisterWindow(lpClassName, hInstance );

    // 创建窗口
    hWnd = CreateMyWindow(lpClassName, hInstance );

    if( NULL == hWnd )
    {
        MessageBox( NULL, TEXT("创建失败Create Window Failed"),
        TEXT("Error"), MB_OK|MB_ICONERROR );
        return NULL;
    }
    else
    {
       // MessageBox( NULL, TEXT("创建成功Create Window Successed"), TEXT("Sucessed"), MB_OK );
    }

    //显示窗口
    DisplayMyWnd(hWnd);
    //消息循环
    // doMessage();
    hAccel = LoadAccelerators (hInstance, lpClassName) ;

    //【消息循环】准备读入用户用键盘和鼠标输入的数据，windows程序维护一个消息队列，在发生输入事件之后，windows将事件转换为一个消息，并将消息放入程序的消息队列中，程序通过执行一块被称为“消息循环”的代码从消息队列中取出消息
    while (GetMessage (&msg, NULL, 0, 0))
    {
        if (hDlgModeless == NULL || !IsDialogMessage (hDlgModeless, &msg))
        {
            if (!TranslateAccelerator (hWnd, hAccel, &msg))
            {
                TranslateMessage (&msg) ;
                DispatchMessage (&msg) ;
            }
        }
    }

    return msg.wParam ;
}  


// 2、窗口处理函数，“关于”消息框的处理程序。当窗口处理消息事件时,系统调用该函数
/*
实际的动作发生在窗口过程中，一个Windows程序可以包含多个窗口过程，
// 1、hwnd 窗口的句柄
// 2、消息的ID号
// 3、消息所对应的参数
// 4、消息所对应的参数
*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    static BOOL      bNeedSave = FALSE ;
    static HINSTANCE hInst ;
    static HWND      hwndEdit ;
    static TCHAR     szFileName[MAX_PATH], szTitleName[MAX_PATH] ;
    int              iSelBeg, iSelEnd, iEnable ;

    switch (message){
        /*
        当应用程序通过调用CreateWindow功能请求创建窗口时，WM_CREATE消息被发送。新窗口的窗口过程在窗口创建后，
        但在窗口可见之前收到此消息。消息在    CreateWindowEx或CreateWindow函数返回之前发送
        */

    case WM_CREATE:
         hInst = ((LPCREATESTRUCT) lParam) -> hInstance ;

        //创建编辑框 根据窗口类创建窗口 ES_MULTILINE多行文本编辑框
        //hwnd 父类句柄  创建 - 窗口类名称 EDIT
        hwndEdit = CreateWindow (TEXT ("EDIT"), TEXT ("**我是编辑框 请在这里输入内容**"),
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
            WS_BORDER | ES_LEFT | ES_MULTILINE |
            ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
            0,10,1000,500, hwnd, (HMENU) EDITID, hInst, NULL) ;


        //将指定的消息发送到窗口或窗口。该函数调用指定窗口的窗口过程，并且在窗口过程处理该消息之前不返回
        SendMessage (hwndEdit, EM_LIMITTEXT, 32000, 0L) ;

        // 初始化通用对话框的内容

        NodepadeFileInitialize (hwnd) ;
        NodepadeFontInitialize (hwndEdit) ;
        DoCaption (hwnd, szTitleName) ;
        return 0 ;

        //就是为了编辑框获取焦点后产生的消息
    case WM_SETFOCUS:

        SetFocus (hwndEdit) ;
        return 0 ;

        //窗口的大小发生变化后，会收到这个消息。

    case WM_SIZE: 
        MoveWindow (hwndEdit, 0, 0, LOWORD (lParam), HIWORD (lParam), TRUE) ;
        return 0 ;


        /*
        当用户在控件中执行操作时，控件可以向对话框过程发送WM_COMMAND消息。这些消息称为通知消息，通知用户输入的过程，并允许其执行适当的响应。
        */
    case WM_COMMAND:

        //获取编辑控件的消息

        if (lParam && LOWORD (wParam) == EDITID)
        {
            switch (HIWORD (wParam))
            {
            case EN_UPDATE :
                bNeedSave = TRUE ;
                return 0 ;

                /*
                当编辑控件无法分配足够的内存以满足特定请求时，将发送EN_ERRSPACE通知消息。编辑控件的父窗口通过WM_COMMAND消息接收该通知消息
                */
            case EN_ERRSPACE :

                /*
                当前文本插入超过编辑控件的指定字符数时，将发送EN_MAXTEXT通知消息。文本插入已被截断。
                */
            case EN_MAXTEXT :
                MessageBox (hwnd, TEXT ("编辑控件内存不足。"),
                    lpClassName, MB_OK | MB_ICONSTOP) ;
                return 0 ;
            }
            break ;
        }


        switch (LOWORD (wParam)){

            //点击New创建新文件
        case IDM_FILE_NEW:

           // MessageBox( NULL, TEXT("IDM_FILE_NEW"),TEXT("IDM_FILE_NEW"), MB_OKCANCEL );
            if (bNeedSave && IDCANCEL == AskAboutSave (hwnd, szTitleName)){
                return 0 ;
            }

            SetWindowText (hwndEdit, TEXT ("\0")) ;
            szFileName[0]  = '\0' ;
            szTitleName[0] = '\0' ;
            DoCaption (hwnd, szTitleName) ;
            bNeedSave = FALSE ;
            return 0 ;


            //点击Open打开文件
        case IDM_FILE_OPEN:
           
            if (bNeedSave && IDCANCEL == AskAboutSave (hwnd, szTitleName)) return 0 ;

            if (NodepanFileOpenDlg (hwnd, szFileName, szTitleName))
            {
                if (!NodepadFileRead (hwndEdit, szFileName))
                {
                    OkMessage (hwnd, TEXT ("无法读取文件"), szTitleName) ;
                    szFileName[0]  = '\0' ;
                    szTitleName[0] = '\0' ;
                }
            }

            DoCaption (hwnd, szTitleName) ;
            bNeedSave = FALSE ;
            return 0 ;



            //点击保存文件
        case IDM_FILE_SAVE:
           
            if (szFileName[0])
            {
                if (NodepadFileWrite (hwndEdit, szFileName))
                {
                    bNeedSave = FALSE ;
                    return 1 ;
                }
                else
                {
                    OkMessage (hwnd, TEXT ("无法写入文件 %s"),
                        szTitleName) ;
                    return 0 ;
                }
            }


            //点击另存为
        case IDM_FILE_SAVE_AS:
           
            if (NodepadeFileSaveDlg (hwnd, szFileName, szTitleName))
            {
                DoCaption (hwnd, szTitleName) ;

                if (NodepadFileWrite (hwndEdit, szFileName))
                {
                    bNeedSave = FALSE ;
                    return 1 ;
                }
                else
                {
                    OkMessage (hwnd, TEXT ("无法写入文件 %s"),
                        szTitleName) ;
                    return 0 ;
                }
            }
            return 0 ;



            //点击打印
        case IDM_FILE_PRINT:
           // MessageBox( NULL, TEXT("IDM_FILE_PRINT"),TEXT("IDM_FILE_PRINT"), MB_OKCANCEL );
            if (!NodepadePrntPrintFile (hInst, hwnd, hwndEdit, szTitleName))
                OkMessage (hwnd, TEXT ("无法打印文件 %s"),
                szTitleName) ;
            return 0 ;


            //点击退出
        case IDM_APP_EXIT:
            SendMessage (hwnd, WM_CLOSE, 0, 0) ;
            return 0 ;


            //点击编辑字体格式
        case IDM_FORMAT_FONT:
            if (NodepadeFontChooseFont (hwnd))
                //函数创建具有指定结构中指定的特征的逻辑字体。该字体随后可以被选择为任何设备上下文的当前字体。
                NodepadeFontSetFont (hwndEdit) ;

            return 0 ;

        }

        break ;


        //关闭应用程序窗口
    case WM_CLOSE:
        if (!bNeedSave || IDCANCEL != AskAboutSave (hwnd, szTitleName))
            DestroyWindow (hwnd) ;

        return 0 ;


        //当结束对话或者一个程序调用系统关闭功能的时候，WM_QUERYENDSESSION消息会被发送给尚未终止的所有窗口。
    case WM_QUERYENDSESSION :
        if (!bNeedSave || IDCANCEL != AskAboutSave (hwnd, szTitleName))
            return 1 ;

        return 0 ;



        //3.7程序退出
        // 窗口销毁时，退出程序。在WndProc中实现
        //当窗口销毁时，会调用WndProc
    case WM_DESTROY:
        MessageBox( NULL, TEXT("消息销毁"),TEXT("窗口销毁，程序退出"), MB_OK );
        //传递给使用者。
        //发送一个退出消息
        NodepadeFontDeinitialize () ;
        PostQuitMessage( 0 ); 

        MessageBox( NULL, TEXT("消息销毁啦"),TEXT("消息销毁啦"), MB_OK );
        return 0;

    }
    // 其他的消息调用缺省的消息处理程序
    return DefWindowProc(hwnd, message, wParam, lParam);
}


// 3、注册窗口类型
BOOL RegisterWindow( LPCSTR lpcWndName, HINSTANCE hInstance ){

    ATOM nAtom = 0;

    // 构造创建窗口参数
    WNDCLASS wndClass = { 0 };

    wndClass.style = CS_HREDRAW | CS_VREDRAW;//窗口类的风格
    wndClass.lpfnWndProc = WndProc;      // 指向窗口过程函数 //窗口处理函数
    wndClass.cbClsExtra = 0;; //窗口类附加数据大小
    wndClass.cbWndExtra = 0;//窗口附加数据大小
    wndClass.hInstance = hInstance;//当前应用程序的实例句柄  //当前应用程序的实例句柄
    wndClass.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( 100 ) );// 100为在HelloWorld.rc文件中定义的ID号 //窗口图标
    wndClass.hCursor =  LoadCursor (NULL, IDC_ARROW) ;//窗口图标
    wndClass.hbrBackground =(HBRUSH) GetStockObject (WHITE_BRUSH) ;//窗口的背景画刷 
    wndClass.lpszMenuName = lpcWndName; //菜单
    wndClass.lpszClassName = lpcWndName;    // 注册的窗口名称，并非标题，以后创建窗口根据此注册的名称创建 //类型名称


    //下面是窗口类的注册
    if (!RegisterClass (&wndClass))
    {
        MessageBox (NULL, TEXT ("This program requires Windows NT!"),
            lpcWndName, MB_ICONERROR) ;
        return 0 ;
    }

}

// 4、创建窗口（lpClassName 一定是已经注册过的窗口类型）
//lpClassName 窗口类型名称
HWND CreateMyWindow( LPCTSTR lpClassName, HINSTANCE hInstance )
{
    HWND hWnd = NULL;
    // 创建窗口  
    //1、lpClassName 窗口类型名称
    //2、窗口名称,显示在窗口左上角
    //3、WS_OVERLAPPEDWINDOW 窗口类型，就是这个窗口的风格
    //4、窗口的左上角X坐边 left 纵坐标距离屏幕的间距
    //5、窗口的左上角Y坐边 shang 横坐标距离屏幕的间距
    //6、窗口的宽度
    //7、窗口的高度
    //8、父窗口句柄
    //9、窗口菜单句柄
    //10、hInstance 应用程序的实例句柄
    //11、创建的参数，一般为NULL
    hWnd = CreateWindow(lpClassName, TEXT("Hello World"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL );
    if( NULL == hWnd )
    {
        MessageBox( NULL, TEXT("Create Window Failed"),
            TEXT("Error"), MB_OK|MB_ICONERROR );
        return NULL;
    }
    else
    {
       // MessageBox( NULL, TEXT("Create Window Successed"),TEXT("Sucessed"), MB_OK );
    }
    return hWnd;
}


// 5、 显示窗口
void DisplayMyWnd( HWND hWnd )
{
    //显示窗口
    //1、显示的窗口句柄
    //2、显示的方式
    ShowWindow( hWnd, SW_SHOW);
    //刷新窗口
    UpdateWindow(hWnd );
}



// 6、消息循环处理函数
//void doMessage()        
//{
//
//    HWND      hwnd ;//窗口句柄
//    MSG       msg ;//消息结构
//    HACCEL    hAccel ;
//
//    MSG msg = { 0 };
//    // 获取消息 
//    // 当接收到WM_QIUT消息时，GetMessage函数返回0，结束循环
//    while( GetMessage( &msg, NULL, 0, 0 ) ) 
//    {
//
//        if (hDlgModeless == NULL || !IsDialogMessage (hDlgModeless, &msg))
//        {
//            if (!TranslateAccelerator (hwnd, hAccel, &msg))
//            {
//                TranslateMessage (&msg) ;
//                DispatchMessage (&msg) ;
//            }
//        }
//
//        // 派发消息，到WindowPro函数处理
//        //DispatchMessage( &msg );
//    }
//}





//窗口的标题显示 例如：窗口名称-副标题  mywin-subTixt
void DoCaption (HWND hwnd, TCHAR * szTitleName)
{
    TCHAR szCaption[1 + MAX_PATH] ;

    wsprintf (szCaption, TEXT ("%s - %s"), lpClassName,
        szTitleName[0] ? szTitleName : UNTITLED) ;

    SetWindowText (hwnd, szCaption) ;
}


//点New 创建新文件时 保存现有的文件内容
short AskAboutSave (HWND hwnd, TCHAR * szTitleName)
{
    TCHAR szBuffer[64 + MAX_PATH] ;
    int   iReturn ;

    wsprintf (szBuffer, TEXT ("保存当前更改 %s?"),
        szTitleName[0] ? szTitleName : UNTITLED) ;

    iReturn = MessageBox (hwnd, szBuffer, lpClassName,
        MB_YESNOCANCEL | MB_ICONQUESTION) ;

    if (iReturn == IDYES)
        if (!SendMessage (hwnd, WM_COMMAND, IDM_FILE_SAVE, 0))
            iReturn = IDCANCEL ;

    return iReturn ;
}
static OPENFILENAME ofn ;



//提示消息
void OkMessage (HWND hwnd, TCHAR * szMessage, TCHAR * szTitleName)
{
    TCHAR szBuffer[64 + MAX_PATH] ;

    wsprintf (szBuffer, szMessage, szTitleName[0] ? szTitleName : UNTITLED) ;

    MessageBox (hwnd, szBuffer, lpClassName, MB_OK | MB_ICONEXCLAMATION) ;
}


BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE ;

    case WM_COMMAND:
        switch (LOWORD (wParam))
        {

        }
        break ;
    }
    return FALSE ;
}