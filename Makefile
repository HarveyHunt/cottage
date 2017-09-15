CFLAGS += -Wall -pedantic -std=c99
CC ?= clang
BIN_NAME ?= cottage
PREFIX ?= /usr/local
BIN_DIR ?= $(PREFIX)/bin
SRCS = cottage.c

all:
	$(CC) $(SRCS) $(CFLAGS) -o $(BIN_NAME)

debug: CFLAGS += -O0 -g -DDEBUG
debug: all

clean:
	@rm -f ./*.o
	@rm -f $(BIN_NAME)

install:
	@install -Dm 0755 $(BIN_NAME) $(DESTDIR)$(BIN_DIR)/$(BIN_NAME)

check:
	@./checkpatch.pl --no-tree --ignore LONG_LINE,NEW_TYPEDEFS,UNNECESSARY_ELSE,MACRO_WITH_FLOW_CONTROL -f $(SRCS)

analyse:
	@scan-build -v -o analyse make debug
