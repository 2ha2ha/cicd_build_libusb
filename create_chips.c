#include <windows.h>
#include <stdio.h>

// 声明DLL中的函数
typedef void (*FUNC)(const char*);

// 初始化芯片列表
// void qInitChips(const char* scanDir);
// 导出 devicelist 到文件
// void qDumpChips(const char* filePath);

int main(int argc, char* argv[]) {
    HMODULE hModule = LoadLibrary("QLink.dll"); // 加载DLL
    if (hModule == NULL) {
        // 如果加载失败，输出错误信息并退出
        puts("Error loading QLink.dll\n");
        getchar();
        return 1;
    }

    // 获取函数的地址
    FUNC qInitChips = (FUNC)GetProcAddress(hModule, "qInitChips");
    FUNC qDumpChips = (FUNC)GetProcAddress(hModule, "qDumpChips");

    if (qInitChips == NULL || qDumpChips == NULL) {
        // 如果获取失败，输出错误信息并退出
        puts("Error getting  function \n");
        FreeLibrary(hModule); // 释放加载的DLL
        return 1;
    }

    #define DEF_FILE    "./chips.txt"
    #define DEF_DIR     "./stlink_lib/bin/chips"

    char *dirPath = NULL;
    char *filePath = NULL;
    switch(argc) {
        case 1:
            qInitChips(DEF_DIR);
            qDumpChips(DEF_FILE);
            break;
        case 2:
            qInitChips(DEF_DIR);
            qDumpChips(argv[1]);
            break;
        case 3:
            qInitChips(argv[2]);
            qDumpChips(argv[1]);
            break;
    }
    puts("====Done====\n");  
    FreeLibrary(hModule); // 释放加载的DLL
    getchar();
    return 0;
}