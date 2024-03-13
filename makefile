CC = gcc  # version?
CXX = g++
CPPFLAGS = -std=c++17 -Wall -Wextra -Wconversion -Werror -fno-rtti -fno-exceptions -fdata-sections -ffunction-sections -g $(EXTRA_CPPFLAGS)
CLANG_FORMAT ?= clang-format
LLVM_CONFIG ?= llvm-config
LLVM_CONFIG_CXX_FLAGS = $(shell $(LLVM_CONFIG) --cxxflags)
LLVM_CONFIG_LD_FLAGS = $(shell $(LLVM_CONFIG) --ldflags)
LLVM_CONFIG_SYSTEM_LIBS = $(shell $(LLVM_CONFIG) --system-libs)
LLVM_CONFIG_LIBS = $(shell $(LLVM_CONFIG) --libs core)

UNAME = $(shell uname -s)
ifeq ($(UNAME),Linux)
PLATFORM_SPECIFIC_LDFLAGS = -Wl,--gc-sections
endif
ifeq ($(UNAME),Darwin)
PLATFORM_SPECIFIC_LDFLAGS = -Wl,dead_strip
endif

CPPFLAGS := $(LLVM_CONFIG_CXX_FLAGS) $(CPPFLAGS)
LDFLAGS := $(LLVM_CONFIG_LD_FLAGS) $(LLVM_CONFIG_SYSTEM_LIBS) $(LLVM_CONFIG_LIBS) $(LDFLAGS) $(PLATFORM_SPECIFIC_LDFLAGS)

# Add new source files here.
SRCS = lexer.cpp parser.cpp compiler.cpp
HDRS = xos.h

OBJS = $(SRCS:.cpp=.o)
EXE = main
TEST = test

.PHONY: all clean format format-checks

all: $(EXE)

$(EXE): $(OBJS) main.o
	$(CXX) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp $(HDRS)
	$(CXX) $(CPPFLAGS) -c $< -o $@

test.o: test.cpp $(HDRS)
	$(CXX) $(CPPFLAGS) -c $< -o $@ -I$(GTEST_HDR)

# This is run with:
#
#   $ make test GTEST_HDR=<path to include dir> GTEST_LIB=<path to lib dir>
#
$(TEST): test.o $(OBJS)
	$(CXX) $(CPPFLAGS) $^ -o $@ -lgtest_main -lgtest -L$(GTEST_LIB) $(LDFLAGS) -lpthread

clean:
	rm -rf $(EXE) *.o $(TEST)

format:
	$(CLANG_FORMAT) --style=google -i $(SRCS) main.cpp test.cpp $(HDRS)

format-check:
	$(CLANG_FORMAT) --style=google $(SRCS) main.cpp test.cpp $(HDRS) --dry-run -Werror
