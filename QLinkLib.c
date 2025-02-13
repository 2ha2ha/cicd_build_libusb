#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "QLinkLib.h"
#include <stlink.h>
#include <common_flash.h>
#include <read_write.h>
#include <usb.h>

#define MEM_READ_SIZE 1024

extern struct stlink_chipid_params dev_const[];
struct stlink_chipid_params *stlink_chipid_get_params_hard(uint32_t chip_id) {
    for(uint32_t i=0; i<70; i++) {
        if(dev_const[i].chip_id == chip_id) {
            return &dev_const[i];
        }
    }
    return &dev_const[0];
}

// 自动加载设备列表
static void qinit() {
    static int init = 0;
    if (init) {
        return;
    }
    init = 1;
    init_chipids("./chips");
}

RET qget(QINFO* info) {
    qinit();
    RET ret = RET_OK;
    info->id = 0;
    info->sflash = 0;
    info->ssram = 0;
    info->type = "unknow";
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
    params = stlink_chipid_get_params_hard(sl->chip_id);
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

RET qreadfunc(uint32_t laddr, uint32_t llen, int(*cb)(uint8_t* data, uint32_t len)) {
    qinit();
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
    uint32_t len = 0;
    uint32_t off = 0;
    uint32_t addr = 0;
    if (laddr==0 && llen==0) {
        addr = sl->flash_base;
        len = sl->flash_size;
    } else {
        addr = laddr;
        len = llen;
    }
    for (off = 0; off < len; off += MEM_READ_SIZE) {
        uint32_t n_read = MEM_READ_SIZE;
        if (off + MEM_READ_SIZE > len) {
            n_read = len - off;
            // align if needed
            if (n_read & 3) { n_read = (n_read + 4) & ~(3); }
        }
        // reads to sl->q_buf
        stlink_read_mem32(sl, addr + off, n_read); 
        if (sl->q_len < 0) {
            ret = RET_READ_ERR;
        }
        else {
            if (cb(sl->q_buf, n_read)) {
                break;
            }
        }
    }
    stlink_reset(sl, RESET_AUTO);
    stlink_run(sl, RUN_NORMAL);
ST_DEBUG:
    stlink_exit_debug_mode(sl);
ST_CLOSE:
    stlink_close(sl);
    return ret;
}

RET qread(uint32_t addr, uint32_t len, uint8_t *data) {
    qinit();
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

// 检查文件是否存在
static BOOL qFileExists(const char* lpFileName) {
    DWORD dwAttrib = GetFileAttributesA(lpFileName);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
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

RET qwrite(const char *path, uint32_t addr) {
    qinit();
    RET ret = RET_OK;
    if (!qFileExists(path)) {
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
            if (stlink_mwrite_flash(sl, mem, size, addr, NO_ERASE)) {
                ret = RET_MWRITE_FLASH;
            }
        }
        else {
            if (stlink_fwrite_flash(sl, path, addr, NO_ERASE)) {
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

RET qwrites(const char* path, const char* addr) {
    uint32_t addru;
    char* endptr = NULL;
    // 执行转换
    addru = strtoul(addr, &endptr, 16);
    // 检查转换错误
    if (endptr == addr || endptr == NULL) {
        return RET_ADDR_STR_ERR;
    }
    if (*endptr != '\0') {
        return RET_ADDR_STR_ERR;
    }
    return qwrite(path, addru);
}


char* memstr(uint32_t u0, uint32_t u1, uint32_t u2, uint32_t u3)
{
    static char mem[16 + 1] = { 0 };
    static union TU {
        uint32_t u32[4];
        uint8_t u8[16];
    } tu;
    tu.u32[0] = u0;
    tu.u32[1] = u1;
    tu.u32[2] = u2;
    tu.u32[3] = u3;
    for (int i = 0; i < 16; i++) {
        mem[i] = isprint(tu.u8[i]) ? tu.u8[i] : '.';
    }
    return mem;
}