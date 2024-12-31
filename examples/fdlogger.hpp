#ifndef FDLOGGER_HPP
#define FDLOGGER_HPP

#include <cstdio>
#include <loge.hpp>

using fd_logger_base = loge<true>;

class fd_logger  : public fd_logger_base {
  int fd_ = -1;

  public:

  void logfn() override {

    /* buffer char array may not be null-terminated */
    buffer[buffer.size() - 1] = '\0';

    dprintf(fd_, "%s\n", buffer.data());
  }

  fd_logger(int fd, fd_logger_base::loge_level loglevel)
    : loge(loglevel) , fd_(fd) {
  }
};

#endif /* FDLOGGER_HPP */
