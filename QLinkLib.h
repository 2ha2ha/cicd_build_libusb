#ifndef QLINKLIB_H
#define QLINKLIB_H

#include <stdint.h>
#ifdef _WINDLL
#define QLINKLIBAPI __declspec(dllexport)
#else
#define QLINKLIBAPI 
#endif

typedef enum RET {
    RET_OK = 0,
    RET_OPEN_ERR,
    RET_DBG_ERR,  
    RET_STATUS_ERR,
    RET_READ_ERR,
    RET_FILE_NOT_FOUND,
    RET_IHEX_ERR,
    RET_MWRITE_FLASH,
    RET_FWRITE_FLASH,
    RET_MWRITE_SRAM,    
    RET_FWRITE_SRAM,    
    RET_ADDR_OUT_RANGE, 
    RET_ADDR_STR_ERR 
} RET;

typedef struct {
    uint32_t id;
    char* type;
    uint32_t sflash;
    uint32_t ssram;
} QINFO;

QLINKLIBAPI RET qget(QINFO* info);
QLINKLIBAPI RET qwrite(const char* path, uint32_t addr);
QLINKLIBAPI RET qwrites(const char* path, const char* addr);
QLINKLIBAPI RET qread(uint32_t addr, uint32_t len, uint8_t *data);
QLINKLIBAPI RET qreadfunc(uint32_t addr, uint32_t len, int(*cb)(uint8_t* data, uint32_t len));
QLINKLIBAPI char* memstr(uint32_t u0, uint32_t u1, uint32_t u2, uint32_t u3);
#endif