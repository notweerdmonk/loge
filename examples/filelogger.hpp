#ifndef FILELOGGER_HPP
#define FILELOGGER_HPP

#include <cstdio>
#include <loge.hpp>

using file_logger_base = loge<true>;

class file_logger final: public file_logger_base {

  public:

  file_logger(const std::string &path, file_logger::loge_level loglevel)
    : loge(loglevel) {

    set_file(path, true);
  }
};

#endif /* FILELOGGER_HPP */
