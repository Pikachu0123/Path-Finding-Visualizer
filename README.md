# Setting Up Raylib

To setup raylib,

run the following commands on Linux:
```
sudo apt-get update
sudo apt-get install build-essential git cmake libglfw3-dev libglew-dev libfreetype6-dev
sudo apt-get install libx11-dev libxrandr-dev libxi-dev libxinerama-dev libxcursor-dev libglfw3-dev

git clone https://github.com/raysan5/raylib.git
cd raylib

mkdir build
cd build
cmake ..
make
sudo make install

mkdir ./my_raylib_project
cd ./my_raylib_project
touch main.cpp
```

main.cpp is our main code

# Running The Code

```
make
```

To run this code, use the Makefile (name is case sensitive) file. For running the Makefile, run the command "make" (it looks for a file named Makefile by default).
