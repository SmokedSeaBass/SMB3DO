# Super Mario Bros.  3-Do (SMB3DO)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/SmokedSeaBass/SMB3DO?include_prereleases)](https://github.com/SmokedSeaBass/SMB3DO/releases) [![GitHub commit activity](https://img.shields.io/github/commit-activity/m/SmokedSeaBass/SMB3DO)](https://github.com/SmokedSeaBass/SMB3DO/commits) 
### A Super Mario Bros. 3 Fangame
A personal project that aims to _three_-do the classic Nintendo NES game Super Mario Bros. 3.  This project arose out of my dissatisfaction of Super Mario Maker and Super Mario Maker 2's shortcomings and limitations, particularly in the SMB3 style.  While ROM hacking is always an option, SMB3's compression makes it a particarly tricky task to mod without overwritting random memory in the ROM (last time I checked).  Plus, creating my own rendition would allow me to add new features and tweak anything to my liking.  So here we are, SMB3 remade for the modern era.

This project is written in C++14 and runs on the SDL2 framework.

<img src="docs/smb3do_v0-1-0_sample2.png"/>

## <img src="docs/smb3do_bullet.png" width="32px"/> Features <img src="docs/smb3do_bullet.png" width="32px"/>
### Current Release (v0.1.0)
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

### Coming Soon
* [ ] Basic camera
* [ ] Hitbox visualization toggle

### Long-Term Plans:
* Implement all features of the original SMB3
* Create my own SMB3 'sequel'
* User-friendly level/map/game editor
* Support for NES, All-Stars, and custom assets
* "Retro" and "Modern" video options
* Controller/keyboard support
* Linux and Windows support
* Variable refresh rate (e.g. 120 Hz) support

[Detailed development roadmap](https://docs.google.com/spreadsheets/d/1Y0XjZVZ6z5f_Yi8HuqKNcNB47MKjdfytgAIx97y7Uow/edit?usp=sharing)

## <img src="docs/smb3do_bullet.png" width="32px"/> How to Use <img src="docs/smb3do_bullet.png" width="32px"/>
Download the `.zip` file for the latest relase in the Releases tab.  Unzip to a location of your choice and then simply run `smb3do.exe`!  If you want to try out the latest features (and bugs), you can optionally build your own executable directly from the source code (see "Building From Source" below).

### Controls
* [W][A][S][D] - Move / [K] - Run / [L] - Jump
* [I] - Toggle integer/best-fit scaling (disables widescreen mode)
* [O] - Toggle widescreen/standard mode (disables integer scaling)
* [P] - Toggle between 8:7 and 4:3 aspect ratios (i.e. between perfectly square and CRT-like pixels)
* [F11] - Toggle fullscreen/windowed mode

## <img src="docs/smb3do_bullet.png" width="32px"/> Building From Source <img src="docs/smb3do_bullet.png" width="32px"/>
The Visual Studio solution file `smb3do.sln` is included for your convenience; after cloning the repository, you should be able to open the file and easily build from within Microsoft Visual Studio.  However, you'll need to first download and install [Simple Directmedia Layer 2.0](https://www.libsdl.org/).  Installation instructions can be found here: [Lazy Foo' Productions - Hello SDL](https://lazyfoo.net/tutorials/SDL/01_hello_SDL/index.php).  Remember to change the project's include directories according to your SDL2 install location.

## <img src="docs/smb3do_bullet.png" width="32px"/> Dependencies and Utilities <img src="docs/smb3do_bullet.png" width="32px"/>
* [Simple Directmedia Layer 2.0](https://www.libsdl.org/) (v2.0.14)
* [TinyXML2](https://github.com/leethomason/tinyxml2) (v8.1.0)
* [Visual Studio Community 2019](https://visualstudio.microsoft.com/) (v16)

## <img src="docs/smb3do_bullet.png" width="32px"/> Credits <img src="docs/smb3do_bullet.png" width="32px"/>
* SmokedSeaBass - Programmer/Designer
* Nintendo - Original author of Super Mario Bros. 3
* Jdaster64 - SMB3 physics breakdown
* Southbird - SMB3 disassembly
