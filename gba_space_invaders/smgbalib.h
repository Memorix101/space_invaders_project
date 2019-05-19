//
// smgbalib v1.0 - a GBA API by Sebastian
//
// Main include file - aggregates all other smgbalib include files!
//

#ifndef SMGBALIB_H_
#define SMGBALIB_H_

//
// INHERITED FROM GBA.H
//

/////////////Type Definitions/////////////
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef volatile unsigned char vu8;
typedef volatile unsigned short vu16;
typedef volatile unsigned long vu32;

typedef const unsigned char const_u8;
typedef const unsigned short const_u16;
typedef const unsigned long const_u32;

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;

typedef unsigned char byte;
typedef unsigned short hword;
typedef unsigned long word;
typedef void (*fp)(void);   //this is the definition you will find in gba.h.  It is just delaring fp to represent a pointer to a function


/////////////////Bits////////////////
#define BIT00 1
#define BIT01 2
#define BIT02 4
#define BIT03 8
#define BIT04 16
#define BIT05 32
#define BIT06 64
#define BIT07 128
#define BIT08 256
#define BIT09 512
#define BIT10 1024
#define BIT11 2048
#define BIT12 4096
#define BIT13 8192
#define BIT14 16384
#define BIT15 32768

///////////////Joystick Keys/////////////////////////////
#define BUTTON_A			0x0001      // A Button
#define BUTTON_B			0x0002      // B Button
#define BUTTON_SELECT	    0x0004      // select button
#define BUTTON_START		0x0008      // START button
#define KEYPAD_RIGHT        0x0010      // Right key
#define KEYPAD_LEFT         0x0020      // Left key
#define KEYPAD_UP           0x0040      // Up key
#define KEYPAD_DOWN         0x0080      // Down key
#define BUTTON_RIGHT	    0x0100      // R shoulder Button
#define BUTTON_LEFT         0x0200		// L shoulder Button

#define TMR_PRESCALER_1CK       0x0000  // Prescaler 1 Clock
#define TMR_PRESCALER_64CK      0x0001  //            64 clocks
#define TMR_PRESCALER_256CK     0x0002  //          256 clocks
#define TMR_PRESCALER_1024CK    0x0003  //        1024 clocks
#define TMR_IF_ENABLE           0x0040  // Interrupt Request Enable
#define TMR_ENABLE              0x0080  // Run Timer

///////////////Interrupts/////////////////////////////
#define INT_VBLANK 0x0001
#define INT_HBLANK 0x0002
#define INT_VCOUNT 0x0004 //you can set the display to generate an interrupt when it reaches a particular line on the screen
#define INT_TIMER0 0x0008
#define INT_TIMER1 0x0010
#define INT_TIMER2 0x0020
#define INT_TIMER3 0x0040
#define INT_COMUNICATION 0x0080 //serial communication interupt
#define INT_DMA0 0x0100
#define INT_DMA1 0x0200
#define INT_DMA2 0x0400
#define INT_DMA3 0x0800
#define INT_KEYBOARD 0x1000
#define INT_CART 0x2000 //the cart can actually generate an interupt
#define INT_ALL 0x4000 //this is just a flag we can set to allow the my function to enable or disable all interrupts. Doesn't actually correspond to a bit in REG_IE

////////////////sound channels/////////////
#define SOUNDINIT			0x8000	// makes the sound restart
#define SOUNDDUTY87			0x0000	//87.5% wave duty
#define SOUNDDUTY75			0x0040	//75% wave duty
#define SOUNDDUTY50			0x0080	//50% wave duty
#define SOUNDDUTY25			0x00C0	//25% wave duty

#define SOUND1PLAYONCE		0x4000	// play sound once
#define SOUND1PLAYLOOP		0x0000	// play sound looped
#define SOUND1INIT			0x8000	// makes the sound restart
#define SOUND1SWEEPSHIFTS(n)	n	// number of sweep shifts (0-7)
#define SOUND1SWEEPINC		0x0000	// sweep add (freq increase)
#define SOUND1SWEEPDEC		0x0008	// sweep dec (freq decrese)
#define SOUND1SWEEPTIME(n)	(n<<4)	// time of sweep (0-7)
#define SOUND1ENVSTEPS(n)	(n<<8)	// envelope steps (0-7)
#define SOUND1ENVINC		0x0800	// envellope increase
#define SOUND1ENVDEC		0x0000	// envellope decrease
#define SOUND1ENVINIT(n)	(n<<12) // initial envelope volume (0-15)


#define SOUND2PLAYONCE		0x4000	// play sound once
#define SOUND2PLAYLOOP		0x0000	// play sound looped
#define SOUND2INIT			0x8000	// makes the sound restart
#define SOUND2ENVSTEPS(n)	(n<<8)	// envelope steps (0-7)
#define SOUND2ENVINC		0x0800	// envellope increase
#define SOUND2ENVDEC		0x0000	// envellope decrease
#define SOUND2ENVINIT(n)	(n<<12) // initial envelope volume (0-15)



#define SOUND3BANK32		0x0000	// Use two banks of 32 steps each
#define SOUND3BANK64		0x0020	// Use one bank of 64 steps
#define SOUND3SETBANK0		0x0000	// Bank to play 0 or 1 (non set bank is written to)
#define SOUND3SETBANK1		0x0040
#define SOUND3PLAY			0x0080	// Output sound

#define SOUND3OUTPUT0		0x0000	// Mute output
#define SOUND3OUTPUT1		0x2000	// Output unmodified
#define SOUND3OUTPUT12		0x4000	// Output 1/2 
#define SOUND3OUTPUT14		0x6000	// Output 1/4 
#define SOUND3OUTPUT34		0x8000  // Output 3/4

#define SOUND3PLAYONCE		0x4000	// Play sound once
#define SOUND3PLAYLOOP		0x0000	// Play sound looped
#define SOUND3INIT			0x8000	// Makes the sound restart


#define SOUND4PLAYONCE		0x4000	// play sound once
#define SOUND4PLAYLOOP		0x0000	// play sound looped
#define SOUND4INIT			0x8000	// makes the sound restart
#define SOUND4ENVSTEPS(n)	(n<<8)	// envelope steps (0-7)
#define SOUND4ENVINC		0x0800	// envellope increase
#define SOUND4ENVDEC		0x0000	// envellope decrease
#define SOUND4ENVINIT(n)	(n<<12) // initial envelope volume (0-15)


#define SOUND4STEPS7		0x0004
#define SOUND4STEPS15		0x0000
#define SOUND4PLAYONCE		0x4000
#define SOUND4PLAYLOOP		0x0000
#define SOUND4INIT			0x8000

/////////////////Registers/////////////////		
#define REG_INTERUPT   *(u32*)0x3007FFC		//Interrupt Register
#define REG_DISPCNT    *(u32*)0x4000000		//Display Control (Mode)
#define REG_DISPCNT_L  *(u16*)0x4000000		//???
#define REG_DISPCNT_H  *(u16*)0x4000002		//???
#define REG_DISPSTAT   *(u16*)0x4000004		//???
#define REG_VCOUNT     *(vu16*)0x4000006		//Vertical Control (Sync)
#define REG_BG0CNT     *(u16*)0x4000008		//Background 0
#define REG_BG1CNT     *(u16*)0x400000A		//Background 1
#define REG_BG2CNT     *(u16*)0x400000C		//Background 2
#define REG_BG3CNT     *(u16*)0x400000E		//Background 3
#define REG_BG0HOFS    *(u16*)0x4000010		//Background 0 Horizontal Offset
#define REG_BG0VOFS    *(u16*)0x4000012		//Background 0 Vertical Offset
#define REG_BG1HOFS    *(u16*)0x4000014		//Background 1 Horizontal Offset
#define REG_BG1VOFS    *(u16*)0x4000016		//Background 1 Vertical Offset
#define REG_BG2HOFS    *(u16*)0x4000018		//Background 2 Horizontal Offset
#define REG_BG2VOFS    *(u16*)0x400001A		//Background 2 Vertical Offset
#define REG_BG3HOFS    *(u16*)0x400001C		//Background 3 Horizontal Offset
#define REG_BG3VOFS    *(u16*)0x400001E		//Background 3 Vertical Offset
#define REG_BG2PA      *(u16*)0x4000020		//Background 2 PA Rotation (pa = x_scale * cos(angle);)
#define REG_BG2PB      *(u16*)0x4000022		//Background 2 PB Rotation (pb = y_scale * sin(angle);)
#define REG_BG2PC      *(u16*)0x4000024		//Background 2 PC Rotation (pc = x_scale * -sin(angle);)
#define REG_BG2PD      *(u16*)0x4000026		//Background 2 PD Rotation (pd = y_scale * cos(angle);)
#define REG_BG2X       *(u32*)0x4000028		//Background 2 X Location
#define REG_BG2X_L     *(u16*)0x4000028		//???
#define REG_BG2X_H     *(u16*)0x400002A		//???
#define REG_BG2Y       *(u32*)0x400002C		//Background 2 Y Location
#define REG_BG2Y_L     *(u16*)0x400002C		//???
#define REG_BG2Y_H     *(u16*)0x400002E		//???
#define REG_BG3PA      *(u16*)0x4000030		//Background 3 PA Rotation (pa = x_scale * cos(angle);)
#define REG_BG3PB      *(u16*)0x4000032		//Background 3 PB Rotation (pb = y_scale * sin(angle);)
#define REG_BG3PC      *(u16*)0x4000034		//Background 3 PC Rotation (pc = x_scale * -sin(angle);)
#define REG_BG3PD      *(u16*)0x4000036		//Background 3 PD Rotation (pd = y_scale * cos(angle);)
#define REG_BG3X       *(u32*)0x4000038		//Background 3 X Location
#define REG_BG3X_L     *(u16*)0x4000038		//???
#define REG_BG3X_H     *(u16*)0x400003A		//???
#define REG_BG3Y       *(u32*)0x400003C		//Background 3 Y Location
#define REG_BG3Y_L     *(u16*)0x400003C		//???
#define REG_BG3Y_H     *(u16*)0x400003E		//???
#define REG_WIN0H      *(u16*)0x4000040		//Window 0 X coords (bits 0-7 right, bits 8-16 left)
#define REG_WIN1H      *(u16*)0x4000042		//Window 1 X coords (bits 0-7 right, bits 8-16 left)
#define REG_WIN0V      *(u16*)0x4000044		//Window 0 Y coords (bits 0-7 bottom, bits 8-16 top)
#define REG_WIN1V      *(u16*)0x4000046		//Window 1 Y coords (bits 0-7 bottom, bits 8-16 top)
#define REG_WININ      *(u16*)0x4000048		//Inside Window Settings
#define REG_WINOUT     *(u16*)0x400004A		//Outside Window Settings
#define REG_MOSAIC     *(u32*)0x400004C		//Mosaic Mode
#define REG_MOSAIC_L   *(u32*)0x400004C		//???
#define REG_MOSAIC_H   *(u32*)0x400004E		//???
#define REG_BLDMOD     *(u16*)0x4000050		//Blend Mode
#define REG_COLEV      *(u16*)0x4000052		//???
#define REG_COLEY      *(u16*)0x4000054		//???

#define REG_SOUND1CNT   *(vu32*)0x4000060	//sound 1
#define REG_SOUND1CNT_L *(vu16*)0x4000060	//
#define REG_SOUND1CNT_H *(vu16*)0x4000062	//
#define REG_SOUND1CNT_X *(vu16*)0x4000064	//

#define REG_SOUND2CNT_L *(vu16*)0x4000068		//sound 2 lenght & wave duty
#define REG_SOUND2CNT_H *(vu16*)0x400006C		//sound 2 frequency+loop+reset

#define REG_SG30       *(vu32*)0x4000070		//???
#define REG_SOUND3CNT  *(vu32*)0x4000070		//???
#define REG_SG30_L     *(vu16*)0x4000070		//???
#define REG_SOUND3CNT_L *(vu16*)0x4000070	//???
#define REG_SG30_H     *(vu16*)0x4000072		//???
#define REG_SOUND3CNT_H *(vu16*)0x4000072	//???
#define REG_SG31       *(vu16*)0x4000074		//???
#define REG_SOUND3CNT_X *(vu16*)0x4000074	//???

#define REG_SOUND4CNT_L *(vu16*)0x4000078		//???
#define REG_SOUND4CNT_H *(vu16*)0x400007C		//???

#define REG_SGCNT0     *(vu32*)0x4000080		
#define REG_SGCNT0_L   *(vu16*)0x4000080		
#define REG_SOUNDCNT   *(vu32*)0x4000080
#define REG_SOUNDCNT_L *(vu16*)0x4000080		//DMG sound control

#define REG_SGCNT0_H   *(vu16*)0x4000082		
#define REG_SOUNDCNT_H *(vu16*)0x4000082		//Direct sound control

#define REG_SGCNT1     *(vu16*)0x4000084		
#define REG_SOUNDCNT_X *(vu16*)0x4000084	    //Extended sound control

#define REG_SGBIAS     *(vu16*)0x4000088		
#define REG_SOUNDBIAS  *(vu16*)0x4000088		//bit rate+sound bias

#define REG_WAVE_RAM0  *(vu32*)0x4000090		//???
#define REG_SGWR0_L    *(vu16*)0x4000090		//???
#define REG_SGWR0_H    *(vu16*)0x4000092		//???
#define REG_WAVE_RAM1  *(vu32*)0x4000094		//???
#define REG_SGWR1_L    *(vu16*)0x4000094		//???
#define REG_SGWR1_H    *(vu16*)0x4000096		//???
#define REG_WAVE_RAM2  *(vu32*)0x4000098		//???
#define REG_SGWR2_L    *(vu16*)0x4000098		//???
#define REG_SGWR2_H    *(vu16*)0x400009A		//???
#define REG_WAVE_RAM3  *(vu32*)0x400009C		//???
#define REG_SGWR3_L    *(vu16*)0x400009C		//???
#define REG_SGWR3_H    *(vu16*)0x400009E		//???

#define REG_SGFIFOA    *(vu32*)0x40000A0		//???
#define REG_SGFIFOA_L  *(vu16*)0x40000A0		//???
#define REG_SGFIFOA_H  *(vu16*)0x40000A2		//???
#define REG_SGFIFOB    *(vu32*)0x40000A4		//???
#define REG_SGFIFOB_L  *(vu16*)0x40000A4		//???
#define REG_SGFIFOB_H  *(vu16*)0x40000A6		//???
#define REG_DMA0SAD     *(u32*)0x40000B0	//DMA0 Source Address
#define REG_DMA0SAD_L   *(u16*)0x40000B0	//DMA0 Source Address Low Value
#define REG_DMA0SAD_H   *(u16*)0x40000B2	//DMA0 Source Address High Value
#define REG_DMA0DAD     *(u32*)0x40000B4	//DMA0 Destination Address
#define REG_DMA0DAD_L   *(u16*)0x40000B4	//DMA0 Destination Address Low Value
#define REG_DMA0DAD_H   *(u16*)0x40000B6	//DMA0 Destination Address High Value
#define REG_DMA0CNT     *(u32*)0x40000B8	//DMA0 Control (Amount)
#define REG_DMA0CNT_L   *(u16*)0x40000B8	//DMA0 Control Low Value
#define REG_DMA0CNT_H   *(u16*)0x40000BA	//DMA0 Control High Value

#define REG_DMA1SAD     *(u32*)0x40000BC	//DMA1 Source Address
#define REG_DMA1SAD_L   *(u16*)0x40000BC	//DMA1 Source Address Low Value
#define REG_DMA1SAD_H   *(u16*)0x40000BE	//DMA1 Source Address High Value
#define REG_DMA1DAD     *(u32*)0x40000C0	//DMA1 Desination Address
#define REG_DMA1DAD_L   *(u16*)0x40000C0	//DMA1 Destination Address Low Value
#define REG_DMA1DAD_H   *(u16*)0x40000C2	//DMA1 Destination Address High Value
#define REG_DMA1CNT     *(u32*)0x40000C4	//DMA1 Control (Amount)
#define REG_DMA1CNT_L   *(u16*)0x40000C4	//DMA1 Control Low Value
#define REG_DMA1CNT_H   *(u16*)0x40000C6	//DMA1 Control High Value

#define REG_DMA2SAD     *(u32*)0x40000C8	//DMA2 Source Address
#define REG_DMA2SAD_L   *(u16*)0x40000C8	//DMA2 Source Address Low Value
#define REG_DMA2SAD_H   *(u16*)0x40000CA	//DMA2 Source Address High Value
#define REG_DMA2DAD     *(u32*)0x40000CC	//DMA2 Destination Address
#define REG_DMA2DAD_L   *(u16*)0x40000CC	//DMA2 Destination Address Low Value
#define REG_DMA2DAD_H   *(u16*)0x40000CE	//DMA2 Destination Address High Value
#define REG_DMA2CNT     *(u32*)0x40000D0	//DMA2 Control (Amount)
#define REG_DMA2CNT_L   *(u16*)0x40000D0	//DMA2 Control Low Value
#define REG_DMA2CNT_H   *(u16*)0x40000D2	//DMA2 Control High Value

#define REG_DMA3SAD     *(u32*)0x40000D4	//DMA3 Source Address
#define REG_DMA3SAD_L   *(u16*)0x40000D4	//DMA3 Source Address Low Value
#define REG_DMA3SAD_H   *(u16*)0x40000D6	//DMA3 Source Address High Value
#define REG_DMA3DAD     *(u32*)0x40000D8	//DMA3 Destination Address
#define REG_DMA3DAD_L   *(u16*)0x40000D8	//DMA3 Destination Address Low Value
#define REG_DMA3DAD_H   *(u16*)0x40000DA	//DMA3 Destination Address High Value
#define REG_DMA3CNT     *(u32*)0x40000DC	//DMA3 Control (Amount)
#define REG_DMA3CNT_L   *(u16*)0x40000DC	//DMA3 Control Low Value
#define REG_DMA3CNT_H   *(u16*)0x40000DE	//DMA3 Control High Value

#define REG_TM0CNT      *(u32*)0x4000100	//Timer 0
#define REG_TM0CNT_L	*(u16*)0x4000100	//Timer 0 count value
#define REG_TM0CNT_H    *(u16*)0x4000102	//Timer 0 Control

#define REG_TM1CNT     *(u32*)0x4000104		//Timer 2
#define REG_TM1CNT_L   *(u16*)0x4000104		//Timer 2 count value
#define REG_TM1CNT_H   *(u16*)0x4000106		//Timer 2 control

#define REG_TM2D       *(u16*)0x4000108		//Timer 3?
#define REG_TM2CNT     *(u16*)0x400010A		//Timer 3 Control

#define REG_TM3D       *(u16*)0x400010C		//Timer 4?
#define REG_TM3CNT     *(u16*)0x400010E		//Timer 4 Control

#define REG_SCD0       *(u16*)0x4000120		//32-bit Normal Serial Communication Data 0 / Multi-play
#define REG_SCD1       *(u16*)0x4000122		//32-bit Normal Serial Communication Data 1 /Multi-play
#define REG_SCD2       *(u16*)0x4000124		//Multi-play Communication Data 2
#define REG_SCD3       *(u16*)0x4000126		//Multi-play Communication Data 3
#define REG_SCCNT      *(u32*)0x4000128		//???
#define REG_SCCNT_L    *(u16*)0x4000128		//???
#define REG_SCCNT_H    *(u16*)0x400012A		//???
#define REG_P1         *(vu16*)0x4000130		//Player 1 Input
#define REG_KEYPAD     *(vu16*)0x4000130		//Player 1 Input
#define REG_P1CNT      *(vu16*)0x4000132		//Player 1 Input Interrupt Status
#define REG_R          *(u16*)0x4000134		//???
#define REG_HS_CTRL    *(u16*)0x4000140		//???
#define REG_JOYRE      *(u32*)0x4000150		//???
#define REG_JOYRE_L    *(u16*)0x4000150		//???
#define REG_JOYRE_H    *(u16*)0x4000152		//???
#define REG_JOYTR      *(u32*)0x4000154		//???
#define REG_JOYTR_L    *(u16*)0x4000154		//???
#define REG_JOYTR_H    *(u16*)0x4000156		//???
#define REG_JSTAT      *(u32*)0x4000158		//???
#define REG_JSTAT_L    *(u16*)0x4000158		//???
#define REG_JSTAT_H    *(u16*)0x400015A		//???
#define REG_IE         *(u16*)0x4000200		//Interrupt Enable
#define REG_IF         *(vu16*)0x4000202		//Interrupt Flags
#define REG_WSCNT      *(u16*)0x4000204		//???
#define REG_IME        *(u16*)0x4000208		//Interrupt Master Enable
#define REG_PAUSE      *(u16*)0x4000300		//Pause

// ----------------- Added by Sebastian -------------------
#define MODE_0 0x0
#define MODE_1 0x1
#define MODE_2 0x2
#define MODE_3 0x3
#define MODE_4 0x4
#define MODE_5 0x5

#define BACKBUFFER 0x10
#define H_BLANK_OAM 0x20 

#define OBJ_MAP_2D 0x0
#define OBJ_MAP_1D 0x40

#define FORCE_BLANK 0x80

#define BG0_ENABLE 0x100
#define BG1_ENABLE 0x200 
#define BG2_ENABLE 0x400
#define BG3_ENABLE 0x800
#define OBJ_ENABLE 0x1000 

#define WIN1_ENABLE 0x2000 
#define WIN2_ENABLE 0x4000
#define WINOBJ_ENABLE 0x8000

//
// DONE INHERITANCE FROM GBA.H
//


//---------- FIXED FONT ----------
#define  fixedfont_WIDTH   128
#define  fixedfont_HEIGHT  128
extern const u16 fixedfontPalette[];
extern const u16 fixedfontData[];

//---------- PALETTES -------------
// A simple palette with all black colours
extern const u16 blackPalette[];
// A palette with a rainbow of colours! Black at 0, White at 1
extern const u16 rainbowPalette[];

//---------- GRAPHICS ----------
// Video buffers
extern u16* FrontBuffer;
extern u16* BackBuffer;
extern u16* videoBuffer;
extern u16* paletteMem;

#define SetVideoMode(mode) REG_DISPCNT = (mode) 

// Wait for a VSYNC and flip buffers
void Flip(void);

// Erase based on blackPalette, and flip buffers
void EraseScreen(void);
// Erase based on current palette, and DO NOT flip buffers
void EraseScreenNoPaletteNoFlip(void);

// Set current palette
void SetPalette( u16* palette );


// NOTE: Though each pixel references a colour from the palette from 0 to 255
//       (which fits in 1 byte), we are passing in a 16-bit (2 bytes) value
//       which plots two actual pixels.
//    
//       As an example, to plot two pixels side by side using color 5 of the
//       palette, we use the value 0x0505 for parameter c

// Plot a pixel, overwriting current pixel value
void PlotPixelOpaque(int x,int y, unsigned short int c);
// Plot a pixel, using first entry in the palette as the transparent colour
void PlotPixelTransparent(int x,int y, unsigned short int c);



// Draw a rectangular bitmap, considering transparency
void DrawBitmapTransparent( u16 x, u16 y, u16 height, u16 width, u16* data );
// Draw a rectangular bitmap ignoring transparency
void DrawBitmapOpaque( u16 x, u16 y, u16 height, u16 width, u16* data );

// Draw a 240x160 bitmap on top of the entire screen
void DrawFullScreenOpaque( u16* data );

// Draw a section of a bitmap - useful when animating strips of frames
void DrawBitmapSectorTransparent( u16 x, u16 y, //where on screen we are writing this character
				u16 height, u16 width, //height and width of the whole character map
				u16 bitmapx, u16 bitmapy, //top left position in the character map where we're starting
				u16 bitmapheight, u16 bitmapwidth, //how many pixels horizontally and vertically we're writing
				u16* data );

// Draw a section of a bitmap - useful when animating strips of frames
void DrawBitmapSectorOpaque( u16 x, u16 y, //where on screen we are writing this character
				u16 height, u16 width, //height and width of the whole character map
				u16 bitmapx, u16 bitmapy, //top left position in the character map where we're starting
				u16 bitmapheight, u16 bitmapwidth, //how many pixels horizontally and vertically we're writing
				u16* data );
				

// ------------ SOUND -------------
// Generate white noise on channel 4 given a pattern. Use with the preset
// white noise patterns below
void PlayWhiteNoise( u16 selector );

// Generate a sound on channel 3 at a given frequency, and for a set amount of time
// - frequency is calculated from the actual frequency (see common notes below)
// - length is between 0 (about 0.04 seconds) and 255 (about 1 second)
void PlayNote( u16 frequency, unsigned char length );

/// Generate a frequency sweep on channel 1 starting from a given frequency
// - frequency range is 0x07FF to 0x0000
// See commonly used frequencies below
void PlayFrequencySweep( u16 frequency );

// Generate a volume sweep on channel 2 starting from a given frequency
// - frequency range is 0x07FF to 0x0000
// See commonly used frequencies below
void PlayVolumeSweep( u16 frequency );


// Great white noise effects! Mainly suited for explosions, crashes
// To be used with PlayWhiteNoise
#define WHITE_NOISE_EXPLOSION_LOW 0x8072
#define WHITE_NOISE_EXPLOSION_HIGH 0x8052
#define WHITE_NOISE_SPLASH 0x8032
#define WHITE_NOISE_CRASH 0x8062
#define WHITE_NOISE_CRUMBLE_HIGH 0x8082
#define WHITE_NOISE_CRUMBLE_LOW 0x8092

// Common frequencies to use with PlayVolumeSweep or PlayFrequencySweep
// They sound pretty nicely when repeated fast. You can get something that
// sounds like a car engine, etc. that way.
#define SWEEP_FREQUENCY_LOWEST 0x0010
#define SWEEP_FREQUENCY_LOWER 0x0080
#define SWEEP_FREQUENCY_LOW 0x0100
#define SWEEP_FREQUENCY_MEDIUM 0x0400
#define SWEEP_FREQUENCY_HIGH 0x0600
#define SWEEP_FREQUENCY_HIGHER 0x0700
#define SWEEP_FREQUENCY_REALLY_HIGH 0x07A0
#define SWEEP_FREQUENCY_CHIRP 0x07E0
#define SWEEP_FREQUENCY_PIN_DROP 0x07F0

// Common musical notes - use these with PlayNote!
//
// They are calculated using the formula:
//
// f =  2048 - (4194304 / (32 * Frequency))
//
// For example, C4's frequency is 261.63 Hz. Plugging that into the above formula
// yields f = 1547. So if you pass in 1547 to PlayNote, you will hear a C4

#define NOTE_C2  44
#define NOTE_CSHARP2_DFLAT2  157
#define NOTE_D2  263
#define NOTE_DSHARP2_EFLAT2  363
#define NOTE_E2  458
#define NOTE_F2  547
#define NOTE_FSHARP2_GFLAT2  631
#define NOTE_G2  711
#define NOTE_GSHARP2_AFLAT2  786
#define NOTE_A2  856
#define NOTE_ASHARP2_BFLAT2  923
#define NOTE_B2  986
#define NOTE_C3  1046
#define NOTE_CSHARP3_DFLAT3  1102
#define NOTE_D3  1155
#define NOTE_DSHARP3_EFLAT3  1205
#define NOTE_E3  1253
#define NOTE_F3  1297
#define NOTE_FSHARP3_GFLAT3  1340
#define NOTE_G3  1379
#define NOTE_GSHARP3_AFLAT3  1417
#define NOTE_A3  1452
#define NOTE_ASHARP3_BFLAT3  1486
#define NOTE_B3  1517
#define NOTE_C4  1547
#define NOTE_CSHARP4_DFLAT4  1575
#define NOTE_D4  1602
#define NOTE_DSHARP4_EFLAT4  1627
#define NOTE_E4  1650
#define NOTE_F4  1673
#define NOTE_FSHARP4_GFLAT4  1694
#define NOTE_G4  1714
#define NOTE_GSHARP4_AFLAT4  1732
#define NOTE_A4  1750
#define NOTE_ASHARP4_BFLAT4  1767
#define NOTE_B4  1783
#define NOTE_C5  1798
#define NOTE_CSHARP5_DFLAT5  1812
#define NOTE_D5  1825
#define NOTE_DSHARP5_EFLAT5  1837
#define NOTE_E5  1849
#define NOTE_F5  1860
#define NOTE_FSHARP5_GFLAT5  1871
#define NOTE_G5  1881
#define NOTE_GSHARP5_AFLAT5  1890
#define NOTE_A5  1899
#define NOTE_ASHARP5_BFLAT5  1907
#define NOTE_B5  1915
#define NOTE_C6  1923
#define NOTE_CSHARP6_DFLAT6  1930
#define NOTE_D6  1936
#define NOTE_DSHARP6_EFLAT6  1943
#define NOTE_E6  1949
#define NOTE_F6  1954
#define NOTE_FSHARP6_GFLAT6  1959
#define NOTE_G6  1964
#define NOTE_GSHARP6_AFLAT6  1969
#define NOTE_A6  1974
#define NOTE_ASHARP6_BFLAT6  1978
#define NOTE_B6  1982
#define NOTE_C7  1985
#define NOTE_CSHARP7_DFLAT7  1989
#define NOTE_D7  1992
#define NOTE_DSHARP7_EFLAT7  1995
#define NOTE_E7  1998
#define NOTE_F7  2001
#define NOTE_FSHARP7_GFLAT7  2004
#define NOTE_G7  2006
#define NOTE_GSHARP7_AFLAT7  2009
#define NOTE_A7  2011
#define NOTE_ASHARP7_BFLAT7  2013
#define NOTE_B7  2015
#define NOTE_C8  2017
#define NOTE_CSHARP8_DFLAT8  2018
#define NOTE_D8  2020
#define NOTE_DSHARP8_EFLAT8  2022

// ------------ TEXT -------------
// Display a single character on screen
void WriteCharacter( u16 x, u16 y, unsigned char c );

// Display text on screen
void WriteStringNarrow( u16 x, u16 y, unsigned char* c );
void WriteStringWide( u16 x, u16 y, unsigned char* c );

// ---------------- UTILS -----------------
// Lock up the CPU for a short while, useful for delaying things
// 255 = 1 second
void Sleep(int i);

// Convert numbers to strings, so that they can be displayed
void Itoa( char* s, u16 number );

// Highest random number
#define RAND_MAX 32767
// Hold the next random number root
extern volatile s32 RAND_RandomData;

// Seed a random sequence based on current V register value
// It's a good idea to call this after an interval of time which depends on
// user input, since each user will press buttons at varying intervals of
// time. Since these intervals vary slightly (some folks are faster at pressing
// START for example), the random sequence should vary wildly from one playthrough
// to the next.
void SeedRandom(void);

// Get a random number between 0 and Value (NON-INCLUSIVE)
s32 NextRandom(s32 Value);

// Wait for vertical sync
void WaitForVblank(void);

// buttons register
extern volatile u32* KEYS;

// Meant to be called at the beginning of your program. Performs the following steps:
// - Set video mode 4
// - Clear both buffers
// - Set a standard rainbow palette
void Initialize(void);

// buttons convenience macros
#define IS_START_PRESSED !((*KEYS) & BUTTON_START)
#define IS_SELECT_PRESSED !((*KEYS) & BUTTON_SELECT)

#define IS_RIGHT_PRESSED !((*KEYS) & KEYPAD_RIGHT)
#define IS_LEFT_PRESSED !((*KEYS) & KEYPAD_LEFT)
#define IS_UP_PRESSED !((*KEYS) & KEYPAD_UP)
#define IS_DOWN_PRESSED !((*KEYS) & KEYPAD_DOWN)

#define IS_SHOULDER_RIGHT_PRESSED !((*KEYS) & BUTTON_RIGHT)
#define IS_SHOULDER_LEFT_PRESSED !((*KEYS) & BUTTON_LEFT)

#define IS_A_PRESSED !((*KEYS) & BUTTON_A)
#define IS_B_PRESSED !((*KEYS) & BUTTON_B)

#endif
