#ifndef GLDEF	/* Release 2.3 */
#define GLDEF
/**************************************************************************
 *									  *
 * 		 Copyright (C) 1984, Silicon Graphics, Inc.		  *
 *									  *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright law.  They  may  not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *									  *
 **************************************************************************/

/* graphics libary header file */

/* maximum X and Y screen coordinates */

#define XMAXSCREEN	1023
#define YMAXSCREEN	767

/* various hardware/software limits	*/

#define ATTRIBSTACKDEPTH	10
#define VPSTACKDEPTH		8
#define MATRIXSTACKDEPTH	32
#define NAMESTACKDEPTH		1025
#define STARTTAG		-2
#define ENDTAG			-3
#define MAXFONTNC		256   /* max size of font offset array */
#define MINFONTNC		128   /* for compatibility with old fonts */

/* names for colors in color map loaded by ginit() */

#define BLACK		0
#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define WHITE		7

/* popup colors */

#define PUP_CLEAR	0
#define PUP_CURSOR	1
#define PUP_BLACK	2
#define PUP_WHITE	3

#ifndef FALSE
#define	FALSE		0
#endif
#ifndef TRUE
#define TRUE		(!FALSE)
#endif

/* typedefs */

typedef unsigned char Byte;
typedef long Boolean;
typedef char *String;

typedef short Angle;
typedef short Screencoord;
typedef short Scoord;
typedef long Icoord;
typedef float Coord;
typedef float Matrix[4][4];

typedef unsigned short Colorindex;
typedef unsigned char RGBvalue;

typedef unsigned short Device;

#define PATTERN_16 16
#define PATTERN_32 32
#define PATTERN_64 64

#define PATTERN_16_SIZE	16
#define PATTERN_32_SIZE	64
#define PATTERN_64_SIZE	256

typedef unsigned short Pattern16[PATTERN_16_SIZE];
typedef unsigned short Pattern32[PATTERN_32_SIZE];
typedef unsigned short Pattern64[PATTERN_64_SIZE];

typedef unsigned short Linestyle;
typedef unsigned short Cursor[16];

typedef struct {
	unsigned short offset;
	Byte w,h;
	char xoff,yoff;
	short width;
} Fontchar;

typedef long Object;
typedef long Tag;
typedef long Offset;

extern void	addtopup();
extern void	arc();
extern void	arcf();
extern void	arcfi();
extern void	arci();
extern void	arcfs();
extern void	arcs();
extern void	attachcursor();
extern void	backbuffer();
extern void	backface();
extern void	bbox2();
extern void	bbox2i();
extern void	bbox2s();
extern void	blankscreen();
extern void	blanktime();
extern void	blink();
extern long	blkqread();
extern void	callfunc();
extern void	callobj();
extern void	capture();
extern void	charstr();
extern void	circ();
extern void	circf();
extern void	circfi();
extern void	circi();
extern void	circfs();
extern void	circs();
extern void	clear();
extern void	clearhitcode();
extern void	clkoff();
extern void	clkon();
extern void	closeobj();
extern void	cmov();
extern void	cmov2();
extern void	cmov2i();
extern void	cmovi();
extern void	cmov2s();
extern void	cmovs();
extern void	color(Colorindex c);
extern void	compactify();
extern void	crv();
extern void	crvn();
extern void	curorigin();
extern void	cursoff();
extern void	curson();
extern void	curvebasis();
extern void	curveit();
extern void	curveprecision();
extern void	cyclemap();
extern void	dbtext();
extern void	defbasis();
extern void	defcursor();
extern void	deflinestyle();
extern void	defpattern(long index, short size, short *mask);
extern long	defpup();
extern void	defrasterfont();
extern void	delobj();
extern void	deltag();
extern void	depthcue();
extern void	devport();
extern long	dopup();
extern long	dopupbut();
extern void	doublebuffer();
extern void	draw();
extern void	draw2();
extern void	draw2i();
extern void	draw2s();
extern void	drawi();
extern void	draws();
extern void	editobj();
extern long	endfeedback();
extern void	endfullscrn();
extern long	endpick();
extern void	endpupmode();
extern long	endselect();
extern void	feedback();
extern void	finish();
extern void	font();
extern void	foreground();
extern void	freepup();
extern void	frontbuffer();
extern void	fudge();
extern void	fullscrn();
extern void	gbegin();
extern void	gconfig();
extern Object	genobj();
extern Tag	gentag();
extern long	getbuffer();
extern Boolean	getbutton();
extern Boolean	getcmmode();
extern long	getcolor();
extern void	getcpos();
extern void	getcursor();
extern Boolean	getdcm();
extern void	getdepth();
extern void	getdev();
extern long	getdisplaymode();
extern long	getfont();
extern void	getgpos();
extern long	getheight();
extern long	gethitcode();
extern Boolean	getlsbackup();
extern long	getlsrepeat();
extern long	getlstyle();
extern long	getlwidth();
extern long	getmap();
extern void	getmatrix();
extern void	getmcolor(Colorindex index, short *red, short *green, short *blue);
extern long	getmem();
extern long	getmonitor();
extern void	getorigin();
extern Object	getopenobj();
extern long	getothermonitor();
extern long	getpattern();
extern long	getplanes();
extern void	getport();
extern Boolean	getresetls();
extern void	getscrmask();
extern long	getshade();
extern void	getsize();
extern void	gettp();
extern long	getvaluator(long device);
extern void	getviewport();
extern long	getwritemask();
extern Boolean	getzbuffer();
extern void	gewrite();
extern void	gexit();
extern void	gflush();
extern void	ginit();
extern void	gselect();
extern void	greset();
extern void	gsync();
extern void	gRGBcolor(long r, long g, long b);
extern void	gRGBcursor();
extern void	gRGBmask();
extern void	imakebackground();
extern void	initnames();
extern Boolean	ismex();
extern Boolean	isobj();
extern Boolean	isqueued();
extern Boolean	istag();
extern void	keepaspect();
extern void	lampoff();
extern void	lampon();
extern void	linewidth();
extern void	loadmatrix(Matrix m);
extern void	loadname();
extern void	lookat(Coord viewx, Coord viewy, Coord viewz, Coord pointx, Coord pointy, Coord pointz, Angle twist);
extern void	lsbackup();
extern void	lsrepeat();
extern void	makeobj();
extern void	maketag();
extern void     mapcolor(Colorindex index, short red, short green, short blue);
extern void	mapw();
extern void	mapw2();
extern void	maxsize();
extern void	minsize();
extern void	move();
extern void	move2();
extern void	move2i();
extern void	move2s();
extern void	movei();
extern void	moves();
extern void	multimap();
extern void	multmatrix();
extern long	newpup();
extern void	newtag();
extern void	noise();
extern void	noport();
extern void	objdelete();
extern void	objinsert();
extern void	objreplace();
extern void	onemap();
extern void	ortho();
extern void	ortho2();
extern void	pagecolor();
extern void	pagewritemask();
extern void	passthrough();
extern void	patch();
extern void	patchbasis();
extern void	patchcurves();
extern void	patchprecision();
extern void	pclos();
extern void	pdr();
extern void	pdr2();
extern void	pdr2i();
extern void	pdr2s();
extern void	pdri();
extern void	pdrs();
extern void	perspective(Angle fovy_, float aspect, Coord near, Coord far);
extern void	pick();
extern void	picksize();
extern void	pmv();
extern void	pmv2();
extern void	pmv2i();
extern void	pmv2s();
extern void	pmvi();
extern void	pmvs();
extern void	pnt();
extern void	pnt2();
extern void	pnt2i();
extern void	pnt2s();
extern void	pnti();
extern void	pnts();
extern void	polarview();
extern void	polf();
extern void	polf2();
extern void	polf2i();
extern void	polf2s();
extern void	polfi();
extern void	polfs();
extern void	poly();
extern void	poly2();
extern void	poly2i();
extern void	poly2s();
extern void	polyi();
extern void	polys();
extern void	popattributes();
extern void	popmatrix();
extern void	popname();
extern void	popviewport();
extern void	prefposition();
extern void	prefsize();
extern void	pupcolor();
extern void	pupmode();
extern void	pushattributes();
extern void	pushmatrix();
extern void	pushname();
extern void	pushviewport();
extern void	qdevice();
extern void	qenter();
extern long	qread();
extern void	qreset();
extern long	qtest();
extern void	rcapture();
extern void	rcrv();
extern void	rcrvn();
extern void	rdr();
extern void	rdr2();
extern void	rdr2i();
extern void	rdr2s();
extern void	rdri();
extern void	rdrs();
extern long	readpixels();
extern long	readRGB();
extern void	rect();
extern void	rectcopy();
extern void	rectf();
extern void	rectfi();
extern void	rectfs();
extern void	recti();
extern void	rects();
extern void	resetls();
extern void	reshapeviewport();
extern void	RGBcolor();
extern void	RGBcursor();
extern void	RGBmode();
extern void	RGBwritemask();
extern void	ringbell();
extern void	rmv();
extern void	rmv2();
extern void	rmv2i();
extern void	rmv2s();
extern void	rmvi();
extern void	rmvs();
extern void	rot();
extern void	rotate(Angle angle, unsigned char axis);
extern void	rpatch();
extern void	rpdr();
extern void	rpdr2();
extern void	rpdr2i();
extern void	rpdr2s();
extern void	rpdri();
extern void	rpdrs();
extern void	rpmv();
extern void	rpmv2();
extern void	rpmv2i();
extern void	rpmv2s();
extern void	rpmvi();
extern void	rpmvs();
extern void	scale();
extern void	screenspace();
extern void	scrmask();
extern void	select();
extern void	setbell();
extern void	setbutton();
extern void	setcursor();
extern void	setdblights();
extern void	setdepth();
extern Boolean	setfastcom();
extern void	setlinestyle();
extern void	setmap();
extern void	setmonitor();
extern void	setpattern();
extern void	setshade();
extern Boolean	setslowcom();
extern void	setvaluator();
extern void	shaderange();
extern void	singlebuffer();
extern void	spclos();
extern void	splf();
extern void	splf2();
extern void	splf2i();
extern void	splf2s();
extern void	splfi();
extern void	splfs();
extern void	stepunit();
extern long	strwidth();
extern void	swapbuffers();
extern void	swapinterval();
extern void	textcolor();
extern void	textinit();
extern void	textport();
extern void	textwritemask();
extern void	tie();
extern void	tpoff();
extern void	tpon();
extern void	translate(Coord x, Coord y, Coord z);
extern void	unqdevice();
extern void	viewport(Screencoord left, Screencoord right, Screencoord bottom, Screencoord top);
extern long	winattach();
extern void	winclose();
extern void	winconstraints();
extern void	window(Coord left, Coord right, Coord bottom, Coord top, Coord near, Coord far);
extern long	winget();
extern void	winmove();
extern long	winopen();
extern void	winpop();
extern void	winposition();
extern void	winpush();
extern void	winreshape();
extern long	winset();
extern void	wintitle();
extern void	writemask();
extern void	writepixels();
extern void	writeRGB();
extern void	xfpt();
extern void	xfpt2();
extern void	xfpt2i();
extern void	xfpt2s();
extern void	xfpt4();
extern void	xfpt4i();
extern void	xfpt4s();
extern void	xfpti();
extern void	xfpts();
extern void	zbuffer();
extern void	zclear();

extern void glcompat(long mode, long value);
extern void shademodel(long mode);
extern void c3f(float c[3]);
extern void n3f(float n[3]);
extern void v3f(float v[3]);
extern void bgnpolygon();
extern void endpolygon();
extern void mmode(long mode);

// Added by LK because demos needed it. I don't know what the real values should be.
#define FLAT 0
#define GLC_OLDPOLYGON 0
#define WINQUIT 0

// Lighting:
#define LMNULL 0
#define AMBIENT 1
#define DIFFUSE 2
#define SPECULAR 3
#define SHININESS 4
#define LOCALVIEWER 5
#define LCOLOR 6
#define POSITION 7

// lmdef():
#define DEFMATERIAL 1
#define DEFLIGHT 2
#define DEFLMODEL 3

// lmbind():
#define LIGHT0 0
#define LIGHT1 1
#define MATERIAL 2
#define LMODEL 3

// mmode():
#define MSINGLE 0
#define MPROJECTION 1
#define MVIEWING 2

#endif // GLDEF
