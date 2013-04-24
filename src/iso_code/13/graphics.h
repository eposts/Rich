
#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif

#if     !defined(__GRAPHX_DEF_)
#define __GRAPHX_DEF_

enum graphics_errors {		
	grOk		   =   0,
	grNoInitGraph	   =  -1,
	grNotDetected	   =  -2,
	grFileNotFound	   =  -3,
	grInvalidDriver    =  -4,
	grNoLoadMem	   =  -5,
	grNoScanMem	   =  -6,
	grNoFloodMem	   =  -7,
	grFontNotFound	   =  -8,
	grNoFontMem	   =  -9,
	grInvalidMode	   = -10,
	grError 	   = -11,   
	grIOerror	   = -12,
	grInvalidFont	   = -13,
	grInvalidFontNum   = -14,
	grInvalidVersion   = -18
};

enum graphics_drivers { 	
	DETECT, 		
	CGA, MCGA, EGA, EGA64, EGAMONO, IBM8514,	
	HERCMONO, ATT400, VGA, PC3270,			
	CURRENT_DRIVER = -1
};

enum graphics_modes {		
    CGAC0      = 0,  
    CGAC1      = 1,  
    CGAC2      = 2,  
    CGAC3      = 3,  
    CGAHI      = 4,  
    MCGAC0     = 0,  
    MCGAC1     = 1,  
    MCGAC2     = 2,  
    MCGAC3     = 3,  
    MCGAMED    = 4,  
    MCGAHI     = 5,  
    EGALO      = 0,  
    EGAHI      = 1,  
    EGA64LO    = 0,  
    EGA64HI    = 1,  
    EGAMONOHI  = 0,  
    HERCMONOHI = 0,  
    ATT400C0   = 0,  
    ATT400C1   = 1,  
    ATT400C2   = 2,  
    ATT400C3   = 3,  
    ATT400MED  = 4,  
    ATT400HI   = 5,  
    VGALO      = 0,  
    VGAMED     = 1,  
    VGAHI      = 2,  
    PC3270HI   = 0,  
    IBM8514LO  = 0,  
    IBM8514HI  = 1   
};



#if	!defined(__COLORS)
#define __COLORS

enum COLORS {
    BLACK,		    
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    DARKGRAY,		    
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE
};
#endif

enum CGA_COLORS {
    CGA_LIGHTGREEN     = 1,		
    CGA_LIGHTRED       = 2,
    CGA_YELLOW	       = 3,

    CGA_LIGHTCYAN      = 1,		
    CGA_LIGHTMAGENTA   = 2,
    CGA_WHITE	       = 3,

    CGA_GREEN	       = 1,		
    CGA_RED	       = 2,
    CGA_BROWN	       = 3,

    CGA_CYAN	       = 1,		
    CGA_MAGENTA        = 2,
    CGA_LIGHTGRAY      = 3
};


enum EGA_COLORS {
    EGA_BLACK		 =  0,	    
    EGA_BLUE		 =  1,
    EGA_GREEN		 =  2,
    EGA_CYAN		 =  3,
    EGA_RED		 =  4,
    EGA_MAGENTA 	 =  5,
    EGA_BROWN		 =  20,
    EGA_LIGHTGRAY	 =  7,
    EGA_DARKGRAY	 =  56,     
    EGA_LIGHTBLUE	 =  57,
    EGA_LIGHTGREEN	 =  58,
    EGA_LIGHTCYAN	 =  59,
    EGA_LIGHTRED	 =  60,
    EGA_LIGHTMAGENTA	 =  61,
    EGA_YELLOW		 =  62,
    EGA_WHITE		 =  63
};

enum line_styles {		setlinestyle */
	SOLID_LINE   = 0,
	DOTTED_LINE  = 1,
	CENTER_LINE  = 2,
	DASHED_LINE  = 3,
	USERBIT_LINE = 4,	
};

enum line_widths {		setlinestyle */
	NORM_WIDTH  = 1,
	THICK_WIDTH = 3,
};

enum font_names {
	DEFAULT_FONT	= 0,	
	TRIPLEX_FONT	= 1,	
	SMALL_FONT	= 2,
	SANS_SERIF_FONT = 3,
	GOTHIC_FONT	= 4
};

#define HORIZ_DIR	0	
#define VERT_DIR	1	

#define USER_CHAR_SIZE	0	

enum fill_patterns {		setfillstyle */
	EMPTY_FILL,		
	SOLID_FILL,		
	LINE_FILL,		
	LTSLASH_FILL,		 fill */
	SLASH_FILL,		 fill with thick lines */
	BKSLASH_FILL,		
	LTBKSLASH_FILL, 	
	HATCH_FILL,		
	XHATCH_FILL,		
	INTERLEAVE_FILL,	
	WIDE_DOT_FILL,		
	CLOSE_DOT_FILL, 	
	USER_FILL		
};

enum putimage_ops {		
	COPY_PUT,		
	XOR_PUT,		
	OR_PUT, 		
	AND_PUT,		
	NOT_PUT 		
};

enum text_just {		
	LEFT_TEXT	= 0,
	CENTER_TEXT	= 1,
	RIGHT_TEXT	= 2,

	BOTTOM_TEXT	= 0,
     
	TOP_TEXT	= 2
};


#define MAXCOLORS 15

struct palettetype {
	unsigned char size;
	signed char colors[MAXCOLORS+1];
};

struct linesettingstype {
	int linestyle;
	unsigned upattern;
	int thickness;
};

struct textsettingstype {
	int font;
	int direction;
	int charsize;
	int horiz;
	int vert;
};

struct fillsettingstype {
	int pattern;
	int color;
};

struct pointtype {
	int x, y;
};

struct viewporttype {
	int left, top, right, bottom;
	int clip;
};

struct arccoordstype {
	int x, y;
	int xstart, ystart, xend, yend;
};

void	   far _Cdecl arc(int x, int y, int stangle, int endangle,
			  int radius);
void	   far _Cdecl bar(int left, int top, int right, int bottom);
void	   far _Cdecl bar3d(int left, int top, int right, int bottom,
			    int depth, int topflag);
void	   far _Cdecl circle(int x, int y, int radius);
void	   far _Cdecl cleardevice(void);
void	   far _Cdecl clearviewport(void);
void	   far _Cdecl closegraph(void);
void	   far _Cdecl detectgraph(int far *graphdriver,int far *graphmode);
void	   far _Cdecl drawpoly(int numpoints, int far *polypoints);
void	   far _Cdecl ellipse(int x, int y, int stangle, int endangle,
			      int xradius, int yradius);
void	   far _Cdecl fillellipse( int x, int y, int xradius, int yradius );
void	   far _Cdecl fillpoly(int numpoints, int far *polypoints);
void	   far _Cdecl floodfill(int x, int y, int border);
void	   far _Cdecl getarccoords(struct arccoordstype far *arccoords);
void	   far _Cdecl getaspectratio(int far *xasp, int far *yasp);
int	   far _Cdecl getbkcolor(void);
int	   far _Cdecl getcolor(void);
struct palettetype * far _Cdecl getdefaultpalette( void );
char *	   far _Cdecl getdrivername( void );
void	   far _Cdecl getfillpattern(char far *pattern);
void	   far _Cdecl getfillsettings(struct fillsettingstype far *fillinfo);
int	   far _Cdecl getgraphmode(void);
void	   far _Cdecl getimage(int left, int top, int right, int bottom,
			       void far *bitmap);
void	   far _Cdecl getlinesettings(struct linesettingstype far *lineinfo);
int	   far _Cdecl getmaxcolor(void);
int	   far _Cdecl getmaxmode(void);
int	   far _Cdecl getmaxx(void);
int	   far _Cdecl getmaxy(void);
char *	   far _Cdecl getmodename( int mode_number );
void	   far _Cdecl getmoderange(int graphdriver, int far *lomode,
				   int far *himode);
unsigned   far _Cdecl getpixel(int x, int y);
void	   far _Cdecl getpalette(struct palettetype far *palette);
int	   far _Cdecl getpalettesize( void );
void	   far _Cdecl gettextsettings(struct textsettingstype far *texttypeinfo);
void	   far _Cdecl getviewsettings(struct viewporttype far *viewport);
int	   far _Cdecl getx(void);
int	   far _Cdecl gety(void);
void	   far _Cdecl graphdefaults(void);
char *	   far _Cdecl grapherrormsg(int errorcode);
void	   far _Cdecl _graphfreemem(void far *ptr, unsigned size);
void far * far _Cdecl _graphgetmem(unsigned size);
int	   far _Cdecl graphresult(void);
unsigned   far _Cdecl imagesize(int left, int top, int right, int bottom);
void	   far _Cdecl initgraph(int  far *graphdriver,
				int  far *graphmode,
				char far *pathtodriver);
int	   far _Cdecl installuserdriver( char far *name, int huge (*detect)(void) );
int	   far _Cdecl installuserfont( char far *name );
void	   far _Cdecl line(int x1, int y1, int x2, int y2);
void	   far _Cdecl linerel(int dx, int dy);
void	   far _Cdecl lineto(int x, int y);
void	   far _Cdecl moverel(int dx, int dy);
void	   far _Cdecl moveto(int x, int y);
void	   far _Cdecl outtext(char far *textstring);
void	   far _Cdecl outtextxy(int x, int y, char far *textstring);
void	   far _Cdecl pieslice(int x, int y, int stangle, int endangle,
			       int radius);
void	   far _Cdecl putimage(int left, int top, void far *bitmap, int op);
void	   far _Cdecl putpixel(int x, int y, int color);
void	   far _Cdecl rectangle(int left, int top, int right, int bottom);
void	   far _Cdecl restorecrtmode(void);
void	   far _Cdecl sector( int X, int Y, int StAngle, int EndAngle,
			      int XRadius, int YRadius );
void	   far _Cdecl setactivepage(int page);
void	   far _Cdecl setallpalette(struct palettetype far *palette);
void	   far _Cdecl setaspectratio( int xasp, int yasp );
void	   far _Cdecl setbkcolor(int color);
void	   far _Cdecl setcolor(int color);
void	   far _Cdecl setfillpattern(char far *upattern, int color);
void	   far _Cdecl setfillstyle(int pattern, int color);
unsigned   far _Cdecl setgraphbufsize(unsigned bufsize);
void	   far _Cdecl setgraphmode(int mode);
void	   far _Cdecl setlinestyle(int linestyle, unsigned upattern,
				   int thickness);
void	   far _Cdecl setpalette(int colornum, int color);
void	   far _Cdecl setrgbpalette(int colornum,
				    int red, int green, int blue);
void	   far _Cdecl settextjustify(int horiz, int vert);
void	   far _Cdecl settextstyle(int font, int direction, int charsize);
void	   far _Cdecl setusercharsize(int multx, int divx,
				      int multy, int divy);
void	   far _Cdecl setviewport(int left, int top, int right, int bottom,
				  int clip);
void	   far _Cdecl setvisualpage(int page);
void	   far _Cdecl setwritemode( int mode );
int	   far _Cdecl textheight(char far *textstring);
int	   far _Cdecl textwidth(char far *textstring);



int	       _Cdecl registerbgidriver(void (*driver)(void));
int	   far _Cdecl registerfarbgidriver(void far *driver);




void	       _Cdecl CGA_driver(void);
void	       _Cdecl EGAVGA_driver(void);
void	       _Cdecl IBM8514_driver(void);
void	       _Cdecl Herc_driver(void);
void	       _Cdecl ATT_driver(void);
void	       _Cdecl PC3270_driver(void);

extern int far _Cdecl CGA_driver_far[];
extern int far _Cdecl EGAVGA_driver_far[];
extern int far _Cdecl IBM8514_driver_far[];
extern int far _Cdecl Herc_driver_far[];
extern int far _Cdecl ATT_driver_far[];
extern int far _Cdecl PC3270_driver_far[];




int	       _Cdecl registerbgifont(void (*font)(void));
int	   far _Cdecl registerfarbgifont(void far *font);




void	       _Cdecl	triplex_font(void);
void	       _Cdecl	  small_font(void);
void	       _Cdecl sansserif_font(void);
void	       _Cdecl	 gothic_font(void);

extern int far _Cdecl	triplex_font_far[];
extern int far _Cdecl	  small_font_far[];
extern int far _Cdecl sansserif_font_far[];
extern int far _Cdecl	 gothic_font_far[];


#endif
