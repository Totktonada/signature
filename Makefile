SRC = \
	u256.c \
	substitution.c \
	hash.c \
	signature.c \
	ioutils.c

OBJ = $(SRC:.c=.o)
HEADERS = $(SRC:.c=.h)

TESTS_SRC = \
	tests/ioutils_test.c \
	tests/substitution_test.c \
	tests/hash_test.c \
	tests/signature_test.c \

TESTS_OBJ = $(TESTS_SRC:.c=.o)
TESTS_HEADERS = $(TESTS_SRC:.c=.h)
TESTS_HEADERS += tests/test_utils.h
TEST_FILES = \
	tests/hashtest1.data \
	tests/hashtest1.hash \
	tests/hashtest2.data \
	tests/hashtest2.hash

EXEC_FILES = $(TESTS_SRC:.c=)

WARNINGS = -Wall -Wextra
DEBUG_CFLAGS = -g -DDEBUG=
RELEASE_CFLAGS = -O2
#DEFINE = -DNO_MMAP
DEFINE =

#CFLAGS = --std=c99 -pedantic $(WARNINGS) $(DEBUG_CFLAGS) $(DEFINE)
#STRIP = /bin/true

CFLAGS = --std=c99 -pedantic $(WARNINGS) $(RELEASE_CFLAGS) $(DEFINE)
STRIP = strip

LDFLAGS = -lgmp

default: $(EXEC_FILES)

tests/ioutils_test: $(OBJ) tests/ioutils_test.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@
	$(STRIP) $@

tests/substitution_test: $(OBJ) tests/substitution_test.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@
	$(STRIP) $@

tests/hash_test: $(OBJ) tests/hash_test.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@
	$(STRIP) $@

tests/signature_test: $(OBJ) tests/signature_test.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@
	$(STRIP) $@

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

hashtest: tests/hash_test $(TEST_FILES)
	./tests/hash_test tests/hashtest1.data
	cat tests/hashtest1.hash
	./tests/hash_test tests/hashtest2.data
	cat tests/hashtest2.hash
