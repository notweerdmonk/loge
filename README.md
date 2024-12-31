<center><img alt="doge" src="images/doge.png"></center>

<center><h1>Log(e)</h1></center>
<center><h5>Log everything</h5></center>

```bash
12-31-2024:14:45:06: test.c:000075: DEBUG   : Address of logger: 0x5596cc360160
12-31-2024:14:45:06: test.c:000076: INFO    : Logger is set at level: -1
12-31-2024:14:45:06: test.c:000077: WARNING : Dereference pointers with care
12-31-2024:14:45:06: test.c:000078: ERROR   : Logger buffer size: 76
12-31-2024:14:45:06: test.c:000079: CRITICAL: Thanks for using logger
```

<hr>

#### C

###### Initialization
```C
  struct loge logger;

  /* Initialize with default buffer size, 6 digits for line number,
   * LOGE_ALL level, stdout stream and default callback function
   */
  loge_setup(
      &logger,      /* struct loge *ploge */
      0,            /* size_t max_log_size */
      -1,           /* int linenumwidth */
      -1,           /* int wdith */
      -1,           /* int precision */
      LOGE_ALL,     /* enum loge_level level */
      NULL,         /* FILE *file */
      NULL          /* log_fn fn */
    );

  LOGE_COLOR(&logger, LOGE_DEBUG, "Address of logger: %p", &logger);
  LOGE_COLOR(&logger, LOGE_INFO, "Logger is set at level: %d", logger.level);
  LOGE_COLOR(&logger, LOGE_WARNING, "Dereference pointers with care");
  LOGE_COLOR(&logger, LOGE_ERROR, "Logger buffer size: %zu", logger.buflen);
  LOGE_COLOR(&logger, LOGE_CRITICAL, "Thanks for using logger");
```

###### Filter log messages based on importance level
```C
  loge_set_level(&logger, LOGE_CRITICAL);
  LOGE(&logger, LOGE_WARNING, "This will not get logged");
  LOGE(&logger, LOGE_ERROR, "This will not get logged");
  LOGE(&logger, LOGE_CRITICAL, "This will get logged");
```

###### Loge to file
```C
  loge_set_file(&logger, "ctest.log");

  LOGE(&logger, LOGE_INFO, "Logger is set at level: %d", logger.level);

  loge_unset_file(&logger);

  /* Logger callback is unset now */
  LOGE(&logger, LOGE_CRITICAL, "This should cause an error message");
```

###### Loge to stdout, stderr
```C
  loge_set_stdout(&logger);
  LOGE_COLOR(&logger, LOGE_DEBUG, "Address of logger: %p", &logger);

  loge_set_stderr(&logger);
  LOGE(&logger, LOGE_INFO, "Logger is set at level: %d", logger.level);
```

###### Loge to an open file descriptor
```C
#ifdef _MSC_VER
  loge_set_fd(&logger, _fileno(stdout));
#else
  loge_set_fd(&logger, STDOUT_FILENO);
#endif
  LOGE(&logger, LOGE_DEBUG, "Address of logger: %p", &logger);
```

###### Loge to TCP/UDP server
```C
  /* Loge to UDP server over IPv6 */
  if (!loge_connect(&logger, "::1", 8887, 0, 1, NULL)) {
    LOGE(&logger, LOGE_DEBUG, "Address of logger: %p", &logger);
    loge_disconnect(&logger);
  }

  /* Loge to TCP server over IPv6 */
  if (!loge_connect(&logger, "::1", 8889, 1, 1, NULL)) {
    LOGE(&logger, LOGE_DEBUG, "Address of logger: %p", &logger);
    loge_disconnect(&logger);
  }
```

###### Loge to syslog
```C
  loge_set_syslog(&logger, LOG_NOTICE);
  LOGE(&logger, LOGE_DEBUG, "Address of logger: %p", &logger);
```

###### Loge using custom callback function with formatted log message
```C
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

  loge_set_fn(&logger, mylogfn);
  LOGE(&logger, LOGE_DEBUG, "Address of logger: %p", &logger);
```

###### Loge using custom callback function with unformatted log data
```C
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

  loge_set_data_fn(&logger, mydatafn);
  LOGE(&logger, LOGE_DEBUG, "Address of logger: %p", &logger);
```

###### Loge arbitrary data and flush message buffer
```C
  /* Use put functions */

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

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
```

<hr>

#### C++

###### Initialization
```C++
  /* Definition */
  template <bool timestamp = true>
  class loge {

    /* Constructors */
    loge(
        std::ostream *p_os_,
        int linenumwidth_ = constants::LINENUMBER_WIDTH,
        int width_ = -1,
        int precision_ = -1,
        enum loge_level level_ = loge_level::ERROR
      ) : level(level_), logfnptr(&loge<timestamp>::logfn) {

      ...
    }

    loge(enum loge_level loglevel = loge_level::ERROR)
      : loge(nullptr, constants::LINENUMBER_WIDTH, -1, -1, loglevel) {
    }

    ...
  };

  loge<false> syslogger(loge<false>::ALL);
```

###### Filter log messages based on importance level
```C++
  loge<> logger(loge<>::ALL);

  logger.set_level(loge<>::CRITICAL);
  LOGE_COLOR(&logger, loge<>::WARNING, "This will not be logged");
```

###### Loge to file
```C++
  loge<> logger(loge<>::ALL);

  logger.set_file("cctest.log", true);
  LOGE(&logger, loge<>::INFO, "Logging to file %s", "cctest.log");
```

###### Loge to stdout, stderr
```C++
  logger.set_stdout();
  LOGE_COLOR(&logger, loge<>::INFO, "Stdout filestream");

  logger.set_stderr();
  LOGE(&logger, loge<>::INFO, "Stderr filestream");
```

###### Loge to an open file descriptor
```C++
#if defined(__linux__) || defined(__linux)
  logger.set_fd(STDOUT_FILENO);
#elif defined(_MSC_VER)
  logger.set_fd(_fileno(stdout));
#endif

  LOGE_COLOR(&logger, loge<>::INFO, "Log to stdio");
```

###### Loge to TCP/UDP server
```C++
  /* Loge to UDP server over IPv6 */
  if (logger.connect("::1", 8887, 0, 1)) {
    LOGE(&logger, file_logger_base::INFO, "Log to UDP socket");
    logger.disconnect();
  }

  /* Loge to TCP server over IPv6 */
  if (logger.connect("::1", 8889, 1, 1)) {
    LOGE(&logger, file_logger_base::INFO, "Log to TCP socket");
    logger.disconnect();
  }
```

###### Loge to syslog
```C++
  loge<false> syslogger(loge<false>::ALL);

  syslogger.set_syslog(LOG_USER | LOG_NOTICE);
  LOGE(&syslogger, loge<false>::INFO, "Log to syslog daemon");
```

###### Loge using custom log function with formatted log message
```C++
  using fd_logger_base = loge<true>;
  
  class fd_logger  : public fd_logger_base {
    int fd_ = -1;
  
    public:
  
    /* Override log function */
    void logfn() override {
  
      /* buffer char array may not be null-terminated */
      buffer[buffer.size() - 1] = '\0';
  
      dprintf(fd_, "%s\n", buffer.data());
    }
  
    fd_logger(int fd, fd_logger_base::loge_level loglevel)
      : loge(loglevel) , fd_(fd) {
    }
  };

#if defined(__linux__) || defined(__linux)
  fd_logger fdlogger(STDOUT_FILENO, fd_logger_base::ALL);
#elif defined(_MSC_VER)
  fd_logger fdlogger(_fileno(stdout), fd_logger_base::ALL);
#endif

  LOGE_COLOR(&fdlogger, fd_logger_base::INFO, "Log to stdout using custom log function");
```

###### Loge using custom log function with unformatted log data
```C++
  using custom_logger_base = loge<true>;
  
  class custom_logger  : public custom_logger_base {
  
    public:
  
    bool datafn(std::ostream *p_os, std::time_t &time,
        const std::string &filename, unsigned int linenum,
        enum loge_level loglevel, const std::string &msg) override {
  
      const char *loglvl_str = get_level_color(loglevel);
  
      char buf[1024];
  
      int done = snprintf(buf, sizeof(buf),
#ifdef _MSC_VER
          "%llu: %s:%04d: %-22s - %s\n",
#else
          "%lu: %s:%04d: %-22s - %s\n",
#endif
          time,
          filename.c_str(), linenum,
          loglvl_str,
          msg.c_str());
  
      p_os->write(buf, done);
  
      return false;
    }
  
    custom_logger(custom_logger_base::loge_level loglevel)
      : loge(loglevel) {
    }
  };
  custom_logger customlogger(custom_logger_base::ALL);

  LOGE(&customlogger, custom_logger_base::INFO,
      "Custom log function with unformatted log data %d %s", 10, "foo");
```

###### Loge arbitrary data and flush message buffer
```C++
  /* Demo for insertion operator */
  loge<true> log(&std::cerr);
  std::time_t t = std::time(nullptr);
  struct tm tm = *std::localtime(&t);
  std::string prefix = "test log: ";

  /* Reset message buffer */
  log.reset();

  log << prefix << "hello: " << 121 << " : " <<
    loge<>::setw(loge<>::constants::NUMBER_WIDTH) << 312 << " : " << tm
    << ": " << loge<>::setw(6) << 1970 << loge<>::endl;

  long l = 0xffffffffffffffff;
  unsigned long u = 0xffffffffffffffff;
  std::size_t s = 0xffffffffffffffff;
  log << "integers: " << loge<>::setw(24) << ' ' << l << ' ' << u << ' ' << s;
  log.flush();

  float f = 2747.33333333;
  double d = 333.33333333;
  log << "fractions: " << loge<>::setw_default() << 312.3145926535 << " "
    << loge<>::setw(12) << loge<>::setprecision(6) << " " << f << " " << d;

  /* Write message to ostream or socket */
  log.flush();
```

