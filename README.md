## Beats of Rage Rumble Edition for Sega Dreamcast

**Beats of Rage** is an open-source 2D customisable video game engine designed by [Senile Team](https://www.senileteam.com/beatsofrage.html) and inspired by Sega's classic [Streets of Rage](https://en.wikipedia.org/wiki/Streets_of_Rage) series of beat'em up games. It was initially released as a video game tribute for [Streets of Rage 2](https://en.wikipedia.org/wiki/Streets_of_Rage_2) with characters from [The King of Fighters](https://en.wikipedia.org/wiki/The_King_of_Fighters).

## Introduction

This repository contains the original **Beats of Rage** source code for the **Sega Dreamcast** console, in its final form. The Rumble Edition, specific for the Sega Dreamcast, was released the **October 21, 2004**.

It's archived here for historical purpose, as [OpenBoR](https://github.com/sega-dreamcast/openbor/) is the newer alternative to run **Beats of Rage** games on the Sega Dreamcast, so you should use [OpenBoR](https://github.com/sega-dreamcast/openbor/) instead of this engine.

## Usage

To build the Beats of Rage game disc, you won't need any additional tool, this repository provides a very outdated GCC toolchain (3.0.4) but it will be sufficient. Please note, the Bootfile (1ST_READ.BIN) is compiled through the src/boot directory and will need a modern KallistiOS environment, like [DreamSDK](https://www.dreamsdk.org/) or any modern KallistiOS 2.0.0+ environment; but there is no need at all to update this program, that's the reason you'll find it already in the repository in the cd_root/ directory.



## Credits

* Initial DOS source code and assets by [Senile Team](https://www.senileteam.com/beatsofrage.html).
* Initial Sega Dreamcast port by [Neill Corlett](http://www.neillcorlett.com/), based on the **v1.0028** source code.
* Rumble Edition and PAL support by [Eric Hasdenteufel](http://la.cible.free.fr/index.htm#BOR) aka [L@ Cible/Ricku34](https://github.com/Ricku34).
* Beats of Rage tools from [OpenBoR Team](http://www.chronocrash.com/).
* Bootfile based on the PAL Menu sample written by [Lawrence Sebald](http://dcemulation.org/) aka [BlueCrab/ljsebald](https://github.com/ljsebald).
* Scripts, documentation and various stuff by [SiZiOUS](http://www.sizious.com/).
