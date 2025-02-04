/*
 * File: logging.c
 *
 * UglyLogging: Slow, yet another wheel reinvented, but enough to make the rest of our code pretty enough.
 * Ugly, low performance, configurable level, logging "framework"
 */

#define __STDC_WANT_LIB_EXT1__ 1

#include <stdint.h>
#include <stdio.h>

#include <stdarg.h>
#include <time.h>

#include "logging.h"

static int32_t max_level = UDEBUG;

int32_t ugly_init(int32_t maximum_threshold) {
  max_level = maximum_threshold;
  return (0);
}

int32_t ugly_log(int32_t level, const char *tag, const char *format, ...) {
  if(level > max_level) {
    return (0);
  }

  fflush(stdout); // flush to maintain order of streams

  va_list args;
  va_start(args, format);
  time_t mytt = time(NULL);

  struct tm *ptt;
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) // C11
  struct tm tt;
  ptt = &tt;
# if defined (_WIN32) || defined(__STDC_LIB_EXT1__)
  localtime_s(&tt, &mytt);
# else
  localtime_r(&mytt, &tt);
# endif
#else
  ptt = localtime(&mytt);
#endif

  fprintf(stderr, "%d-%02d-%02dT%02d:%02d:%02d ", ptt->tm_year + 1900,
          ptt->tm_mon + 1, ptt->tm_mday, ptt->tm_hour, ptt->tm_min, ptt->tm_sec);

  switch (level) {
  case UDEBUG:
    fprintf(stderr, "DEBUG %s: ", tag);
    break;
  case UINFO:
    fprintf(stderr, "INFO %s: ", tag);
    break;
  case UWARN:
    fprintf(stderr, "WARN %s: ", tag);
    break;
  case UERROR:
    fprintf(stderr, "ERROR %s: ", tag);
    break;
  default:
    fprintf(stderr, "%d %s: ", level, tag);
    break;
  }

  vfprintf(stderr, format, args);
  fflush(stderr);
  va_end(args);
  return (1);
}

/*
 *  Log message levels.
 *  - LIBUSB_LOG_LEVEL_NONE (0)    : no messages ever printed by the library
 * (default)
 *  - LIBUSB_LOG_LEVEL_ERROR (1)   : error messages are printed to stderr
 *  - LIBUSB_LOG_LEVEL_WARNING (2) : warning and error messages are printed to
 * stderr
 *  - LIBUSB_LOG_LEVEL_INFO (3)    : informational messages are printed to
 * stderr
 *  - LIBUSB_LOG_LEVEL_DEBUG (4)   : debug and informational messages are
 * printed to stderr
 */
int32_t ugly_libusb_log_level(enum ugly_loglevel v) {
#ifdef __FreeBSD__
  // FreeBSD includes its own reimplementation of libusb.
  // Its libusb_set_debug() function expects a lib_debug_level
  // instead of a lib_log_level and is verbose enough to drown out
  // all other output.
  switch (v) {
  case UDEBUG:
    return (3); // LIBUSB_DEBUG_FUNCTION + LIBUSB_DEBUG_TRANSFER
  case UINFO:
    return (1); // LIBUSB_DEBUG_FUNCTION only
  case UWARN:
    return (0); // LIBUSB_DEBUG_NO
  case UERROR:
    return (0); // LIBUSB_DEBUG_NO
  }
  return (0);
#else
  switch (v) {
  case UDEBUG:
    return (4);
  case UINFO:
    return (3);
  case UWARN:
    return (2);
  case UERROR:
    return (1);
  }
  return (2);
#endif
}
