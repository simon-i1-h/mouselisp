.PHONY: all test clean coverage bear

CC = clang
CFLAGS = -Wall -Wextra -Og -g3 -std=c17 -pedantic -Wimplicit-fallthrough \
	$$(pkg-config --cflags bdw-gc)

PROG = mouselisp
SRCS = mouselisp.c util.c file.c string.c arith.c
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)
GCNO = $(SRCS:.c=.gcno)
GCDA = $(SRCS:.c=.gcda)
GCOV = $(SRCS:.c=.c.gcov)
LDFLAGS = -fuse-ld=lld $$(pkg-config --libs bdw-gc)

all: $(PROG)

coverage: CFLAGS += --coverage
coverage: all
	./$(PROG)
	for gcda in $(GCDA); do llvm-cov gcov $$gcda; done

bear: clean
	bear -- $(MAKE) all

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c -o $@ $<

clean:
	rm -f $(PROG) $(OBJS) $(DEPS) $(GCNO) $(GCDA) $(GCOV) \
		compile_commands.json

test: all
	./$(PROG)

-include $(DEPS)
