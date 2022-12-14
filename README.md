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
    ├── SDL2                    # all SDL files
    ├── build                   # compiled files with necessary dll files
    ├── img                     # files to creat textures for the gameobjects
    ├── src                     # source files
    │   ├── main                # main function with gameloop
    │   ├── game                # init, handleEvents, update, render and clear functions
    │   ├── handleinput         # gamepad, mouse and keyboard input handling
    │   ├── gameobjects         # all gameobjects with collision
    │   ├── initialization      # functions to initialize gameobjects   
    │   ├── background          # matrix of points to visualize movement
    │   ├── shapes              # simple shapes for debugging
    │   └── UIelements          # all UI elements 
    ├── .gitignore
    └── README.md


## Objectives

These are our future objective in no particular order:

- [ ] Make asteroids move and spawn at random positions
- [ ] Add collectable items
- [ ] Add score and menu 
- [ ] Finish with a fun game!

## Accomplished goals

Already implemented are the following:

- [x] Controllable ship with animations
- [x] Keyboard and gamepad input handling
- [x] Rendering of asteroids
- [x] circle-shaped collision detection and debugging option
- [x] Add bigger asteroids
- [x] Add shots to split or destroy asteroids
- [x] Separate calculation from framerate

## How to try it

<details>
  <summary>Click here</summary>
  
  You can either compile and run the game with the instruction given below or you can simply start the executale located under ./build.
    
  ### Setup
  
  We are using VSC and GCC with MinGW to compile our game. You can learn how to get it [here](https://code.visualstudio.com/docs/cpp/config-mingw). After you installed it you have to create a `.vscode` folder in your project to change some settings (see next sections). This is most certainly not the only way how to run our code, but this is how we do it. If you want further guidance look [here](https://dev.to/giovannicodes/setup-sdl2-with-visual-studio-code-and-mingw64-on-windows-14c5).
  
  ### VSC Settings
  
  To run our project please create the three following `.json` files in your `.vscode` folder. Make sure that your MinGW installation directory matches the one mentioned below (C:/msys64/mingw64/bin/*) or change it to your personal one.
  
  `c_pp_properties.json`
  
  ```json
    {
        "configurations": [
            {
                "name": "Win32",
                "includePath": [
                    "${workspaceFolder}/**/*",
                    "${workspaceFolder}\\SDL2\\include"
                ],
                "defines": [
                    "_DEBUG",
                    "UNICODE",
                    "_UNICODE"
                ],
                "windowsSdkVersion": "10.0.19041.0",
                "compilerPath": "C:/msys64/mingw64/bin/g++.exe",
                "cStandard": "c17",
                "cppStandard": "c++17",
                "intelliSenseMode": "gcc-x64"
            }
        ],
        "version": 4
    }
  ```
  
  `launch.json`
  
  ```json
    {
        "version": "0.2.0",
        "configurations": [
            {
                "name": "(gdb)",
                "type": "cppdbg",
                "request": "launch",
                "program": "${workspaceFolder}\\build\\game.exe",
                "args": [],
                "stopAtEntry": false,
                "cwd": "${workspaceFolder}\\build",
                "environment": [],
                "externalConsole": false,
                "MIMode": "gdb",
                "miDebuggerPath": "C:\\msys64\\mingw64\\bin\\gdb.exe",
                "setupCommands": [
                    {
                        "description": "Enably pretty printing",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ],
                "preLaunchTask": "SDL2"
            }
        ]
    }
  ```
  
  `tasks.json`
  
  ```json
    {
        "version": "2.0.0",
        "tasks": [
            {
                "type": "shell",
                "label": "SDL2",
                "command": "C:\\msys64\\mingw64\\bin\\g++.exe",
                "args": [
                    "-Wall",
                    "-g",
                    "src\\*.cpp",
                    "-o",
                    "build\\game.exe",
                    "-I${workspaceFolder}/SDL2/include",
                    "-L${workspaceFolder}/SDL2/lib",
                    "-lmingw32",
                    "-lSDL2main",
                    "-lSDL2",
                    "-lSDL2_image",
                    "-mwindows",
                    "-mconsole"
                ],
                "options": {
                    "cwd": "${workspaceFolder}"
                },
                "problemMatcher": [
                    "$gcc"
                ],
                "group": "build"
            }
        ]
    }
  ```
  
  
</details>


