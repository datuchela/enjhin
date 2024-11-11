# Give Up GitHub

This project has given up GitHub.  ([See Software Freedom Conservancy's *Give Up  GitHub* site for details](https://GiveUpGitHub.org).)

You can now find this project at [https://codeberg.org/datuch/enjhin](https://codeberg.org/datuch/enjhin) instead.

Any use of this project's code by GitHub Copilot, past or present, is done without our permission.  We do not consent to GitHub's use of this project's code in Copilot.

Join us; you can [give up GitHub](https://GiveUpGitHub.org) too!

![Logo of the GiveUpGitHub campaign](https://sfconservancy.org/static/img/GiveUpGitHub.png)

# Enjhin
Soft body physics engine in C using [raylib](https://github.com/raysan5/raylib)  

![A GIF showcasing Enjhin](./docs/enjhin-animation.gif)

## Building from source
### On Linux
First, make sure you have `gcc` and `make` installed on your system.  
Then, you can run `make build` or just `make` to build the executable for Linux.  

#### Cross-compile (Linux to Windows)
If you would like to build a Windows executable from Linux, you will need to have the `x86_64-w64-mingw32-cc` cross-compiler installed.  
Once you have that, run `make clean` to remove any old `.o` files, and then you can run `make OS_FROM=linux OS_FOR=windows`.  

### On Windows
Requirements are the same for Windows, you will need to have `gcc` and `make` installed,  
and then you can run `make OS_FOR=windows` to build Windows executable.


## Changelog
- Added Gravity
- Added world boundary collisions
- Added an ability to tweak the engine without pausing
- Added an ability to reset the engine
- Fixed engine mouse events still firing when tweaking settings in GUI

## Authors
- [merabi0013](https://github.com/merabi0013)
- [datuchela](https://github.com/datuchela)

