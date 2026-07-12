<h1 align="center">CHIPEmu</h1>
<img src="../Images/Breakout.png" width="1366" height="721">
<br>

### CHIPEmu is simply a still WIP CHIP-8 Emulator I have written in C. Simply benefits from OpenGL 3.3(from GLAD), GLFW3 to display the video buffer while also benefitting from miniaudio to create sound effects. Anyone can change the code and compile their version of it without any problems. The emulator still has some minor bugs as I am not experienced in emulating. I will be updating this repository depending on the need.

<h2 align="center">How To Use</h2>

### Launch the CHIPEmu file, press ESC on your keyboard and you will see a menu on your left. This menu has a button for loading ROMs and a slider for Cycle Delay. You can interact with the GUI to use the ROMs and tune the game's speed with the Cycle Delay any way you want.

<h2 align="center">How To Compile</h2>

### A normal user doesn't need that step to be done but if you want to seriously compile this thing yourself you need to use CMake and GCC/G++ on your device.


### Windows users, you need `CMake` and `MinGW` for the compilation process! First in the CHIPEmu project folder, create a new directory named build. Open your command prompt in the build directory and simply write `CMake -G "MinGW Makefiles" ..` If it succeeds without any errors follow with this command: `CMake --build .` Your CHIPEmu.exe file should be inside the build directory. You can now run the application.

### Linux users, you need `CMake`, `GCC`, `OpenGL Tools` and `GLFW Developer Tools` installed in your terminal! I will not explain how to install them because they are pretty much explained anywhere, we will straight just compile the code. Open your terminal and go to the CHIPEmu project folder, create a build folder with `mkdir build`, then write `cmake ..` If it succeeds without any errors proceed with: `cmake --build .` Your CHIPEmu file should be inside the build directory. You can type `chmod +x CHIPEmu` in your terminal to grant the application executable perms then write `./CHIPEmu` to start the application.

<h2 align="center">Eklenecekler</h2>

### Alot of stuff is actually implemented but I am kinda annoyed by the theme so I might add a new theme that you can change in the left-hand menu. Will add other stuff depending on the need.