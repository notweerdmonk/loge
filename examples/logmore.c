#include <loge.hpp>

extern struct loge logger;

void log_more_stuff() {
  LOGE(&logger, LOGE_INFO, "This is logged from another file");
}
