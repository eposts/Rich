
#include <stdio.h>


#define	VERTICAL		480		
#define	HORIZONTAL		640		

#define	BUFSIZE			38400	

#define	ARC_RES			64		

#define	PBYTES			11		


#define	LINE			0x01	
#define	BOX				0x02	
#define	CIRCLE			0x03	
#define	TEXT			0x04	
#define	ARC				0x05	
#define	GROUP			0x06	
#define	ACOPY			0x07	
#define	RCOPY			0x08	


unsigned char drawing[32000];
unsigned dpos;


char font[4608], dfile[65], ffile[65], *device = "LPT1";
int vbseg, printer = -1;


char *printers[] = {
	"LASERJET",
	"EPSON" }


char xyorder, xyorders[] = {
	-1,		
	0 }		


unsigned sine[] = {
	    0,  1608,  3216,  4821,  6424,  8022,  9616, 11204,
	12785, 14359, 15924, 17479, 19024, 20557, 22078, 23586,
	25079, 26557, 28020, 29465, 30893, 32302, 33692, 35062,
	36410, 37736, 39040, 40320, 41575, 42806, 44011, 45190,
	46341, 47464, 48559, 49624, 50660, 51665, 52639, 53581,
	54491, 55368, 56212, 57022, 57797, 58538, 59243, 59913,
	60547, 61144, 61705, 62228, 62714, 63162, 63571, 63943,
	64276, 64571, 64826, 65043, 65220, 65358, 65457, 65516 };

main(argc, argv)
	int argc;
	char *argv[];
{
	unsigned i, j;
	char *ptr;
	FILE *fp, *pfp;

	
	*dfile = 0;
	concat(ffile, "MICROCAD.FNT");

	
	for(i=1; i < argc; ++i) {
		ptr = argv[i];
		switch((toupper(*ptr++) << 8) | toupper(*ptr++)) {
			case 'F=' :		
				concat(ffile, ptr, ".FNT");
				break;
			case 'D=' :		
				device = ptr;
				break;
			case '?' << 8:	
			case '/?' :
			case '-?' :
				goto help;
			default:		
				if(!*dfile) {	
					concat(dfile, argv[i], ".DWG");
					break; }
				for(ptr = argv[i]; *ptr; ++ptr)	
					*ptr = toupper(*ptr);
				for(j=0; j < (sizeof(printers)* Lookup name */
					if(strbeg(printers[j], argv[i])) {
						if(printer != -1) {
							printf("Ambiguous printer name!\n");
							goto help; }
						printer = j; } }
				if(printer == -1) {
					printf("Unknown printer name!\n");
					goto help; } } }

	
	if((printer == -1) || !*dfile) {
		printf("Drawing name and printer type required!\n");
	help:
		printf("\nUse: MCPRINT <drawing file> <printer type> [F=font file] [D=device]\n");
		printf("\nCopyright 1992-1993 Dave Dunfield\nAll rights reserved.\n\n");
		printf("Available printers are:\n");
		for(j=0; j < (sizeof(printers)/2); ++j)
			printf("   %s\n", printers[j]);
		exit(-1); }

	
	fp = fopen(ffile, "rbvq");
	fread(font, sizeof(font), fp);
	fclose(fp);

	
	fp = fopen(dfile, "rbvq");
	fread(drawing, PBYTES, fp);
	i = fread(drawing, sizeof(drawing), fp);
	fclose(fp);

	
	while(i < sizeof(drawing))
		drawing[i++] = 0;

	
	set_raw(pfp = fopen(device, "wbvq"));

	
	printf("Initalize... ");
	if(!(vbseg = alloc_seg((BUFSIZE+15)/16)))
		abort("Not enough memory!");

	
	for(i=0; i < BUFSIZE; ++i)
		poke(vbseg, i, 0);

	
	printf("Draw objects... ");
	xyorder = xyorders[printer];
	dpos = 0;
	while(!draw_object(0, 0));

	printf("Print... ");

	
	switch(printer) {
		case 0 :		
			fprintf(pfp,"\x1B*t75R");			
			fprintf(pfp,"\x1B*r0A");			
			for(dpos=i=0; i < HORIZONTAL; ++i) {
				fprintf(pfp,"\x1B*b60W");		
				for(j=0; j < (VERTICAL/8); ++j)
					putc(peek(vbseg, dpos++), pfp); }
			fprintf(pfp,"\x1B*rB\f");			
			break;
		case 1 :		
			for(i=0; i < (HORIZONTAL/8); ++i) {
				fprintf(pfp,"\x1BK\xE0\x01");	
				for(j=0; j < BUFSIZE; j += (HORIZONTAL/8))
					putc(peek(vbseg, i + j), pfp);
				fprintf(pfp,"\r\x1BJ\x18"); }	
		}
	
	
	fclose(pfp);
	printf("Done.");
}


draw_object(xoffset, yoffset)
	int xoffset, yoffset;
{
	int x, y, i, j, k, l;
	char buffer[80], *ptr;

	i = drawing[dpos++];
	x = dvalue() + xoffset;
	y = dvalue() + yoffset;

	switch(i) {
		case LINE :
			line(x, y, x+dvalue(), y+dvalue());
			break;
		case BOX :
			box(x, y, x+dvalue(), y+dvalue());
			break;
		case CIRCLE :
			circle(x, y, dvalue());
			break;
		case TEXT :
			i = dvalue();
			ptr = buffer;
			do
				*ptr++ = j = drawing[dpos++];
			while(j);
			text(buffer, x, y, i);
			break;
		case ARC:
			arc(x, y, dvalue(), dvalue(), dvalue());
			break;
		case GROUP :
			i = dvalue();
			j = dpos;
			while((dpos - j) < i)
				draw_object(x, y);
			break;
		case RCOPY :
			i = dpos - 5;
			i += dvalue();
			goto gocopy;
		case ACOPY :
			i = dvalue();
		gocopy:
			j = dpos;
			dpos = i+1;
			k = dvalue();
			l = dvalue();
			dpos = i;
			draw_object(x - k, y - l);
			dpos = j;
			break;
		default:
			abort("Corrupt drawing file!");
		case 0 :
			return -1; }
	return 0;
}


line(x1, y1, x2, y2)
	int x1, y1, x2, y2;
{
	int i, w, h;
	
	if((w = abs(x1 - x2)) >= (h = abs(y1 - y2))) {
		if(x1 > x2) {
			i = x1;
			x1 = x2;
			x2 = i;
			i = y1;
			y1 = y2;
			y2 = i; }
		if(y1 < y2) {
			for(i=0; i < w; ++i)
				set_pixel(x1+i, y1+scale(i, h, w)); }
		else {
			for(i=0; i < w; ++i)
				set_pixel(x1+i, y1-scale(i, h, w)); } }
	
	else {
		if(y1 > y2) {
			i = x1;
			x1 = x2;
			x2 = i;
			i = y1;
			y1 = y2;
			y2 = i; }
		if(x1 < x2) {
			for(i=0; i < h; ++i)
				set_pixel(x1+scale(i, w, h), y1+i); }
		else {
			for(i=0; i < h; ++i)
				set_pixel(x1-scale(i, w, h), y1+i); } }

	set_pixel(x2, y2);
}


box(x1, y1, x2, y2)
	int x1, y1, x2, y2;
{
	line(x1, y1, x2, y1);		
	line(x1, y1+1, x1, y2-1);	
	line(x2, y1+1, x2, y2-1);	
	line(x1, y2, x2, y2);		
}

circle(x, y, r)
	int x, y, r;
{
	int i, j, k, rs, lj;

	rs = (lj = r)*r;
	for(i=0; i <= r; ++i) {
		j = k = sqrt(rs - (i*i));
		do {
			set_pixel(x+i, y+j);
			set_pixel(x+i, y-j);
			set_pixel(x-i, y+j);
			set_pixel(x-i, y-j); }
		while(++j < lj);
		lj = k; }
}


arc(x, y, r, v1, v2)
	int x, y, r;
	unsigned char v1, v2;
{
	int rs, i, j, ax, x1, y1, x2, y2;

	x2 = -1;
	rs = r*r;

	do {
		j = (ARC_RES-1) - (i = v1 & (ARC_RES-1));
		switch(v1 & (ARC_RES*3)) {
			case ARC_RES*0 :	
				x1 = x + (ax = scale(r, sine[i], -1));
				y1 = y - sqrt(rs - (ax*ax));
				break;
			case ARC_RES*1 :	
				x1 = x + (ax = scale(r, sine[j], -1));
				y1 = y + sqrt(rs - (ax*ax));
				break;
			case ARC_RES*2 :	
				x1 = x - (ax = scale(r, sine[i], -1));
				y1 = y + sqrt(rs - (ax*ax));
				break;
			case ARC_RES*3 :	
				x1 = x - (ax = scale(r, sine[j], -1));
				y1 = y - sqrt(rs - (ax*ax)); }
		if(x2 != -1)
			line(x2, y2, x1, y1);
		x2 = x1;
		y2 = y1; }
	while(v1++ != v2);
}


text(string, x, y, s)
	char *string;
	int x, y, s;
{
	unsigned i, j, b;
	unsigned char *ptr;

	y -= scale(24, s, 100);
	while(*string) {
		ptr = &font[(*string++ - ' ') * 48];

		for(i=0; i < 24; ++i) {
			b = (*ptr++ << 8) | *ptr++;
			for(j=0; j < 16; ++j) {
				if(b & 0x8000)
					set_pixel(x+scale(j,s,100), y+scale(i,s,100));
				b <<= 1; } }
		x += scale(18, s, 100); }
}


dvalue()
{
	return (drawing[dpos++] << 8) | drawing[dpos++];
}


set_pixel(x, y)
	unsigned x, y;
{
	unsigned byte;

	if((x < HORIZONTAL) && (y < VERTICAL)) {
		y = (VERTICAL-1) - y;	

		if(xyorder) {	
			byte = (x * (VERTICAL 8);
			poke(vbseg, byte, peek(vbseg, byte) | (0x80 >> (y % 8))); }
		else {			
			byte = (y * (HORIZONTAL 8);
			poke(vbseg, byte, peek(vbseg, byte) | (0x80 >> (x % 8))); } }
}


scale(value, mul, div) asm
{
		MOV		AX,8[BP]		; Get value
		MUL		WORD PTR 6[BP]	; Multiply to 32 bit product
		MOV		BX,4[BP]		; Get divisor
		DIV		BX				; Divide back to 16 bit result
		SHR		BX,1			; /2 for test
		JZ		scale1			; Special case (/1)
		INC		DX				; .5 rounds up
		SUB		BX,DX			; Set 'C' if remainder > half
		ADC		AX,0			; Increment result to scale
scale1:
}


set_raw(fp) asm
{
		MOV		BX,4[BP]		; Get file pointer
		MOV		AX,4400h		; IOCTL:Get info
		MOV		BX,1[BX]		; Get DOS file handle
		INT		21h				; Ask DOS
		TEST	DL,80h			; Device?
		JZ		setr1			; Not a device...
		XOR		DH,DH			; Force DH=0
		OR		DL,20h			; Set RAM mode
		MOV		AX,4401h		; IOCTL:Set info
		INT		21h				; Ask DOS
setr1:
}
