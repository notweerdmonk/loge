.DEFAULT_GOAL: all

CXX_VERSION := c++2a
C_VERSION := c11

PROJECT_ROOT ?= $(realpath ..)

EXAMPLES_DIR := $(PROJECT_ROOT)/examples

INCLUDE_DIRS := $(PROJECT_ROOT) $(CURDIR)

INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIRS))

INCLUDE_FILES := $(foreach dir,$(INCLUDE_DIRS),$(wildcard $(dir)/*.hpp))
INCLUDE_FILES += $(foreach dir,$(INCLUDE_DIRS),$(wildcard $(dir)/*.h))

GNU_WARNINGS := -Wall -Wextra -Werror -Wno-implicit-fallthrough
CFLAGS :=

DEBUG := $(strip $(DEBUG))
ifneq ($(DEBUG),)
CFLAGS += -ggdb3 --save-temps
ifeq ($(DEBUG),DEBUG)
CFLAGS += -DDEBUG_LEVEL=1
endif
ifeq ($(DEBUG),LOGS)
CFLAGS += -DDEBUG_LEVEL=2
endif
endif

ifneq ($(strip $(EXPLOIT_ARCH)),)
CFLAGS += -DEXP_ARCH=EXP_ARCH_$(EXPLOIT_ARCH)
NO_OPTIMIZE := 1
endif
ifneq ($(strip $(EXPLOIT_SYSCALLS)),)
CFLAGS += -DEXP_SYSCALLS=$(EXPLOIT_SYSCALLS)
NO_OPTIMIZE := 1
endif

ifneq ($(NO_OPTIMIZE),)
OPTMIZATION = -O0
undefine NO_OPTIMIZE
endif

# Linux targets

LINUX_C_SOURCES := ctest.c logmore.c

LINUX_C_OBJECTS := $(subst .c,.o,$(LINUX_C_SOURCES))

LINUX_CXX_SOURCES := cctest.cc

LINUX_CXX_OBJECTS := $(subst .cc,.o,$(LINUX_CXX_SOURCES))

LINUX_C_TARGETS := ctest

LINUX_CXX_TARGETS := cctest

all: linux ctestwin cctestwin

linux: $(LINUX_C_TARGETS) $(LINUX_CXX_TARGETS)

%.o: %.c
	gcc $(GNU_WARNINGS) -std=$(C_VERSION) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

%.o: %.cc
	g++ $(GNU_WARNINGS) -std=$(CXX_VERSION) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(LINUX_C_OBJECTS): $(INCLUDE_FILES)

ctest: ctest.o logmore.o
	gcc $(GNU_WARNINGS) -std=$(C_VERSION) $(CFLAGS) $(INCLUDE_FLAGS) $^ -o $@

$(LINUX_CXX_OBJECTS): $(INCLUDE_FILES)

cctest: cctest.o
	g++ $(GNU_WARNINGS) -std=$(CXX_VERSION) $(CFLAGS) $(INCLUDE_FLAGS) $^ -o $@

clean-tests-linux:
	-rm -f \
		$(LINUX_C_OBJECTS) \
		$(LINUX_CXX_OBJECTS) \
		$(LINUX_CXX_OBJECTS:.o=.ii) \
		$(LINUX_CXX_OBJECTS:.o=.s) \
		$(LINUX_C_TARGETS) \
		$(LINUX_CXX_TARGETS) \
		$(LINUX_C_OBJECTS:.o=.i) \
		$(LINUX_C_OBJECTS:.o=.s)

# Windows targets

INCLUDE_FLAGS_MSVC := $(foreach include_dir, $(INCLUDE_DIRS), /I$(include_dir))

cctestwin: ../loge.hpp fdlogger.hpp filelogger.hpp cctest.cc
	cl.exe /EHsc $(INCLUDE_FLAGS_MSVC) cctest.cc /Fe: $@

ctestwin: ../loge.hpp ctest.c logmore.c
	cl.exe $(INCLUDE_FLAGS_MSVC) ctest.c logmore.c /Fe: $@

clean-tests-win:
	-rm -f test.obj logmore.obj ctestwin.exe cctestwin.exe

# Seccomp exploit

SECCOMP_EXPLOIT_C_SOURCES := seccomp-exploit.c

SECCOMP_EXPLOIT_ASM_SOURCES := write32.S

SECCOMP_EXPLOIT_C_OBJECTS := $(subst .c,.o,$(SECCOMP_EXPLOIT_C_SOURCES))
SECCOMP_EXPLOIT_ASM_OBJECTS := $(subst .S,.o,$(SECCOMP_EXPLOIT_ASM_SOURCES))
SECCOMP_EXPLOIT_OBJECTS := $(SECCOMP_EXPLOIT_C_OBJECTS) $(SECCOMP_EXPLOIT_ASM_OBJECTS)

SECCOMP_EXPLOIT_TARGETS := seccomp-exploit

%.o: %.S
	gcc -no-pie -c $< -o $@

$(SECCOMP_EXPLOIT_C_OBJECTS): %.o: %.c
	echo $@
	echo $^
	gcc $(GNU_WARNINGS) -std=$(C_VERSION) $(CFLAGS) --no-pie $(INCLUDE_FLAGS) -c $< -o $@

$(SECCOMP_EXPLOIT_TARGETS): $(SECCOMP_EXPLOIT_OBJECTS)
	gcc $(GNU_WARNINGS) -std=$(C_VERSION) $(CFLAGS) $(OPTMIZATION) -no-pie $(INCLUDE_FLAGS) $^ -o $@

clean-exploit:
	-rm -f \
		$(SECCOMP_EXPLOIT_TARGETS) \
		$(SECCOMP_EXPLOIT_C_OBJECTS) \
		$(SECCOMP_EXPLOIT_C_OBJECTS:.o=.i) \
		$(SECCOMP_EXPLOIT_C_OBJECTS:.o=.s) \
		$(SECCOMP_EXPLOIT_ASM_OBJECTS) \
		$(SECCOMP_EXPLOIT_ASM_OBJECTS:.o=.i) \
		$(SECCOMP_EXPLOIT_ASM_OBJECTS:.o=.s) \

# Common targets

clean: clean-exploit clean-tests-linux clean-tests-win
