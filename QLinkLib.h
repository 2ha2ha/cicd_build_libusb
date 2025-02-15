#ifndef QLINKLIB_H
#define QLINKLIB_H

#include <stdint.h>

#define AUTO_LOAD_CHIPS

#ifdef _WINDLL
#define QLINKLIBAPI __declspec(dllexport)
#else
#define QLINKLIBAPI 
#endif

typedef enum RET {
    RET_OK = 0,            // 成功
    RET_OPEN_ERR,          // 打开错误
    RET_DBG_ERR,           // 调试错误
    RET_STATUS_ERR,        // 状态错误
    RET_READ_ERR,          // 读取错误
    RET_FILE_NOT_FOUND,    // 文件未找到
    RET_IHEX_ERR,          // IHEX 文件错误
    RET_MWRITE_FLASH,      // 烧录 Flash 内存错误
    RET_FWRITE_FLASH,      // 烧录 Flash 文件错误
    RET_MWRITE_SRAM,       // 烧录 SRAM 内存错误
    RET_FWRITE_SRAM,       // 烧录 SRAM 文件错误
    RET_ADDR_OUT_RANGE,    // 地址超出范围
} RET;

typedef struct {
    uint32_t id;        // 芯片ID
    char* type;         // 芯片名称
    uint32_t sflash;    // Flash 大小
    uint32_t ssram;     // SRAM 大小
} QINFO;

// 获取设备信息
QLINKLIBAPI RET qInfo(QINFO* info);

// 烧录文件: 文件路径, 烧录地址
QLINKLIBAPI RET qWrite(const char* filePath, uint32_t addr);

// 读取数据: 要读取的地址, 要读取的长度, 读取到的数据
QLINKLIBAPI RET qRead(uint32_t addr, uint32_t len, uint8_t *data);

// 添加指定目录下的芯片配置文件, 可以不设置, 有内置默认值
QLINKLIBAPI void qInitChips(const char* scanDir);

// 根据错误码获取错误信息
QLINKLIBAPI const char *qGetErrMsg(RET errCode);

#ifdef AUTO_LOAD_CHIPS
void qAutoLoadChips();
// 导出 devicelist 到文件
QLINKLIBAPI void qDumpChips(const char* filePath);
#endif

#endif