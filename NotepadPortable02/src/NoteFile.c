#include "NoteFile.h"

static OPENFILENAME ofn;

DWORD FileInit(HWND hwnd)
{
    static TCHAR szFilter[] = TEXT("Text Files (*.TXT)\0*.txt\0")  \
        TEXT("ASCII Files (*.ASC)\0*.asc\0") \
        TEXT("All Files (*.*)\0*.*\0\0");

    // OPENFILENAME
    ofn.lStructSize = sizeof(OPENFILENAME); // 结构的长度（以字节为单位）
    ofn.hwndOwner = hwnd;                   // 拥有对话框的窗口的句柄
    ofn.hInstance = NULL;                   // 包含对话框模板的内存对象的句柄 、由 lpTemplateName 成员命名的对话框模板的模块的句柄
    ofn.lpstrFilter = szFilter;             // 包含以 null 结尾的筛选器字符串对的缓冲区
    ofn.lpstrCustomFilter = NULL;           // 一个静态缓冲区，其中包含一对以 null 结尾的筛选器字符串，用于保留用户选择的筛选器模式
    ofn.nMaxCustFilter = 0;                 // 由 lpstrCustomFilter 标识的缓冲区的大小（以字符为单位）
    ofn.nFilterIndex = 0;                   // 文件类型控件中当前所选筛选器的索引
    ofn.lpstrFile = NULL;                   // 用于初始化文件名编辑控件的 文件名
    ofn.nMaxFile = MAX_PATH;                // lpstrFile 指向的缓冲区的大小（以字符为单位）
    ofn.lpstrFileTitle = NULL;              // 文件名和扩展名（没有所选文件的路径信息）
    ofn.nMaxFileTitle = MAX_PATH;           // lpstrFileTitle 指向的缓冲区的大小（以字符为单位）
    ofn.lpstrInitialDir = NULL;             // 初始目录
    ofn.lpstrTitle = NULL;                  // 要放置在对话框标题栏中的字符串
    ofn.Flags = 0;                          // 一组可用于初始化对话框的位标志
    ofn.nFileOffset = 0;                    // 从路径的开头到 由 lpstrFile 指向的字符串中文件名的从零开始的偏移量（以字符为单位）
    ofn.nFileExtension = 0;                 // 从路径的开头到字符串中由 lpstrFile 指向的文件扩展名的从零开始的偏移量（以字符为单位）
    ofn.lpstrDefExt = TEXT("txt");          // 默认扩展
    ofn.lCustData = 0L;                     // 系统传递给 lpfnHook 成员标识的挂钩过程的应用程序定义数据
    ofn.lpfnHook = NULL;                    // 指向挂钩过程的指针
    ofn.lpTemplateName = NULL;              // hInstance 成员标识的模块中的对话框模板资源的名称

    return 0;
}

//点击Open打开文件
BOOL FileOpenDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = pstrFileName;
    ofn.lpstrFileTitle = pstrTitleName;
    ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

    return GetOpenFileName(&ofn);
}

// Save File
BOOL FileSaveDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = pstrFileName;
    ofn.lpstrFileTitle = pstrTitleName;
    ofn.Flags = OFN_OVERWRITEPROMPT;

    return GetSaveFileName(&ofn);
}

BOOL FileRead(HWND hwndEdit, PTSTR pstrFileName)
{
    BYTE   bySwap;
    DWORD  dwBytesRead;
    HANDLE hFile;
    int    i, iFileLength, iUniTest;
    PBYTE  pBuffer, pText, pConv;

    // Open the file.
    hFile = CreateFile(
        pstrFileName, 
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        0, 
        NULL
    );
    if (INVALID_HANDLE_VALUE == hFile)
        return FALSE;

    // Get file size in bytes and allocate memory for read.
    // Add an extra two bytes for zero termination.
    iFileLength = GetFileSize(hFile, NULL);
    pBuffer = (PBYTE)malloc(iFileLength + 2);

    // 移动指定文件的文件指针
    //SetFilePointer(hFile, 1, NULL, FILE_BEGIN);  

    // Read file and input terminating zeros at end.
    ReadFile(hFile, pBuffer, iFileLength, &dwBytesRead, NULL);
    CloseHandle(hFile);
    pBuffer[iFileLength] = '\0';
    pBuffer[iFileLength + 1] = '\0';

    // Test to see if the text is unicode
    iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;
    if (IsTextUnicode(pBuffer, iFileLength, &iUniTest))
    {
        pText = pBuffer + 2;
        iFileLength -= 2;
        if (iUniTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE)
        {
            for (i = 0; i < iFileLength / 2; i++)
            {
                bySwap = ((BYTE*)pText)[2 * i];
                ((BYTE*)pText)[2 * i] = ((BYTE*)pText)[2 * i + 1];
                ((BYTE*)pText)[2 * i + 1] = bySwap;
            }
        }

        // Allocate memory for possibly converted string
        pConv = (PBYTE)malloc(iFileLength + 2);

#ifndef UNICODE
        // If the edit control is not Unicode, convert Unicode text to 
        // non-Unicode (ie, in general, wide character).
        WideCharToMultiByte(CP_ACP, 0, (PWSTR)pText, -1, pConv,
            iFileLength + 2, NULL, NULL);
#else
        // If the edit control is Unicode, just copy the string
        lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif
    }
    else      // the file is not Unicode
    {
        pText = pBuffer;

        // Allocate memory for possibly converted string.
        pConv = (PBYTE)malloc(2 * iFileLength + 2);
        // If the edit control is Unicode, convert ASCII text.
#ifdef UNICODE
        // 将字符串映射到 UTF-16 (宽字符) 字符串。 字符串不一定来自多字节字符集。
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pText, -1, (PTSTR)pConv, iFileLength + 1);
#else
        // If not, just copy buffer
        lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif
    }
    SetWindowText(hwndEdit, (PTSTR)pConv);
    free(pBuffer);
    free(pConv);

    return TRUE;
}

BOOL MapFileRead(HWND hwndEdit, PTSTR pstrFileName)
{
    BYTE    bySwap;
    DWORD   dwBytesRead;
    DWORD   dwFileMapSize;              // 映射文件大小   
    HANDLE  hFile;                      // 文件句柄
    HANDLE  hMapFile;                   // 文件映射句柄
    LPVOID  lpAddr;                     // 文件映射虚拟内存指针
    PDWORD   dwTextHand, dwTextTail;     // 读文件指针
    int     i, iFileLength, iUniTest;
    PBYTE   pBuffer, pText, pConv;

    // 创建文件句柄
    hFile = CreateFile(
        pstrFileName,                           // 要创建或打开的文件或设备的名称。
        GENERIC_READ | GENERIC_WRITE,           // 请求访问文件或设备（可以汇总为读取、写入或 0）以指示两者都没有)
        FILE_SHARE_READ | FILE_SHARE_WRITE,     // 文件或设备的请求共享模式，可以读取、写入、删除所有这些或无 (引用下表) 。 对属性或扩展属性的访问请求不受此标志的影响。
        NULL,                                   // 指向一个 SECURITY_ATTRIBUTES 结构的指针，该结构包含两个独立但相关的数据成员：可选的安全描述符，以及一个布尔值，该值确定返回的句柄是否可以由子进程继承。
        OPEN_EXISTING,                          // 对存在或不存在的文件或设备执行的操作。
        0,                                      // 文件或设备属性和标志， FILE_ATTRIBUTE_NORMAL 是文件最常见的默认值。
        NULL                                    // 具有 GENERIC_READ 访问权限的模板文件的有效句柄。 模板文件为正在创建的文件提供文件属性和扩展属性。
    );
    if (INVALID_HANDLE_VALUE == hFile)
        goto CLOSE;

    // 创建文件映射句柄
    hMapFile = CreateFileMapping(
        hFile,              // 创建文件映射对象的句柄
        NULL,               // 指向 SECURITY_ATTRIBUTES 结构的指针，该结构确定返回的句柄是否可以由子进程继承
        PAGE_READWRITE,     // 允许为只读、写入时复制或读取/写入访问映射视图
        0,                  // 文件映射对象的最大大小的高阶 DWORD
        0,                  // 文件映射对象最大大小的低序 DWORD
        pstrFileName        // 文件映射对象的名称。
    );
    if (NULL == hMapFile)
        goto CLOSE;

    // 将文件映射到虚拟内存中，返回内存指针
    lpAddr = MapViewOfFile(hMapFile, FILE_MAP_COPY, 0, 0, 0);
    if (NULL == lpAddr)
        goto CLOSE;

    // 获取文件大小
    dwFileMapSize = GetFileSize(hFile, NULL);

    // 读文件
    dwTextHand = (PDWORD)lpAddr;
    dwTextTail = ((PDWORD)lpAddr + 0x20);

    // Test to see if the text is unicode
    iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;
    if (IsTextUnicode(lpAddr, dwFileMapSize, &iUniTest))
    {
        SetWindowText(hwndEdit, (PTSTR)dwTextHand);
    }
    else
    {
        pConv = (PBYTE)malloc(2 * dwFileMapSize + 2);
        // 将字符串映射到 UTF-16 (宽字符) 字符串。 字符串不一定来自多字节字符集。
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)dwTextHand, -1, (PTSTR)pConv, dwFileMapSize);
        SetWindowText(hwndEdit, (PTSTR)pConv);
    }
    // 写文件
    //*(PDWORD)lpAddr = 0xFFFFFFFF;

    // 强制更新缓存，应用场景：文件过大时强制写入文件
    //FlushViewOfFile(((PDWORD)lpAddr), 4);

    // 关闭资源
    UnmapViewOfFile(lpAddr);
    CloseHandle(hFile);
    CloseHandle(hMapFile);
    return TRUE;

CLOSE:
    CloseHandle(hFile);
    CloseHandle(hMapFile);
    UnmapViewOfFile(lpAddr);
    return FALSE;
}


BOOL FileWrite(HWND hwndEdit, PTSTR pstrFileName)
{
    DWORD  dwBytesWritten;
    HANDLE hFile;
    int    iLength;
    PTSTR  pstrBuffer;
    WORD   wByteOrderMark = 0xFEFF;

    // Open the file, creating it if necessary
    if (INVALID_HANDLE_VALUE == (hFile = CreateFile(pstrFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL)))
        return FALSE;

    // Get the number of characters in the edit control and allocate
    // memory for them.
    iLength = GetWindowTextLength(hwndEdit);
    pstrBuffer = (PTSTR)malloc((iLength + 1) * sizeof(TCHAR));

    if (!pstrBuffer)
    {
        CloseHandle(hFile);
        return FALSE;
    }

#ifdef UNICODE
    // If the edit control will return Unicode text, write the
    // byte order mark to the file.
    WriteFile(hFile, &wByteOrderMark, 2, &dwBytesWritten, NULL);

#endif
    // Get the edit buffer and write that out to the file.
    GetWindowText(hwndEdit, pstrBuffer, iLength + 1);
    WriteFile(hFile, pstrBuffer, iLength * sizeof(TCHAR),
        &dwBytesWritten, NULL);

    if ((iLength * sizeof(TCHAR)) != (int)dwBytesWritten)
    {
        CloseHandle(hFile);
        free(pstrBuffer);
        return FALSE;
    }

    CloseHandle(hFile);
    free(pstrBuffer);

    return TRUE;
}
