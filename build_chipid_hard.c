
#include "stlink-static/chipid.c"

int32_t ugly_log(int32_t level, const char *tag, const char *format, ...){return 0;}

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

int main() {
  init_chipids("stlink-static/Program Files (x86)/stlink/config/chips");
  stlink_print_devicelist();
  return 0;
}
