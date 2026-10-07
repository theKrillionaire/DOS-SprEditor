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

void redrawCurColor(int curColor) {
	_setcolor(curColor);
	_rectangle(_GFILLINTERIOR, 73, 0, 83, 10);
}

void redrawCanvas(char* spriteData) {
	int i = 0;
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

int main(int argc, char** argv) {
	unsigned long tick = 0;
    unsigned long lastTick = 0;
	
	FILE* spriteFile = NULL;
	
    int mouseX = 0;
    int mouseY = 0;
    int mouseButtons = 0;
	int mouseButtonsOld = 0;
	
	int curColor = 15;
	int curColorOld = 15;
    
	char* spriteData;
    
	int fi = 0;
	
	if(argc == 2) {
		spriteFile = fopen(argv[1], "r+");
		
		if(spriteFile == NULL) {
			spriteFile = fopen(argv[1], "w+");
		}
	} else {
		printf("Error! this program requires one(1) argument passed!!\n\nUsage: spredit file.spr\n");
		return 1;
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
	redrawCurColor(curColor);
	redrawCanvas(spriteData);
	
    while(1) {
		char buff[24];
		lastTick = tick;
		tick = *(unsigned long far *)MK_FP(0x40,0x6c);
		mouseButtonsOld = mouseButtons;
		updatemouse(&mouseX, &mouseY, &mouseButtons);
		
		if(mouseButtons == 1 && mouseButtonsOld != 1) {
				curColorOld = curColor;
				if(	mouseX >= 0 && mouseX <= 25 && mouseY >= 0 && mouseY <= 25 ) {           // start of first row
					curColor = 0;
				} else if( mouseX >= 0 && mouseX <= 25 && mouseY >= 25 && mouseY <= 50 ) {
					curColor = 1;
				} else if( mouseX >= 0 && mouseX <= 25 && mouseY >= 50 && mouseY <= 75 ) {
					curColor = 2;
				} else if( mouseX >= 0 && mouseX <= 25 && mouseY >=75 && mouseY <= 100 ) {
					curColor = 3;
				} else if( mouseX >= 0 && mouseX <= 25 && mouseY >= 100 && mouseY <= 125 ) {
					curColor = 4;
				} else if( mouseX >= 0 && mouseX <= 25 && mouseY >= 125 && mouseY <= 150 ) {
					curColor = 5;
				} else if( mouseX >= 0 && mouseX <= 25 && mouseY >= 150 && mouseY <= 175 ) {
					curColor = 6;
				} else if( mouseX >= 0 && mouseX <= 25 && mouseY >= 175 && mouseY <= 200 ) {
					curColor = 7;
				} else if( mouseX >= 25 && mouseX <= 50 && mouseY >= 0 && mouseY <= 25 ) {    // start of second row
					curColor = 8;
				} else if( mouseX >= 25 && mouseX <= 50 && mouseY >= 25 && mouseY <= 50 ) {
					curColor = 9;
				} else if( mouseX >= 25 && mouseX <= 50 && mouseY >= 50 && mouseY <= 75 ) {
					curColor = 10;
				} else if( mouseX >= 25 && mouseX <= 50 && mouseY >=75 && mouseY <= 100 ) {
					curColor = 11;
				} else if( mouseX >= 25 && mouseX <= 50 && mouseY >= 100 && mouseY <= 125 ) {
					curColor = 12;
				} else if( mouseX >= 25 && mouseX <= 50 && mouseY >= 125 && mouseY <= 150 ) {
					curColor = 13;
				} else if( mouseX >= 25 && mouseX <= 50 && mouseY >= 150 && mouseY <= 175 ) {
					curColor = 14;
				} else if( mouseX >= 25 && mouseX <= 50 && mouseY >= 175 && mouseY <= 200 ) {  // end of second row
					curColor = 15;
				}
				
				if(curColor != curColorOld) {
					redrawCurColor(curColor);
				}
				
			} else if (mouseButtons == 1) {
				if(mouseX > 87 && mouseX < 279 && mouseY > 3 && mouseY < 195) {
					int x2 = (mouseX - 87) / 12;
					int y2 = (mouseY - 3) / 12;
					
					int i2 = y2 * 16 + x2;
					
					spriteData[i2] = curColor;
					
					hidemouse();
					_setcolor(curColor);
					_rectangle( _GFILLINTERIOR,
						87 + x2 * 12,
						3 + y2 * 12,
						87 + x2 * 12 + 11	,
						3 + y2 * 12 + 11
					);
					showmouse();
				}
			}
		
		if(tick - lastTick >= 1) { 
			
			
			if(kbhit()) {
				int key = getch();
						
				switch (key) {
					case 'e':
					case 'E':
						_setvideomode(_DEFAULTMODE);
						printf("balls");
						return 0;
						break;
					case 'a':
					case 'A':
						_outtext("testicle");
				}
    		}
		}
	}
    _setvideomode(_DEFAULTMODE);
	
	fclose(spriteFile);
	free(spriteData);
	
	printf("spriteData %s, spriteFile %i", spriteData, spriteFile); 
	
    return 0;
}
