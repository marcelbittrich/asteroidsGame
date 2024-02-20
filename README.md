# asteroidsGame

![Asteroids](https://github.com/marcelbittrich/asteroidsGame/assets/113523293/b29f295e-d5d9-49a0-9755-3568b1908ec6)

:joystick::rocket: An "Asteroids" like game written in C++ with SDL2

## Description

This is a 2D arcade game inspired by the old arcade classic "Astroids". This project intends to create a fun game and learn the fundamentals of game development and C++ along the way. These are my first steps in this direction and the application will be improved continuously. If you have any tips, advice or questions feel free to contact me!

### Structure

The SDL2 library is used to setup a window and provide rendering functions. Input devices like Mouse, Keyboard and Controllers are supported. 
The game runs on a typical gameloop with a FSM, the "gameState", which determineds the current HandleEvents, Update and Render functions.

### Code layout 

The code is separate in a sensible manner. The goal is to keep it as easy to read as possible.
As it is a VS project, a fairly typical file structure is used:

    .
    ├── Asteroids               # project directory
    │   ├── font                # font files  
    │   ├── img                 # images for textures
    │   ├── sfx                 # sound files
    │   └── src                 # all source code - probably the place you wanna go
    ├── dependencies            # SDL2 library
    └── Asteroids.sln

## Accomplished goals

Already implemented are the following:

- [x] Controllable ship with animations
- [x] Keyboard and gamepad input handling
- [x] circle-shaped collision detection
- [x] shots split or destroy asteroids
- [x] asteroids move and spawn at random positions
- [x] collectable items
- [x] score and menu
- [x] sounds  

## How to try it

This game is developed with Visual Studio 2022 and you can compile it yourself. After you cloned the repository and opened the solution file there should be no additional setup necessary.
