#pragma once

#include <glog/logging.h>
#include <cstdio>
#include <cstdlib>


#define ERRIF(b, msg) \
  if (b) {  \
    LOG(FATAL) << msg;   \
    exit(EXIT_FAILURE); \
  }
