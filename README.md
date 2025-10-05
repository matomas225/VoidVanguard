# VoidVanguard

A thrilling 2D space shooter game built with SDL2 lib and C language. Control a cube in space, defend against waves of enemy cubes, upgrade your abilities, and achieve the highest score!

## Features

- Dynamic enemy waves of cubes
- Upgrade system for weapon and abilities
- Sound effects and background music
- Multiple menus: Main, Sound, Upgrades, GameOver
- Cross-platform support (Windows and Linux)

## Dependencies

### Windows

- No additional dependencies required. All necessary DLLs are included in the release folder.

### Linux

- SDL2: `sudo apt install libsdl2-dev` (Ubuntu/Debian) or `pacman -S sdl2` (Arch)
- SDL2_mixer: `sudo apt install libsdl2-mixer-dev` or `pacman -S sdl2_mixer`

## How to Run

### Windows

1. Download the `VoidVanguard-Windows` folder.
2. Run `VoidVanguard.exe`.
3. Enjoy the game!

### Linux

1. Download the `VoidVanguard-Linux` folder.
2. Ensure SDL2 and SDL2_mixer are installed (see Dependencies).
3. Run `./VoidVanguard` in the terminal.
4. Enjoy the game!

## Controls

- WASD: Move
- Left Mouse Click: Shoot
- Enter: Select menu options
- Escape: Pause/Exit

## How It Works

- Start in the main menu.
- Select options to begin the game.
- Control your cube to survive waves of enemy cubes, gain coins for killing enemie, and upgrade your cube in the upgrade menu.
- Adjust sound settings in the sound menu.
- Game over opens menu with restart options.

## Building from Source

If you want to build from source:

- Install dependencies.
- Run `make` for Linux or `make windows` for Windows cross-compilation.
- Requires mingw-w64 for Windows builds.

## Credits

- Built with SDL2
- Sounds and assets included

## Support

If you enjoy the game, consider buying me a coffee: [Buy Me a Coffee](https://buymeacoffee.com/matomas225)

