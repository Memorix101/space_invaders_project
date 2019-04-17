<img src="https://user-images.githubusercontent.com/1466920/54886277-a9087a00-4e86-11e9-84b6-07ff961d3888.png" width="1000"></p>
### Porting a space invaders game to different platforms, frameworks and game engines

The journey began in summer of 2016 when I wanted to make a game for the Sega Dreamcast.
My first and later attempts failed, but I picked it up sometime later in autumn.
This time I wanted to bring this little game to Dreamcast (instead of the other full game idea).
First I a made a version in SDL 1.2 (because there was a port of for Dreamcast) on PC to easily see what is going on and later recompiled it using the Dreamcast homebrew toolchain. Well, I had to learn
my lesson about GCC C compilers and how bitchy they can be and also that the SDL port for Dreamcast is broken and no longer maintained. But hope was not lost! There was an OpenGL port for Dreamcast too. So, I had to learn how to use OpenGL.
I read a few tutorials and started to rework the SDL stuff into OpenGL using GLFW3
<br/>
<br/>
![](https://i.imgur.com/CVn5vGA.jpg)
###### OpenGL example I made to visualize what is going on here (My drawing code is flipped btw 😂😬)
(but I was too lazy to port the font over but managed to do this for all other platforms who had no SDL_ttf support using bitmap fonts).
<br/>

<img src="https://i.imgur.com/M5GHwK0.png" width="500"></p>
###### Dreamcast OpenGL version

<img src="https://i.imgur.com/hE111dv.png" width="500"></p>
###### PSP SDL version
Then at some point this became some race about *"for how many different platforms can I port this thing?"*.

So, I set up some kind of requirements for the platforms I want to port this.
<br/>

### Homebrew platform port requirements
- No hardware modding requirements like mod chips or soldering ...
- In case of the need of CFW (Custom Firmware) make sure it will not result in a ban (PSN, Xbox Live Nintendo Network) or other negative consequences for the user (game save loss, high possibility of bricking).
- Easy to use (and safe) modding setup (if needed)
- I need to like the platform 🙃

<br/>

Also there are ports for my own [platforms](https://memorix101.wordpress.com/videogame-console-project/).<br/>
[G-eon a virtual video game console](https://memorix101.itch.io/g-eon)
I started working on in 2016 and released the first stable version in April 2017
<br/>
<img src="https://img.itch.zone/aW1hZ2UvMTA5MTg0LzUwNzQ0Ni5wbmc=/347x500/Zx6ev1.png" width="300">
<img src="https://img.itch.zone/aW1hZ2UvMTA5MTg0LzUwOTg1Mi5wbmc=/347x500/e2Xtui.png" width="300">
<br/>
and Xenon a video game console I was working on since my early childhood.
Reworked it as a Raspberry Pi Linux based operating system in 2018. It's unreleased (and will possibly never be).<br/><br/>
<img src="https://memorix101.files.wordpress.com/2016/07/ad_2.png?w=1400&h=" width="300">
<br/>

###### 2016 artwork
[Xenon Prototype 2018 #1 (German)](https://youtu.be/GlDQQUI2lLc?t=60)<br/>

### The bad port saga
For sure there are bad ports and rushed ones (which are actually bad).
Some ports are bad because I had no idea how to use the platform correctly or was too restricted by the toolchain I was/had to use. Some toolchains are well documented but most are not.
<br/>

The **Gameboy Advanced** port was really adventurous. I know the GBA was using a colour palette for everything on screen but I thought it would be system internally. Well, things are different as I had to learn. I had to bring it with my game.

<img src="https://i.imgur.com/6wPHgpz.png" width="500"><br/>
###### First attempt
<img src="https://i.imgur.com/eDBu751.png" width="500"><br/>
###### Second attempt
<img src="https://i.imgur.com/1x3xmWk.png" width="500"><br/>
###### I had the idea to redraw the sprites myself (really really bad move) 😂

I was really struggling to get it working correctly. But with the magic of
some homebrew palette tools and the power of Photoshop I could fix it in the end. The sound chip is another story 😬 4 sound channels. Music (playing on 3 channels as I remember but not 100% sure) and 3 different sounds which need to play at the same time. Sorry for that ... .
<br/>

<img src="https://i.imgur.com/FgFhb9B.png" width="500"><br/>
###### 🙃🙃🙃

The port for **MS-DOS** was way more chill, but finding a working toolchain, libraries and documentation was really hard. I got the idea after watching a [GDC 2016 talk by the creator of *Retro City Rampage* Brian Provinciano](https://youtu.be/kSKeWH4TY9Y). I really loved that I was able to directly access the hardware and even the BIOS! 😮

<img src="https://i.imgur.com/M8MGojw.png" width="500"><br/>
###### First run in MS-DOS in wrong RGB mode
<img src="https://i.imgur.com/1w5BFR1.png" width="500"><br/>
###### Sometime later run in MS-DOS in correct RGB mode. Yeay! 🙃
<img src="https://i.imgur.com/qO4YZVz.jpg" width="500"><br/>
###### "How can I use transparency?!"
<img src="https://i.imgur.com/CXuaJHC.png" width="500"><br/>
###### Final game
<br/>

The **Sega Saturn** toolchain for example was kind of complicated as the
compiler was really bitchy and sometimes was not in the mood to do his job.
*"What file are you talking about? I can't see it", "Well, I already compiled it when you are too stupid to spot the binary not my problem","Not in the mood ask me tomorrow"*. For some reason I was not able to get music playback to work, what I'm really sorry about. ☹
<br/>
<br/>
The **Wii** version has only the issue with sound playback as the audio library has no much examples how to use the DSP (Audio hardware) correctly.
Also I was too lazy to implement a solid audio channel system. So, when a new sound is played it interrupts the playing one in the current situation.
Music playback is not possible this way as it will be interrupted when the first laser is shot.
*I blame the people here who are responsible that SDL (and SDL_mixer) is not working for Wii with the current devkitPro setup. Shame on you.*
**SHAAAAAME!!!** 😂😂😂
<br/>
And ignore my laziness and incompetence to write a proper solution myself with the given tools 🙃

The **F#** version was almost a straight C# port as I'm using the same library (MonoGame ❤). Sure, I was ignoring the functional aspects of the language most of the time (which was a reason to port it, actually 😂) but hands down it is really tough to learn and to understand this way of programming. I really love (and sometimes hate) this Python like indent and dedent writing thing of F#. Had the same fun with GDScript in Godot.
But some things are way different in F# and I never would have thought I would miss the the `break` statement in a programming language 😂

<img src="https://i.imgur.com/wxdx78W.gif" width="500"><br/>

The orignal is written in C as the Dreamcast toolchain was not that great in C++ support. So I wanted to rewrite it into **C++**. I wanted to be at least on C++11 level. So, I made OOP like port. Lists `std::list<T>`, classes and ranged loops `for(Type& v : a) { ... }`. Well, didn't work as expected in the first place 😂

<img src="https://i.imgur.com/sHbj5Bw.gif" width="500"><br/>
###### Flickering sprites because of C style for loop (Int i = 0; i < T.size(); i++)
<img src="https://i.imgur.com/wX8Tjdw.gif" width="500"><br/>
###### Ranged for loop glitch. Had to add s goto statement (what makes me feel so dirty 😞)
<img src="https://i.imgur.com/4QfwREA.gif" width="500"><br/>
###### Debugging the ranged for loop glitch. Fucked it up even more 😂😂😂
<br/>

And then there is **Unreal Engine 4** and it's annoying texture compression. That thing is powerful but bloated and kind of complex at first.
But mostly you will find a fix or workaround somewhere in the wiki or their AnswerHub.

<img src="https://i.imgur.com/hJgsVmM.gif" width="500"><br/>
###### Eeeeeh. Stooop it pleeaaaase 😢
<br/>

### Final words
However, this way you learn pretty much (especially how to write performant and good (C) code 😂) and by all the stress and frustration it makes a ton of fun. Who could imagine this little game would be so enriching in the process of developing. In German we have the phrase "Der Weg ist das Ziel" what would be translated to "The way is the goal". I hope you understand the idea behind the meaning. Sometimes the way to the destination is more enchriching than the place you want to reach. Musicians and artist should know what I'm speaking about. The experience of creation can be something so emotionally powerful and wonderful.
You may be disappointed with your end results but the way to it was worth it.
<br/>

We will see what the future holds. I still have many ideas for platforms and languages I want to port this. 🙂