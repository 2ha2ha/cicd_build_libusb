#include "stlink-static/chipid.c"

void stlink_add_devicelist(struct stlink_chipid_params arr[]) {
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
    ts->next           = devicelist;
    devicelist = ts;
    // void set_devicelist(struct stlink_chipid_params *set);
    // set_devicelist(ts);
  }
}

void stlink_print_devicelist() {
  struct stlink_chipid_params *dev = NULL;
  printf("struct stlink_chipid_params devicelist_const[] = {");
  for (dev = devicelist; dev != NULL; dev = dev->next) {
    printf("\n");
    printf("{");
    printf("\"%s\",",dev->dev_type);
    printf("NULL,"); // ref_manual_id
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
  printf("void auto(){stlink_add_devicelist(devicelist_const);}");
}

void stlink_auto_devicelist() {
    stlink_add_devicelist(devicelist_const);
}


int32_t ugly_log(int32_t level, const char *tag, const char *format, ...) {
  va_list args;
  va_start(args, format);
  printf("[%d] %s: ", level, tag);
  vprintf(format, args);
  va_end(args);
  return 0;
}

void stlink_print_devicelist() {
  struct stlink_chipid_params *dev = NULL;
  FILE *file = fopen("chipid_hard.c", "w");
  if (file == NULL) {
    perror("Failed to open file");
    return;
  }
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(file, "// Generated on: %04d-%02d-%02d %02d:%02d:%02d\n", 
          tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
          tm.tm_hour, tm.tm_min, tm.tm_sec);
  fprintf(file, "#include <stlink.h>\n");
  fprintf(file, "struct stlink_chipid_params dev_const[] = {\n{\"unknown\",NULL,0},\n");
  for (dev = devicelist; dev != NULL; dev = dev->next) {
    fprintf(file, "{");
    fprintf(file, "\"%s\",", dev->dev_type); 
    fprintf(file, "NULL,");//dev->ref_manual_id
    fprintf(file, "0x%x," ,  dev->chip_id);
    fprintf(file, "%d,"   ,  dev->flash_type); 
    fprintf(file, "0x%x," ,  dev->flash_size_reg); 
    fprintf(file, "0x%x," ,  dev->flash_pagesize); 
    fprintf(file, "0x%x," ,  dev->sram_size); 
    fprintf(file, "0x%x," ,  dev->bootrom_base);
    fprintf(file, "0x%x," ,  dev->bootrom_size);
    fprintf(file, "0x%x," ,  dev->option_base);
    fprintf(file, "0x%x," ,  dev->option_size);
    fprintf(file, "%d,"   ,  dev->flags);
    fprintf(file, "0x%x," ,  dev->otp_base);
    fprintf(file, "0x%x," ,  dev->otp_size);
    fprintf(file, "NULL");
    fprintf(file, "},");
    fprintf(file, "\n");
  }
  fprintf(file, "{NULL}\n};");
  fclose(file);
}

int main() {
  init_chipids("stlink-static/Program Files (x86)/stlink/config/chips");
  stlink_print_devicelist();
  return 0;
}
