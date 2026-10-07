#include <stdio.h>
#include <stdlib.h>
#include <graph.h>
#include <dos.h>
#include <conio.h>

union REGS inregs, outregs;

int initmouse() {
    inregs.x.ax = 0;
    int86(0x33, &inregs, &outregs);
    return outregs.x.ax;  
}

void setmousescreensize() {
	inregs.x.ax = 7;
	inregs.x.dx = 639;
	inregs.x.cx = 0;
	int86(0x33, &inregs, &outregs);
	inregs.x.ax = 8;
	inregs.x.dx = 199;
	inregs.x.cx = 0;
	int86(0x33, &inregs, &outregs);
}

void updatemouse(int* x, int* y, int* buttons) {
    inregs.x.ax = 3;
    int86(0x33, &inregs, &outregs);
    *x = outregs.x.cx / 2;
    *y = outregs.x.dx;
    *buttons = outregs.x.bx;
}

void showmouse() {
    inregs.x.ax = 1;
    int86(0x33, &inregs, &outregs);    
}

void hidemouse() {
    inregs.x.ax = 2;
    int86(0x33, &inregs, &outregs);    
}

void drawHUD() {
	int x = 0;
	int y = 0;
	int c = 0;
	int i = 0;
	hidemouse();
	_clearscreen(_GCLEARSCREEN);
	
	for(x; x < 2; x++) {
		y = 0;
		for(y; y < 8; y++) {
			_setcolor(c);
			_rectangle(_GFILLINTERIOR, x * 25, y * 25, x * 25 + 25, y * 25 + 25);
			_setcolor(0);
			_rectangle(_GBORDER, x * 25, y * 25, x * 25 + 25, y * 25 + 25);
			c++;
		}
	}
	_setcolor(15);
	_rectangle(_GBORDER, 86, 2, 280, 196);
	
	showmouse();
}

void redrawCurColor(int curColor, int secColor) {
	_setcolor(curColor);
	_rectangle(_GFILLINTERIOR, 73, 0, 83, 10);
	
	_setcolor(secColor);
	_rectangle(_GFILLINTERIOR, 73, 12, 83, 22);
}

void redrawCanvas(char* spriteData) {
	int i = 0;
	hidemouse();
	for(i = 0; i < 256; i++) {
		int x2 = i % 16; 
		int y2 = i / 16;
		
		_setcolor(spriteData[i]);
		_rectangle( _GFILLINTERIOR,
			87 + x2 * 12,
			3	 + y2 * 12,
			87 + x2 * 12 + 12	,
			3 + y2 * 12 + 12
		);
	}
	showmouse();
}

void writeToFile(FILE* writeFile, char* spriteData) {
	int i = 0;
	rewind(writeFile);
	for(i; i < 256; i++) {
		fputc(spriteData[i], writeFile	);
	}
	_settextposition(25, 10);
	_settextcolor(8);
	_outtext("Saved sprite!");
}

void drawCanvasPixel(int x, int y, int color) {
	
	hidemouse();
	_setcolor(color);
	_rectangle( _GFILLINTERIOR,
		87 + x * 12,
		3 + y * 12,
		87 + x * 12 + 11	,
		3 + y * 12 + 11
	);
	showmouse();
}

void changeSetColor(int* curColor, int mouseX, int mouseY) {
	int curColorOld = *curColor;
	int mouseFirstColumn = mouseX >= 0 && mouseX <= 25;
	int mouseSecondColumn = mouseX >= 25 && mouseX <= 50;
	int y3 = mouseY / 25;
	curColorOld = *curColor;
	if(mouseFirstColumn) {
		*curColor = y3;
	} else if (mouseSecondColumn) {
		*curColor = y3 + 8;
	}
}

int main(int argc, char** argv) {
	unsigned long tick = 0;
    unsigned long lastTick = 0;
	
	FILE* spriteFile = NULL;
	FILE* writeFile = NULL;
	
    int mouseX = 0;
    int mouseY = 0;
    int mouseButtons = 0;
	int mouseButtonsOld = 0;
	
	int curColor = 15;
	int curColorOld = 15;
	int secColor = 2;
	int secColorOld = 2;
    
	char* spriteData;
    
	int fi = 0;
	
	if(argc == 2) {
		spriteFile = fopen(argv[1], "r+");
		writeFile = spriteFile;
	} else if(argc == 3) {
		spriteFile = fopen(argv[1], "r");
		writeFile = fopen(argv[2], "r+");	
	} else {
		printf("Error! this program requires one(1) or two(2) arguments passed!!\n\nUsage: \"SPREDIT FILE.SPR\" will open or create a file.\nUsage: \"SPREDIT OPEN.SPR SAVE.SPR\" will open from OPEN.SPR, and save into SAVE.SPR.\n");
		return 1;
	}
	if(spriteFile == NULL) {
		if(argc == 2) {
			spriteFile = fopen(argv[1], "w+");
			writeFile = spriteFile;
				if(spriteFile == NULL) {
					perror("Mysterious file error detected!");
					return 1;
				}
		} else if (argc == 3) {
			spriteFile = fopen(argv[1], "w+");
			writeFile = fopen(argv[2], "w+");
				if(spriteFile == NULL) {
					perror("Mysterious file error detected!");
					return 1;
				}
		}
	} else if(writeFile == NULL) {
		if(argc == 2) {
			spriteFile = fopen(argv[1], "w+");
			writeFile = spriteFile;
				if(spriteFile == NULL) {
					perror("Mysterious file error detected!");
					return 1;
				}
		} else if (argc == 3) {
			spriteFile = fopen(argv[1], "w+");
			writeFile = fopen(argv[2], "w+");
				if(spriteFile == NULL) {
					perror("Mysterious file error detected!");
					return 1;
				}
		}
	}
	
    if(_setvideomode(_MRES16COLOR) == 0) {
        printf("Sorry! No available screen modes were found!");
		_setvideomode(_DEFAULTMODE);
		return 1;
    }

    if(!initmouse()) {
        _setvideomode(_DEFAULTMODE);
        printf("No mouse driver detected!");
        return 1;
    }
	
	spriteData = malloc(sizeof(char) * 256);
	
	if(spriteData == NULL) {
		_setvideomode(_DEFAULTMODE);
		fclose(spriteFile);
		fclose(writeFile);
		free(spriteData);	
		printf("error! malloc refused!!");
		return 1;	
	}
	
	for(fi; fi < 256; fi++) {
		int data = fgetc(spriteFile);
		if(data != EOF) {
			spriteData[fi] = data;
		} else {
			spriteData[fi] = 0;
		}
	}
	
	setmousescreensize();
	showmouse();
	
	drawHUD();
	redrawCurColor(curColor, secColor);
	redrawCanvas(spriteData);
	
    while(1) {
		char buff[24];
		lastTick = tick;
		tick = *(unsigned long far *)MK_FP(0x40,0x6c);
		mouseButtonsOld = mouseButtons;
		updatemouse(&mouseX, &mouseY, &mouseButtons);

		if (mouseButtons == 1) {
			if(mouseX > 87 && mouseX < 279 && mouseY > 3 && mouseY < 195) {
				int x2 = (mouseX - 87) / 12;
				int y2 = (mouseY - 3) / 12;
					
				int i2 = y2 * 16 + x2;
				drawCanvasPixel(x2,y2, curColor);
				spriteData[i2] = curColor;
			} else if(mouseX <= 50) {
				curColorOld = curColor;
				changeSetColor(&curColor, mouseX, mouseY);
				if(curColor != curColorOld) {
					redrawCurColor(curColor, secColor);
				}
			}
		} else if (mouseButtons == 2) {
			if(mouseX > 87 && mouseX < 279 && mouseY > 3 && mouseY < 195) {
				int x2 = (mouseX - 87) / 12;
				int y2 = (mouseY - 3) / 12;
				
				int i2 = y2 * 16 + x2;
				drawCanvasPixel(x2,y2, secColor);
				spriteData[i2] = secColor;
			}
			if(mouseX <= 50 && mouseButtonsOld != 2) {
				secColorOld = secColor;
				changeSetColor(&secColor, mouseX, mouseY);
				if(secColor != secColorOld) {
					redrawCurColor(curColor, secColor);
				}
			}
		}else if (mouseButtons == 4 && mouseButtonsOld != 4) {
			int x3 = 0;
			int y3 = 0;
			for(x3; x3 < 16; x3++) {
				y3 = 0;
				for(y3; y3 < 16; y3++) {
					if((x3 + y3) % 2) spriteData[y3 * 16 + x3] = curColor;
					else spriteData[y3 * 16 + x3] = secColor;
					//i++;
				}
			}
			redrawCanvas(spriteData);
		}
		
		if(tick - lastTick >= 1) { 
			
			
			if(kbhit()) {
				int key = getch();
						
				switch (key) {
					case 'e':
					case 'E':
						hidemouse();
						_setvideomode(_DEFAULTMODE);
						fclose(spriteFile);
						fclose(writeFile);
						free(spriteData);
						printf("balls");
						return 0;
						break;
					case 'a':
					case 'A':
						_outtext("testicle");
					case 's':
					case 'S':
						writeToFile(writeFile, spriteData);
						break;
				}
    		}
		}
	}
	
	hidemouse();
    _setvideomode(_DEFAULTMODE);
	
	fclose(spriteFile);
	fclose(writeFile);
	free(spriteData);	
	
    return 0;
}
