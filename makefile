CC ?= gcc  # version?
CXX ?= g++
CPPFLAGS = -std=c++11  # TODO: Update to c++17?

# Add new source files here.
SRCS = main.cpp lexer.cpp

OBJS = $(SRCS:.cpp=.o)
EXE = main

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(CPPFLAGS) $? -o $@

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(EXE)
