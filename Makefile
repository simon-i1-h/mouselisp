.PHONY: all test clean coverage bear

CXX = clang++
CXXFLAGS = -Wall -Wextra -Og -g3 -std=c++20 -pedantic -Wimplicit-fallthrough \
	$$(pkg-config --cflags bdw-gc)

PROG = mouselisp
SRCS = mouselisp.cpp util.cpp file.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)
GCNO = $(SRCS:.cpp=.gcno)
GCDA = $(SRCS:.cpp=.gcda)
GCOV = $(SRCS:.cpp=.cpp.gcov)
LDFLAGS = -fuse-ld=lld $$(pkg-config --libs bdw-gc)

all: $(PROG)

coverage: CXXFLAGS += --coverage
coverage: all
	./$(PROG)
	for gcda in $(GCDA); do llvm-cov gcov $$gcda; done

bear: clean
	bear -- $(MAKE) all

$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

clean:
	rm -f $(PROG) $(OBJS) $(DEPS) $(GCNO) $(GCDA) $(GCOV) \
		compile_commands.json

test: all
	./$(PROG)

-include $(DEPS)
