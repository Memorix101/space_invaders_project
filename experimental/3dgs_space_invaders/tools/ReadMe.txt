////// A4 Font Generator ///////
Copyright 2000, Neonlite Software

Version 1.1 Release English Version
08/01/2000


Contents
--------
1. Legal
2. KNOWN BUGS
3. FAQ
4. INSTRUCTIONS
5. History


--------------------------------------------------------------------------
LEGAL:
--------------------------------------------------------------------------
You may use this product and any data and/or files output by this product 
freely and without cost now or in the future. This includes commercial use.
You may distribute the program and associated files any way you like as 
long as this readme file is distributed with it intact. Users of this product
must understand that the author is in no way responsible for any problems
that might occur from the use of this program. I can't see how any harm
could possibly come from using it, but I need to protect myself from the
Trolls that sometimes hang out on the A4 forum ;)

--------------------------------------------------------------------------
KNOWN BUGS:
--------------------------------------------------------------------------
- None are Known!


--------------------------------------------------------------------------
FAQ:
--------------------------------------------------------------------------

Q: My characters seem too spaced out
A: That is because A4 only handles Fixed Width Fonts 
   (All characters take up an equal amount of space)
   and you have chosen a variable width font. I center
   variable width fonts inside the space they should 
   take up. You can adjust the spacing between characters
   by using the "Space Between Characters" spinner.

Q: My palettes are the wrong colors
A: You must run this with your windows settings in 16-bit
   or higher unless your palette happens to be the exact 
   system palette :)

--------------------------------------------------------------------------
INSTRUCTIONS:
--------------------------------------------------------------------------

Very simple to use:

1. Press the (*)Choose... button in the Font group and 
   select a Font, Font Style and Size from the Font 
   Chooser Dialog.
   
2. In the Properties group, Press the Binocular button 
   next to the (*)Palette: field and select your palette 
   file. It may be .raw, .lmp or .pcx formats.
   
3. Choose your Foreground color by pressing the Paintbrush 
   button next to it, and select the color from the Color 
   Picker.

4. You can repeat for the background color, but the 
   transparent color is chosen by default and should
   be Ok.

5. If you want a shadow, check the Shadow? box and choose 
   the color as you did in step 3. If outline works, repeat
   for the Outline.
   
6. You can change the spacing between all the characters in
   your font by adjusting the "Space Between Characters" spinner.
   
7. In the Generation Group, choose a file to hold your font. It
   should end in .bmp. You can use the default name if you want,
   but choose the correct directory (your A4 work directory).

8. You can change the WDL filename and location too or even if 
   you don't want one. This file contains the line of WDL you
   need in order to define your font in your WDL Script.
   
9. Choose 128 Characters or 256 Characters for your A4 Format.
   
10. Press the Generate button. If everything is good, you will 
   see your generated font.
  
11. To use the font, make sure the .bmp file is in the same directory
    as your WDL scripts. Then, open the generated text file and copy
	the line of WDL code (or you can copy it from the WDL File preview 
	field in the tool). Paste this line at the top of your WDL file and
	you can use it like any other font.
	
	
Good luck and post any feedback to the A4 Forum under the appropriate topic
in User Contributions section. 

http://www.conitec.net/ubbcgi/Ultimate.cgi  or
http://www.conitec.com/ubbcgi/Ultimate.cgi


--------------------------------------------------------------------------
HISTORY:
--------------------------------------------------------------------------

New Features in release 1.1 GA:
- added the ability to change the width spacing of all characters in
  in the generated font. You can preview the results as you adjust the 
  value. Variable-width fonts may still not look as desired because W's
  and M's will not take up equal space as I's and 1's.

Fixed bugs/new featues in realease 1.0 GA:
- Cleaned up the bitmap viewer I wrote to display the generated font.
  Scroll bars work better and it resizes based on your screen dimensions.
  It also displays all of the bitmap instead of clipping some right & botom
  pixels.
- The real Font you used last is remebered in the .ini file when you close
  the tool. It used to default to the default font.
  
Fixed bugs in realease 1.0 beta 2:
- The Outline feature works now, but is limited to Truetype fonts only
  (MS Windows fault, not mine!).
- When the directories specified for Output file did not exist, a blank 
  dialog box popped up instead of an informative one.
   
