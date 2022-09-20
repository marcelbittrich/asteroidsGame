# asteroidsGame
An "Asteroids" like game written in C++ with SDL2

## Description

This is a small 2D game inspired by the old arcade classic "Astroids". It is currently developed by two persons, a friend of mine and me. Our goal is to make a fun game and learn game developent and C++ along the way. These are our first steps in this direction, so if you have any tips, advices or questions feel free to contact me!

### Structure

We use SDL2 to initialise the game, setup a window and than render it black. We check for connected input devices (i.e. a Xbox Controller) and than enter the gameloop. It adds all our gameobjects, the Ship and the asteroids, to the screen, handles all iputs and updates the position of the gameobjects.

### Code layout 

We try to seperate the code in a sensible manner. Our goal is to keep it as easy to read as possible.

At this moment we use the following file structure:

    .
    ├── SDL2                    # All SDL files
    ├── build                   # Compiled files with necessary dll files
    ├── img                     # Files to creat textures for the gameobjects
    ├── src                     # Source files
    │   ├── main.cpp            # 
    │   ├── game.cpp            # 
    │   ├── handleinput.cpp     # 
    │   ├── gameobjects.cpp     # 
    │   └── shapes.cpp          # 
    ├── .gitignore
    └── README.md


## Objectives

These are our future objective in no paticular order:

- [ ] Make a fun game!
- [ ] Add bigger asteroids
- [ ] Add shots to split or destoy astroids
- [ ] Make them all move and spwan at random positions
- [ ] Add collectable items
- [ ] Seperate calculation from framerate
- [ ] Add score and menu 

## Acomplished goals

Already implemented are the following:

- [x] Controllable ship with animations
- [x] Keyboard and gamepad input handeling
- [x] Rendering of small asteroids
- [x] Simple circle-shaped hit detection and debugging option



