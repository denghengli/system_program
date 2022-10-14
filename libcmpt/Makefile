PROGRAM = libcmpt
VERSION = v20220320
AUTHOR = denghengli
DATE = $(shell date +%Y%m%d)
#BIN_NAME = $(PROGRAM)_$(DATE)
BIN_NAME = bin/$(PROGRAM)
CC = gcc

BUILD_DIR = ./.build
INCLUDEPATHS = $(patsubst %,-I%,$(shell find ./lib/*/include ./include ./src -maxdepth 4 -type d))
#CFLAGS = -D__PROGRAM=\"$(PROGRAM)\" -D__AUTHOR=\"$(AUTHOR)\" -D__VERSION=\"$(VERSION)\" #通过宏定义传递程序版本信息
CFLAGS += -Wall $(INCLUDEPATHS)
#LIBPATHS = $(patsubst %,-L%,$(wildcard ./lib/*/lib))
#LDFLAGS = $(LIBPATHS) -lxml2 -lsqlite3 -lcmpt2 -lcurl -lcrypto -lssl -lrt -lhardware -lz -lw5100hb
LDFLAGS = -pthread -lsqlite3

OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(wildcard src/*.c) $(wildcard lib/*/src/*.c))
HEADS = $(wildcard include/*.h src/*.h lib/*/include/*.h lib/*/src/*.h)

all: $(BIN_NAME)

$(BIN_NAME): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^

$(OBJS): $(BUILD_DIR)/%.o: %.c $(HEADS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $(CFLAGS) -c $<

print:
	@echo BUILD_DIR = $(BUILD_DIR)
	
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(PROGRAM)*

.PHONY: clean all



