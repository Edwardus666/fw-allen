#ifndef  _PRINTF_H_
#define  _PRINTF_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "ArchDef.h"
#include <stdarg.h>
  
#define PRINTF_BUF_SIZE 1024

#define Printf printf
#define PFL(level, x) {if (g_dwDebugLevel & level) printf x;}

#ifdef __cplusplus
}
#endif

#endif

