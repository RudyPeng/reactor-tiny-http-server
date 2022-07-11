#include "util.h"
#include <cstdlib>

void errIf(bool b, const char *msg) {
  if (b) {
    perror(msg);
    exit(EXIT_FAILURE);
  }
}