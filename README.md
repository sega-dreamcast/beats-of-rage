## Beats of Rage Rumble Edition for Sega Dreamcast

**Beats of Rage** is an open-source 2D customisable video game engine designed by [Senile Team](https://www.senileteam.com/beatsofrage.html) and inspired by Sega's classic [Streets of Rage](https://en.wikipedia.org/wiki/Streets_of_Rage) series of beat'em up games. It was initially released as a video game tribute for [Streets of Rage 2](https://en.wikipedia.org/wiki/Streets_of_Rage_2) with characters from [The King of Fighters](https://en.wikipedia.org/wiki/The_King_of_Fighters).

## Introduction

This repository contains the original **Beats of Rage** source code for the **Sega Dreamcast** console, in its final form. The **Rumble Edition**, specific to the Sega Dreamcast, was released the **October 21, 2004**.

This special **Rumble Edition** also includes a broadcast selector, between **NTSC** or **PAL**. By default, the NTSC version (the best) is used, but if needed (mainly if you have an European configured Dreamcast without a VGA cable attached), the startup loader will let you choose to run the game on PAL or NTSC broadcast signal.

Please note, this repository exists only for historical purpose, as [OpenBoR](https://github.com/sega-dreamcast/openbor/) is the newer and better alternative to run **Beats of Rage** games on the Sega Dreamcast, so you should use [OpenBoR](https://github.com/sega-dreamcast/openbor/) instead of this engine.

## Compiling

To build the **Beats of Rage** game disc, you won't need any additional tool, this repository provides a very outdated [GCC](https://gcc.gnu.org/) compiler (`3.0.4`) but it will be sufficient for this case.

The loader (`1ST_READ.BIN`) is compiled through the `src/boot` directory and will need a modern [KallistiOS](http://gamedev.allusion.net/softprj/kos/) environment, like [DreamSDK](https://www.dreamsdk.org/) or any modern [KallistiOS](http://gamedev.allusion.net/softprj/kos/) 2.0.0+ environment; but there is no need at all to update this program, for that reason you'll find it already compiled in the repository in the `cd_root/` directory.

To create the **Beats of Rage** disc, you just have to:

1. Double-click on the `build.bat` script.
2. If everything is OK, just double-click on the `makedisc.bat` script.
3. You'll have a `bor.cdi` generated file: your **Beats of Rage** game disc is now ready!

Then you can clean every intermediate file by double-clicking on the `clean.bat` script.

If for any reason you need to regenerate the loader, go to the `src/loader` directory through a [KallistiOS](http://gamedev.allusion.net/softprj/kos/) enabled environment (e.g. [DreamSDK](https://www.dreamsdk.org/)) then:

1. Open your shell.
2. Enter: `make && make dist`.
3. Enter: `./install.sh`

The message `1ST_READ.BIN processed successfully` should be displayed: your loader has been updated in the `cd_root` directory. Enter now `make clean` to do some cleanup.

## Usage

Once the `bor.cdi` file is generated, you just have to burn this file with **Padus DiscJuggler**, **Alcohol 120%** or any burning software which supports **DiscJuggler** images.

This **Beats of Rage** engine is emulated through [Demul](http://demul.emulation64.com/) at least but feel free to test any other emulator if you want to.

## Credits

* Initial DOS source code and assets by [Senile Team](https://www.senileteam.com/beatsofrage.html).
* Initial Sega Dreamcast port by [Neill Corlett](http://www.neillcorlett.com/), based on the **v1.0028** source code.
* Rumble Edition and PAL support by [Eric Hasdenteufel](http://la.cible.free.fr/index.htm#BOR) aka [L@ Cible/Ricku34](https://github.com/Ricku34).
* Beats of Rage tools from [OpenBoR Team](http://www.chronocrash.com/).
* Loader based on the PAL Menu sample written by [Lawrence Sebald](http://dcemulation.org/) aka [BlueCrab/ljsebald](https://github.com/ljsebald).
* Scripts, documentation and various stuff by [SiZiOUS](http://www.sizious.com/).

