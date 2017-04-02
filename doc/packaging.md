# Packaging a Sol game for distribution

To package your game for distribution, you should create a zip archive containing the game's source code and assets. The game's `main.lua` file should be at the root of this archive.

The zip file should be renamed to `pak0` (with no extension) and placed in the same directory as the Sol executable. When Sol runs it will search for the `pak0` file and load it if it exists.

### Windows

The dynamically linked libraries should be included when distributing your game. On Windows these are `SDL.dll` and `lua51.dll`. The Sol executable can be renamed to the title of your game. This should result in the following files:

```
game_title
├── game_title.exe  (sol executable)
├── pak0            (zip archive of game)
├── lua51.dll
└── SDL.dll

```

### MacOS

The dynamically linked libraries should also be included on MacOS when distributing your game. On MacOS these are `libSDL-1.2.0.dylib` and `libluajit-5.1.2.dylib` or `liblua5.1.dylib`. You need to place these files in certain folders for MacOS. This should result in the following files:

```
game_title.app
└── Contents
    ├── Frameworks
    │   ├── libSDL-1.2.0.dylib
    │   └── libluajit-5.1.2.dylib  (or liblua5.1.dylib)
    ├── Info.plist
    ├── MacOS
    │   └── sol                   (sol executable)
    └── Resources
        └── pak0                   (zip archive of game)

```

### Linux

For Linux, it is encouraged to distribute with a file saying what packages need to be installed. This should result in the following files:

```
game_title
├── game_title  (sol executable)
├── pak0        (zip archive of game)
└── readme.txt  (file supplying information on packages)

```
