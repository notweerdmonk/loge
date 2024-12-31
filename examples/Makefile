CPP_VERSION := c++2a
C_VERSION := c11

INCLUDE_DIRS = .. .
INCLUDE_FLAGS := $(foreach include_dir, $(INCLUDE_DIRS), -I$(include_dir))
INCLUDE_FLAGS_MSVC := $(foreach include_dir, $(INCLUDE_DIRS), /I$(include_dir))

CFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function

all: ctest cctest ctestwin cctestwin

cctest: ../loge.hpp fdlogger.hpp filelogger.hpp test.cc
	g++ -ggdb3 -Wall -Wextra -std=$(CPP_VERSION) $(CFLAGS) $(INCLUDE_FLAGS) test.cc -o $@

cctestwin: ../loge.hpp fdlogger.hpp filelogger.hpp test.cc
	cl.exe /EHsc $(INCLUDE_FLAGS_MSVC) test.cc /Fe: $@

ctestwin: ../loge.hpp test.c logmore.c
	cl.exe $(INCLUDE_FLAGS_MSVC) test.c logmore.c /Fe: $@

ctest: ../loge.hpp test.c logmore.c
	gcc -ggdb3 -Wall -Wextra -std=$(C_VERSION) $(CFLAGS) $(INCLUDE_FLAGS) test.c logmore.c -o $@

clean:
	rm -f ctest cctest test.obj logmore.obj ctestwin.exe cctestwin.exe
