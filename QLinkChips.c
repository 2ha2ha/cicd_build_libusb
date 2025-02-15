#include "QLinkLib.h"

#ifdef AUTO_LOAD_CHIPS
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <stlink.h>
#include <common_flash.h>
#include <read_write.h>
#include <usb.h>

// 添加到 chipid.c 的代码
// struct stlink_chipid_params **qGetDevicelist(){return &devicelist;}
struct stlink_chipid_params *qGetDevicelist();

#include "chips.txt"

void qDumpChips(const char* filePath) {
    FILE *file = fopen(filePath, "w+");
    if (file == NULL) {
        return;
    }
    fprintf(file, "#define DEVICELIST_CONST \n");
    fprintf(file, "struct stlink_chipid_params devicelist_const[] ={{\"unknow\"},");
    for (struct stlink_chipid_params *dev = *qGetDevicelist(); dev != NULL; dev = dev->next) {
        fprintf(file, "\n{\"%s\",", dev->dev_type);
        fprintf(file, "\"%s\",", dev->ref_manual_id);
        fprintf(file, "0x%x,", dev->chip_id);
        fprintf(file, "%d,",   dev->flash_type);
        fprintf(file, "0x%x,", dev->flash_size_reg);
        fprintf(file, "0x%x,", dev->flash_pagesize);
        fprintf(file, "0x%x,", dev->sram_size);
        fprintf(file, "0x%x,", dev->bootrom_base);
        fprintf(file, "0x%x,", dev->bootrom_size);
        fprintf(file, "0x%x,", dev->option_base);
        fprintf(file, "0x%x,", dev->option_size);
        fprintf(file, "%d,",   dev->flags);
        fprintf(file, "0x%x,", dev->otp_base);
        fprintf(file, "0x%x},",dev->otp_size);
    }
    fprintf(file, "\n{NULL}};");
    fclose(file);
}

static void qAddChips(struct stlink_chipid_params arr[]) {
  int i = 0;
  for(i=0; arr[i].dev_type != NULL; i++) {
    struct stlink_chipid_params *ts;
    ts = calloc(1, sizeof(struct stlink_chipid_params));
    if (ts==NULL) { continue; }
    ts->dev_type       = arr[i].dev_type;
    ts->ref_manual_id  = arr[i].ref_manual_id;
    ts->chip_id        = arr[i].chip_id;
    ts->flash_type     = arr[i].flash_type;
    ts->flash_size_reg = arr[i].flash_size_reg;
    ts->flash_pagesize = arr[i].flash_pagesize;
    ts->sram_size      = arr[i].sram_size;
    ts->bootrom_base   = arr[i].bootrom_base;
    ts->bootrom_size   = arr[i].bootrom_size;
    ts->option_base    = arr[i].option_base;
    ts->option_size    = arr[i].option_size;
    ts->flags          = arr[i].flags;
    ts->otp_base       = arr[i].otp_base;
    ts->otp_size       = arr[i].otp_size;
    ts->next           = *qGetDevicelist();
    *qGetDevicelist()  = ts;
  }
}

void qAutoLoadChips() {
    static int isInit = 0;
    if (isInit) {
        return;
    }
    isInit = 1;
    #ifdef DEVICELIST_CONST
    qAddChips(devicelist_const);
    #endif
}

#endif
