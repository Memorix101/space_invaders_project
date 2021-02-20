# Platform ideas and status

- PlayStation 2
  - ps2sdk has no (useful) documentation. ~~There is a booting build of the game but it's not working and I actually don't know why. Just black screen. IOP modules load correctly but something is wrong here~~ 😞<br/>
  - Seems like ps2sdk is partly broken here and there. 🤔😮
  - Done on 02/02/2021 &#x2611;
    <br/>
    <br/>
- PlayStation
  - Not sure. No plans.
    <br/>
    <br/>
- N64
  - Not sure. No plans. Last time I checked their homebrew stuff it was pretty unappealing to me.
    <br/>
    <br/>
- SNES
  - No plans. Also it seems there is no C compiler. Only ASM. I prefer Sega ❤ btw.
    <br/>
    <br/>
- Sega Mega Drive / Genesis
  - There are no tools to convert the colour palette of the sprites and the documentation is outdated. Tricky situation. 😬 There is a unfinished build.
    <br/>
    <br/>
- [Emscripten](https://emscripten.org/index.html)
  - ~~Well, it's a mess. The build crashes right away. I tried so much, but can't get it working. (SDL2) 😥😥😥~~<br/>
  - Done on 02/02/2021 using raylib &#x2611;
    <br/>
    <br/>
- [Rust](https://www.rust-lang.org/)
  - This will take sometime. I want it to become nice but there is a unfinished build.
    <br/>
    <br/>
- Python
  - ~~Maybe, but there is an unfinished build in Python 3.~~<br/>
  - Done on 03/01/2020 &#x2611;
    <br/>
    <br/>
- PS3
  - Not sure. I looked a bit into it and development seems not very appealing.
  - Waiting until it's safe to install CFW without the worry about a ban. Homebrew on RPCS3 is not good yet (It's slow and unstable).
    <br/>
    <br/>
- Original Xbox
  - I don't own one (yet). Is CXBX-Reloaded capable to run homebrew flawlessly?! Homebrew games were much too slow last time I tested it.
  - Tricky situation. Developing with the XDK needs quite some work to setup (A virtual machine with Windows XP is recommended). The [nxdk](https://github.com/XboxDev/nxdk) does not work with CXBX-Reloaded. 🙁
    <br/>
    <br/>
- Xbox 360
  - Probably never because I don't want to do it (JTAG/Jailbreak) on my own Xbox 360. Maybe if I can get a JTAG one on eBay. (XNA is dead on Xbox 360 😢😥😥). Can Xenia run homebrew well?! Never tested it. Also the homebrew situation is kind of meh over there and there is the XNA version ...
    <br/>
    <br/>
- Java
  - Besides the libGDX version there is also an unfinished LWJGL3 version. I'm not sure if I should finish it. Because it would be similar to the GLFW3 version in C but in Java classes.
    <br/>
    <br/>
- PS Vita
  - I don't own one and not sure if Vita3k is "good" enough.
  - Done on 19/02/2021 &#x2611;
    <br/>
    <br/>
- HTML5
  - ~~Sure, when I'm in the mood.~~<br/>
  - Done using Phaser 3 on 22/04/2019 &#x2611;
    <br/>
    <br/>
- GameCube
  - Same situation as Wii. Audio would be a mess (Both use the same [SDK](https://devkitpro.org/wiki/Getting_Started)). Also I don't know if SDL is there and actually I don't want to rework it for [GX](https://devkitpro.org/wiki/libogc/GX).
  - The issue with the audio is that SDL is throwing errors while linking which I can't resolve. The other is that I am too lazy to write something on my own using the SDK tools (which are quite good so far).
    <br/>
    <br/>
- MS-DOS
  - I'm planning to rework the DOS version at some point in the future as it is unstable/broken.
    <br/>
    <br/>
- 3DS
  - ~~I am considering it but I am not sure how I can use the second screen usefully.~~
  - Done on 09/02/2021 &#x2611;