#include <iostream>
#include <Windows.h>

int main()
{
    HINSTANCE h = nullptr;
    // D:\\WorkSpace\\Cpp\\Test\\Project1\\src\\virbox64.dll
    h = LoadLibrary(L".\\lib\\virbox64.dll");
    if (h)
    {
        FreeLibrary(h);
        std::cout << "Load Library OK" << std::endl;
        return 0;
    }
    std::cout << "Load Library ERROR!" << std::endl;
    std::cout << GetLastError() << std::endl;
    return 1;
}
