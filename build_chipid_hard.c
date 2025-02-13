#include "stlink-static/chipid.c"
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
    fprintf(file, "0x%x"  ,  dev->otp_size);
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
