SRCMODULES = \
	simple_substitution.c \
	tests/simple_substitution_test.c

OBJMODULES = $(SRCMODULES:.c=.o)
HEADERS = $(SRCMODULES:.c=.h)

EXEC_FILE = simple_substitution_test

WARNINGS = -Wall -Wextra
DEBUG_CFLAGS = -g -DDEBUG=
RELEASE_CFLAGS = -O2
DEFINE = 

#CFLAGS = --std=c99 -pedantic $(WARNINGS) $(DEBUG_CFLAGS) $(DEFINE)
CFLAGS = --std=c99 -pedantic $(WARNINGS) $(RELEASE_CFLAGS) $(DEFINE)
#LDFLAGS = -lgmp

default: $(EXEC_FILE)

tests/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC_FILE): $(OBJMODULES)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	strip ${EXEC_FILE}

ifneq (clean, $(MAKECMDGOALS))
ifneq (clang_analyze_clean, $(MAKECMDGOALS))
-include deps.mk
endif
endif

deps.mk: $(SRCMODULES) $(HEADERS)
	$(CC) -MM $^ > $@

clean:
	rm -f *.o tests/*.o $(EXEC_FILE) deps.mk *.core core

clang_analyze_clean:
	rm -f *.h.gch *.plist
