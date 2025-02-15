#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "QLinkLib.h"
#include <stlink.h>
#include <common_flash.h>
#include <read_write.h>
#include <usb.h>

// 加载设备列表
static void qInitChips(const char* scanDir) {
    static int isInit = 0;
    if (!isInit && scanDir!=NULL) {
        isInit = 1;
        init_chipids(scanDir);
    }
    #ifdef AUTO_LOAD_CHIPS
    static int isLoad = 0;
    if (!isLoad) {
        isLoad = 1;
        qAutoLoadChips();
    }
    #endif
}

// 获取设备信息
RET qInfo(QINFO* info) {
    qInitChips(NULL);
    RET ret = RET_OK;
    info->id = 0;
    info->sflash = 0;
    info->ssram = 0;
    info->type = "unknown";
    stlink_t* sl = stlink_open_usb(0, 1, NULL, 0);
    if (sl == NULL) {
        return RET_OPEN_ERR;
    }
    if (stlink_force_debug(sl)) {
        ret = RET_DBG_ERR;
        goto ST_CLOSE;
    }
    if (stlink_status(sl)) {
        ret = RET_STATUS_ERR;
        goto ST_DEBUG;
    }
    const struct stlink_chipid_params* params = NULL;
    params = stlink_chipid_get_params(sl->chip_id);
    info->id = sl->chip_id;
    info->sflash = sl->flash_size;
    info->ssram = sl->sram_size;
    info->type = params ? params->dev_type : info->type;
    stlink_reset(sl, RESET_AUTO);
    stlink_run(sl, RUN_NORMAL);
ST_DEBUG:
    stlink_exit_debug_mode(sl);
ST_CLOSE:
    stlink_close(sl);
    return ret;
}

// 读取数据: 要读取的地址, 要读取的长度, 读取到的数据
RET qRead(uint32_t addr, uint32_t len, uint8_t *data) {
    qInitChips(NULL);
    RET ret = RET_OK;
    stlink_t* sl = stlink_open_usb(0, 1, NULL, 0);
    if (sl == NULL) {
        return RET_OPEN_ERR;
    }
    if (stlink_force_debug(sl)) {
        ret = RET_DBG_ERR;
        goto ST_CLOSE;
    }
    if (stlink_status(sl)) {
        ret = RET_STATUS_ERR;
        goto ST_DEBUG;
    }

    if (len & 3) { len = (len + 4) & ~(3); }
    if (stlink_read_mem32(sl, addr, len)) {
        ret = RET_READ_ERR;
    }
    if (sl->q_len < 0) {
        ret = RET_READ_ERR;
    }
    else {
        memcpy(data, sl->q_buf, len);
    }
    stlink_reset(sl, RESET_AUTO);
    stlink_run(sl, RUN_NORMAL);
ST_DEBUG:
    stlink_exit_debug_mode(sl);
ST_CLOSE:
    stlink_close(sl);
    return ret;
}

// 检查文件或文件夹是否存在
static BOOL qPathExists(const char* lpPath) {
    return (GetFileAttributesA(lpPath) != INVALID_FILE_ATTRIBUTES);
}

// 检查文件名是否以 ".hex" 结尾
static BOOL qIsHex(const char* path) {
    // 获取文件名的长度
    size_t len = strlen(path);
    // 检查文件名长度是否足够长以包含 ".hex" 后缀
    if (len < 5) {
        return FALSE;
    }
    return  (strcmp(&path[len - 4], ".hex") == 0) ||
        (strcmp(&path[len - 4], ".HEX") == 0);
}

// 烧录文件: 文件路径, 烧录地址
RET qWrite(const char *path, uint32_t addr) {
    qInitChips(NULL);
    RET ret = RET_OK;
    if (!qPathExists(path)) {
        return RET_FILE_NOT_FOUND;
    }
    BOOL isHex = qIsHex(path);
    stlink_t* sl = stlink_open_usb(0, 1, NULL, 0);
    if (sl == NULL) {
        return RET_OPEN_ERR;
    }
    if (stlink_force_debug(sl)) {
        ret = RET_DBG_ERR;
        goto ST_CLOSE;
    }
    if (stlink_status(sl)) {
        ret = RET_STATUS_ERR;
        goto ST_DEBUG;
    }
    static uint8_t* mem = NULL;
    uint32_t size = 0;
    if (isHex) {
        if (stlink_parse_ihex(path, stlink_get_erased_pattern(sl), &mem, &size, &addr)) {
            ret = RET_IHEX_ERR;
            goto ST_DEBUG;
        }
    }
    if ((addr >= sl->flash_base) && (addr < sl->flash_base + sl->flash_size)) {
        if (isHex) {
            if (stlink_mwrite_flash(sl, mem, size, addr, SECTION_ERASE)) {
                ret = RET_MWRITE_FLASH;
            }
        }
        else {
            if (stlink_fwrite_flash(sl, path, addr, SECTION_ERASE)) {
                ret = RET_FWRITE_FLASH;
            }
        }
    }
    else if ((addr >= sl->sram_base) && (addr < sl->sram_base + sl->sram_size)) {
        if (isHex) {
            if (stlink_mwrite_sram(sl, mem, size, addr)) {
                ret = RET_MWRITE_SRAM;
            }
        }
        else {
            if (stlink_fwrite_sram(sl, path, addr)) {
                ret = RET_FWRITE_SRAM;
            }
        }
    }
    else {
        ret = RET_ADDR_OUT_RANGE;
    }
    stlink_reset(sl, RESET_AUTO);
    stlink_run(sl, RUN_NORMAL);
ST_DEBUG:
    stlink_exit_debug_mode(sl);
ST_CLOSE:
    stlink_close(sl);
    free(mem);
    return ret;
}

#define ERRMSG_COUNT (sizeof(qErrMsg)/sizeof(qErrMsg[0]))
#define ERRMSG_UNKNOWN "未知错误码: %d"
char *qErrMsg[] = {
    "成功",                // RET_OK
    "无法连接 STLINK",      // RET_OPEN_ERR
    "无法进入 DBUG 模式",   // RET_DBG_ERR
    "无法获取 STLINK 状态", // RET_STATUS_ERR
    "读取错误",             // RET_READ_ERR
    "文件未找到",           // RET_FILE_NOT_FOUND
    "HEX 文件格式有问题",   // RET_IHEX_ERR
    "无法写入 flash",       // RET_MWRITE_FLASH
    "无法写入 flash",       // RET_FWRITE_FLASH
    "无法写入 sram",        // RET_MWRITE_SRAM
    "无法写入 sram",        // RET_FWRITE_SRAM
    "地址异常"              // RET_ADDR_OUT_RANGE
};

// 根据错误码获取错误信息
const char *qGetErrMsg(RET errCode) {
    static char unknown[128] = {0} ;
    if(errCode < 0 || errCode >= ERRMSG_COUNT) {
        sprintf(unknown, ERRMSG_UNKNOWN, errCode);
        return unknown;
    }
    return qErrMsg[errCode];
}
