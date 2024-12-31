#if defined(__linux) || defined(__linux__)
#include <unistd.h>
#endif
#include <loge.hpp>
#include <filelogger.hpp>
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

#if defined(__linux__) || defined(__linux)
  fd_logger fdlogger(STDOUT_FILENO, fd_logger_base::ALL);
#elif defined(_MSC_VER)
  fd_logger fdlogger(_fileno(stdout), fd_logger_base::ALL);
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

  file_logger filelogger("./cctest.log", file_logger_base::ALL);

  LOGE(&filelogger, file_logger_base::INFO, "File %d %s", 10, "foo");
  LOGE(&filelogger, file_logger_base::DEBUG, "File %d %s", 10, "bar");
  LOGE(&filelogger, file_logger_base::WARNING, "File %d %s", 10, "baz");
  LOGE(&filelogger, file_logger_base::ERROR, "File %d %s", 10, "pebkac");
  LOGE(&filelogger, file_logger_base::CRITICAL, "File %d %s", 10, "grokking");

  /*
   * Call unset_file() to free the ofstream object associated with output file
   */
  filelogger.unset_file();

  LOGE(&filelogger, file_logger_base::INFO, "This will cause an error message");

  filelogger.set_stderr();
  filelogger.set_level(file_logger_base::CRITICAL);

  LOGE_COLOR(&filelogger, file_logger_base::INFO,
      "Filter by importance level %d %s", 10, "foo");
  LOGE_COLOR(&filelogger, file_logger_base::DEBUG,
      "Filter by importance level %d %s", 10, "bar");
  LOGE_COLOR(&filelogger, file_logger_base::WARNING,
      "Filter by importance level %d %s", 10, "baz");
  LOGE_COLOR(&filelogger, file_logger_base::ERROR,
      "Filter by importance level %d %s", 10, "pebkac");
  LOGE_COLOR(&filelogger, file_logger_base::CRITICAL,
      "Filter by importance level %d %s", 10, "grokking");

  filelogger.set_level(file_logger_base::ALL);

  /* Any logger object can be reused to log to an open file descriptor */
#if defined(__linux__) || defined(__linux)
  filelogger.set_fd(STDOUT_FILENO);
#elif defined(_MSC_VER)
  filelogger.set_fd(_fileno(stdout));
#endif

  LOGE_COLOR(&filelogger, file_logger_base::INFO,
      "Reused as stdio %d %s", 10, "foo");
  LOGE_COLOR(&filelogger, file_logger_base::DEBUG,
      "Reused as stdio %d %s", 10, "bar");
  LOGE_COLOR(&filelogger, file_logger_base::WARNING,
      "Reused as stdio %d %s", 10, "baz");
  LOGE_COLOR(&filelogger, file_logger_base::ERROR,
      "Reused as stdio %d %s", 10, "pebkac");
  LOGE_COLOR(&filelogger, file_logger_base::CRITICAL,
      "Reused as stdio %d %s", 10, "grokking");

  filelogger.unset_fd();

  /* Any logger object can be reused to log to stdout or stderr */
  filelogger.set_stdout();

  LOGE_COLOR(&filelogger, file_logger_base::INFO,
      "Stdout filestream %d %s", 10, "foo");
  LOGE_COLOR(&filelogger, file_logger_base::DEBUG,
      "Stdout filestream %d %s", 10, "bar");
  LOGE_COLOR(&filelogger, file_logger_base::WARNING,
      "Stdout filestream %d %s", 10, "baz");
  LOGE_COLOR(&filelogger, file_logger_base::ERROR,
      "Stdout filestream %d %s", 10, "pebkac");
  LOGE_COLOR(&filelogger, file_logger_base::CRITICAL,
      "Stdout filestream %d %s", 10, "grokking");

  filelogger.set_stderr();

  LOGE(&filelogger, file_logger_base::INFO,
      "Stderr filestream %d %s", 10, "foo");
  LOGE(&filelogger, file_logger_base::DEBUG,
      "Stderr filestream %d %s", 10, "bar");
  LOGE(&filelogger, file_logger_base::WARNING,
      "Stderr filestream %d %s", 10, "baz");
  LOGE(&filelogger, file_logger_base::ERROR,
      "Stderr filestream %d %s", 10, "pebkac");
  LOGE(&filelogger, file_logger_base::CRITICAL,
      "Stderr filestream %d %s", 10, "grokking");

#if defined(__linux) || defined(__linux__)

  /* Reuse to log to syslog */
  filelogger.set_syslog(LOG_USER | LOG_NOTICE);

  LOGE(&filelogger, file_logger_base::INFO,
      "Reused for syslog %d %s", 10, "foo");
  LOGE(&filelogger, file_logger_base::DEBUG,
      "Reused for syslog %d %s", 10, "bar");
  LOGE(&filelogger, file_logger_base::WARNING,
      "Reused for syslog %d %s", 10, "baz");
  LOGE(&filelogger, file_logger_base::ERROR,
      "Reused for syslog %d %s", 10, "pebkac");
  LOGE(&filelogger, file_logger_base::CRITICAL,
      "Reused for syslog %d %s", 10, "grokking");

#endif

  /* Sets logfnptr to default logging member function */
  filelogger.unset_logfn();
  /* Sets the ostream pointer to nullptr */
  filelogger.unset_ostream();

  /* Never logged */
  LOGE(&filelogger, file_logger_base::INFO, "Never %d %s", 10, "foo not"
      "logged");
  LOGE(&filelogger, file_logger_base::DEBUG, "Never %d %s", 10, "bar");
  LOGE(&filelogger, file_logger_base::WARNING, "Never %d %s", 10, "baz");
  LOGE(&filelogger, file_logger_base::ERROR, "Never %d %s", 10, "pebkac");
  LOGE(&filelogger, file_logger_base::CRITICAL, "Never %d %s", 10, "grokking");

  /* Reuse for writing to UDP socket */
  if (filelogger.connect("::1", 8887, 0, 1)) {
    LOGE(&filelogger, file_logger_base::INFO,
        "UDP socket %d %s", 10, "foo");
    LOGE(&filelogger, file_logger_base::DEBUG,
        "UDP socket %d %s", 10, "bar");
    LOGE(&filelogger, file_logger_base::WARNING,
        "UDP socket %d %s", 10, "baz");
    LOGE(&filelogger, file_logger_base::ERROR,
        "UDP socket %d %s", 10, "pebkac");
    LOGE(&filelogger, file_logger_base::CRITICAL,
        "UDP socket %d %s", 10, "grokking");

    /* Disconnect from TCP socket */
    filelogger.disconnect();
  }

  /* Reuse for writing to TCP socket */
  if (filelogger.connect("::1", 8889, 1, 1)) {
  //if (filelogger.connect("127.0.0.1", 8889, 1, 0)) {

    LOGE(&filelogger, file_logger_base::INFO,
        "TCP socket %d %s", 10, "foo");
    LOGE(&filelogger, file_logger_base::DEBUG,
        "TCP socket %d %s", 10, "bar");
    LOGE(&filelogger, file_logger_base::WARNING,
        "TCP socket %d %s", 10, "baz");
    LOGE(&filelogger, file_logger_base::ERROR,
        "TCP socket %d %s", 10, "pebkac");
    LOGE(&filelogger, file_logger_base::CRITICAL,
        "TCP socket %d %s", 10, "grokking");

    /* Disconnect from TCP socket */
    filelogger.disconnect();
  }

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
  loge<false> syslogger(loge<false>::ALL);
  syslogger.set_syslog(LOG_USER | LOG_NOTICE);

  LOGE(&syslogger, loge<false>::INFO, "Syslog %d %s", 10, "foo");
  LOGE(&syslogger, loge<false>::DEBUG, "Syslog %d %s", 10, "bar");
  LOGE(&syslogger, loge<false>::WARNING, "Syslog %d %s", 10, "baz");
  LOGE(&syslogger, loge<false>::ERROR, "Syslog %d %s", 10, "pebkac");
  LOGE(&syslogger, loge<false>::CRITICAL, "Syslog %d %s", 10, "grokking");

  /* Reuse for writing to file */
  syslogger.set_file("./cctest.log", true);

  LOGE(&syslogger, loge<false>::INFO,
      "Reused as file %d %s", 10, "foo");
  LOGE(&syslogger, loge<false>::DEBUG,
      "Reused as file %d %s", 10, "bar");
  LOGE(&syslogger, loge<false>::WARNING,
      "Reused as file %d %s", 10, "baz");
  LOGE(&syslogger, loge<false>::ERROR,
      "Reused as file %d %s", 10, "pebkac");
  LOGE(&syslogger, loge<false>::CRITICAL,
      "Reused as file %d %s", 10, "grokking");

  /* Reset logger */
  syslogger.unset_file();

  /* Logged to syslog */
  LOGE(&syslogger, loge<false>::INFO, "Syslog again %d %s", 10, "foo");
  LOGE(&syslogger, loge<false>::DEBUG, "Syslog again %d %s", 10, "bar");
  LOGE(&syslogger, loge<false>::WARNING, "Syslog again %d %s", 10, "baz");
  LOGE(&syslogger, loge<false>::ERROR, "Syslog again %d %s", 10, "pebkac");
  LOGE(&syslogger, loge<false>::CRITICAL, "Syslog again %d %s", 10, "grokking");
#endif

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

  return 0;
}
