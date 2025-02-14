void stlink_print_devicelist() {
  struct stlink_chipid_params *dev = NULL;
  printf("struct stlink_chipid_params devicelist_const[] ={{\"unknow\"},");
  for (dev = devicelist; dev != NULL; dev = dev->next) {
    printf("\n{");
    printf("\"%s\",",dev->dev_type);
    printf("NULL,");
    printf("0x%x," , dev->chip_id);
    printf("%d,"   , dev->flash_type);
    printf("0x%x," , dev->flash_size_reg);
    printf("0x%x," , dev->flash_pagesize);
    printf("0x%x," , dev->sram_size);
    printf("0x%x," , dev->bootrom_base);
    printf("0x%x," , dev->bootrom_size);
    printf("0x%x," , dev->option_base);
    printf("0x%x," , dev->option_size);
    printf("%d,"   , dev->flags);
    printf("0x%x," , dev->otp_base);
    printf("0x%x"  , dev->otp_size);
    printf("},");
  }
  printf("{NULL}\n};");
}

void stlink_add_devicelist(struct stlink_chipid_params arr[]) {
  int i = 0;
  for(i=0; arr[i].dev_type != NULL; i++) {
    struct stlink_chipid_params *ts;
    ts = calloc(1, sizeof(struct stlink_chipid_params));
    if (ts==NULL) {
        continue;
    }
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
    ts->next           = devicelist;
    devicelist = ts;
  }
}

void stlink_auto_devicelist() {
    stlink_add_devicelist(devicelist_const);
}

