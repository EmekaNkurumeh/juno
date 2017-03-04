# Packaging a Juno game for distribution

To package your game for distribution, you should create a zip archive containing the game's source code and assets. The game's `main.lua` file should be at the root of this archive.

The zip file should be renamed to `pak0` (with no extension) and placed in the same directory as the Juno executable. When Juno runs it will search for the `pak0` file and load it if it exists.

Alternatively, you may run the command `juno --pack dir out`, where `dir` is the directory that contains your game's source code and assets and `out` is directory where you want the Juno executable and the `pak0` file placed.

### Windows

The dynamically linked libraries should be included when distributing your game. On Windows these are `SDL.dll` and `lua51.dll`. The Juno executable can be renamed to the title of your game. This should result in the following files:

```
game_title
├── game_title.exe  (juno executable)
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
    │   └── juno                   (juno executable)
    └── Resources
        └── pak0                   (zip archive of game)

```

### Linux

For Linux, it is encouraged to distribute with a file saying what packages need to be installed. This should result in the following files:

```
game_title
├── game_title  (juno executable)
├── pak0        (zip archive of game)
└── readme.txt  (file supplying information on packages)

```
