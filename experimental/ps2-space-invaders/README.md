# ps2-space-invaders

Space Invaders written in C++ for PS2 using [ps2dev](https://github.com/ps2dev/ps2dev)

##### Note: This is experimental and broken !!!
- audio is not working (program stalls on SDL_INIT_AUDIO)
- minimal performance issue
- ~~enemy shooting is not working (not sure if rand or deltatime issue) edit: it was both~~ 

![](https://user-images.githubusercontent.com/1466920/106803964-11b90400-6665-11eb-9f41-9aba810eb7c2.png)

## Setting up the PS2DEV Compiling Environment

#### Step 1
`cd $HOME && gedit ~/.bashrc`

`export PS2DEV=/usr/local/ps2dev`
`export PS2SDK=$PS2DEV/ps2sdk`
`export GSKIT=$PS2DEV/gsKit`
`export PATH=$PATH:$PS2DEV/bin:$PS2DEV/ee/bin:$PS2DEV/iop/bin:$PS2DEV/dvp/bin:$PS2SDK/bin`

### Step 2

`sudo mkdir /usr/local/ps2dev`
`sudo chmod 777 /usr/local/ps2dev`

### Step 3

`sudo apt-get update`
`sudo apt -y install build-essential gcc clang clang-format make cmake patch git texinfo flex bison gettext wget libgsl-dev`
`sudo apt-get update`
`sudo apt-get -y install texinfo bison flex gettext libgmp3-dev libmpfr-dev libmpc-dev zlib1g-dev`

### Step 4

`cd $HOME && git clone https://github.com/ps2dev/ps2dev.git ps2dev && cd ps2dev && ./build-all.sh`

### Bonus 

`cd $HOME && git clone https://github.com/ps2dev/ps2eth.git ps2eth && cd ps2eth && make clean all install && cd $HOME`

[source](https://www.ps2-home.com/forum/viewtopic.php?t=9488)