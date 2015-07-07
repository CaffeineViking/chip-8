program_NAME := chip-8
program_MAIN_OBJ := src/main.o
test_NAME := $(program_NAME)_test
test_MAIN_OBJ := src/main_test.o

test_C_SRCS := $(wildcard src/*_test.c)
test_C_SRCS := $(test_C_SRCS) $(wildcard src/**/*_test.c)
test_CXX_SRCS := $(wildcard src/*_test.cpp)
test_CXX_SRCS := $(test_CXX_SRCS) $(wildcard src/**/*_test.cpp)

program_C_SRCS := $(wildcard src/*.c)
program_C_SRCS := $(program_C_SRCS) $(wildcard src/**/*.c)
program_C_SRCS := $(filter-out $(test_C_SRCS), $(program_C_SRCS))
program_CXX_SRCS := $(wildcard src/*.cpp)
program_CXX_SRCS := $(program_CXX_SRCS) $(wildcard src/**/*.cpp)
program_CXX_SRCS := $(filter-out $(test_CXX_SRCS), $(program_CXX_SRCS))

program_C_OBJS := $(program_C_SRCS:.c=.o)
program_CXX_OBJS := $(program_CXX_SRCS:.cpp=.o)
program_OBJS := $(program_C_OBJS) $(program_CXX_OBJS)

test_C_OBJS := $(test_C_SRCS:.c=.o)
test_CXX_OBJS := $(test_CXX_SRCS:.cpp=.o)
test_OBJS := $(filter-out $(program_MAIN_OBJ), $(program_OBJS)) $(test_C_OBJS) $(test_CXX_OBJS)

program_INCLUDE_DIRS :=
program_LIBRARY_DIRS :=
program_LIBRARIES :=

CPPFLAGS += $(foreach includedir, $(program_INCLUDE_DIRS), -I$(includedir))
LDFLAGS += $(foreach librarydir, $(program_LIBRARY_DIRS), -L$(librarydir))
LDFLAGS += $(foreach library, $(program_LIBRARIES), -l$(library))

CFLAGS += -std=c11 -Wall -Wextra -pedantic
CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic

DEBUG := NO
ifeq ($(DEBUG), YES)
CFLAGS += -g
CXXFLAGS += -g
program_NAME := $(program_NAME)_debug
test_NAME := $(test_NAME)_debug
endif

RELEASE := NO
ifeq ($(RELEASE), YES)
CFLAGS += -O2
CXXFLAGS += -O2
program_NAME := $(program_NAME)_release
test_NAME := $(test_NAME)_release
endif

.PHONY: all test program run run_test run_program clean clean_test clean_program distclean distclean_test distclean_program distrun distrun_test distrun_program
all: test program
test: bin/$(test_NAME).out
program: bin/$(program_NAME).out

bin/$(test_NAME).out: $(test_OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(test_OBJS) -o bin/$(test_NAME).out $(LDFLAGS) $(TARGET_ARCH)
bin/$(program_NAME).out: $(program_OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(program_OBJS) -o bin/$(program_NAME).out $(LDFLAGS) $(TARGET_ARCH)

run: run_test run_program
run_test: test
	bin/$(test_NAME).out $(ARGS)
run_program: program
	bin/$(program_NAME).out $(ARGS)

clean: clean_test clean_program
clean_test:
	@- $(RM) $(test_OBJS)
clean_program:
	@- $(RM) $(program_OBJS)

distclean: distclean_test distclean_program
distclean_test: clean_test
	@- $(RM) bin/$(test_NAME)*
distclean_program: clean_program
	@- $(RM) bin/$(program_NAME)*

distrun: distclean_test distclean_program run
distrun_test: distclean_test run_test
distrun_program: distclean_program run_program
