#include <windows.h>
#include "resource.h"


#define EDITID   1
#define UNTITLED TEXT ("(���±�)")


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
// ������nodeFile.c��
void NodepadeFileInitialize (HWND) ;
BOOL NodepadFileRead (HWND, PTSTR) ;
BOOL NodepadFileWrite (HWND hwndEdit, PTSTR pstrFileName);
BOOL NodepadeFileOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
BOOL NodepadeFileSaveDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);


// ������nodepadePrnt.c��
BOOL NodepadePrntPrintFile (HINSTANCE hInst, HWND hwnd, HWND hwndEdit,  PTSTR szTitleName);

// ������nodepadeFont.c��

void NodepadeFontInitialize   (HWND) ;
BOOL NodepadeFontChooseFont (HWND hwnd);
void NodepadeFontSetFont (HWND hwndEdit);



// ȫ�ֱ���
static HWND  hDlgModeless ;
//���Ը��˵�ָ��һ���ͳ�������ͬ���ַ���ID�������ܷ������ǵ��ò˵���
static  TCHAR lpClassName[] = TEXT("NODEPAD_MINI");  

// 1�� ��ں���
int WINAPI WinMain( HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,int nShowCmd ){

    HWND hWnd = NULL;
    MSG       msg ;//��Ϣ�ṹ
    HACCEL    hAccel ;

    // ע�ᴰ��
    RegisterWindow(lpClassName, hInstance );

    // ��������
    hWnd = CreateMyWindow(lpClassName, hInstance );

    if( NULL == hWnd )
    {
        MessageBox( NULL, TEXT("����ʧ��Create Window Failed"),
        TEXT("Error"), MB_OK|MB_ICONERROR );
        return NULL;
    }
    else
    {
       // MessageBox( NULL, TEXT("�����ɹ�Create Window Successed"), TEXT("Sucessed"), MB_OK );
    }

    //��ʾ����
    DisplayMyWnd(hWnd);
    //��Ϣѭ��
    // doMessage();
    hAccel = LoadAccelerators (hInstance, lpClassName) ;

    //����Ϣѭ����׼�������û��ü��̺������������ݣ�windows����ά��һ����Ϣ���У��ڷ��������¼�֮��windows���¼�ת��Ϊһ����Ϣ��������Ϣ����������Ϣ�����У�����ͨ��ִ��һ�鱻��Ϊ����Ϣѭ�����Ĵ������Ϣ������ȡ����Ϣ
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


// 2�����ڴ������������ڡ���Ϣ��Ĵ�����򡣵����ڴ�����Ϣ�¼�ʱ,ϵͳ���øú���
/*
ʵ�ʵĶ��������ڴ��ڹ����У�һ��Windows������԰���������ڹ��̣�
// 1��hwnd ���ڵľ��
// 2����Ϣ��ID��
// 3����Ϣ����Ӧ�Ĳ���
// 4����Ϣ����Ӧ�Ĳ���
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
        ��Ӧ�ó���ͨ������CreateWindow�������󴴽�����ʱ��WM_CREATE��Ϣ�����͡��´��ڵĴ��ڹ����ڴ��ڴ�����
        ���ڴ��ڿɼ�֮ǰ�յ�����Ϣ����Ϣ��    CreateWindowEx��CreateWindow��������֮ǰ����
        */

    case WM_CREATE:
         hInst = ((LPCREATESTRUCT) lParam) -> hInstance ;

        //�����༭�� ���ݴ����ഴ������ ES_MULTILINE�����ı��༭��
        //hwnd ������  ���� - ���������� EDIT
        hwndEdit = CreateWindow (TEXT ("EDIT"), TEXT ("**���Ǳ༭�� ����������������**"),
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
            WS_BORDER | ES_LEFT | ES_MULTILINE |
            ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
            0,10,1000,500, hwnd, (HMENU) EDITID, hInst, NULL) ;


        //��ָ������Ϣ���͵����ڻ򴰿ڡ��ú�������ָ�����ڵĴ��ڹ��̣������ڴ��ڹ��̴������Ϣ֮ǰ������
        SendMessage (hwndEdit, EM_LIMITTEXT, 32000, 0L) ;

        // ��ʼ��ͨ�öԻ��������

        NodepadeFileInitialize (hwnd) ;
        NodepadeFontInitialize (hwndEdit) ;
        DoCaption (hwnd, szTitleName) ;
        return 0 ;

        //����Ϊ�˱༭���ȡ������������Ϣ
    case WM_SETFOCUS:

        SetFocus (hwndEdit) ;
        return 0 ;

        //���ڵĴ�С�����仯�󣬻��յ������Ϣ��

    case WM_SIZE: 
        MoveWindow (hwndEdit, 0, 0, LOWORD (lParam), HIWORD (lParam), TRUE) ;
        return 0 ;


        /*
        ���û��ڿؼ���ִ�в���ʱ���ؼ�������Ի�����̷���WM_COMMAND��Ϣ����Щ��Ϣ��Ϊ֪ͨ��Ϣ��֪ͨ�û�����Ĺ��̣���������ִ���ʵ�����Ӧ��
        */
    case WM_COMMAND:

        //��ȡ�༭�ؼ�����Ϣ

        if (lParam && LOWORD (wParam) == EDITID)
        {
            switch (HIWORD (wParam))
            {
            case EN_UPDATE :
                bNeedSave = TRUE ;
                return 0 ;

                /*
                ���༭�ؼ��޷������㹻���ڴ��������ض�����ʱ��������EN_ERRSPACE֪ͨ��Ϣ���༭�ؼ��ĸ�����ͨ��WM_COMMAND��Ϣ���ո�֪ͨ��Ϣ
                */
            case EN_ERRSPACE :

                /*
                ��ǰ�ı����볬���༭�ؼ���ָ���ַ���ʱ��������EN_MAXTEXT֪ͨ��Ϣ���ı������ѱ��ضϡ�
                */
            case EN_MAXTEXT :
                MessageBox (hwnd, TEXT ("�༭�ؼ��ڴ治�㡣"),
                    lpClassName, MB_OK | MB_ICONSTOP) ;
                return 0 ;
            }
            break ;
        }


        switch (LOWORD (wParam)){

            //���New�������ļ�
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


            //���Open���ļ�
        case IDM_FILE_OPEN:
           
            if (bNeedSave && IDCANCEL == AskAboutSave (hwnd, szTitleName)) return 0 ;

            if (NodepanFileOpenDlg (hwnd, szFileName, szTitleName))
            {
                if (!NodepadFileRead (hwndEdit, szFileName))
                {
                    OkMessage (hwnd, TEXT ("�޷���ȡ�ļ�"), szTitleName) ;
                    szFileName[0]  = '\0' ;
                    szTitleName[0] = '\0' ;
                }
            }

            DoCaption (hwnd, szTitleName) ;
            bNeedSave = FALSE ;
            return 0 ;



            //��������ļ�
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
                    OkMessage (hwnd, TEXT ("�޷�д���ļ� %s"),
                        szTitleName) ;
                    return 0 ;
                }
            }


            //������Ϊ
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
                    OkMessage (hwnd, TEXT ("�޷�д���ļ� %s"),
                        szTitleName) ;
                    return 0 ;
                }
            }
            return 0 ;



            //�����ӡ
        case IDM_FILE_PRINT:
           // MessageBox( NULL, TEXT("IDM_FILE_PRINT"),TEXT("IDM_FILE_PRINT"), MB_OKCANCEL );
            if (!NodepadePrntPrintFile (hInst, hwnd, hwndEdit, szTitleName))
                OkMessage (hwnd, TEXT ("�޷���ӡ�ļ� %s"),
                szTitleName) ;
            return 0 ;


            //����˳�
        case IDM_APP_EXIT:
            SendMessage (hwnd, WM_CLOSE, 0, 0) ;
            return 0 ;


            //����༭�����ʽ
        case IDM_FORMAT_FONT:
            if (NodepadeFontChooseFont (hwnd))
                //������������ָ���ṹ��ָ�����������߼����塣�����������Ա�ѡ��Ϊ�κ��豸�����ĵĵ�ǰ���塣
                NodepadeFontSetFont (hwndEdit) ;

            return 0 ;

        }

        break ;


        //�ر�Ӧ�ó��򴰿�
    case WM_CLOSE:
        if (!bNeedSave || IDCANCEL != AskAboutSave (hwnd, szTitleName))
            DestroyWindow (hwnd) ;

        return 0 ;


        //�������Ի�����һ���������ϵͳ�رչ��ܵ�ʱ��WM_QUERYENDSESSION��Ϣ�ᱻ���͸���δ��ֹ�����д��ڡ�
    case WM_QUERYENDSESSION :
        if (!bNeedSave || IDCANCEL != AskAboutSave (hwnd, szTitleName))
            return 1 ;

        return 0 ;



        //3.7�����˳�
        // ��������ʱ���˳�������WndProc��ʵ��
        //����������ʱ�������WndProc
    case WM_DESTROY:
        MessageBox( NULL, TEXT("��Ϣ����"),TEXT("�������٣������˳�"), MB_OK );
        //���ݸ�ʹ���ߡ�
        //����һ���˳���Ϣ
        NodepadeFontDeinitialize () ;
        PostQuitMessage( 0 ); 

        MessageBox( NULL, TEXT("��Ϣ������"),TEXT("��Ϣ������"), MB_OK );
        return 0;

    }
    // ��������Ϣ����ȱʡ����Ϣ�������
    return DefWindowProc(hwnd, message, wParam, lParam);
}


// 3��ע�ᴰ������
BOOL RegisterWindow( LPCSTR lpcWndName, HINSTANCE hInstance ){

    ATOM nAtom = 0;

    // ���촴�����ڲ���
    WNDCLASS wndClass = { 0 };

    wndClass.style = CS_HREDRAW | CS_VREDRAW;//������ķ��
    wndClass.lpfnWndProc = WndProc;      // ָ�򴰿ڹ��̺��� //���ڴ�����
    wndClass.cbClsExtra = 0;; //�����฽�����ݴ�С
    wndClass.cbWndExtra = 0;//���ڸ������ݴ�С
    wndClass.hInstance = hInstance;//��ǰӦ�ó����ʵ�����  //��ǰӦ�ó����ʵ�����
    wndClass.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( 100 ) );// 100Ϊ��HelloWorld.rc�ļ��ж����ID�� //����ͼ��
    wndClass.hCursor =  LoadCursor (NULL, IDC_ARROW) ;//����ͼ��
    wndClass.hbrBackground =(HBRUSH) GetStockObject (WHITE_BRUSH) ;//���ڵı�����ˢ 
    wndClass.lpszMenuName = lpcWndName; //�˵�
    wndClass.lpszClassName = lpcWndName;    // ע��Ĵ������ƣ����Ǳ��⣬�Ժ󴴽����ڸ��ݴ�ע������ƴ��� //��������


    //�����Ǵ������ע��
    if (!RegisterClass (&wndClass))
    {
        MessageBox (NULL, TEXT ("This program requires Windows NT!"),
            lpcWndName, MB_ICONERROR) ;
        return 0 ;
    }

}

// 4���������ڣ�lpClassName һ�����Ѿ�ע����Ĵ������ͣ�
//lpClassName ������������
HWND CreateMyWindow( LPCTSTR lpClassName, HINSTANCE hInstance )
{
    HWND hWnd = NULL;
    // ��������  
    //1��lpClassName ������������
    //2����������,��ʾ�ڴ������Ͻ�
    //3��WS_OVERLAPPEDWINDOW �������ͣ�����������ڵķ��
    //4�����ڵ����Ͻ�X���� left �����������Ļ�ļ��
    //5�����ڵ����Ͻ�Y���� shang �����������Ļ�ļ��
    //6�����ڵĿ��
    //7�����ڵĸ߶�
    //8�������ھ��
    //9�����ڲ˵����
    //10��hInstance Ӧ�ó����ʵ�����
    //11�������Ĳ�����һ��ΪNULL
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


// 5�� ��ʾ����
void DisplayMyWnd( HWND hWnd )
{
    //��ʾ����
    //1����ʾ�Ĵ��ھ��
    //2����ʾ�ķ�ʽ
    ShowWindow( hWnd, SW_SHOW);
    //ˢ�´���
    UpdateWindow(hWnd );
}



// 6����Ϣѭ��������
//void doMessage()        
//{
//
//    HWND      hwnd ;//���ھ��
//    MSG       msg ;//��Ϣ�ṹ
//    HACCEL    hAccel ;
//
//    MSG msg = { 0 };
//    // ��ȡ��Ϣ 
//    // �����յ�WM_QIUT��Ϣʱ��GetMessage��������0������ѭ��
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
//        // �ɷ���Ϣ����WindowPro��������
//        //DispatchMessage( &msg );
//    }
//}





//���ڵı�����ʾ ���磺��������-������  mywin-subTixt
void DoCaption (HWND hwnd, TCHAR * szTitleName)
{
    TCHAR szCaption[1 + MAX_PATH] ;

    wsprintf (szCaption, TEXT ("%s - %s"), lpClassName,
        szTitleName[0] ? szTitleName : UNTITLED) ;

    SetWindowText (hwnd, szCaption) ;
}


//��New �������ļ�ʱ �������е��ļ�����
short AskAboutSave (HWND hwnd, TCHAR * szTitleName)
{
    TCHAR szBuffer[64 + MAX_PATH] ;
    int   iReturn ;

    wsprintf (szBuffer, TEXT ("���浱ǰ���� %s?"),
        szTitleName[0] ? szTitleName : UNTITLED) ;

    iReturn = MessageBox (hwnd, szBuffer, lpClassName,
        MB_YESNOCANCEL | MB_ICONQUESTION) ;

    if (iReturn == IDYES)
        if (!SendMessage (hwnd, WM_COMMAND, IDM_FILE_SAVE, 0))
            iReturn = IDCANCEL ;

    return iReturn ;
}
static OPENFILENAME ofn ;



//��ʾ��Ϣ
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