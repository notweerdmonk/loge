#if defined(__linux) || defined(__linux__)
#include <unistd.h>
#endif
#include <loge.hpp>
#include <fdlogger.hpp>

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

int main() {

  loge<true> logger(loge<>::ALL);

  LOGE_COLOR(&logger, loge<>::INFO, "Default stdout %d %s", 10, "foo");
  LOGE_COLOR(&logger, loge<>::DEBUG, "Default stdout %d %s", 10, "bar");
  LOGE_COLOR(&logger, loge<>::WARNING, "Default stdout %d %s", 10, "baz");
  LOGE_COLOR(&logger, loge<>::ERROR, "Default stdout %d %s", 10, "pebkac");
  LOGE_COLOR(&logger, loge<>::CRITICAL, "Default stdout %d %s", 10, "grokking");

  logger.set_file("./cctest.log");
  LOGE(&logger, loge<>::INFO, "File %d %s", 10, "foo");
  LOGE(&logger, loge<>::DEBUG, "File %d %s", 10, "bar");
  LOGE(&logger, loge<>::WARNING, "File %d %s", 10, "baz");
  LOGE(&logger, loge<>::ERROR, "File %d %s", 10, "pebkac");
  LOGE(&logger, loge<>::CRITICAL, "File %d %s", 10, "grokking");
  /*
   * Call unset_file() to free the ofstream object associated with output file
   */
  logger.unset_file();

  LOGE(&logger, loge<>::INFO, "This will not get logged");

  /* Any logger object can be reused to log to an open file descriptor */
#if defined(__linux__) || defined(__linux) || defined(__FreeBSD__) \
  || defined(__OpenBSD__)

  logger.set_fd(STDOUT_FILENO);

#elif defined(_MSC_VER)
  logger.set_fd(_fileno(stdout));
#else
  logger.set_fd(fileno(stdout));
#endif
  LOGE_COLOR(&logger, loge<>::INFO, "Reused as stdio %d %s", 10, "foo");
  LOGE_COLOR(&logger, loge<>::DEBUG, "Reused as stdio %d %s", 10, "bar");
  LOGE_COLOR(&logger, loge<>::WARNING, "Reused as stdio %d %s", 10, "baz");
  LOGE_COLOR(&logger, loge<>::ERROR, "Reused as stdio %d %s", 10, "pebkac");
  LOGE_COLOR(&logger, loge<>::CRITICAL, "Reused as stdio %d %s", 10,
      "grokking");
  logger.unset_fd();

  logger.set_level(loge<>::WARNING);
  logger.set_stderr();
  LOGE(&logger, loge<>::INFO, "Filter by importance level %d %s", 10, "foo");
  LOGE(&logger, loge<>::DEBUG, "Filter by importance level %d %s", 10, "bar");
  LOGE(&logger, loge<>::WARNING, "Filter by importance level %d %s", 10, "baz");
  LOGE(&logger, loge<>::ERROR, "Filter by importance level %d %s", 10,
      "pebkac");
  LOGE(&logger, loge<>::CRITICAL, "Filter by importance level %d %s", 10,
      "grokking");

  logger.set_level(loge<>::ALL);

  /* Any logger object can be reused to log to stdout or stderr */
  logger.set_stdout();
  LOGE_COLOR(&logger, loge<>::INFO, "Stdout filestream %d %s", 10, "foo");
  LOGE_COLOR(&logger, loge<>::DEBUG, "Stdout filestream %d %s", 10, "bar");
  LOGE_COLOR(&logger, loge<>::WARNING, "Stdout filestream %d %s", 10, "baz");
  LOGE_COLOR(&logger, loge<>::ERROR, "Stdout filestream %d %s", 10, "pebkac");
  LOGE_COLOR(&logger, loge<>::CRITICAL, "Stdout filestream %d %s", 10,
      "grokking");

  logger.set_stderr();
  LOGE(&logger, loge<>::INFO, "Stderr filestream %d %s", 10, "foo");
  LOGE(&logger, loge<>::DEBUG, "Stderr filestream %d %s", 10, "bar");
  LOGE(&logger, loge<>::WARNING, "Stderr filestream %d %s", 10, "baz");
  LOGE(&logger, loge<>::ERROR, "Stderr filestream %d %s", 10, "pebkac");
  LOGE(&logger, loge<>::CRITICAL, "Stderr filestream %d %s", 10, "grokking");

  /* Reuse for writing to UDP socket */
  if (logger.connect("::1", 8887, 0, 1)) {

    LOGE(&logger, loge<>::INFO, "UDP socket %d %s", 10, "foo");
    LOGE(&logger, loge<>::DEBUG, "UDP socket %d %s", 10, "bar");
    LOGE(&logger, loge<>::WARNING, "UDP socket %d %s", 10, "baz");
    LOGE(&logger, loge<>::ERROR, "UDP socket %d %s", 10, "pebkac");
    LOGE(&logger, loge<>::CRITICAL, "UDP socket %d %s", 10, "grokking");

    /* Disconnect from TCP socket */
    logger.disconnect();
  }

  /* Reuse for writing to TCP socket */
  if (logger.connect("::1", 8889, 1, 1)) {
  //if (logger.connect("127.0.0.1", 8889, 1, 0)) {

    LOGE(&logger, loge<>::INFO, "TCP socket %d %s", 10, "foo");
    LOGE(&logger, loge<>::DEBUG, "TCP socket %d %s", 10, "bar");
    LOGE(&logger, loge<>::WARNING, "TCP socket %d %s", 10, "baz");
    LOGE(&logger, loge<>::ERROR, "TCP socket %d %s", 10, "pebkac");
    LOGE(&logger, loge<>::CRITICAL, "TCP socket %d %s", 10, "grokking");

    /* Disconnect from TCP socket */
    logger.disconnect();
  }

  /* Set log callback function to default logging member function */
  logger.unset_logfn();
  /* Set the ostream pointer to nullptr */
  logger.unset_ostream();
  /* Never logged */
  LOGE(&logger, loge<>::INFO, "Never %d %s", 10, "foo");
  LOGE(&logger, loge<>::DEBUG, "Never %d %s", 10, "bar");
  LOGE(&logger, loge<>::WARNING, "Never %d %s", 10, "baz");
  LOGE(&logger, loge<>::ERROR, "Never %d %s", 10, "pebkac");
  LOGE(&logger, loge<>::CRITICAL, "Never %d %s", 10, "grokking");

  /* Log using custom log function with formatted log message */
#if defined(__linux__) || defined(__linux) || defined(__FreeBSD__) \
  || defined(__OpenBSD__)
  fd_logger fdlogger(STDOUT_FILENO, fd_logger_base::ALL);
#elif defined(_MSC_VER)
  fd_logger fdlogger(_fileno(stdout), fd_logger_base::ALL);
#else
  fd_logger fdlogger(fileno(stdout), fd_logger_base::ALL);
#endif
  LOGE_COLOR(&fdlogger, fd_logger_base::INFO,
      "Stdout fileno %d %s", 10, "foo");
  LOGE_COLOR(&fdlogger, fd_logger_base::DEBUG,
      "Stdout fileno %d %s", 10, "bar");
  LOGE_COLOR(&fdlogger, fd_logger_base::WARNING,
      "Stdout fileno %d %s", 10, "baz");
  LOGE_COLOR(&fdlogger, fd_logger_base::ERROR,
      "Stdout fileno %d %s", 10, "pebkac");
  LOGE_COLOR(&fdlogger, fd_logger_base::CRITICAL,
      "Stdout fileno %d %s", 10, "grokking");

  /* Log using cutom log function with unformatted log data */
  custom_logger customlogger(custom_logger_base::ALL);
  LOGE(&customlogger, custom_logger_base::INFO,
      "Custom log function with unformatted log data %d %s", 10, "foo");
  LOGE(&customlogger, custom_logger_base::DEBUG,
      "Custom log function with unformatted log data %d %s", 10, "bar");
  LOGE(&customlogger, custom_logger_base::WARNING,
      "Custom log function with unformatted log data %d %s", 10, "baz");
  LOGE(&customlogger, custom_logger_base::ERROR,
      "Custom log function with unformatted log data %d %s", 10, "pebkac");
  LOGE(&customlogger, custom_logger_base::CRITICAL,
      "Custom log function with unformatted log data %d %s", 10, "grokking");

#if defined(__linux) || defined(__linux__)

  /* Reuse to log to syslog */
  logger.set_syslog(LOG_USER | LOG_NOTICE);
  LOGE(&logger, loge<>::INFO, "Reused for syslog %d %s", 10, "foo");
  LOGE(&logger, loge<>::DEBUG, "Reused for syslog %d %s", 10, "bar");
  LOGE(&logger, loge<>::WARNING, "Reused for syslog %d %s", 10, "baz");
  LOGE(&logger, loge<>::ERROR, "Reused for syslog %d %s", 10, "pebkac");
  LOGE(&logger, loge<>::CRITICAL, "Reused for syslog %d %s", 10, "grokking");

#endif

  /* Demo for insertion operator */
  loge<true> log(&std::cerr);

  std::time_t t = std::time(nullptr);
  struct tm tm = *std::localtime(&t);
  std::string prefix = "test log: ";

  /* Reset message buffer */
  log.reset();

  /* Use loge<>::endl to flush message buffer */
  log << prefix << "hello: " << 121  << '-' << 2025L << " : " <<
    loge<>::setw(loge<>::constants::NUMBER_WIDTH) << 312 << " : "
    << tm << ": " << loge<>::setw(6) << 1970 << loge<>::endl;

  long l = 0xffffffffffffffff;
  unsigned long u = 0xffffffffffffffff;
  std::size_t s = 0xffffffffffffffff;
  log << "integers: " << loge<>::setw(24) << ' ' << l << ' ' << u << ' ' << s;
  /* Write message to ostream */
  log.flush();

  float f = 2747.33333333;
  double d = 333.33333333;
  log << "fractions: " << loge<>::setw_default() << 312.3145926535 << " "
    << loge<>::setw(12) << loge<>::setprecision(6) << " " << f << " " << d;

  /* Write message to ostream */
  log.flush();

  return 0;
}
