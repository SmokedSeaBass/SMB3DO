TARGET := smb3do.exe
ARCH := $(MSYSTEM_CARCH)

#=======================================#
#          PROJECT DIRECTORIES          #
#=======================================#
ROOT_DIRECTORY     := $(MSYSTEM_PREFIX)
PROJECT_DIRECTORY  := .
APP_DIRECTORY      := bin/$(ARCH)
OBJECT_DIRECTORY   := build/$(ARCH)
DOCUMENT_DIRECTORY := docs
EXTERNAL_DIRECTORY := external
INCLUDE_DIRECTORY  := include
RESOURCE_DIRECTORY := data
SOURCE_DIRECTORY   := src

#=======================================#
#           COMPILER OPTIONS            #
#=======================================#
HEADERS   := $(wildcard $(INCLUDE_DIRECTORY)/*.h) $(wildcard $(EXTERNAL_DIRECTORY)/*/include/*.h)
SOURCES   := $(wildcard $(SOURCE_DIRECTORY)/*.cpp) $(wildcard $(EXTERNAL_DIRECTORY)/*/src/*.cpp)
OBJECTS   := $(patsubst $(SOURCE_DIRECTORY)/%.cpp,$(OBJECT_DIRECTORY)/%.o,$(SOURCES)) $(OBJECT_DIRECTORY)/smb3do.res
INCLUDES  := $(INCLUDE_DIRECTORY) $(wildcard $(EXTERNAL_DIRECTORY)/*/include)
LIBRARIES := $(wildcard $(EXTERNAL_DIRECTORY)/*/lib/$(ARCH)/*.dll)

CXX := g++
CXXFLAGS := -std=c++14 -w $(shell pkg-config --cflags sdl2 sdl2_image sdl2_ttf)

#=======================================#
#            LINKER OPTIONS             #
#=======================================#
LDFLAGS := 
LDLIBS := $(shell pkg-config --libs sdl2 sdl2_image sdl2_mixer sdl2_ttf) -mconsole -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread

#=======================================#
#                 COLOR                 #
#=======================================#
ANSI_LBLUE = \x1b[38;5;12m
ANSI_RESET = \x1b[0m

#=======================================#
#               RECIPIES                #
#=======================================#
.PHONY: all build debug clean

all: build $(APP_DIRECTORY)/$(TARGET)

$(APP_DIRECTORY)/$(TARGET): $(OBJECTS)
	@echo -e "$(ANSI_LBLUE):: Linking...$(ANSI_RESET)"
	$(CXX) $(CXXFLAGS) $(addprefix -I,$(INCLUDES)) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@echo -e "$(ANSI_LBLUE):: Copying .dll files...$(ANSI_RESET)"
	$(foreach lib,$(LIBRARIES),cp $(lib) $(APP_DIRECTORY)/$(notdir $(lib);))
	@echo -e "$(ANSI_LBLUE):: Copying data files...$(ANSI_RESET)"
	cp -r $(RESOURCE_DIRECTORY) $(APP_DIRECTORY)/data

$(OBJECT_DIRECTORY)/%.o: $(SOURCE_DIRECTORY)/%.cpp
	@echo -e "$(ANSI_LBLUE):: Compiling source file $<...$(ANSI_RESET)"
	$(CXX) $(CXXFLAGS) $(addprefix -I,$(INCLUDES)) -c $< -o $@

$(OBJECT_DIRECTORY)/%.res: $(SOURCE_DIRECTORY)/%.rc
	@echo -e "$(ANSI_LBLUE):: Compiling resource file $<...$(ANSI_RESET)"
	windres -i $< -o $@ -O coff $(addprefix -I,$(INCLUDES))

debug: CXXFLAGS += -DDEBUG -g
debug: all

build:
	@mkdir -p $(APP_DIRECTORY)
	@mkdir -p $(OBJECT_DIRECTORY)
	@mkdir -p $(DOCUMENT_DIRECTORY)
	@mkdir -p $(EXTERNAL_DIRECTORY)
	@mkdir -p $(INCLUDE_DIRECTORY)
	@mkdir -p $(RESOURCE_DIRECTORY)
	@mkdir -p $(SOURCE_DIRECTORY)

clean:
	@echo -e "$(ANSI_LBLUE):: Cleaning...$(ANSI_RESET)"
	@rm -rvf $(OBJECT_DIRECTORY)
	@rm -rvf $(APP_DIRECTORY)
