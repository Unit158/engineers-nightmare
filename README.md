# Engineer's Nightmare [![Build Status](https://travis-ci.org/engineers-nightmare/engineers-nightmare.svg)](https://travis-ci.org/engineers-nightmare/engineers-nightmare)

Engineer's Nightmare (working title) is a multiplayer cooperative story-driven game where a team of players fight to keep a spaceship in working order while a DM tries to tear it apart.

Starting from scratch, or a partially pre-fabricated base, players will build a spaceship while fending off events generated by the DM. Such events might include warp core overload, or a hostile boarding party.

Centered on an engineer's point of view, Engineer's Nightmare provides a unique gameplay experience. Run miles of wiring, isolate critical systems to prevent cascading failures, expose yourself to hard vacuum. Join the Engineer's Corps today.


# Current state

So far, have implemented solid player movement, including jumping and crouching, crawling inside blocks, etc; tools for
placing and removing block framing, surfaces of several types, block-mounted and surface-mounted entities of several
types; Lightfield propagated from certain entities.

![example](https://raw.githubusercontent.com/engineers-nightmare/engineers-nightmare/master/misc/en-2015-05-18-1.png)
![example](https://raw.githubusercontent.com/engineers-nightmare/engineers-nightmare/master/misc/en-2015-08-04.png)


# Building and running

## Obtaining source from GitHub

    git clone --recursive https://github.com/engineers-nightmare/engineers-nightmare.git

or

    git clone https://github.com/engineers-nightmare/engineers-nightmare.git
    cd engineers-nightmare
    git submodule update --init

## Dependencies

 * assimp
 * bullet
 * freetype6
 * libconfig
 * libepoxy
 * mman-win32 (For Windows build)
 * SDL2
 * SDL2_image

NB: this above list must be kept in sync with `.travis.yml`

## Building on Linux

build:

    cmake .
    make

test:

    make test

run:

    ./nightmare

## Building on Windows

Visual Studio 2015 is the only officially supported Windows build.

 * Clone or download this repo.
 * Copy `windows-dependencies` folder from [windows-dependencies](https://github.com/engineers-nightmare/windows-dependencies) into the cloned folder.
 * Open nightmare.sln
