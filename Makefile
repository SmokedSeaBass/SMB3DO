TARGET := smb3do.exe

#=======================================#
#          PROJECT DIRECTORIES          #
#=======================================#
PROJECT_DIRECTORY  := .
APP_DIRECTORY      := build/Win32/Debug
OBJECT_DIRECTORY   := build/Win32/Debug/obj
DOCUMENT_DIRECTORY := docs
EXTERNAL_DIRECTORY := external
LIBRARY_DIRECTORY  := lib
RESOURCE_DIRECTORY := data
SOURCE_DIRECTORY   := src

MINGW_DIRECTORY    := D:/msys64/mingw32

#=======================================#
#           COMPILER OPTIONS            #
#=======================================#
SOURCES := $(wildcard $(SOURCE_DIRECTORY)/*.cpp) $(EXTERNAL_DIRECTORY)/tinyxml2/tinyxml2.cpp
OBJECTS := $(patsubst $(SOURCE_DIRECTORY)/%.cpp,$(OBJECT_DIRECTORY)/%.o,$(SOURCES)) $(OBJECT_DIRECTORY)/smb3do.res

CXX := $(MINGW_DIRECTORY)/bin/g++
CXXFLAGS := -std=c++14 -w -mwindows -mconsole 

#=======================================#
#            LINKER OPTIONS             #
#=======================================#
LDFLAGS := -L$(MINGW_DIRECTORY)/lib
LDLIBS := -lSDL2_image -lSDL2_ttf -lmingw32 -lSDL2main -lSDL2 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic

INCLUDES := -I$(EXTERNAL_DIRECTORY) -I$(MINGW_DIRECTORY)/include/SDL2

#=======================================#
#                TARGETS                #
#=======================================#
.PHONY: all build clean

all: build $(APP_DIRECTORY)/$(TARGET)

$(APP_DIRECTORY)/$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	@echo "Copying .dll files..."
	cp $(EXTERNAL_DIRECTORY)/sdl2/lib/x86/SDL2.dll $(APP_DIRECTORY)/SDL2.dll
	cp $(EXTERNAL_DIRECTORY)/sdl2-image/lib/x86/SDL2_image.dll $(APP_DIRECTORY)/SDL2_image.dll
	cp $(EXTERNAL_DIRECTORY)/sdl2-ttf/lib/x86/SDL2_ttf.dll $(APP_DIRECTORY)/SDL2_ttf.dll
	@echo "Copying target to project directory..."
	cp $(APP_DIRECTORY)/$(TARGET) $(PROJECT_DIRECTORY)/$(TARGET)
	cp $(EXTERNAL_DIRECTORY)/sdl2/lib/x86/SDL2.dll $(PROJECT_DIRECTORY)/SDL2.dll
	cp $(EXTERNAL_DIRECTORY)/sdl2-image/lib/x86/SDL2_image.dll $(PROJECT_DIRECTORY)/SDL2_image.dll
	cp $(EXTERNAL_DIRECTORY)/sdl2-ttf/lib/x86/SDL2_ttf.dll $(PROJECT_DIRECTORY)/SDL2_ttf.dll

$(OBJECTS): $(OBJECT_DIRECTORY)/%.o: $(SOURCE_DIRECTORY)/%.cpp
	@echo "Compiling..."
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

$(OBJECT_DIRECTORY)/smb3do.res: smb3do.rc
	@echo "Building resource file..."
	windres smb3do.rc -O coff -o $(OBJECT_DIRECTORY)/smb3do.res

build:
	@echo "Building workspace..."
	@mkdir -p $(APP_DIRECTORY)
	@mkdir -p $(OBJECT_DIRECTORY)
	@mkdir -p $(DOCUMENT_DIRECTORY)
	@mkdir -p $(EXTERNAL_DIRECTORY)
	@mkdir -p $(LIBRARY_DIRECTORY)
	@mkdir -p $(RESOURCE_DIRECTORY)
	@mkdir -p $(SOURCE_DIRECTORY)

clean:
	@echo "Cleaning..."
	@rm -rvf $(OBJECT_DIRECTORY)
	@rm -rvf $(APP_DIRECTORY)/$(TARGET)
	@rm -rvf $(APP_DIRECTORY)/SDL2.dll
	@rm -rvf $(APP_DIRECTORY)/SDL2_image.dll
	@rm -rvf $(APP_DIRECTORY)/SDL2_ttf.dll
	@rm -rvf $(TARGET)
	@rm -rvf SDL2.dll
	@rm -rvf SDL2_image.dll
	@rm -rvf SDL2_ttf.dll
