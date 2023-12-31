CC ?= gcc  # version?
CXX ?= g++
CPPFLAGS = -std=c++17 -Wall -Wextra -Wconversion -Werror -fno-rtti -fno-exceptions $(EXTRA_CPPFLAGS)

# Add new source files here.
SRCS = lexer.cpp parser.cpp
HDRS = xos.h

OBJS = $(SRCS:.cpp=.o)
EXE = main
TEST = test

.PHONY: all clean format format-checks

all: $(EXE)

$(EXE): $(OBJS) main.o
	$(CXX) $(CPPFLAGS) $^ -o $@

%.o: %.cpp $(HDRS)
	$(CXX) $(CPPFLAGS) -c $< -o $@

test.o: test.cpp $(HDRS)
	$(CXX) $(CPPFLAGS) -c $< -o $@ -I$(GTEST_HDR)

# This is run with:
#
#   $ make test GTEST_HDR=<path to include dir> GTEST_LIB=<path to lib dir>
$(TEST): test.o $(OBJS)
	$(CXX) $(CPPFLAGS) $^ -o $@ -lgtest_main -lgtest -L$(GTEST_LIB)

clean:
	rm -rf $(EXE) *.o $(TEST)

format:
	clang-format --style=google -i $(SRCS) main.cpp test.cpp $(HDRS)

format-check:
	clang-format --style=google $(SRCS) main.cpp test.cpp $(HDRS) --dry-run -Werror
