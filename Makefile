.PHONY: all test clean coverage bear loc format

CC = clang
CFLAGS = -Wall -Wextra -Og -g3 -std=c17 -pedantic -Wimplicit-fallthrough \
	$$(pkg-config --cflags bdw-gc)

PROG = mouselisp
SRCS = mouselisp.c util.c file.c string.c arith.c object.c parser.c machine.c \
	builtin.c init.c exception.c t/test.c
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
	./$(PROG) --test
	python t/object_dump.py
	./$(PROG) --test-unique

loc:
	wc -l mouselisp.h $(SRCS)
	wc -l t/object_dump.py

format:
	clang-format -i mouselisp.h $(SRCS)

-include $(DEPS)
