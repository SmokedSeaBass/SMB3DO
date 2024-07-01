# Super Mario Bros. 3-Do (SMB3DO)

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/SmokedSeaBass/SMB3DO?include_prereleases)](https://github.com/SmokedSeaBass/SMB3DO/releases) [![GitHub commit activity](https://img.shields.io/github/commit-activity/m/SmokedSeaBass/SMB3DO/dev?logo=github)](https://github.com/SmokedSeaBass/SMB3DO/commits/dev)

### A Super Mario Bros. 3 fangame

A personal project that aims to th**re**e-**do** the classic Nintendo NES game Super Mario Bros. 3.  This project arose out of my dissatisfaction of Super Mario Maker and Super Mario Maker 2's shortcomings and limitations, particularly in the SMB3 style.  While ROM hacking is always an option, SMB3's compression makes it somewhat tricky to mod (last time I checked).  Plus, creating my own rendition would allow me to add new features and tweak anything to my liking.  So, here we are.

This project is written in C++17 and uses the [SDL3](https://github.com/libsdl-org/SDL) framework.

## <img src="docs/images/bullet_point.png" width="32px"/> Screenshots <img src="docs/images/bullet_point.png" width="32px"/>

<img src="docs/images/smb3do_v0-2-0_sample4.png" width=40%/> <img src="docs/images/smb3do_v0-2-0_sample2.png" width=40%/>
<img src="docs/images/smb3do_v0-2-0_sample3.png" width=80%/>
<img src="docs/images/smb3do_v0-2-0_sample5.png" width=40%/><br>

## <img src="docs/images/bullet_point.png" width="32px"/> Features and Changelog <img src="docs/images/bullet_point.png" width="32px"/>

### Current Release (v0.2.0)

* Basic camera
* Basic level format
* Animated Mario
* Improved variable refresh rate support
* Hitbox visualization toggle
* On-screen debug info
* Collectable coins
* Semisolid platforms

#### v0.1.0

* Keyboard input (see "How to Use")
* Hardware accelerated rendering
* Support for multiple resolutions
* Fullscreen/windowed modes
* SD/Widescreen video modes
* Basic variable refresh rate support
* Simple sprite/image blitting
* Tileset and tilemap rendering
* Tile-based collision
* Basic SMB3-accurate player physics (walking, running, and jumping)

### Long-Term Plans

* Implement all features of the original SMB3
* Create my own SMB3 'sequel' game
* User-friendly level/map/game editor
* Support for NES, All-Stars, and custom assets
* "Retro" and "Modern" video options
* Controller support
* Proper Linux support
* Comprehensive variable refresh rate support

[Detailed development roadmap](https://docs.google.com/spreadsheets/d/1Y0XjZVZ6z5f_Yi8HuqKNcNB47MKjdfytgAIx97y7Uow/edit?usp=sharing)

## <img src="docs/images/bullet_point.png" width="32px"/> How to Use <img src="docs/images/bullet_point.png" width="32px"/>

Download the `.zip` file for the latest relase in the Releases tab.  Unzip to a location of your choice and then simply run `smb3do.exe`!  If you want to try out the latest features (and bugs), you can build your own executable directly from the source code (see "Building From Source" below).

### Controls

* <img src="docs/images/keys/wasd.png" style="display:inline-block;height:24px;margin-bottom:-6px"/>: Move / <img src="docs/images/keys/k.png" style="display:inline-block;height:24px;margin-bottom:-6px"/>: Run / <img src="docs/images/keys/l.png" style="display:inline-block;height:24px;margin-bottom:-6px"/>: Jump
* <img src="docs/images/keys/h.png" style="display:inline-block;height:24px;margin-bottom:-6px"/> - Toggle hitbox visual
* <img src="docs/images/keys/u.png" style="display:inline-block;height:24px;margin-bottom:-6px"/> - Toggle debug info
* <img src="docs/images/keys/i.png" style="display:inline-block;height:24px;margin-bottom:-6px"/> - Toggle integer/best-fit scaling (disables widescreen mode)
* <img src="docs/images/keys/o.png" style="display:inline-block;height:24px;margin-bottom:-6px"/> - Toggle widescreen/standard mode (disables integer scaling)
* <img src="docs/images/keys/p.png" style="display:inline-block;height:24px;margin-bottom:-6px"/> - Toggle between 8:7 and 4:3 aspect ratios (i.e. between perfectly square and CRT-like pixels)
* <img src="docs/images/keys/f11.png" style="display:inline-block;height:24px;margin-bottom:-6px"/> - Toggle fullscreen/windowed mode

## <img src="docs/images/bullet_point.png" width="32px"/> Building From Source <img src="docs/images/bullet_point.png" width="32px"/>

Visual Studio solution and project files have been removed for now as part of streamlining the build process.  The project instead includes a Makefile and now compiles using `g++` on MinGW32/MinGW64 (Windows) or Linux.

### Windows

1. Install [MSYS2](https://www.msys2.org/)
2. Install `SDL3`, `SDL3_image`, `SDL3_mixer`, and `SDL3_ttf`
    * At the moment SDL3 prebuilt binaries are unavailable for MSYS2 environments.  You will have to [compile SDL3](https://wiki.libsdl.org/SDL3/Installation) yourself.
3. Clone this repo
4. Run `make release` from the project root
5. Run `./bin/{win32|win64}/smb3do.exe`

### Linux (Debian)

1. Install `SDL3`, `SDL3_image`, `SDL3_mixer`, and `SDL3_ttf`
    * `sudo apt-get install libsdl3-dev libsdl3-image-dev libsdl3-mixer-dev libsdl3-ttf-dev`
2. Clone this repo
3. Run `make release` from the project root
4. Run `./bin/linux/smb3do`

## <img src="docs/images/bullet_point.png" width="32px"/> Dependencies <img src="docs/images/bullet_point.png" width="32px"/>

* [Simple Directmedia Layer 3.0](https://www.libsdl.org/) (v3.1.0)
* [TinyXML2](https://github.com/leethomason/tinyxml2) (v10.0.0)

## <img src="docs/images/bullet_point.png" width="32px"/> Credits <img src="docs/images/bullet_point.png" width="32px"/>

* SmokedSeaBass - Designer and Core Programmer

### Special Thanks

* Jdaster64 - SMB3 player physics breakdown
* Southbird - SMB3 disassembly
* Christopher Hebert - ["Reconstructing Cave Story" YouTube series](https://www.youtube.com/watch?v=IufkC1IRY2Q&list=PL006xsVEsbKjSKBmLu1clo85yLrwjY67X)
* Lazy Foo' Productions - [Beginning Game Programming v2.0](https://lazyfoo.net/tutorials/SDL/index.php) (SDL2 Tutorials)
* Nintendo - Developer/Publisher of Super Mario Bros. 3
