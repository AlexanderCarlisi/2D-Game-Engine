#include "logger.h"
#include <stdio.h>
#include "engine_options.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

static FILE* file;

void logger_init(int logLevel) {
  if (logLevel == 0)
    file = fopen(EO_LOG_FILE, "a");
  else if(logLevel == 1) {
    file = fopen(EO_LOG_FILE, "w");
  }

  if (file == NULL) {
    printf("\n[ERROR] >>> Logger initialization failed <<<\n");
  }
  
  time_t now = time(NULL);
  struct tm* t = localtime(&now);
  
  fprintf(file, "[LOG BEGIN] @[%d/%d/%d] @[%d:%d:%d]\n",
    t->tm_mon+1, t->tm_mday, t->tm_year+1900, t->tm_hour, t->tm_min, t->tm_sec);
}

void logger_write(int encapsulationDepth, int tabDepth, const char* message, bool error) {
  // time_t now = time(NULL);
  // struct tm* t = localtime(&now);

  // char* out = calloc(encapsulationDepth * 2 + tabDepth + strlen(message) + (error) ? 7 : 0 + 2, sizeof(char));

  const char* out = "%s%s %s %s\n";
  const char* errOut = "[ERROR]";
  char* open = calloc(tabDepth + encapsulationDepth, sizeof(char));
  for (int i = 0; i < tabDepth + encapsulationDepth; i++) {
    open[i] = (i < tabDepth) ? '\t' : '>';
  }
  char* close = calloc(encapsulationDepth, sizeof(char));
  for (int i = 0; i < encapsulationDepth; i++) {
    close[i] = '<';
  }

  printf("open %s \n", open);
  printf("close %s \n", close);
  fprintf(file, out, (error) ? errOut : "", open, message, close);
}

void logger_free() {
  fprintf(file, "\n[LOG CLOSE]\n\n");
  fclose(file);
}
