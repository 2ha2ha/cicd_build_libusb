#include <windows.h>

// 声明DLL中的函数
typedef void (*FunctionType)();

int main() {
    HMODULE hModule = LoadLibrary("./stlink_lib/bin/stlink.dll"); // 加载DLL
    if (hModule == NULL) {
        // 如果加载失败，输出错误信息并退出
        fprintf(stderr, "Error loading a.dll\n");
        return 1;
    }

    // 获取函数的地址
    FunctionType bFunc = (FunctionType)GetProcAddress(hModule, "b");
    if (bFunc == NULL) {
        // 如果获取失败，输出错误信息并退出
        fprintf(stderr, "Error getting address of function b\n");
        FreeLibrary(hModule); // 释放加载的DLL
        return 1;
    }

    // 调用函数
    bFunc();

    // 释放加载的DLL
    FreeLibrary(hModule);
    return 0;
}