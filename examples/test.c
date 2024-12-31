#undef _GNU_SOURCE
#define _GNU_SOURCE 1 /* For strcasestr */

#include <loge.hpp>

struct loge logger;

static
void mylogfn(const struct loge *ploge) {
  if (!ploge) {
    return;
  }

  const char *msg = loge_bufptr(ploge);
  /* Filter messages that contain "log" (case insensitive) */
#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__)
  if (strcasestr(msg, "log") == NULL) {
    return;
  }
#else
  if (strstr(msg, "log") == NULL &&
      strstr(msg, "LOG") == NULL &&
      strstr(msg, "Log") == NULL) {
    return;
  }
#endif

  char *newmsg = strreplace(msg, "Logger", "This logger");
  if (!newmsg) {
    return;
  }

  FILE *file = loge_fileptr(ploge);
  fprintf(file, ANSI_BOLD"%s\n", newmsg);

  free(newmsg);
}

void mydatafn(
    FILE *file,
    time_t timestamp,
    const char *filename,
    int linenum,
    enum loge_level level,
    const char *msg
  ) {

  if (!file) {
    return;
  }

  const char *loglvl_str = loge_get_level_color(level);

  fprintf(file,
#ifdef _MSC_VER
      "%llu: %s:%04d: %-22s: %s\n",
#else
      "%lu: %s:%04d: %-22s: %s\n",
#endif
      timestamp,
      filename, linenum,
      loglvl_str,
      msg);
}

void log_stuff() {
  LOGE_COLOR(&logger, LOGE_DEBUG, "Address of logger: %p", &logger);
  LOGE_COLOR(&logger, LOGE_INFO, "Logger is set at level: %d", logger.level);
  LOGE_COLOR(&logger, LOGE_WARNING, "Dereference pointers with care");
  LOGE_COLOR(&logger, LOGE_ERROR, "Logger buffer size: %zu", logger.buflen);
  LOGE_COLOR(&logger, LOGE_CRITICAL, "Thanks for using logger");
}

void log_stuff_nocolor() {
  LOGE(&logger, LOGE_DEBUG, "Address of logger: %p", &logger);
  LOGE(&logger, LOGE_INFO, "Logger is set at level: %d", logger.level);
  LOGE(&logger, LOGE_WARNING, "Dereference pointers with care");
  LOGE(&logger, LOGE_ERROR, "Logger buffer size: %zu", logger.buflen);
  LOGE(&logger, LOGE_CRITICAL, "Thanks for using logger");
}

extern void log_more_stuff();

int main() {
  /* Initialize with increased buffer size, 6 digits for line number,
   * LOGE_ALL level, stdout stream and default callback function
   */
  loge_setup(
      &logger,      /* struct loge *ploge */
      2048,         /* size_t max_log_size */
      -1,           /* int linenumwidth */
      -1,           /* int wdith */
      -1,           /* int precision */
      LOGE_ALL,     /* enum loge_level level */
      NULL,         /* FILE *file */
      NULL          /* log_fn fn */
    );

  log_stuff();
  log_more_stuff();

  /* Log to a file */
  loge_set_file(&logger, "ctest.log");
  log_stuff_nocolor();
  loge_unset_file(&logger);

  /* Logger callback is unset now */
  LOGE(&logger, LOGE_CRITICAL, "This should cause an error message");

  /* Log to an open file descriptor */
#ifdef _MSC_VER
  loge_set_fd(&logger, _fileno(stdout));
#else
  loge_set_fd(&logger, STDOUT_FILENO);
#endif
  log_stuff();

  /* Filter log messages based on importance level */
  loge_set_level(&logger, LOGE_WARNING);
  loge_set_stderr(&logger);
  log_stuff_nocolor();

  loge_set_level(&logger, LOGE_ALL);

  /* Log using UDP */
#if 1
  /* Connect using IPv6 */
  if (!loge_connect(&logger, "::1", 8887, 0, 1, NULL)) {
#else
  /* Connect using IPv4 */
  if (!loge_connect(&logger, "127.0.0.1", 8888, 0, 0, NULL)) {
#endif
    log_stuff_nocolor();
    loge_disconnect(&logger);
  }

  /* Log using TCP */
#if 1
  /* Connect using IPv6 */
  if (!loge_connect(&logger, "::1", 8889, 1, 1, NULL)) {
#else
  /* Connect using IPv4 */
  if (!loge_connect(&logger, "127.0.0.1", 8888, 1, 0, NULL)) {
#endif
    log_stuff_nocolor();
    loge_disconnect(&logger);
  }

  /* Log using custom callback function with formatted log message */
  loge_set_stdout(&logger);
  loge_set_fn(&logger, mylogfn);
  log_stuff();

  /* Log using custom callback function with unformatted log data */
  loge_set_data_fn(&logger, mydatafn);
  log_stuff_nocolor();

#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__)

  /* Log to syslog */
  loge_set_syslog(&logger, LOG_NOTICE);
  log_stuff_nocolor();

#endif

  /* Use put functions */

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  loge_set_stderr(&logger);

  /* Reset message buffer */
  loge_reset(&logger);

  loge_put_str(&logger, "hello ");
  loge_put_time(&logger, &tm);

  loge_put_char(&logger, ' ');

  loge_put_int(&logger, 12);
  loge_put_char(&logger, '-');
  loge_put_long(&logger, 2025L);

  loge_put_char(&logger, ' ');

  loge_set_width(&logger, 8);
  loge_put_int(&logger, 312);

  loge_put_char(&logger, ' ');

  loge_set_width(&logger, 12);
  loge_set_precision(&logger, 6);
  loge_put_float(&logger, 2747.33333333);

  loge_put_char(&logger, ' ');

  loge_put_double(&logger, 333.33333333);

  /* Flush message buffer */
  loge_flush(&logger);

  loge_destroy(&logger);

  return 0;
}

#undef _GNU_SOURCE
