TARGET = opengritel
SRC_DIR = src
OBJ_DIR = obj

ROOT_DIR := $(shell pwd)
CC = $(ROOT_DIR)/clang
CXX = $(ROOT_DIR)/clang++

LMDB_CFLAGS :=
LMDB_LIBS := -llmdb
GTK_CFLAGS :=
GTK_LIBS :=

ifneq ($(shell command -v pkg-config 2>/dev/null),)
    LMDB_CFLAGS := $(shell pkg-config --cflags lmdb 2>/dev/null)
    LMDB_LIBS := $(shell pkg-config --libs lmdb 2>/dev/null)
    GTK_CFLAGS := $(shell pkg-config --cflags gtk4 2>/dev/null)
    GTK_LIBS := $(shell pkg-config --libs gtk4 2>/dev/null)
endif

COMMON_CFLAGS = -Wall -Wextra -Wno-unused-parameter -Werror=implicit-function-declaration $(LMDB_CFLAGS) $(GTK_CFLAGS)
LDFLAGS = -pthread $(LMDB_LIBS) $(GTK_LIBS) -lm -llmdb
CLANG_INCLUDE_DIRS := $(shell echo '' | $(CC) -E -x c - -v 2>&1 | awk '/^ \// { print "-I" $$1 }')
INCLUDE_DIR = $(CLANG_INCLUDE_DIRS) -I./$(SRC_DIR)/include
COMMON_CFLAGS += $(INCLUDE_DIR)

BUILD_MODE ?= DEVELOPMENT
DEBUG_MODE ?= DEVELOPMENT
LOG_TO ?= SCREEN
ifeq ($(BUILD_MODE), PRODUCTION)
    FINAL_CFLAGS = $(COMMON_CFLAGS) -O3 -march=native -fomit-frame-pointer -fvectorize -DNDEBUG -DPRODUCTION
else
    ifeq ($(LOG_TO), FILE)
        FINAL_CFLAGS = $(COMMON_CFLAGS) -g -O3 -Werror -DDEVELOPMENT -DTOFILE
    else
        FINAL_CFLAGS = $(COMMON_CFLAGS) -g -O3 -Werror -DNDEBUG -DDEVELOPMENT -DTOSCREEN
    endif
endif

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),NetBSD)
    DISTRO_ID := netbsd
else ifeq ($(UNAME_S),FreeBSD)
    DISTRO_ID := freebsd
else ifeq ($(UNAME_S),OpenBSD)
    DISTRO_ID := openbsd
else
    DISTRO_ID := $(shell . /etc/os-release 2>/dev/null && echo $$ID || echo unknown)
endif

PKG_MANAGER := $(shell \
    if [ "$(DISTRO_ID)" = "netbsd" ]; then echo "pkgin"; \
    elif [ "$(DISTRO_ID)" = "freebsd" ]; then echo "pkg"; \
    elif [ "$(DISTRO_ID)" = "openbsd" ]; then echo "pkg_add"; \
    elif [ "$(DISTRO_ID)" = "rocky" ]; then echo "dnf"; \
    else echo "unsupported"; fi)

ifeq ($(UNAME_S),OpenBSD)
    USE_SUDO := doas
else
    USE_SUDO := $(shell command -v sudo >/dev/null 2>&1 && echo sudo || echo "")
endif

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: clean all debug

define install_pkg
    @echo ">> Memeriksa: $(1)"
    @if command -v $(1) >/dev/null 2>&1; then \
        echo ">> $(1) sudah tersedia (binary)."; \
    elif [ "$(PKG_MANAGER)" = "unsupported" ]; then \
        echo "!! Distribusi tidak didukung. Install $(1) manual."; \
    elif [ "$(PKG_MANAGER)" = "pkgin" ]; then \
        if pkg_info -e $(1) >/dev/null 2>&1; then \
            echo ">> $(1) sudah terinstal (paket)."; \
        else \
            echo ">> Menginstal $(1) via $(PKG_MANAGER)..."; \
            $(USE_SUDO) $(PKG_MANAGER) -y install $(1) || true; \
        fi; \
    elif [ "$(PKG_MANAGER)" = "pkg" ]; then \
        if pkg info -e $(1) >/dev/null 2>&1; then \
            echo ">> $(1) sudah terinstal (paket)."; \
        else \
            echo ">> Menginstal $(1) via $(PKG_MANAGER)..."; \
            $(USE_SUDO) $(PKG_MANAGER) install -y $(1) || true; \
        fi; \
    elif [ "$(PKG_MANAGER)" = "pkg_add" ]; then \
        if pkg_info -e $(1)-* >/dev/null 2>&1; then \
            echo ">> $(1) sudah terinstal (paket)."; \
        else \
            echo ">> Menginstal $(1) via $(PKG_MANAGER)..."; \
            $(USE_SUDO) $(PKG_MANAGER) $(1) || true; \
        fi; \
    elif [ "$(PKG_MANAGER)" = "dnf" ]; then \
        if dnf list installed $(1) >/dev/null 2>&1; then \
            echo ">> $(1) sudah terinstal (paket)."; \
        else \
            echo ">> Menginstal $(1) via $(PKG_MANAGER)..."; \
            $(USE_SUDO) $(PKG_MANAGER) -y install $(1) || true; \
        fi; \
    else \
        echo "!! Package manager tidak dikenali."; \
    fi
endef

libraries:
	@echo "Menginstall library production opengritel untuk $(DISTRO_ID) menggunakan $(PKG_MANAGER)..."
ifeq ($(DISTRO_ID),netbsd)
	$(call install_pkg,clang)
	@if [ ! -e $(CC) ]; then \
		$(USE_SUDO) $(PKG_MANAGER) -y install llvm; \
		echo ">> Membuat symlink $(CC)..."; \
		$(USE_SUDO) ln -s /usr/pkg/bin/clang $(CC); \
	else \
		echo ">> $(CC) sudah ada."; \
	fi
	@if [ ! -e $(CXX) ]; then \
		echo ">> Membuat symlink $(CXX)..."; \
		$(USE_SUDO) ln -s /usr/pkg/bin/clang++ $(CXX); \
	else \
		echo ">> $(CXX) sudah ada."; \
	fi
	$(call install_pkg,lmdb)
	$(call install_pkg,pkg-config)
	$(call install_pkg,gtk+4)
else ifeq ($(DISTRO_ID),freebsd)
	@if [ ! -e $(CC) ]; then \
		$(USE_SUDO) $(PKG_MANAGER) install -y llvm; \
		echo ">> Membuat symlink $(CC)..."; \
		$(USE_SUDO) ln -s /usr/bin/clang $(CC); \
	else \
		echo ">> $(CC) sudah ada."; \
	fi
	@if [ ! -e $(CXX) ]; then \
		echo ">> Membuat symlink $(CXX)..."; \
		$(USE_SUDO) ln -s /usr/bin/clang++ $(CXX); \
	else \
		echo ">> $(CXX) sudah ada."; \
	fi
	$(call install_pkg,lmdb)
	$(call install_pkg,pkgconf)
	$(call install_pkg,gtk4)
else ifeq ($(DISTRO_ID),openbsd)
	$(call install_pkg,clang-tools-extra)
	@if [ ! -e $(CC) ]; then \
		echo ">> Membuat symlink $(CC)..."; \
		$(USE_SUDO) ln -s /usr/local/bin/clang-21 $(CC); \
		$(USE_SUDO) $(PKG_MANAGER) llvm-21.1.2p0; \
	else \
		echo ">> $(CC) sudah ada."; \
	fi
	@if [ ! -e $(CXX) ]; then \
		echo ">> Membuat symlink $(CXX)..."; \
		$(USE_SUDO) ln -s /usr/local/bin/clang++-21 $(CXX); \
	else \
		echo ">> $(CXX) sudah ada."; \
	fi
	$(call install_pkg,lmdb)
	$(call install_pkg,bear)
	$(call install_pkg,gtk+4)
else ifeq ($(DISTRO_ID),rocky)
	$(call install_pkg,dnf-plugins-core)
	$(USE_SUDO) dnf config-manager --set-enabled crb
	$(call install_pkg,epel-release)
	$(USE_SUDO) dnf makecache
	$(call install_pkg,clang)
	@if [ ! -e $(CC) ]; then \
		$(USE_SUDO) $(PKG_MANAGER) -y install llvm; \
		echo ">> Membuat symlink $(CC)..."; \
		$(USE_SUDO) ln -s /usr/bin/clang $(CC); \
	else \
		echo ">> $(CC) sudah ada."; \
	fi
	@if [ ! -e $(CXX) ]; then \
		echo ">> Membuat symlink $(CXX)..."; \
		$(USE_SUDO) ln -s /usr/bin/clang++ $(CXX); \
	else \
		echo ">> $(CXX) sudah ada."; \
	fi
	$(call install_pkg,lmdb-libs)
	$(call install_pkg,lmdb-devel)
	$(call install_pkg,pkg-config)
	$(call install_pkg,gtk4-devel)
endif

dev:
	@if [ ! -e $(CC) ]; then ln -sf $$(which clang) $(CC); fi
	$(MAKE) $(TARGET)
	@echo "-------------------------------------"
	@echo "opengritel dikompilasi dalam mode DEVELOPMENT!"
	@echo "Executable: $(TARGET)"
	@echo "-------------------------------------"

prod:
	@if [ ! -e $(CC) ]; then ln -sf $$(which clang) $(CC); fi
	$(MAKE) $(TARGET) BUILD_MODE=PRODUCTION
	@echo "-------------------------------------"
	@echo "opengritel dikompilasi dalam mode PRODUCTION!"
	@echo "Executable: $(TARGET)"
	@echo "-------------------------------------"

$(TARGET): $(OBJS)
	$(CC) $(FINAL_CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(FINAL_CFLAGS) -c -o $@ $<

clean:
	@echo "Membersihkan file objek dan executable..."
	rm -rf $(OBJ_DIR) $(TARGET) compile_commands.json

nobearall: prod

nobeardebug: dev

all: libraries
	bear -- $(MAKE) nobearall || true
	@echo "Jalankan ./opengritel dalam mode production..."

debug: libraries
	bear -- $(MAKE) nobeardebug || true
	@echo "Jalankan ./opengritel dalam mode debug..."
