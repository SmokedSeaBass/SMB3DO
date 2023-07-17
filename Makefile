#!/bin/sh
#=======================================#
#         DETECT ENV AND TARGET         #
#=======================================#
DETECTED_OS := $(shell uname -s)
ifeq ($(findstring MINGW32, $(DETECTED_OS)), MINGW32)
	TARGET_ENV := win32
	TARGET := smb3do.exe
else ifeq ($(findstring MINGW64, $(DETECTED_OS)), MINGW64)
	TARGET_ENV := win64
	TARGET := smb3do.exe
else ifeq ($(findstring Linux, $(DETECTED_OS)), Linux)
	TARGET_ENV := linux
	TARGET := smb3do
endif
TARGET_ENV ?= unknown

#=======================================#
#          PROJECT DIRECTORIES          #
#=======================================#
PROJECT_DIRECTORY  := .
APP_DIRECTORY      := bin/$(TARGET_ENV)
OBJECT_DIRECTORY   := build/$(TARGET_ENV)
DOCUMENT_DIRECTORY := docs
EXTERNAL_DIRECTORY := external
INCLUDE_DIRECTORY  := include
RESOURCE_DIRECTORY := data
SOURCE_DIRECTORY   := src

EXTERNAL_PACKAGES := $(notdir $(shell find $(EXTERNAL_DIRECTORY) -mindepth 1 -maxdepth 1 -type d))
EXTERNAL_ROOT_DIRECTORIES := $(addprefix $(EXTERNAL_DIRECTORY)/, $(EXTERNAL_PACKAGES))
EXTERNAL_INCLUDE_DIRECTORIES := $(addsuffix /include, $(EXTERNAL_ROOT_DIRECTORIES))
EXTERNAL_SOURCE_DIRECTORIES := $(addsuffix /src, $(EXTERNAL_ROOT_DIRECTORIES))
EXTERNAL_LIBRARY_DIRECTORIES := $(addsuffix /lib/$(TARGET_ENV), $(EXTERNAL_ROOT_DIRECTORIES))
EXTERNAL_OBJECT_DIRECTORIES := $(addsuffix /build/$(TARGET_ENV), $(EXTERNAL_ROOT_DIRECTORIES))

#=======================================#
#             PROJECT ITEMS             #
#=======================================#
SOURCES   := $(wildcard $(SOURCE_DIRECTORY)/*.cpp) $(wildcard $(SOURCE_DIRECTORY)/*.rc)
OBJECTS   := $(strip $(patsubst %, $(OBJECT_DIRECTORY)/%.o, $(basename $(notdir $(filter $(SOURCE_DIRECTORY)/%.cpp, $(SOURCES))))))
ifeq ($(findstring win, $(TARGET_ENV)), win)
	OBJECTS += $(strip $(patsubst %, $(OBJECT_DIRECTORY)/%.res, $(basename $(notdir $(filter $(SOURCE_DIRECTORY)/%.rc, $(SOURCES))))))
endif


EXTERNAL_SOURCES := $(strip $(foreach D, $(EXTERNAL_SOURCE_DIRECTORIES), $(wildcard $(D)/*.cpp)))
EXTERNAL_OBJECTS := $(strip $(foreach P, $(EXTERNAL_PACKAGES), $(patsubst %.cpp, $(EXTERNAL_DIRECTORY)/$(P)/build/$(TARGET_ENV)/%.o, $(notdir $(filter $(EXTERNAL_DIRECTORY)/$(P)/src/%, $(EXTERNAL_SOURCES))))))
EXTERNAL_LIBRARIES := $(strip $(foreach D, $(EXTERNAL_LIBRARY_DIRECTORIES), $(wildcard $(D)/*.dll)))

#=======================================#
#           COMPILER OPTIONS            #
#=======================================#
CXX := g++
CXXFLAGS := -std=c++14 -w $(shell pkg-config --cflags sdl2 SDL2_image SDL2_mixer SDL2_ttf)
ifeq ($(TARGET_ENV), win32)
	CXXFLAGS += -m32
endif

#=======================================#
#            LINKER OPTIONS             #
#=======================================#
LDFLAGS := 
LDFLAGS := $(shell pkg-config --libs sdl2 SDL2_image SDL2_mixer SDL2_ttf)
ifeq ($(findstring win,$(TARGET_ENV)), win)
	LDFLAGS += -mconsole
endif
LDFLAGS += -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic

#=======================================#
#                 COLOR                 #
#=======================================#
ANSI_LBLUE = \033[38;5;12m
ANSI_RESET = \033[0m

#=======================================#
#               RECIPIES                #
#=======================================#
.PHONY: all build debug release clean help

all: help

$(APP_DIRECTORY)/$(TARGET): $(OBJECTS) $(EXTERNAL_OBJECTS)
	@printf "$(ANSI_LBLUE)[?] Linking... [?]$(ANSI_RESET)\n"
	$(CXX) -o $@ $^ $(LDFLAGS)
	@printf "$(ANSI_LBLUE)[?] Copying .dll files... [?]$(ANSI_RESET)\n"
	$(foreach L,$(EXTERNAL_LIBRARIES),cp $(L) $(APP_DIRECTORY)/$(notdir $(L);))
	@printf "$(ANSI_LBLUE)[?] Copying data files... [?]$(ANSI_RESET)\n"
	cp -r $(RESOURCE_DIRECTORY) $(APP_DIRECTORY)/data

$(OBJECT_DIRECTORY)/%.o: $(SOURCE_DIRECTORY)/%.cpp
	@printf "$(ANSI_LBLUE)[?] Compiling source file $<... [?]$(ANSI_RESET)\n"
	$(CXX) $(CXXFLAGS) $(addprefix -I,$(INCLUDE_DIRECTORY)) $(addprefix -I,$(EXTERNAL_INCLUDE_DIRECTORIES)) -c $< -o $@

$(OBJECT_DIRECTORY)/%.res: $(SOURCE_DIRECTORY)/%.rc
	@printf "$(ANSI_LBLUE)[?] Compiling resource file $<... [?]$(ANSI_RESET)\n"
	windres -i $< -o $@ -O coff $(addprefix -I,$(INCLUDE_DIRECTORY))

$(EXTERNAL_DIRECTORY)/tinyxml2/build/$(TARGET_ENV)/%.o: $(EXTERNAL_DIRECTORY)/tinyxml2/src/%.cpp
	@printf "$(ANSI_LBLUE)[?] Compiling external source file $<... [?]$(ANSI_RESET)\n"
	$(CXX) $(CXXFLAGS) $(addprefix -I,$(EXTERNAL_INCLUDE_DIRECTORIES)) -c $< -o $@

build:
	@printf "$(ANSI_LBLUE)[?] Building for environment: $(TARGET_ENV) [?]$(ANSI_RESET)\n"
	@mkdir -p $(APP_DIRECTORY)
	@mkdir -p $(OBJECT_DIRECTORY)
	@mkdir -p $(DOCUMENT_DIRECTORY)
	@mkdir -p $(EXTERNAL_DIRECTORY)
	@mkdir -p $(foreach P, $(EXTERNAL_PACKAGES), $(EXTERNAL_DIRECTORY)/$(P)/build/$(TARGET_ENV))
	@mkdir -p $(INCLUDE_DIRECTORY)
	@mkdir -p $(RESOURCE_DIRECTORY)
	@mkdir -p $(SOURCE_DIRECTORY)

debug:
	$(eval CXXFLAGS += -DDEBUG -g)
	@$(MAKE) release

release: | build $(APP_DIRECTORY)/$(TARGET)

clean:
	@printf "$(ANSI_LBLUE)[?] Cleaning... [?]$(ANSI_RESET)\n"
	@rm -rvf $(OBJECT_DIRECTORY)
	@rm -rvf $(foreach ext, $(EXTERNAL_PACKAGES), $(EXTERNAL_DIRECTORY)/$(ext)/build)
	@rm -rvf $(APP_DIRECTORY)

help:
	@printf "usage:\n    make {debug|release}\n"
