# This Makefile is based on a template (lib+tests.makefile version 1.0.0).
# See: https://github.com/writeitinc/makefile-templates

NAME = loser
SOURCE_DIR = $(WORKING_DIR)/src
TEST_DIR = $(WORKING_DIR)/tests

ifndef NAME
$(error NAME is not set)
endif

CFLAGS = $(WFLAGS) $(OPTIM) $(IFLAGS)

WFLAGS = -Wall -Wextra -pedantic -std=c99 -Winline
IFLAGS = -I$(INCLUDE_DIR)

WORKING_DIR = .
BUILD_DIR = build

INCLUDE_DIR = $(BUILD_DIR)/include
HEADER_DIR = $(INCLUDE_DIR)/$(NAME)

OBJ_DIR = $(BUILD_DIR)/obj
STATIC_OBJ_DIR = $(OBJ_DIR)/static
SHARED_OBJ_DIR = $(OBJ_DIR)/shared
TEST_OBJ_DIR = $(OBJ_DIR)/tests

LIB_DIR = $(BUILD_DIR)/lib
BIN_DIR = $(BUILD_DIR)/bin

LIBRARIES = $(STATIC_LIB) $(SHARED_LIB)
STATIC_LIB = $(LIB_DIR)/lib$(NAME).a
SHARED_LIB = $(LIB_DIR)/lib$(NAME).so

BINARIES = $(BIN_DIR)/test $(BIN_DIR)/benchmark-funcs

.PHONY: default
default: release

.PHONY: release
release: OPTIM = -O3 $(LTOFLAGS)
release: dirs headers $(LIBRARIES) $(BINARIES)

.PHONY: debug
debug: DEBUG = -fsanitize=address,undefined
debug: OPTIM = -g
debug: dirs headers $(LIBRARIES) $(BINARIES)

# library

SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
HEADERS = $(wildcard $(SOURCE_DIR)/*.h)
STATIC_OBJS = $(patsubst $(SOURCE_DIR)/%.c, $(STATIC_OBJ_DIR)/%.o, $(SOURCES))
SHARED_OBJS = $(patsubst $(SOURCE_DIR)/%.c, $(SHARED_OBJ_DIR)/%.o, $(SOURCES))

PIC_FLAGS = -fPIC

$(STATIC_LIB): $(STATIC_OBJS)
	$(AR) crs $@ $^

$(SHARED_LIB): $(SHARED_OBJS)
	$(CC) -o $@ $^ -shared $(PIC_FLAGS) $(LDFLAGS)

$(STATIC_OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	$(CC) -o $@ $< -c $(CFLAGS) $(DEBUG) $(DEFINES)

$(SHARED_OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	$(CC) -o $@ $< -c $(PIC_FLAGS) $(CFLAGS) $(DEBUG) $(DEFINES)

# headers

.PHONY: headers
headers: $(HEADER_DIR)

$(HEADER_DIR): $(HEADERS)
	mkdir -p $@
	cp -u -t $@/ $^
	touch $@

# tests

TEST_HEADERS = $(wildcard $(TEST_DIR)/*.h)

TEST_LDFLAGS = -L$(LIB_DIR) -l:lib$(NAME).a -l:libtyrant.a

$(BIN_DIR)/%: $(TEST_OBJ_DIR)/%.o $(LIBRARIES)
	$(CC) -o $@ $< $(TEST_LDFLAGS) $(DEBUG) $(DEFINES)

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.c $(HEADERS) $(TEST_HEADERS)
	$(CC) -o $@ $< -c $(CFLAGS) $(DEBUG) $(DEFINES)

# deps

TYRANT_DIR = $(WORKING_DIR)/tyrant
SEIFU_DIR = $(WORKING_DIR)/seifu

.PHONY: deps
deps: tyrant seifu

.PHONY: tyrant
tyrant:
	make -f $(TYRANT_DIR)/Makefile WORKING_DIR=$(TYRANT_DIR)

.PHONY: seifu
seifu: $(BUILD_DIR)/include/seifu/seifu.h

$(BUILD_DIR)/include/seifu/seifu.h: $(SEIFU_DIR)/src/seifu/seifu.h
	mkdir -p $(INCLUDE_DIR)/seifu
	cp $< $@

# dirs

.PHONY: dirs
dirs: $(INCLUDE_DIR)/ $(STATIC_OBJ_DIR)/ $(SHARED_OBJ_DIR)/ $(TEST_OBJ_DIR)/ $(LIB_DIR)/ $(BIN_DIR)/

%/:
	mkdir -p $@

# install

VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)
VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_PATCH = 0

DEST_DIR = # root

.PHONY: install-linux
install-linux:
	install -Dm755 "build/lib/lib$(NAME).so"        "$(DEST_DIR)/usr/lib/lib$(NAME).so.$(VERSION)"
	ln -snf        "lib$(NAME).so.$(VERSION)"       "$(DEST_DIR)/usr/lib/lib$(NAME).so.$(VERSION_MAJOR)"
	ln -snf        "lib$(NAME).so.$(VERSION_MAJOR)" "$(DEST_DIR)/usr/lib/lib$(NAME).so"
	
	install -Dm644 -t "$(DEST_DIR)/usr/lib/"                    "build/lib/lib$(NAME).a"
	install -Dm644 -t "$(DEST_DIR)/usr/include/$(NAME)/"        "build/include/$(NAME)/$(NAME).h"
	install -Dm644 -t "$(DEST_DIR)/usr/share/licenses/$(NAME)/" "LICENSE"
	install -Dm644 -t "$(DEST_DIR)/usr/share/doc/$(NAME)/"      "README.md"

# clean

.PHONY: clean
clean:
	$(RM) -r build/*
