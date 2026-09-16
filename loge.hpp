/*
  MIT License

  Copyright (c) 2026 notweerdmonk

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

/**
 * @file loge.hpp
 * @author notweerdmonk
 * @brief Log to stdout, stderr, file, file descriptor, syslog, UDP/TCP sockets
 */

#ifndef _LOGE_HPP_
#define _LOGE_HPP_

#if defined(__linux) || defined(__linux__)

#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#define _POSIX_C_SOURCE 1 /* For fdopen */

#ifdef _DEFAULT_SOURCE
#undef _DEFAULT_SOURCE
#endif
#define _DEFAULT_SOURCE 1 /* For syscall */

#ifdef _GNU_SOURCE
#undef _GNU_SOURCE
#endif
#define _GNU_SOURCE 1 /* For reallocarray */


/* linux */
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <linux/seccomp.h>
#include <linux/filter.h>
#include <linux/audit.h>
#include <sys/prctl.h>
#include <sys/syscall.h>

#elif defined(_WIN64)

/* win32 */
#include <io.h>
#include <fcntl.h>
#define _WINSOCKAPI_
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef ERROR
#undef ERROR
#endif

#endif

/* libc */
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

/* glibc and BSD libc only */
#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#include <syslog.h>
#endif

/***************************** Common code starts *****************************/


/* ANSI SGR escape sequences */
#define ANSI_FG_BLACK   "\x1b[30m"
#define ANSI_FG_RED     "\x1b[31m"
#define ANSI_FG_GREEN   "\x1b[32m"
#define ANSI_FG_YELLOW  "\x1b[33m"
#define ANSI_FG_BLUE    "\x1b[34m"
#define ANSI_FG_MAGENTA "\x1b[35m"
#define ANSI_FG_CYAN    "\x1b[36m"
#define ANSI_FG_WHITE   "\x1b[37m"

#define ANSI_BG_BLACK   "\x1b[40m"
#define ANSI_BG_RED     "\x1b[41m"
#define ANSI_BG_GREEN   "\x1b[42m"
#define ANSI_BG_YELLOW  "\x1b[43m"
#define ANSI_BG_BLUE    "\x1b[44m"
#define ANSI_BG_MAGENTA "\x1b[45m"
#define ANSI_BG_CYAN    "\x1b[46m"
#define ANSI_BG_WHITE   "\x1b[47m"

#define ANSI_BOLD       "\x1b[1m"
#define ANSI_UNDERLINE  "\x1b[4m"
#define ANSI_STRIKE     "\x1b[9m"
#define ANSI_SLOW_BLINK "\x1b[5m"

#define ANSI_RESET      "\x1b[0m"
#define ANSI_BG_RESET   "\x1b[49m"


/* Error reporting macros */
#ifndef __cplusplus

#ifdef lgperror

#warning lgperror previously defined

#undef lgperror

#endif

#define lgperror(msg) \
  do { \
    fprintf(stderr, "%s: %d: " msg ": %s\n", \
            __func__, \
            __LINE__, \
            strerror(errno) \
          ); \
  } while (0)

#ifdef lgerror

#warning lgerror previoulsy defined

#undef lgerror

#endif

#include <stdio.h>

#define lgerror(msg, ...) \
  do { \
    fprintf(stderr, "%s: %d: " msg "\n", \
            __func__, \
            __LINE__, \
            __VA_ARGS__ \
          ); \
  } while (0)

#if defined DEBUG_LEVEL && DEBUG_LEVEL >= 2

#ifdef lgdebug

#warning lgdebug previously defined

#undef lgdebug

#endif

#define lgdebug(fmt, ...) \
  do { \
    fprintf(stderr, "%s: %d: " fmt "\n", \
            __func__, \
            __LINE__, \
            __VA_ARGS__ \
          ); \
  } while (0)

#else /* !DEBUG_LEVEL || !DEBUG_LEVEL >= 2 */

#define lgdebug(...)

#endif /* DEBUG_LEVEL && DEBUG_LEVEL >= 2 */

#else /* !__cplusplus */

#ifdef lgperror

#warning lgperror previoulsy defined

#undef lgperror

#endif

#include <iostream>
#include <array>
#include <cstdio>

#define lgperror(msg) \
  do { \
    std::cerr << __func__ << ": " << msg << ": " << strerror(errno) << '\n'; \
  } while (0)

#ifdef lgerror

#warning lgerror previoulsy defined

#undef lgerror

#endif

#define lgerror(fmt, ...) \
  do { \
    enum { LGERROR_BUFFER_SIZE = 1024 }; \
    std::array<char, LGERROR_BUFFER_SIZE> buffer; \
    snprintf(buffer.data(), LGERROR_BUFFER_SIZE, \
        fmt, __VA_ARGS__); \
    buffer[LGERROR_BUFFER_SIZE - 1] = '\0'; \
    std::cerr << __func__ << ": " << buffer.data() << '\n'; \
  } while (0)

#ifdef lgdebug

#warning lgdebug previously defined

#undef lgdebug

#endif

#if defined DEBUG_LEVEL && DEBUG_LEVEL >= 2

#define lgdebug(fmt, ...) \
  do { \
    enum { LGDEBUG_BUFFER_SIZE = 1024 }; \
    std::array<char, LGDEBUG_BUFFER_SIZE> buffer; \
    snprintf(buffer.data(), LGDEBUG_BUFFER_SIZE, \
        fmt, __VA_ARGS__); \
    buffer[LGDEBUG_BUFFER_SIZE - 1] = '\0'; \
    std::cerr << __func__ << ": " << buffer.data() << '\n'; \
  } while (0)

#else /* !DEBUG_LEVEL || !DEBUG_LEVEL >= 2 */

#define lgdebug(...)

#endif /* DEBUG_LEVEL && DEBUG_LEVEL >= 2 */

#endif /* !__cplusplus */

/* Stringification macros */
#define __xstr(s) __tostr(s)
#define __tostr(s) #s

/* Unused variables and functions */
#if __cplusplus >= 201703L

#define UNUSED [[maybe_unused]]

#elif defined(__GNUC__)

#define UNUSED __attribute__((unused))

#else

#define UNUSED

#endif

#define UNUSED_VAR(x) (void)x

/* Adjustment for fdopen */
#if defined(_MSC_VER)

#ifdef dup
#undef dup
#endif
#define dup _dup

#ifdef fdopen
#undef fdopen
#endif
#define fdopen _fdopen

#ifdef close
#undef close
#endif
#define close _close

#elif !defined(_POSIX_C_SOURCE) && !defined(_MSC_VER)

/* Declare fdopen anyway */
extern FILE *fdopen(int fd, const char *mode);

#endif

#define dup_str __xstr(dup)
#define fdopen_str __xstr(fdopen)
#define close_str __xstr(close)

/* Adjustment for seccomp */
#ifdef seccomp
#undef seccomp
#endif
#define seccomp(op, flags, args)  syscall(SYS_seccomp, op, flags, args)


#ifdef __cplusplus

extern "C" {

#endif

/******************* safe_call forward declaration starts *********************/

typedef int (*safe_callee_type)(size_t nargs, va_list va);

enum {
  SAFE_CALL_BLOCK_ARCH_NONE   = 0,
  SAFE_CALL_BLOCK_ARCH_I386   = 1,
  SAFE_CALL_BLOCK_ARCH_x86_64 = 2,

  SAFE_CALL_OFFSET      = 4,
  SAFE_CALL_MASK        = 0x80000000,
  SAFE_CALL_CALLEE_NULL = SAFE_CALL_MASK
};

#define EXP_ARCH_NONE   1
#define EXP_ARCH_I386   2
#define EXP_ARCH_x86_64 3

#ifndef EXP_ARCH
enum {
  GATE_SAFECALL_ARCH_BLOCK = SAFE_CALL_BLOCK_ARCH_I386 | SAFE_CALL_BLOCK_ARCH_x86_64
};
#elif EXP_ARCH == EXP_ARCH_I386
enum {
  GATE_SAFECALL_ARCH_BLOCK = SAFE_CALL_BLOCK_ARCH_x86_64
};
#elif EXP_ARCH == EXP_ARCH_x86_64
enum {
  GATE_SAFECALL_ARCH_BLOCK = SAFE_CALL_BLOCK_ARCH_I386
};
#else
enum {
  GATE_SAFECALL_ARCH_BLOCK = 0
};
#endif

enum {
  GATE_SAFECALL_SYSCALLS_MASK = SIZE_MAX
};

#define BPF_JMP_OFFSET_CAST(...) ((uint8_t)(__VA_ARGS__))

#define SAFE_CALL_STATUS(argidx) \
  (SAFE_CALL_MASK | ((argidx) + 1) << SAFE_CALL_OFFSET)

static
void safe_call_wait_status(
    int status,
    const char *msgexit,
    const char *msgterm,
    const char *msgstop
);

static
unsigned int
safe_call(
    safe_callee_type callee,
    const size_t n_block_syscalls,
    const unsigned int *syscalls,
    uint64_t block_arch,
    const size_t nchecks,
    const size_t nargs,
    ...
);

/* List of syscalls to be blocked */
static
const unsigned int nr_syscalls[] = {

#if defined(__linux) || defined(__linux__)

  __NR_ptrace,
  __NR_execve,
  __NR_execveat,
  __NR_fork,
  __NR_vfork,
  __NR_clone,
  __NR_setuid,
  __NR_setgid,
  __NR_setresuid,
  __NR_setresgid,
  __NR_ioctl,
  __NR_chroot,
  /* Allow write */
#if !defined(EXP_SYSCALLS) && 0
  __NR_write, /* TODO: Use syscall args filter for such syscalls */
#endif
  __NR_umask,
  __NR_mount,
  __NR_umount2,
  __NR_bind,
  __NR_listen,
  __NR_accept,
  __NR_sendto,
  __NR_recvfrom,
  __NR_sendmsg,
  __NR_recvmsg,
#ifndef EXP_SYSCALLS
  __NR_sendfile,
#endif
  __NR_kill,
  __NR_tgkill,
  __NR_tkill,
  __NR_fchmod,
  __NR_fchown,
  __NR_chmod,
  __NR_chown,
  __NR_rename,
  __NR_renameat,
  __NR_getdents,
  __NR_getdents64,
  __NR_prctl
#ifndef EXP_SYSCALLS
  ,__NR_mprotect,
#endif

#else

  0

#endif

};

/******************** safe_call forward declaration ends **********************/

/**
 * It is advisable to have only one bit set in the mask argument to enable the
 * compiler ot generate a BTR instruction.
 */
/* TODO: achieve real BTR in assembly */
static
inline
uint64_t btr(uint64_t *value, uint64_t mask) {
  if (!value) {
    return 0;
  }

  uint64_t old = *value & mask;

  *value &= ~mask;

  return old;
}

#if defined(__linux) || defined(__linux__)

typedef int native_handle;

/**
 * @brief Opaque type representing a network socket
 */
typedef int socket_type;

enum { LOGE_SOCK_ERR = -1 };

static
inline
native_handle socket_to_native(socket_type sock) {
  return (native_handle)sock;
}

static
inline
socket_type native_to_socket(native_handle sock) {
  return (socket_type)sock;
}

static
inline
void shutdown_socket(socket_type sock) {
  shutdown(socket_to_native(sock), SHUT_RDWR);
}

static
inline
void destroy_socket(socket_type sock) {
  close(socket_to_native(sock));
}

#else /* defined(__linux) || defined(__linux__) */

#pragma comment(lib, "ws2_32.lib")

typedef int native_handle;

typedef SOCKET socket_type;

enum { LOGE_SOCK_ERR = SOCKET_ERROR };

static
inline
native_handle socket_to_native(socket_type sock) {
  return _open_osfhandle((intptr_t)sock, _O_RDONLY | _O_BINARY);
}

static
inline
socket_type native_to_socket(native_handle sock) {
  return _get_osfhandle(sock);
}

static
inline
void shutdown_socket(socket_type sock) {
  shutdown(sock, SD_BOTH);
}

static
inline
void destroy_socket(socket_type sock) {
  closesocket(sock);
}

#endif

/**
 * @brief Replace all occurences of a substring with anohter string. On success
 * a new dynamically allocated string is returned that should be freed after
 * use.
 *
 * https://stackoverflow.com/a/779960
 *
 * @param str The string in which the replacement happens
 * @param pat The substring that will get replaced
 * @param rep The string that will replace the substring
 * @return NULL on failure, pointer to a dynamically allocated string on
 * success
 */
UNUSED
static
char* strreplace(const char *str, const char *pat, const char *rep) {
  const char *start;
  char *p;
  char *result;
  size_t patlen;
  size_t replen;
  size_t reslen;
  size_t offset;
  size_t count;

  if (!str || !pat) {
    return NULL;
  }

  patlen = strlen(pat);
  if (!patlen) {
    return NULL;
  }

  if (!rep) {
    rep = "";
  }
  replen = strlen(rep);

  start = str;
  for (count = 0; (p = strstr((char*)start, pat)); count++) {
    start = p + patlen;
  }

  reslen = strlen(str) + (replen - patlen) * count;
  if (!reslen) {
    return NULL;
  }

  p = result = (char*)malloc(reslen + 1);
  if (!result) {
    return NULL;
  }

  while (count--) {
    start = strstr(str, pat);
    offset = start - str;
    p = strncpy(p, str, offset) + offset;
    p = strncpy(p, rep, replen) + replen;
    str += offset + patlen;
    reslen -= offset + replen;
  }
  strncpy(p, str, reslen);
  p[reslen] = '\0';

  return result;
}

#if defined(_MSC_VER) || !defined(__GLIBC__)

static
int dprintf(int d, const char *fmt, ...) {
 enum { BUFFER_SIZE_DPRINTF = 4096 };

 va_list ap;
 int done;
 int at = 0, nwrite = 0;
 char *buf;

 buf = (char*)malloc(sizeof(char) * BUFFER_SIZE_DPRINTF);
 if (!buf) {
   return 0;
 }

 va_start(ap, fmt);
 done = vsnprintf(buf, BUFFER_SIZE_DPRINTF, fmt, ap);
 va_end(ap);

 do {
   nwrite = _write(d, buf + at, done);
   at += nwrite;
   done -= nwrite;
  } while (done > 0);

 free(buf);

 return at;
}

#endif

/**
 * @brief Connect to a TCP server and associate an output stream with the
 * socket. This stream will be set as the output stream for the logger. The log
 * callback function will be overriden by the default. IPv4 and IPv6 addresses
 * are supported.
 * @param ploge Pointer to struct loge
 * @param ip IPv4 or IPv6 address
 * @param port Port number
 * @param ipv6 Uses IPv6 if set to non-zero value, else uses IPv4
 *
 * @see loge_disconnect()
 */
static
socket_type sock_connect(const char *host, unsigned short port, int type, int ipv6) {

  socket_type sock = LOGE_SOCK_ERR;

  if (!host) {
    return sock;
  }

#if defined(__linux) || defined(__linux__)

  struct sockaddr_in server_addr;
  struct sockaddr_in6 server_addr6;

  int domain = ipv6 ? AF_INET6 : AF_INET;

  if ((sock = socket(domain, type > 0 ? SOCK_STREAM : SOCK_DGRAM, 0)) < 0) {
    lgperror("socket failed");
    return sock;
  }

  if (ipv6) {
    memset(&server_addr6, 0, sizeof(struct sockaddr_in6));
    server_addr6.sin6_family = AF_INET6;
    server_addr6.sin6_port = htons(port);
  } else {
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
  }

  struct sockaddr *addr = ipv6 ?
    (struct sockaddr *)&server_addr6 :
    (struct sockaddr *)&server_addr;

  size_t addr_size = ipv6 ?
    sizeof(struct sockaddr_in6) :
    sizeof(struct sockaddr_in);

  int ret;
  /* Convert IPv4/IPv6 address from text to binary form */
  if (ipv6) {
    ret = inet_pton(AF_INET6, host, &server_addr6.sin6_addr);
  } else {
    ret = inet_pton(AF_INET, host, &server_addr.sin_addr);
  }
  if (ret <= 0) {
    lgperror("inet_pton failed");
    goto error;
  }

  if (connect(sock, addr, addr_size) == 0) {
    return sock;
  } else {
    lgperror("connect failed");
  }

error:
  close(sock);
  sock = LOGE_SOCK_ERR;

#elif defined(_WIN64)

  WSADATA wsadata;

  int ret = WSAStartup(MAKEWORD(2,2), &wsadata);

  struct sockaddr_in server_addr;
  struct sockaddr_in6 server_addr6;

  int domain = ipv6 ? AF_INET6 : AF_INET;

  if ((sock = socket(domain, type > 0 ? SOCK_STREAM : SOCK_DGRAM, 0)) < 0) {
    lgperror("socket failed");
    return sock;
  }

  if (ipv6) {
    memset(&server_addr6, 0, sizeof(struct sockaddr_in6));
    server_addr6.sin6_family = AF_INET6;
    server_addr6.sin6_port = htons(port);
  } else {
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
  }

  struct sockaddr *addr = ipv6 ?
    (struct sockaddr *)&server_addr6 :
    (struct sockaddr *)&server_addr;

  socklen_t addr_size = ipv6 ?
    sizeof(struct sockaddr_in6) :
    sizeof(struct sockaddr_in);

  /* Convert IPv4/IPv6 address from text to binary form */
  if (ipv6) {
    ret = inet_pton(AF_INET6, host, &server_addr6.sin6_addr);
  } else {
    ret = inet_pton(AF_INET, host, &server_addr.sin_addr);
  }
  if (ret <= 0) {
    lgperror("inet_pton failed");
    goto error;
  }

  if (connect(sock, addr, addr_size) != SOCKET_ERROR) {
    return sock;
  } else {
    lgperror("connect failed");
  }

error:
  closesocket(sock);
  WSACleanup();
  sock = LOGE_SOCK_ERR;

#endif

  return sock;
}

static
const char *loglevel_strtbl[] =  {
  "",
  "DEBUG",
  "INFO",
  "WARNING",
  "ERROR",
  "CRITICAL"
};

static
const char *loglevel_strtbl_color[] =  {
  "",
  ANSI_FG_BLUE   ANSI_BG_RESET "DEBUG"    ANSI_RESET,
  ANSI_FG_GREEN  ANSI_BG_RESET "INFO"     ANSI_RESET,
  ANSI_FG_YELLOW ANSI_BG_RESET "WARNING"  ANSI_RESET,
  ANSI_FG_RED    ANSI_BG_RESET "ERROR"    ANSI_RESET,
  ANSI_FG_WHITE  ANSI_BG_RED   "CRITICAL" ANSI_RESET
};

/****************************** Common code ends ******************************/


/******************************* C code starts ********************************/

#ifndef __cplusplus

/**
 * @brief Macro to be used for logging information.
 * @param ploge Pointer to struct loge.
 * @param level Log type/level. Messages below this level will not be logged.
 * @param ... Message format string and associated arguments.
 */
#define LOGE(ploge, level, ...) \
  do { \
    struct loge *p = (ploge); \
    if (p != NULL) \
      loge_log( \
          p, \
          (level) & ~LOGCOLOR, \
          __LINE__, \
          __FILE__, \
          __VA_ARGS__ \
        ); \
  } while (0)

#define LOGE_COLOR(ploge, level, ...) \
  do { \
    struct loge *p = (ploge); \
    if (p) \
      loge_log( \
          p, \
          (level) | LOGCOLOR, \
          __LINE__, \
          __FILE__, \
          __VA_ARGS__ \
        ); \
  } while (0)

#define LOGE_TYPE(entime, level) \
  (int)( ( (!!(entime)) << LOGTIMESTAMPSHIFT ) | (level) )

#define LOGE_ENTIME(type) \
  (int)(!!(type & LOGTIMESTAMP))

#define LOGE_LEVEL(type) \
  (enum loge_level)(type & ~LOGTIMESTAMP)

#define LOGE_LOGTYPE(encolor, level) \
  (int)( ( (!!(encolor)) << LOGCOLORSHIFT ) | (level) )

#define LOGE_ENCOLOR(type) \
  (int)(!!(type & LOGCOLOR))

#define LOGE_LOGLEVEL(type) \
  (enum loge_level)(type & ~LOGCOLOR)

enum loge_constants {
  LINENUMBER_WIDTH = 6,
  NUMBER_WIDTH = 8,
  BUFFER_SIZE = 1024,
  LOGTIMESTAMPSHIFT = 31,
  LOGCOLORSHIFT = 31,
  LOGTIMESTAMP = 1 << LOGTIMESTAMPSHIFT,
  LOGCOLOR = 1 << LOGCOLORSHIFT
};

/**
 * @brief Enum type for levels of logging. Each level can represent a class of
 * information that is to be logged
 *
 * The log level can be used to filter messages. All messages that have log
 * type/level less that the specified type/level will not get logged.
 */
enum loge_level {
  LOGE_ALL = 0,  /**< Everything */
  LOGE_DEBUG,     /**< Debug information */
  LOGE_INFO,      /**< Run time information like statuses, user output, etc */
  LOGE_WARNING,   /**< Warnings */
  LOGE_ERROR,     /**< Errors */
  LOGE_CRITICAL,  /**< Critical errors */
  LOGE_MAX
};

/* Forward declaration */
struct loge;

/**
 * @brief Defines the type for function used as log callback function.
 * @param ploge Pointer to struct loge
 *
 * @see loge_set_fn()
 * @see loge_log()
 */
typedef void (*log_fn)(const struct loge *ploge);

/**
 * @brief Defines type for function used as raw data callback function.
 * @param ploge,Pointer to struct loge
 * @param file Output stream of the logger
 * @param timestamp Number of seconds since Epoch
 * @param filename Name of the source file
 * @param linenum Line number of the source file from where logging happened
 * @param level Type/level of log
 * @param msg User message. This is the result of applying arguments to the
 * format string passed to loge_log()
 *
 * @see loge_set_data_fn()
 * @see loge_log()
 */
typedef void (*log_data_fn)(
    FILE *file,
    time_t timestamp,
    const char *filename,
    int linenum,
    enum loge_level level,
    const char *msg
  );

struct loge {
  char *bufptr;
  char buffer[BUFFER_SIZE];
  size_t bufcap;
  size_t buflen;
  log_fn plogfn, pprevlogfn;
  log_data_fn pdatafn;
  FILE *file;
  socket_type sockfd;
  int log_type;
  int linenumwidth;
  int width;
  int precision;
  int syslog_priority;
};

static
int loge_logfn_va(size_t nargs UNUSED, va_list va) {
  const struct loge *ploge = va_arg(va, struct loge*);

  ploge->plogfn(ploge);

  return 0;
}

static
int loge_datafn_va(size_t nargs UNUSED, va_list va) {
  const struct loge *ploge = va_arg(va, const struct loge*);
  FILE *file = va_arg(va, FILE*);
  time_t timestamp = va_arg(va, time_t);
  const char *filename = va_arg(va, const char*);
  int linenum = va_arg(va, int);
  enum loge_level level = va_arg(va, enum loge_level);
  const char *msg = va_arg(va, const char*);

  ploge->pdatafn(
      file,
      timestamp,
      filename,
      linenum,
      level,
      msg
    );

  return 0;
}

/**
 * @brief Get pointer to a string containing the name of the log type/level
 * @param level Enum mentioning the log type/level
 * @return Pointer to a string
 *
 * @see enum loge_level
 */
UNUSED
static
const char* loge_get_level(enum loge_level level) {
  return level > LOGE_ALL && level < LOGE_MAX ?
    loglevel_strtbl[level] :
    NULL;
}

/**
 * @brief Get pointer to a string containing the name of the log type/level
 * along with ANSI escape codes for colored output
 * @param level Enum mentioning the log type/level
 * @return Pointer to a string
 *
 * @see enum loge_level
 */
UNUSED
static
const char* loge_get_level_color(enum loge_level level) {
  return level > LOGE_ALL && level < LOGE_MAX ?
    loglevel_strtbl_color[level] :
    NULL;
}

UNUSED
static
const char* loge_bufptr(const struct loge *ploge) {
  return !ploge ? (const char*)ploge : ploge->bufptr;
}

UNUSED
static
FILE* loge_fileptr(const struct loge *ploge) {
  return !ploge ? (FILE*)ploge : ploge->file;
}

/**
 * @brief Write a string followed by a newline character to a stream. This is
 * the default callback function.
 * @param ploge Pointer to struct loge
 */
UNUSED
static
void logfn_internal(const struct loge *ploge) {
  if (!ploge) {
    return;
  }

  FILE *file = loge_fileptr(ploge);
  if (!file) {
    return;
  }

  fputs(loge_bufptr(ploge), file);
  fputc('\n', file);
  fflush(file);
}

/* glibc and BSD libc only */
#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__)

UNUSED
static
void logfn_syslog(const struct loge *ploge) {
  if (!ploge) {
    return;
  }

  const char *msg = loge_bufptr(ploge);
  syslog(ploge->syslog_priority, "%s\n", msg);
}

#endif

/**
 * @brief Set the log type/level to be filtered by the logger. Messages below
 * this level will not be logged.
 * @param ploge Pointer to struct loge
 * @param level Enum mentioning the log type/level
 *
 * @see enum loge_level
 */
UNUSED
static
void loge_set_level(struct loge *ploge, enum loge_level level) {
  if (!ploge) {
    return;
  }

  int en_timestamp = LOGE_ENTIME(ploge->log_type);

  if (level < LOGE_MAX) {
    ploge->log_type = LOGE_TYPE(en_timestamp, level);
  }
}

/**
 * @brief Set the callback function to be used for writing the log message to
 * a stream. This will override previous callback function if a non-NULL
 * address is passed.
 *
 * The output stream and log message are passed on to this callback function.
 * Log string provided to this function will be formatted as:
 * m-dd-yyyy : HH-mm-ss : filename:linenum : loglevel : user_message
 *
 * User message will be the result of applying arguments to a format string.
 *
 * @param ploge Pointer to struct loge
 * @param fn Pointer to the callback function
 *
 * @see log_fn
 * @see loge_log()
 */
UNUSED
static
void loge_set_fn(struct loge *ploge, log_fn fn) {
  if (!ploge || !fn) {
    return;
  }

  ploge->pprevlogfn = ploge->plogfn;
  ploge->plogfn = fn;
}

/**
 * @brief Set the callback function that can be used alternatively for writing
 * the the log message to a stream. This will override previous callback
 * function even when a NULL address is passed. Additional arguments are passed
 * to this callback function.
 * @param ploge Pointer to struct loge
 * @param fn Pointer to the callback function
 *
 * @see log_data_fn
 * @see loge_log()
 */
UNUSED
static
void loge_set_data_fn(struct loge *ploge, log_data_fn fn) {
  if (!ploge) {
    return;
  }

  ploge->pdatafn = fn;
}

UNUSED
static
FILE* loge_set_fd(struct loge *ploge, int fd) {
  if (!ploge) {
    return NULL;;
  }

  FILE *prev = ploge->file;

  if (fd < 0) {
    return prev;
  }

  int newfd = dup(fd);
  if (newfd < 0) {
    lgperror(dup_str" failed");
    return prev;
  }
  FILE *file = fdopen(newfd, "w");
  if (!file) {
    lgperror(fdopen_str" failed");
    close(newfd);
    return prev;
  }

  ploge->file = file;

  ploge->pprevlogfn = ploge->plogfn;
  ploge->plogfn = &logfn_internal;

  return prev;
}

UNUSED
static
void loge_unset_fd(struct loge *ploge) {
  if (!ploge) {
    return;
  }

  if (ploge->file != stdout && ploge->file != stderr) {
    fclose(ploge->file);
  }

  ploge->file = NULL;

  ploge->plogfn = ploge->pprevlogfn;
}

/**
 * @brief Set the stream to which log messages will be written.
 * @param ploge Pointer to struct loge
 * @param file Pointer to the stream
 */
UNUSED
static
FILE* loge_set_fileptr(struct loge *ploge, FILE *file) {
  if (!ploge) {
    return NULL;
  }

  FILE *prev = ploge->file;
  if (file) {
    ploge->file = file;
  }
  return prev;
}

UNUSED
static
FILE* loge_unset_fileptr(struct loge *ploge) {
  if (!ploge) {
    return NULL;
  }

  FILE *prev = ploge->file;
  ploge->file = NULL;
  return prev;
}

/**
 * @brief Set the stdout stream as the output stream. The default callback
 * function will also be set. User may override it.
 * @param ploge Pointer to struct loge
 */
UNUSED
static
FILE* loge_set_stdout(struct loge *ploge) {
  if (!ploge) {
    return NULL;
  }

  ploge->pprevlogfn = ploge->plogfn;
  ploge->plogfn = &logfn_internal;

  FILE *prev = ploge->file;
  ploge->file = stdout;
  return prev;
}

/**
 * @brief Set the stderr stream as the output stream. The default callback
 * function will also be set. User may override it.
 * @param ploge Pointer to struct loge
 */
UNUSED
static
FILE* loge_set_stderr(struct loge *ploge) {
  if (!ploge) {
    return NULL;
  }

  ploge->pprevlogfn = ploge->plogfn;
  ploge->plogfn = &logfn_internal;

  FILE *prev = ploge->file;
  ploge->file = stderr;
  return prev;
}

/* glibc and BSD libc only */
#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__)

UNUSED
static
FILE* loge_set_syslog(struct loge *ploge, int priority) {
  if (!ploge) {
    return NULL;
  }

  FILE *prev = ploge->file;

  /* Clear FILE pointer and pdatafn */
  ploge->file = NULL;
  ploge->pdatafn = NULL;

  ploge->syslog_priority = priority;

  ploge->pprevlogfn = ploge->plogfn;
  ploge->plogfn = &logfn_syslog;

  return prev;
}

#endif

/**
 * @brief Set file which will be created or appended to, for writing log
 * messages. The file will be opened and a stream will be associated with it.
 * This stream will be set as the output stream for the logger. The default
 * callback function will also be set. User may override it.
 * @param ploge pointer to struct loge
 * @param filepath Relative or absolute path of the output file
 */
UNUSED
static
FILE* loge_set_file(struct loge *ploge, const char *filepath) {
  if (!ploge) {
    return NULL;
  }

  FILE *prev = ploge->file;

  if (!filepath) {
    return prev;
  }

  int fd = -1;
  FILE *file = NULL;

#if defined(__linux) || defined(__linux__)

  fd = open(filepath,
      O_WRONLY|O_CREAT|O_APPEND|O_SYNC,
      S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  if (!fd) {
    lgperror("open failed");
    return prev;
  }

  file = fdopen(fd, "w");
  if (!file) {
    lgperror("fdopen failed");
    if (fd > -1) {
      close(fd);
    }
    return prev;
  }

#else

  file = fopen(filepath, "a");
  if (!file) {
    lgperror("fopen failed");
    return prev;
  }

#endif

  ploge->file = file;

  ploge->pprevlogfn = ploge->plogfn;
  ploge->plogfn = &logfn_internal;

  return prev;
}

/**
 * @brief Close the output file stream and restore the callback function.
 * @param ploge pointer to struct loge
 */
UNUSED
static
void loge_unset_file(struct loge *ploge) {
  if (!ploge || !ploge->file) {
    return;
  }

  if (ploge->file != stdout && ploge->file != stderr) {
    fclose(ploge->file);
  }

  ploge->file = NULL;

  ploge->plogfn = ploge->pprevlogfn;
}

/* glibc and BSD libc only */
#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__)

UNUSED
static
int loge_set_syslog_priority(struct loge *ploge, int priority) {
  if (!ploge) {
    return -1;
  }

  int prev_priority = ploge->syslog_priority;
  ploge->syslog_priority = priority;

  return prev_priority;
}

UNUSED
static
int loge_syslog_priority(struct loge *ploge) {
  return ploge ? ploge->syslog_priority : -1;
}

#endif

/**
 * @brief Initialize the logger.
 * @param ploge Pointer to struct loge
 * @param max_log_size If 0, a statically allocated buffer with default size of
 * `BUFFER_SIZE` is used, else the buffer is dynamically allocated.
 * @param linenumwidth Width of line number (right aligned and padded on the
 * left with zeroes). Defaults to `NUMBER_WIDTH` if 0 is passed.
 * @param width Width for integers and integral part of decimal numbers.
 * @param precision Precision for fractional part of decimal numbers.
 * @param logtype Type of log, bitmask of enum loge_level OR'd with 0x80000000
 * if timestamp is to be logged. Messages below this level will not be logged.
 * @param file Output stream to write log messages to. Defaults to stdout if
 * NULL * is passed.
 * @param fn Callback function used for writing log messages to output
 * steam. Default callback function will be set if NULL is passed.
 *
 * @see log_fn
 * @see LOGE_TYPE
 */
UNUSED
static
void loge_setup(
    struct loge *ploge,
    size_t max_log_size,
    int linenumwidth,
    int width,
    int precision,
    int log_type,
    FILE *file,
    log_fn fn
  ) {

  if (!ploge) {
    return;
  }

  ploge->bufptr = ploge->buffer;
  ploge->bufcap = BUFFER_SIZE * sizeof(char);

  if (max_log_size > BUFFER_SIZE) {
    char *mem = (char*)malloc(max_log_size * sizeof(char));
    if (!mem) {
      lgerror("malloc for %zu bytes failed: %s",
          max_log_size * sizeof(char),
          strerror(errno));
      return;
    }

    ploge->bufptr = mem;
    ploge->bufcap = max_log_size * sizeof(char);
  }

  ploge->buflen = 0;

  ploge->linenumwidth = LINENUMBER_WIDTH;
  if (linenumwidth > -1) {
    ploge->linenumwidth = linenumwidth;
  }

  ploge->width = -1;
  if (width > -1) {
    ploge->width = width;
  }
  ploge->precision = NUMBER_WIDTH >> 1;
  if (precision > -1) {
    ploge->precision = precision;
  }

  int en_timestamp = LOGE_ENTIME(log_type);
  enum loge_level level = LOGE_LEVEL(log_type);

  ploge->log_type = LOGE_TYPE(en_timestamp, LOGE_INFO);
  if (level < LOGE_MAX) {
    ploge->log_type = LOGE_TYPE(en_timestamp, level);
  }

  ploge->sockfd = -1;

  ploge->file = NULL;

  ploge->pprevlogfn = NULL;
  ploge->plogfn = NULL;
  ploge->pdatafn = NULL;

  /* Set default stream as stdout and use default logger function */
  loge_set_stdout(ploge);

  if (file != NULL) {
    ploge->file = file;
  }

  /* loge_set_stdout() would have set pprevlogfn to NULL */
  if (fn != NULL) {
    ploge->plogfn = fn;
  }

  ploge->syslog_priority = -1;
}

UNUSED
static
void loge_set_default_width(struct loge *ploge) {
  if (!ploge) {
    return;
  }

  ploge->width = -1;
}

UNUSED
static
void loge_set_width(struct loge *ploge, int width) {
  if (!ploge) {
    return;
  }

  if (width > -1) {
    ploge->width = width;
  }
}

UNUSED
static
void loge_set_precision(struct loge *ploge, unsigned int precision) {
  if (!ploge) {
    return;
  }

  ploge->precision = precision;
}

/**
 * @brief Deallocates memory used for internal log buffer if dynamically
 * allocated buffer was opted for.
 * @param ploge Pointer to struct loge
 */
UNUSED
static
void loge_destroy(struct loge *ploge) {
  if (!ploge) {
    return;
  }

#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__)
  if (ploge->syslog_priority > -1) {
    closelog();
  }
#endif

  if ( (!ploge->bufptr) || (ploge->bufptr == ploge->buffer) ) {
    return;
  }

  free(ploge->bufptr);
  ploge->bufptr = ploge->buffer;
}

/**
 * @brief Connect to a TCP server and associate an output stream with the
 * socket. This stream will be set as the output stream for the logger. The log
 * callback function will be overriden by the default. IPv4 and IPv6 addresses
 * are supported.
 * @param ploge Pointer to struct loge
 * @param ip IPv4 or IPv6 address
 * @param port Port number
 * @param ipv6 Use IPv6 if set to non-zero value, else use IPv4
 * @paramm fileptr Pointer for a FILE* which shall store the previous FILE*
 * used by the logger
 *
 * @see loge_disconnect()
 */
UNUSED
static
int loge_connect(struct loge *ploge, const char *host,
    unsigned short port, int type, int ipv6, FILE **fileptr) {

  if (!ploge || !host) {
    return -1;
  }

  socket_type sock = sock_connect(host, port, type, ipv6);
  if (sock == LOGE_SOCK_ERR) {
    return -1;
  }

  FILE *file = fdopen(socket_to_native(sock), "w");
  if (!file) {
    lgperror(fdopen_str" failed");
    destroy_socket(sock);
    return -1;
  }

  ploge->sockfd = sock;

  ploge->pprevlogfn = ploge->plogfn;
  ploge->plogfn = &logfn_internal;

  FILE *prev = loge_set_fileptr(ploge, file);
  if (fileptr) {
    *fileptr = prev;
  }

  return 0;
}

/**
 * @brief Disconnect from TCP server. Associated output stream gets closed
 * along with the underlying socket. The output stream pointer is set to NULL
 * and needs to be set afterwards.
 * @param ploge Pointer to struct loge
 *
 * @see loge_connect()
 */
UNUSED
static
void loge_disconnect(struct loge *ploge) {
  if (!ploge) {
    return;
  }

  if (ploge->sockfd == -1) {
    return;
  }

  /* Stop data communication */
  shutdown_socket(ploge->sockfd);

  /* fclose will close the underlying socket fd on linux */
  fclose(ploge->file);

  ploge->file = NULL;
  ploge->sockfd = -1;

  ploge->plogfn = ploge->pprevlogfn;
}

UNUSED
static
void loge_reset_logfn(struct loge *ploge) {
  if (!ploge) {
    return;
  }

  ploge->pprevlogfn = ploge->plogfn;
  ploge->plogfn = &logfn_internal;
}

UNUSED
static
void loge_reset(struct loge *ploge) {
  if (!ploge) {
    return;
  }

  ploge->buflen = 0;
}

UNUSED
static
void loge_flush(struct loge *ploge) {
  if (!ploge) {
    return;
  }

  if (ploge->plogfn) {
    ploge->plogfn(ploge);
  } else {
    lgerror("log callback not set for logger %p", ploge);
  }

  loge_reset(ploge);
}

/**
 * @brief Log information as string, to an output stream.
 *
 * If raw data callback has been set this function will be called. The function
 * shall be called with the following arguments:
 *
 * Otherwise the log message callback will be called. Log message will be
 * formatted as:
 * m-dd-yyyy:HH-mm-ss: filename:linenum: loglevel: <user message>
 *
 * User message shall be in the form of a format string followed by arguments.
 *
 * When the logger is connected to a UDP/TCP server, the output stream will be
 * associated with the socket descriptor that will be used for sending UDP/TCP
 * messages.
 *
 * struct loge *ploge       Pointer to struct loge
 * int logtype              Bitmask of enum loge_level for the level of log
 *                          message, and 0x80000000 for colored message
 * int linenum              Line number to log in log message
 * const char *filename     Output file
 * const char *msg          User message format string
 * ...                      Arguments to format string
 *
 * @param ploge Pointer to struct loge
 * @param logtype Type of log, bitmask of enum loge_level OR'd with 0x80000000
 * @param linenum Line number of the source file from where logging happened
 * for colored output
 * @param filename Name of the source file
 * @param msg Format string for user message that will be appended to formatted
 * information
 * @param ... Arguments for user message format string
 *
 * @see log_fn
 * @see log_data_fn
 */
UNUSED
static
void loge_log(
    struct loge *ploge,
    int logtype,
    int linenum,
    const char *filename,
    const char *msg,
    ...
  ) {

  time_t t = time(NULL);
  struct tm localtm = *localtime(&t);

  if (!ploge) {
    return;
  }

  enum loge_level loglevel = LOGE_LOGLEVEL(logtype);
  enum loge_level mylevel = LOGE_LEVEL(ploge->log_type);

  if (loglevel >= LOGE_MAX ||
      loglevel <= LOGE_ALL ||
      loglevel < mylevel) {
    return;
  }

  int en_color = LOGE_ENCOLOR(logtype);

  const char *loglvl_tbl = en_color ?
    loglevel_strtbl_color[loglevel] :
    loglevel_strtbl[loglevel];

  int len = 0;

  if (!ploge->pdatafn) {
    int en_timestamp = LOGE_ENTIME(ploge->log_type);

    if (en_timestamp) {
      len = snprintf(
          ploge->bufptr, ploge->bufcap,
          "%02d-%02d-%04d:%02d:%02d:%02d: %s:%0*d: %-*s: ",
          localtm.tm_mon + 1, localtm.tm_mday, localtm.tm_year + 1900,
          localtm.tm_hour, localtm.tm_min, localtm.tm_sec,
          filename,
          ploge->linenumwidth, linenum,
          en_color ? 22 : 8, loglvl_tbl
        );

    } else {
      len = snprintf(
          ploge->bufptr, ploge->bufcap,
          "%s:%0*d: %-*s: ",
          filename,
          ploge->linenumwidth, linenum,
          en_color ? 22 : 8, loglvl_tbl
        );
    }
  }

  va_list args;
  va_start(args, msg);

  len += vsnprintf(ploge->bufptr + len, ploge->bufcap - len, msg, args);

  va_end(args);

  ploge->buflen = len;

  int ret = 0;

  if (ploge->pdatafn) {
    if (
        (
          ret = safe_call(
            loge_datafn_va,
            sizeof(nr_syscalls) / sizeof(nr_syscalls[0]),
            nr_syscalls,
            GATE_SAFECALL_ARCH_BLOCK,
            2,
            7,
            ploge->file != NULL,
            ploge->bufptr != NULL,
            ploge, ploge->file, t, filename, linenum, loglevel,
            ploge->bufptr
          )
        ) != 0
    ) {

      switch (ret) {
        case 0: {
          break;
        }
        case SAFE_CALL_STATUS(0): {
          lgerror("logger FILE pointer is NULL, ploge: %p, ploge->file: %p",
              ploge, ploge->file);
          break;
        }
        case SAFE_CALL_STATUS(1): {
          lgerror("logger message buffer is NULL, ploge: %p, ploge->bufptr: %p",
              ploge, ploge->bufptr);
          break;
        }
        default:
          lgerror("logger data callback function safe call returned %d", ret);

#if defined(__linux) || defined(__linux__)

        safe_call_wait_status(
            ret,
            "logger data callback function",
            "logger data callback function",
            "logger data callback function"
          );

#endif

      }
    }

  } else if (
      (
        ret = safe_call(
          loge_logfn_va,
          0,
          NULL,
          SAFE_CALL_BLOCK_ARCH_NONE,
          3,
          1,
          ploge->plogfn != NULL,
#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__)
          ploge->plogfn == (log_fn)&logfn_syslog || ploge->file != NULL,
#else
          ploge->file != NULL,
#endif
          ploge->bufptr != NULL,
          ploge
      )
    ) != 0
  ) {
    switch (ret) {
      case SAFE_CALL_STATUS(0): {
        lgerror("logger callback function pointer is NULL, ploge: %p, "
            "ploge->plogfn: %p", ploge, ploge->plogfn);
        break;
      }
      case SAFE_CALL_STATUS(1): {
        lgerror("logger FILE pointer is NULL, ploge: %p, ploge->file: %p",
            ploge, ploge->file);
        break;
      }
      case SAFE_CALL_STATUS(2): {
        lgerror("logger message buffer is NULL, ploge: %p, ploge->bufptr: %p",
            ploge, ploge->bufptr);
        break;
      }
      default:
        lgerror("logger callback function safe call returned %x, error: %s",
            ret, strerror(errno));
    }

  }
}

UNUSED
static
size_t loge_put_char(struct loge *ploge, char c) {
  if (!ploge) {
    return 0;
  }

  if (ploge->buflen < ploge->bufcap) {
    ploge->bufptr[ploge->buflen++] = c;
  }

  return sizeof(c);
}

UNUSED
static
size_t loge_put_str(struct loge *ploge, const char *pstr) {
  if (!ploge || !pstr) {
    return 0;
  }

  size_t ncopy = strlen(pstr);
  size_t maxcopy = ploge->bufcap - ploge->buflen - 1;

  if (ncopy > maxcopy) {
    ncopy = maxcopy;
  }

  memcpy(ploge->bufptr + ploge->buflen, pstr, ncopy);

  ploge->buflen += ncopy;

  /* Keep buffer null terminated */
  ploge->bufptr[ploge->buflen] = '\0';

  return ncopy;
}

UNUSED
static
size_t loge_put_int(struct loge *ploge, int n) {
  if (!ploge) {
    return 0;
  }

  int len = 0;
  if (ploge->width > -1) {
    len = snprintf(ploge->bufptr + ploge->buflen,
        ploge->bufcap - ploge->buflen,
        "%0*d", ploge->width, n);

  } else {
    len = snprintf(ploge->bufptr + ploge->buflen,
        ploge->bufcap - ploge->buflen, "%d", n);
  }

  ploge->buflen += len;

  /* Keep buffer null terminated */
  ploge->bufptr[ploge->buflen] = '\0';

  return len;
}

UNUSED
static
size_t loge_put_uint(struct loge *ploge, unsigned int n) {
  if (!ploge) {
    return 0;
  }

  int len = 0;
  if (ploge->width > -1) {
    len = snprintf(ploge->bufptr + ploge->buflen,
        ploge->bufcap - ploge->buflen,
        "%0*u", ploge->width, n);

  } else {
    len = snprintf(ploge->bufptr + ploge->buflen,
        ploge->bufcap - ploge->buflen, "%u", n);
  }

  ploge->buflen += len;

  /* Keep buffer null terminated */
  ploge->bufptr[ploge->buflen] = '\0';

  return len;
}

UNUSED
static
size_t loge_put_long(struct loge *ploge, long n) {
  if (!ploge) {
    return 0;
  }

  int len = 0;
  if (ploge->width > -1) {
    len = snprintf(ploge->bufptr + ploge->buflen,
        ploge->bufcap - ploge->buflen,
        "%0*ld", ploge->width, n);

  } else {
    len = snprintf(ploge->bufptr + ploge->buflen,
        ploge->bufcap - ploge->buflen, "%ld", n);
  }

  ploge->buflen += len;

  /* Keep buffer null terminated */
  ploge->bufptr[ploge->buflen] = '\0';

  return len;
}

UNUSED
static
size_t loge_put_ulong(struct loge *ploge, unsigned long n) {
  if (!ploge) {
    return 0;
  }

  int len = 0;
  if (ploge->width > -1) {
    len = snprintf(ploge->bufptr + ploge->buflen,
        ploge->bufcap - ploge->buflen,
        "%0*lu", ploge->width, n);

  } else {
    len = snprintf(ploge->bufptr + ploge->buflen,
        ploge->bufcap - ploge->buflen, "%lu", n);
  }

  ploge->buflen += len;

  /* Keep buffer null terminated */
  ploge->bufptr[ploge->buflen] = '\0';

  return len;
}

UNUSED
static
size_t loge_put_float(struct loge *ploge, float f) {
  if (!ploge) {
    return 0;
  }

  int len = 0;
  if (ploge->width > -1) {
    len = snprintf(ploge->bufptr + ploge->buflen,
        ploge->bufcap - ploge->buflen,
        "%#0*.*f", ploge->width, ploge->precision, f);

  } else {
    len = snprintf(ploge->bufptr + ploge->buflen,
        ploge->bufcap - ploge->buflen,
        "%#f", f);
  }

  ploge->buflen += len;

  /* Keep buffer null terminated */
  ploge->bufptr[ploge->buflen] = '\0';

  return len;
}

UNUSED
static
size_t loge_put_double(struct loge *ploge, double f) {
  if (!ploge) {
    return 0;
  }

  int len = 0;
  if (ploge->width > -1) {
    len = snprintf(ploge->bufptr + ploge->buflen,
        ploge->bufcap - ploge->buflen,
        "%0#*.*f", ploge->width, ploge->precision, f);

  } else {
    len = snprintf(ploge->bufptr + ploge->buflen,
        ploge->bufcap - ploge->buflen,
        "%#f", f);
  }

  ploge->buflen += len;

  /* Keep buffer null terminated */
  ploge->bufptr[ploge->buflen] = '\0';

  return len;
}

UNUSED
static
size_t loge_put_time(struct loge *ploge, struct tm *ptm) {
  if (!ploge || !ptm) {
    return 0;
  }

  int len = snprintf(ploge->bufptr + ploge->buflen,
      ploge->bufcap - ploge->buflen,
      "%02d-%02d-%04d:%02d:%02d:%02d",
      ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_year + 1900,
      ptm->tm_hour, ptm->tm_min, ptm->tm_sec
    );

  ploge->buflen += len;

  /* Keep buffer null terminated */
  ploge->bufptr[ploge->buflen] = '\0';

  return len;
}


#endif /* !__cplusplus */

/******************************** C code ends *********************************/


#ifdef __cplusplus

} /* extern "C" */

#endif

/****************************** C++ code starts *******************************/

#ifdef __cplusplus

/* libstd++ */
#if defined(__GLIBCXX__)
#include <ext/stdio_filebuf.h>
#endif
#include <type_traits>
#include <iostream>
#include <fstream>
#include <array>
#include <cstring>
#include <cstdarg>
#include <ctime>

#define LOGE(ploge, level, ...) \
  do { \
    if ((ploge) != nullptr) \
      (ploge)->log( \
          (level) & ~loge<>::loge_level::LOGCOLOR, \
          __LINE__, \
          __FILE__, \
          __VA_ARGS__ \
        ); \
  } while (0)

#define LOGE_COLOR(ploge, level, ...) \
  do { \
    if ((ploge) != nullptr) \
      (ploge)->log( \
          (level) | loge<>::loge_level::LOGCOLOR, \
          __LINE__, \
          __FILE__, \
          __VA_ARGS__ \
        ); \
  } while (0)

#define LOGE_LOGTYPE(encolor, level) \
  static_cast<int>( ( (!!(encolor)) << loge::loge_level::LOGCOLORSHIFT ) | (level) )

#define LOGE_ENCOLOR(type) \
  static_cast<int>(!!(type & loge::loge_level::LOGCOLOR))

#define LOGE_LOGLEVEL(type) \
  static_cast<enum loge_level>(type & ~loge::loge_level::LOGCOLOR)

/* Forward declare class loge */
template <
  bool timestamp = true,
  std::size_t buffer_size = 0
>
class loge;

template <
  bool timestamp,
  std::size_t buffer_size
>
class loge {

  public:

  enum constants {
    LINENUMBER_WIDTH = 6,
    NUMBER_WIDTH = 8,
    BUFFER_SIZE = 1024,
    LOGCOLORSHIFT = 31
  };

  enum loge_level {
    ALL = 0,
    INFO,
    DEBUG,
    WARNING,
    ERROR,
    CRITICAL,
    MAX,
    LOGCOLOR = 1 << loge::constants::LOGCOLORSHIFT
  };

  private:

  using width_type = struct _width_type {
    int width;

    _width_type() : width(-1) {
    }

    _width_type(int width_) : width(width_) {
    }

    _width_type(const struct _width_type &other) {
      this->width = other.width;
    }

    _width_type(struct _width_type &&other) {
      this->width = other.width;
    }

    struct _width_type& operator=(const struct _width_type &other) {
      this->width = other.width;
      return *this;
    }

    struct _width_type& operator=(struct _width_type &&other) {
      this->width = other.width;
      return *this;
    }

    struct _width_type& operator=(int width_) {
      width = width_;
      return *this;
    }

    operator int() const {
      return width;
    }

    int to_int() const {
      return width;
    }
  };

  using precision_type = struct _precision_type {
    int precision;

    _precision_type() : precision(-1) {
    }

    _precision_type(int precision_) : precision(precision_) {
    }

    _precision_type(const struct _precision_type &other) {
      this->precision = other.precision;
    }

    _precision_type(struct _precision_type &&other) {
      this->precision = other.precision;
    }

    struct _precision_type& operator=(const struct _precision_type &other) {
      this->precision = other.precision;
      return *this;
    }

    struct _precision_type& operator=(struct _precision_type &&other) {
      this->precision = other.precision;
      return *this;
    }

    struct _precision_type& operator=(int precision_) {
      precision = precision_;
      return *this;
    }

    operator int() const {
      return precision;
    }

    int to_int() const {
      return precision;
    }
  };

  /* Member variables start */

  enum loge_level level;

  width_type linenumwidth = constants::LINENUMBER_WIDTH;
  width_type width = -1;
  precision_type precision = -1;

  using logfntype = void(loge<timestamp, buffer_size>::*)();
  logfntype prevlogfnptr = nullptr;
  logfntype logfnptr = &loge<timestamp, buffer_size>::logfn_internal;

  using endl_type = std::true_type;

  int syslog_priority = -1;

  protected:

  /**
   * NOTE
   * Caution for user while working with p_os
   * p_os can be set as nullptr. User callbacks shall check it.
   */
  std::ostream *p_os = nullptr;

  socket_type sock = LOGE_SOCK_ERR;

  std::array<
    char,
    (
      !buffer_size ?
      static_cast<std::size_t>(constants::BUFFER_SIZE) :
      buffer_size
    )
  > buffer;
  std::size_t buflen = 0;

  /* Member variables end */

  private:

#if __cplusplus < 201703L

  template <
    bool timestamp_ = timestamp,
    typename std::enable_if<timestamp_, int>::type = 0
  >
  int make_prefix(char *buffer, std::size_t bufcap, struct tm *ptm, bool color,
      const char *filename, int linenumber, const char *loglvlstr) {

    return snprintf(buffer, bufcap,
        "%02d-%02d-%04d:%02d:%02d:%02d: %s:%0*d: %-*s: ",
        ptm->tm_mon + 1,
        ptm->tm_mday,
        ptm->tm_year + 1900,
        ptm->tm_hour,
        ptm->tm_min,
        ptm->tm_sec,
        filename,
        static_cast<int>(linenumwidth),
        linenumber,
        color ? 22 : 8,
        loglvlstr
      );
  }

  template <
    bool timestamp_ = timestamp,
    typename std::enable_if<!timestamp_, int>::type = 0
  >
#ifndef _MSC_VER
  int make_prefix(char *buffer, std::size_t bufcap,
      struct tm *ptm UNUSED,
#else
  int make_prefix(char *buffer, std::size_t bufcap, struct tm *ptm,
#endif
      bool color, const char *filename, int linenumber,
      const char *loglvlstr) {

    return snprintf(buffer, bufcap,
        "%s:%0*d: %-*s: ",
        filename,
        static_cast<int>(linenumwidth),
        linenumber,
        color ? 22 : 8,
        loglvlstr
      );
  }

#endif /* __cplusplus < 201703L */

  void logfn_internal() {
    if (p_os) {
      p_os->write(buffer.data(), buflen);
      p_os->put('\n');
      p_os->flush();
    }
  }

  static
  int datafn_va(size_t nargs UNUSED, va_list va) {
    loge<> *ploge = va_arg(va, loge<>*);
    std::ostream *p_os = va_arg(va, std::ostream*);
    time_t t = va_arg(va, time_t);
    const char *filename = va_arg(va, const char*);
    int linenumber = va_arg(va, int);
    enum loge<>::loge_level loglevel =
      static_cast<enum loge<>::loge_level>(va_arg(va, int));
    const char *msg = va_arg(va, const char*);

    return ploge->datafn(
      ploge,
      p_os,
      t,
      filename,
      linenumber,
      loglevel,
      msg
    );
  }

  /* glibc and BSD libc only */
#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__)

  void logfn_syslog() {
    /* buffer char array may not be null-terminated */
    buffer[buffer.size() - 1] = '\0';

    syslog(syslog_priority, "%s\n", buffer.data());
  }

#endif

  virtual
  void logfn() {
    logfn_internal();
  }

  /**
   * NOTE
   * Override this virtual member function to return true when overriding the
   * datafn virtual member function
   */
  virtual
  constexpr bool is_datafn_implemented() const {
    return false;
  }

  virtual int datafn(
    loge<> *ploge UNUSED,
    std::ostream* p_os UNUSED,
    std::time_t& time UNUSED,
    const std::string& filename UNUSED,
    unsigned int linenum UNUSED,
    enum loge_level loglevel UNUSED,
    const std::string& msg UNUSED
  ) {
    return -1;
  }

  public:

  loge(enum loge_level loglevel = loge_level::ERROR)
    : loge(nullptr, -1, -1, -1, loglevel) {
  }

  loge(std::ostream &os_, enum loge_level loglevel = loge_level::ERROR)
    : loge(loglevel) {

    set_ostream(&os_);
  }

  loge(std::ofstream &ofs_, enum loge_level loglevel = loge_level::ERROR)
    : loge(loglevel) {

    set_ostream(&ofs_);
  }

  loge(
      std::ostream *p_os_,
      int linenumwidth_ = -1,
      int width_ = -1,
      int precision_ = -1,
      enum loge_level level_ = loge_level::INFO
    ) : level(loge_level::INFO), width(width_), precision(precision_) {

    if (linenumwidth_ > -1) {
      linenumwidth = linenumwidth_;
    }

    if (level_ < loge_level::MAX) {
      level = level_;
    }

    set_stdout();

    if (p_os_) {
      p_os = p_os_;
    }
  }

  virtual
  ~loge() {
#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__)
    if (syslog_priority > -1) {
      closelog();
    }
#endif

    unset_file();
  }

  const char* get_level(enum loge_level level) {
    return level > loge<>::ALL && level < loge<>::MAX ?
      loglevel_strtbl[level] :
      NULL;
  }

  const char* get_level_color(enum loge_level level) {
    return level > loge<>::ALL && level < loge<>::MAX ?
      loglevel_strtbl_color[level] :
      NULL;
  }

  void set_level(enum loge_level level_) {
    level = level_;
  }

  std::ostream* set_ostream(std::ostream *p_os_) {
    std::ostream *prev = p_os;
    if (p_os_) {
      p_os = p_os_;
    }
    return prev;
  }

  std::ostream* set_ostream(std::ofstream *p_ofs_) {
    std::ostream *prev = p_os;
    if (p_ofs_) {
      p_os = p_ofs_;
    }
    return prev;
  }

  std::ostream* unset_ostream() {
    std::ostream *prev = p_os;
    p_os = nullptr;
    return prev;
  }

  std::ostream* set_stdout() {
    prevlogfnptr = logfnptr;
    logfnptr = &loge<timestamp, buffer_size>::logfn_internal;

    std::ostream *prev = p_os;
    p_os = &std::cout;
    return prev;
  }

  std::ostream* set_stderr() {
    prevlogfnptr = logfnptr;
    logfnptr = &loge<timestamp, buffer_size>::logfn_internal;

    std::ostream *prev = p_os;
    p_os = &std::cerr;
    return prev;
  }

  /* glibc and BSD libc only */
#if defined(__GLIBC__) || defined(__FreeBSD__) || defined(__OpenBSD__)

  std::ostream* set_syslog(int priority) {
    syslog_priority = priority;

    prevlogfnptr = logfnptr;
    logfnptr = &loge<timestamp, buffer_size>::logfn_syslog;

    return unset_ostream();
  }

#endif

/* libstdc++ */
#if defined(__GLIBCXX__)

  /* Set POSIX file descriptor */
  std::ostream* set_fd(int fd) {
    std::ostream *prev = p_os;

    if (fd < 0) {
      return prev;
    }

    int newfd = dup(fd);
    if (newfd < 0) {
      lgperror("dup failed");
      return prev;
    }

    using filebuf_type = __gnu_cxx::stdio_filebuf<char>;
    filebuf_type *p_fb = new filebuf_type(newfd, std::ios::out);

    std::ostream *p_ofs = new std::ostream(p_fb);
    if (!p_ofs) {
      close(newfd);
      return prev;
    }

    p_os = p_ofs;

    prevlogfnptr = logfnptr;
    logfnptr = &loge<timestamp, buffer_size>::logfn_internal;

    return prev;
  }

  void unset_fd() {
    if (!p_os) {
      return;
    }

    if (p_os != &std::cout && p_os != &std::cerr) {
      delete p_os->rdbuf();
      delete p_os;
    }
    p_os = nullptr;

    logfnptr = prevlogfnptr;
  }

#elif defined(_MSC_VER)

  std::ostream* set_fd(int fd) {
    std::ostream *prev = p_os;

    if (fd < 0) {
      return prev;
    }

    FILE *file = _fdopen(fd, "wb");
    if (!file) {
      lgperror("_fdopen failed");
      return prev;
    }

    std::ostream *p_ofs = new std::ofstream(file);
    if (!p_ofs) {
      return prev;
    }

    p_os = p_ofs;

    prevlogfnptr = logfnptr;
    logfnptr = &loge<timestamp, buffer_size>::logfn_internal;

    return prev;
  }

  void unset_fd() {
    if (!p_os) {
      return;
    }

    if (p_os != &std::cout && p_os != &std::cerr) {
      delete p_os;
    }
    p_os = nullptr;

    logfnptr = prevlogfnptr;
  }

#endif /* defined(__GLIBCXX__) */

  std::ostream* set_file(const std::string &filepath, bool append=true) {
    std::ostream *prev = p_os;

    if (filepath.length() == 0) {
      return prev;
    }

    std::ios_base::openmode mode = std::ios_base::out;
    !append ? (void)(mode |= std::ios_base::trunc) :
      (void)(mode |= std::ios_base::app);

    std::ostream *p_ofs = new std::ofstream(filepath, mode);
    if (!p_ofs) {
      return prev;
    }

    p_os = p_ofs;

    prevlogfnptr = logfnptr;
    logfnptr = &loge<timestamp, buffer_size>::logfn_internal;

    return prev;
  }

  void unset_file() {
    if (!p_os) {
      return;
    }

    if (p_os != &std::cout && p_os != &std::cerr) {
      delete p_os;
    }
    p_os = nullptr;

    logfnptr = prevlogfnptr;
  }

  void reset_logfn() {
    prevlogfnptr = logfnptr;
    logfnptr = &loge<timestamp, buffer_size>::logfn_internal;
  }

  inline
  void reset() {
    buflen = 0;
  }

  void flush() {
    (this->*logfnptr)();
    reset();
  }

  bool connect(const char *host, unsigned short port, int type = 0,
      int ipv6 = 0, std::ostream **prev = nullptr) {

    if (!host) {
      return false;
    }

    socket_type sock = sock_connect(host, port, type, ipv6);
    if (sock == LOGE_SOCK_ERR) {
      return false;
    }

    this->sock = sock;

    std::ostream *prev_os = set_fd(socket_to_native(sock));
    if (prev) {
      *prev = prev_os;
    }

    prevlogfnptr = logfnptr;
    logfnptr = &loge<timestamp, buffer_size>::logfn_internal;

    return true;
  }

  void disconnect() {
    if (sock == -1) {
      return;
    }

    /* Stop data communication */
    shutdown_socket(sock);

    destroy_socket(sock);

#if defined(_WIN64)
    WSACleanup();
#endif

    sock = -1;

    unset_fd();

    logfnptr = prevlogfnptr;
  }

  void log(
      int logtype,
      int linenumber,
      const char *filename,
      const char *msg,
      ...
    ) {

    std::time_t t = std::time(NULL);
    struct tm localtm = *std::localtime(&t);

    enum loge_level loglevel = LOGE_LOGLEVEL(logtype);

    if (loglevel >= loge_level::MAX ||
        loglevel < level) {
      return;
    }

    int en_color = LOGE_ENCOLOR(logtype);

    const char *loglvlstr = en_color ?
      loglevel_strtbl_color[loglevel] :
      loglevel_strtbl[loglevel];

    int len = 0;

#if __cplusplus >= 201703L

    if constexpr (timestamp) {
      len = snprintf(buffer.data(), buffer.size(),
          "%02d-%02d-%04d:%02d:%02d:%02d: %s:%0*d: %-*s: ",
          localtm.tm_mon + 1, localtm.tm_mday, localtm.tm_year + 1900,
          localtm.tm_hour, localtm.tm_min, localtm.tm_sec,
          filename,
          (unsigned int)static_cast<int>(linenumwidth), linenumber,
          en_color ? 22 : 8, loglvlstr
        );

    } else {
      len = snprintf(buffer.data(), buffer.size(),
          "%s:%0*d: %-*s: ",
          filename,
          static_cast<int>(linenumwidth), linenumber,
          en_color ? 22 : 8, loglvlstr
        );
    }

#else /* __cplusplus >= 201703L */

    len =
      make_prefix(buffer.data(), buffer.size(), &localtm, en_color, filename,
          linenumber, loglvlstr);

#endif /* __cplusplus >= 201703L */

    std::va_list args;
    va_start(args, msg);
    len += vsnprintf(buffer.data() + len, buffer.size() - len, msg, args);
    va_end(args);

    buflen = len;

    unsigned int ret = 0;

    if (is_datafn_implemented()) {
      ret =
        safe_call(
            datafn_va,
            sizeof(nr_syscalls) / sizeof(nr_syscalls[0]),
            nr_syscalls,
            GATE_SAFECALL_ARCH_BLOCK,
            1,
            7,
            p_os != nullptr,
            this, p_os, t, filename, linenumber, loglevel, msg
        );

      switch (ret) {
        case 0: {
          break;
        }
        case SAFE_CALL_STATUS(0): {
          lgerror("logger std::ostream pointer is nullptr, logger: %p, p_os: %p",
              this, p_os);
          break;
        }
        default:
          lgerror("logger data member function safe call returned %d", ret);

#if defined(__linux) || defined(__linux__)

          safe_call_wait_status(
              ret,
              "logger data member function",
              "logger data member function",
              "logger data member function"
            );

#endif

      }
    } else {
      (this->*logfnptr)();
    }
  }

  loge<timestamp, buffer_size>& operator<<(
      const loge<timestamp,
      buffer_size> &other
  ) {
    if (this != &other) {
      this->level = other.level;
      this->linenumwidth = other.linenumwidth;
      this->width = other.width;
      this->precision = other.precision;

      std::size_t ncopy = other.buflen;
      if (ncopy > this->buflen) {
        ncopy = this->buflen;
      }
      memcpy(this->buffer.data(), other.buffer.data(), ncopy);
    }
    return *this;
  }

  loge<timestamp, buffer_size>& operator<<(const char *pstr) {
    if (!pstr) {
      return *this;
    }

    std::size_t ncopy = strlen(pstr);
    std::size_t maxcopy = buffer.size() - buflen - 1;

    if (ncopy > maxcopy) {
      ncopy = maxcopy;
    }

    memcpy(buffer.data() + buflen, pstr, ncopy);

    buflen += ncopy;

    /* Keep buffer null terminated */
    buffer[buflen] = '\0';

    return *this;
  }

  loge<timestamp, buffer_size>& operator<<(const std::string &str) {
    return this->operator<<(str.c_str());
  }

  loge<timestamp, buffer_size>& operator<<(char c) {
    if (buflen < buffer.size()) {
      buffer[buflen++] = c;
    }
    return *this;
  }

  loge<timestamp, buffer_size>& operator<<(unsigned char c) {
    return this->operator<<(static_cast<char>(c));
  }

  loge<timestamp, buffer_size>& operator<<(short n) {
    int widthval = static_cast<int>(width);
    int len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
        widthval > -1 ? "%0*hd" : "%hd",
        widthval > -1 ? widthval : n, n);

    buflen += len;

    return *this;
  }

  loge<timestamp, buffer_size>& operator<<(unsigned short n) {
    int widthval = static_cast<int>(width);
    int len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
        widthval > -1 ? "%0*hu" : "%hu",
        widthval > -1 ? widthval : n, n);

    buflen += len;

    return *this;
  }

  loge<timestamp, buffer_size>& operator<<(int n) {
    int widthval = static_cast<int>(width);
    int len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
        widthval > -1 ? "%0*d" : "%d",
        widthval > -1 ? widthval : n, n);

    buflen += len;

    return *this;
  }

  loge<timestamp, buffer_size>& operator<<(unsigned int n) {
    int widthval = static_cast<int>(width);
    int len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
        widthval > -1 ? "%0*u" : "%u",
        widthval > -1 ? widthval : n, n);

    buflen += len;

    return *this;
  }

  loge<timestamp, buffer_size>& operator<<(long n) {
    int widthval = static_cast<int>(width);
    int len = 0;
    if (widthval > -1) {
      len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
          "%0*ld", widthval, n);

    } else {
      len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
          "%0ld", n);
    }

    buflen += len;

    return *this;
  }

  loge<timestamp, buffer_size>& operator<<(long long n) {
    int widthval = static_cast<int>(width);
    int len = 0;
    if (widthval > -1) {
      len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
          "%0*lld", widthval, n);

    } else {
      len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
          "%0lld", n);
    }

    buflen += len;

    return *this;
  }

  loge<timestamp, buffer_size>& operator<<(unsigned long n) {
    int widthval = static_cast<int>(width);
    int len = 0;
    if (widthval > -1) {
      len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
          "%0*lu", widthval, n);

    } else {
      len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
          "%0lu", n);
    }

    buflen += len;

    return *this;
  }

  loge<timestamp, buffer_size>& operator<<(unsigned long long n) {
    int widthval = static_cast<int>(width);
    int len = 0;
    if (widthval > -1) {
      len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
          "%0*llu", widthval, n);

    } else {
      len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
          "%0llu", n);
    }

    buflen += len;

    return *this;
  }

  loge<timestamp, buffer_size>& operator<<(float f) {
    int widthval = static_cast<int>(width);
    int precisionval = static_cast<int>(precision);
    int len = 0;
    if (widthval > -1) {
      if (precisionval > -1) {
        len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
            "%0#*.*f", widthval, precisionval, f);

      } else {
        len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
            "%0#*f", widthval, f);
      }

    } else {
      if (precisionval > -1) {
        len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
            "%0#.*f", precisionval, f);

      } else {
        len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
            "%0#f", f);
      }
    }

    buflen += len;

    return *this;
  }

  loge<timestamp, buffer_size>& operator<<(double f) {
    int widthval = static_cast<int>(width);
    int precisionval = static_cast<int>(precision);
    int len = 0;
    if (widthval > -1) {
      if (precisionval > -1) {
        len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
            "%0#*.*f", widthval, precisionval, f);

      } else {
        len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
            "%0#*f", widthval, f);
      }

    } else {
      if (precisionval > -1) {
        len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
            "%0#.*f", precisionval, f);

      } else {
        len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
            "%0#f", f);
      }
    }

    buflen += len;

    return *this;
  }

#if __cplusplus >= 201703L

  loge<timestamp, buffer_size>& operator<<(struct tm &localtm) {
    if constexpr (timestamp) {
      int len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
          "%02d-%02d-%04d:%02d:%02d:%02d",
          localtm.tm_mon + 1, localtm.tm_mday, localtm.tm_year + 1900,
          localtm.tm_hour, localtm.tm_min, localtm.tm_sec
        );

      buflen += len;
    }

    return *this;
  }

#else /* __cplusplus >= 201703L */

  template <
    bool timestamp_ = timestamp,
    typename std::enable_if<timestamp_, int>::type = 0
  >
  void insert_tm(struct tm &localtm) {
    int len = snprintf(buffer.data() + buflen, buffer.size() - buflen,
        "%02d-%02d-%04d:%02d:%02d:%02d",
        localtm.tm_mon + 1, localtm.tm_mday, localtm.tm_year + 1900,
        localtm.tm_hour, localtm.tm_min, localtm.tm_sec
      );

    buflen += len;
  }

  template <
    bool timestamp_ = timestamp,
    typename std::enable_if<!timestamp_, int>::type = 0
  >
#ifndef _MSC_VER
  void insert_tm(struct tm &localtm UNUSED) {
#else
  void insert_tm(struct tm &localtm) {
#endif
  }

  loge<timestamp, buffer_size>& operator<<(struct tm &localtm) {
    insert_tm(localtm);
    return *this;
  }

#endif /* __cplusplus >= 201703L */

  static
  width_type setw_default() {
    return width_type(-1);
  }

  static
  width_type setw(unsigned int width) {
    return width_type(width);
  }

  static
  width_type setw(int width) {
    return width_type(width);
  }

  loge<timestamp, buffer_size>& operator<<(width_type width_) {
    width = width_;
    return *this;
  }

  static
  precision_type setprecision(unsigned int precision_) {
    return precision_type(precision_);
  }

  static
  precision_type setprecision(int precision_) {
    return precision_type(precision_);
  }

  loge<timestamp, buffer_size>& operator<<(precision_type precision_) {
    precision = precision_;
    return *this;
  }

  static
  constexpr const endl_type endl = endl_type();

  loge<timestamp, buffer_size>& operator<<(endl_type endl UNUSED) {
    flush();
    return *this;
  }

};

#endif /* __cplusplus */

/******************************* C++ code ends ********************************/

#undef _POSIX_C_SOURCE

#ifdef __cplusplus

extern "C" {

#endif

/*************************** safe_call code starts ****************************/

static size_t __safe_call_syscalls_mask = GATE_SAFECALL_SYSCALLS_MASK;

#ifndef __cplusplus

static
inline
struct sock_filter* alloc_sock_filter(void *ptr, size_t nmemb, size_t size) {
  return reallocarray(ptr, nmemb, size);
}

#else /* __cplusplus */

static
inline
struct sock_filter* alloc_sock_filter(void *ptr, size_t nmemb, size_t size) {
  return reinterpret_cast<struct sock_filter*>(
      reallocarray(ptr, nmemb, size)
  );
}

#endif /* __cplusplus */

static
inline
void safe_call_wait_status(
    int status,
    const char *msgexit,
    const char *msgterm,
    const char *msgstop
) {
#if defined(__linux) || defined(__linux__)

  if (WIFEXITED((status))) {
    lgerror("%s%s %d", msgexit, ": safe call process exited, callee returned ",
        WEXITSTATUS((status)));
  } else if (WIFSIGNALED((status))) {
    lgerror("%s%s %d: %s", msgterm, ": safe call process terminated by signal ",
        WTERMSIG((status)), strsignal(WTERMSIG((status))));
  } else if (WIFSTOPPED((status))) {
    lgerror("%s%s %d: %s", msgstop, ": safe call process stopped by signal ",
        WSTOPSIG((status)), strsignal(WSTOPSIG((status))));
  }

#endif /* __linux || __linux__ */
}

static
unsigned short safe_call_make_filter(
    size_t n_block_syscalls,
    const unsigned int *syscalls,
    uint64_t block_arch,
    struct sock_filter **prev_filter
) {
  uint8_t n_block_archs = block_arch ? __builtin_popcount(block_arch) : 0;

  unsigned short filter_size =
    (n_block_syscalls + !!n_block_syscalls) +
    (!!n_block_archs + n_block_archs);

  if (filter_size) {
    filter_size += 3;
  } else {
    return 0;
  }

  if (!prev_filter) {
    return 0;
  }

  struct sock_filter *filter = prev_filter ? *prev_filter : NULL;
  filter = alloc_sock_filter(filter, sizeof(struct sock_filter), filter_size);

  if (!filter) {
    return 0;
  }

  if (prev_filter) {
    *prev_filter = filter;
  }

  unsigned short i = 0;
  uint8_t jmp_offset = 0;

  if (n_block_syscalls) {
    jmp_offset = n_block_syscalls - 1;

    lgdebug("jmp_offset: %d", jmp_offset);
    lgdebug("filter size: %d", i);

    filter[i++] = (struct sock_filter)BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
        (offsetof(struct seccomp_data, nr)));

    for (uint8_t j = 0; j < n_block_syscalls - 1; ++j) {
      lgdebug("filter size: %d", i);

      filter[i++] = (struct sock_filter)BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K,
          syscalls[j], jmp_offset, 0);

      --jmp_offset;

      lgdebug("jmp_offset: %d", jmp_offset);
      lgdebug("j: %d", j);
    }

    jmp_offset = (!!n_block_archs + n_block_archs) + 2;

    lgdebug("filter size: %d", i);

    filter[i++] = (struct sock_filter)BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K,
        syscalls[n_block_syscalls - 1], 0, jmp_offset);

    lgdebug("filter size: %d", i);
  }

  jmp_offset = (!!n_block_archs + n_block_archs);

  if (block_arch) {
    lgdebug("jmp_offset: %d", jmp_offset);
    lgdebug("filter size: %d", i);

    filter[i++] = (struct sock_filter)BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
        (offsetof(struct seccomp_data, arch)));

    --jmp_offset;

    lgdebug("jmp_offset: %d", jmp_offset);
  }

  for (uint8_t k = 0; k < n_block_archs; ++k) {
    uint32_t arch =
      btr(&block_arch, SAFE_CALL_BLOCK_ARCH_I386) ?
        AUDIT_ARCH_I386 :
        btr(&block_arch, SAFE_CALL_BLOCK_ARCH_x86_64) ?
          AUDIT_ARCH_X86_64 : 0;

    lgdebug("%s", "using arch block");
    lgdebug("block_arch: %lu", block_arch);
    lgdebug("arch: %x", arch);
    lgdebug("filter size: %d", i);

    filter[i++] = (struct sock_filter)BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K,
        arch, jmp_offset, 0);

    --jmp_offset;

    lgdebug("jmp_offset: %d", jmp_offset);
    lgdebug("k: %d", k);
  }

  filter[i++] = (struct sock_filter)BPF_STMT(BPF_JMP | BPF_JA, 1);

  filter[i++] = (struct sock_filter)BPF_STMT(BPF_RET | BPF_K,
      SECCOMP_RET_KILL);

  filter[i++] = (struct sock_filter)BPF_STMT(BPF_RET | BPF_K,
      SECCOMP_RET_ALLOW);

  lgdebug("filter size: %d", i);

  return i;
}

static
unsigned int
safe_call(
    safe_callee_type callee,
    size_t n_block_syscalls
#if defined(__linux) || defined(__linux__)
    UNUSED,
#else
    ,
#endif
    const unsigned int *syscalls
#if defined(__linux) || defined(__linux__)
    UNUSED,
#else
    ,
#endif
    uint64_t block_arch,
    const size_t nchecks,
    const size_t nargs,
    ...
) {

  if (!callee) {
    return SAFE_CALL_CALLEE_NULL;
  }

#if defined(__linux) || defined(__linux__)

  va_list va;
  va_start(va, nargs);

  for (size_t i = 0; i < nchecks; ++i) {
    int result = va_arg(va, int);
    if (!result) {
      va_end(va);
      return SAFE_CALL_STATUS(i);
    }
  }

  va_end(va);

  n_block_syscalls &= __safe_call_syscalls_mask;

  switch (n_block_syscalls) {
    case 0:
      break;
    default:
      /* Static data for BPF filter used by seccomp */
      static unsigned short filter_size = 0;
      static struct sock_filter *filter = NULL;

      if (!filter) {
        filter_size = safe_call_make_filter(
            n_block_syscalls,
            syscalls,
            block_arch,
            &filter
        );
      }

      if (!filter_size || !filter) {
        lgperror("safe_call_make_filter failed");
        break;
      }

      lgdebug("filter: %p", filter);
      lgdebug("filter size: %d", filter_size);

      pid_t pid = fork();

      int ret = 0;

      if (pid == 0) {
        /* Block syscalls with seccomp */
        prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);

        struct sock_fprog prog = {
          .len = filter_size,
          .filter = filter
        };

        if (seccomp(SECCOMP_SET_MODE_FILTER, 0, &prog)) {
          lgperror("seccomp failed");
        }

        ret = (*callee)(nargs, va);

        exit(ret);
      }

      wait(&ret);
      lgdebug("safe_call child ret: %d", ret);
      return ret;
  }

#endif

  return (*callee)(nargs, va);
}

/**************************** safe_call code ends *****************************/

#ifdef __cplusplus

} /* extern "C" */

#endif

#undef _GNU_SOURCE
#undef _DEFAULT_SOURCE

#endif /* _LOGE_HPP_ */
