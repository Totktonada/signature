SRC = \
	u256.c \
	substitution.c \
	hash.c \
	ioutils.c

OBJ = $(SRC:.c=.o)
HEADERS = $(SRC:.c=.h)

TESTS_SRC = \
	tests/ioutils_test.c \
	tests/substitution_test.c \
	tests/hash_test.c

TESTS_OBJ = $(SRC:.c=.o)
TESTS_HEADERS = $(SRC:.c=.h)

EXEC_FILES = $(TESTS_SRC:.c=)

WARNINGS = -Wall -Wextra
DEBUG_CFLAGS = -g -DDEBUG=
RELEASE_CFLAGS = -O2
DEFINE = 

#CFLAGS = --std=c99 -pedantic $(WARNINGS) $(DEBUG_CFLAGS) $(DEFINE)
CFLAGS = --std=c99 -pedantic $(WARNINGS) $(RELEASE_CFLAGS) $(DEFINE)
#LDFLAGS = -lgmp

default: $(EXEC_FILES)

tests/ioutils_test: $(OBJ) tests/ioutils_test.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	strip $@

tests/substitution_test: $(OBJ) tests/substitution_test.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	strip $@

tests/hash_test: $(OBJ) tests/hash_test.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	strip $@

ifneq (clean, $(MAKECMDGOALS))
ifneq (clang_analyze_clean, $(MAKECMDGOALS))
-include deps.mk
endif
endif

deps.mk: $(SRC) $(TESTS_SRC) $(HEADERS) $(TESTS_HEADERS)
	$(CC) -MM $^ > $@

clean:
	rm -f $(OBJ) $(TESTS_OBJ) $(EXEC_FILES) deps.mk *.core core

clang_analyze_clean:
	rm -f *.h.gch *.plist
