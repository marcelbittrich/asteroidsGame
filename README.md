# asteroidsGame
:joystick::rocket: An "Asteroids" like game written in C++ with SDL2

## Description

This is a small 2D game inspired by the old arcade classic "Astroids". It is currently developed by two persons, a friend of mine and me. Our goal is to make a fun game and learn game development and C++ along the way. These are our first steps in this direction, so if you have any tips, advice or questions feel free to contact me!

### Structure

We use SDL2 to initialize the game, setup a window and then render it black. We check for connected input devices (i.e. a Xbox Controller) and then enter the gameloop. It adds all our gameobjects, the Ship and the asteroids, to the screen, handles all inputs and updates the position of the gameobjects.

### Code layout 

We try to separate the code in a sensible manner. Our goal is to keep it as easy to read as possible.

At this moment we use the following file structure:

    .
    ├── SDL2                    # All SDL files
    ├── build                   # Compiled files with necessary dll files
    ├── img                     # Files to creat textures for the gameobjects
    ├── src                     # Source files
    │   ├── main.cpp                # Main function with gameloop
    │   ├── game.cpp                # Init, handleEvents, update, render and clear functions
    │   ├── handleinput.cpp         # Gamepad and keyboard input handling
    │   ├── gameobjects.cpp         # All gameobjects with collision
    │   └── shapes.cpp              # Simple shapes for debugging
    ├── .gitignore
    └── README.md


## Objectives

These are our future objective in no particular order:

- [ ] Make a fun game!
- [ ] Add bigger asteroids
- [ ] Add shots to split or destroy asteroids
- [ ] Make them all move and spawn at random positions
- [ ] Add collectable items
- [ ] Separate calculation from framerate
- [ ] Add score and menu 

## Accomplished goals

Already implemented are the following:

- [x] Controllable ship with animations
- [x] Keyboard and gamepad input handling
- [x] Rendering of small asteroids
- [x] Simple circle-shaped hit detection and debugging option

## Changelog

2022-09-19 - added circle shapes to visualize the collision radius of the gameobjects


