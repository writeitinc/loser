NAME = loser

CFLAGS = $(WFLAGS) $(OPTIM) $(IFLAGS)
LFLAGS = -L$(LIB_DIR) -l:lib$(NAME).a -ltyrant

WFLAGS = -Wall -Wextra -pedantic -std=c99 -Winline
IFLAGS = -I$(INCLUDE_DIR)

WORKING_DIR = .
BUILD_DIR = build

INCLUDE_DIR = $(BUILD_DIR)/include
HEADER_DIR = $(INCLUDE_DIR)/$(NAME)

OBJ_DIR = $(BUILD_DIR)/obj
STATIC_OBJ_DIR = $(OBJ_DIR)/static
SHARED_OBJ_DIR = $(OBJ_DIR)/shared
TESTS_OBJ_DIR = $(OBJ_DIR)/tests

LIB_DIR = $(BUILD_DIR)/lib
STATIC_LIB = $(LIB_DIR)/lib$(NAME).a
SHARED_LIB = $(LIB_DIR)/lib$(NAME).so
LIBRARIES = $(STATIC_LIB) $(SHARED_LIB)

BIN_DIR = $(BUILD_DIR)/bin
BINARIES = $(BIN_DIR)/test $(BIN_DIR)/benchmark-funcs

.PHONY: default
default: release

.PHONY: release
release: OPTIM = -O3
release: dirs headers $(LIBRARIES) $(BINARIES)

.PHONY: debug
debug: DEBUG = -fsanitize=address,undefined
debug: OPTIM = -g
debug: dirs headers $(LIBRARIES) $(BINARIES)

# tests

TESTS_DIR = $(WORKING_DIR)/tests

$(BIN_DIR)/%: $(TESTS_OBJ_DIR)/%.o $(LIBRARIES)
	$(CC) -o $@ $< $(LFLAGS) $(DEBUG) $(DEFINES)

$(TESTS_OBJ_DIR)/%.o: $(TESTS_DIR)/%.c $(HEADERS)
	$(CC) -o $@ $< -c $(CFLAGS) $(DEBUG) $(DEFINES)

# library

SRC_DIR = $(WORKING_DIR)/src

SOURCES = $(wildcard $(SRC_DIR)/*.c)
HEADERS = $(wildcard $(SRC_DIR)/*.h)
STATIC_OBJS = $(patsubst $(SRC_DIR)/%.c, $(STATIC_OBJ_DIR)/%.o, $(SOURCES))
SHARED_OBJS = $(patsubst $(SRC_DIR)/%.c, $(SHARED_OBJ_DIR)/%.o, $(SOURCES))

PIC_FLAGS = -fPIC
RELRO_FLAGS = -Wl,-z,relro,-z,now

$(STATIC_LIB): $(STATIC_OBJS)
	$(AR) crs $@ $^

$(SHARED_LIB): $(SHARED_OBJS)
	$(CC) -o $@ $^ -shared $(PIC_FLAGS) $(RELRO_FLAGS) $(LFLAGS)

$(STATIC_OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) -o $@ $< -c $(CFLAGS) $(DEBUG) $(DEFINES)

$(SHARED_OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) -o $@ $< -c $(PIC_FLAGS) $(CFLAGS) $(DEBUG) $(DEFINES)

# headers

.PHONY: headers
headers: $(HEADER_DIR)

$(HEADER_DIR): $(HEADERS)
	mkdir -p $@
	cp -u $^ $@/
	touch $@

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
dirs: $(STATIC_OBJ_DIR)/ $(SHARED_OBJ_DIR)/ $(TESTS_OBJ_DIR)/ $(LIB_DIR)/ $(BIN_DIR)/ $(INCLUDE_DIR)/

%/:
	mkdir -p $@

# clean

.PHONY: clean
clean:
	$(RM) -r build/*
