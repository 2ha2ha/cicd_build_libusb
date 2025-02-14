#ifndef QLINKLIB_H
#define QLINKLIB_H

#include <stdint.h>

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
RET qinfo(QINFO* info);

// 烧录文件: 文件路径, 烧录地址
RET qwrite(const char* filePath, uint32_t addr);

// 读取数据: 要读取的地址, 要读取的长度, 读取到的数据
RET qread(uint32_t addr, uint32_t len, uint8_t *data);

#endif