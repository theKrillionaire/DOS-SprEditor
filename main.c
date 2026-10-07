#include <stdio.h>
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
	
	for(x = 0; x < 16; x++) {
		int x2 = i % 16; 
		int y2 = i / 16;
		
		_setcolor(c);
		_rectangle( _GFILLINTERIOR,
			87 + x * 12,
			3	 + y * 12,
			87 + x * 12 + 12,
			3 + y * 12 + 12
		);
	}
	
	_setcolor(15);
	_rectangle(_GBORDER, 86, 2, 280, 196);
	
	showmouse();
}

void redrawCurColor(int curColor) {
	_setcolor(curColor);
	_rectangle(_GFILLINTERIOR, 73, 0, 83, 10);
}

int main() {
	unsigned long tick = 0;
    unsigned long lastTick = 0;

    int mouseX = 0;
    int mouseY = 0;
    int mouseButtons = 0;
	int mouseButtonsOld = 0;
	
	int curColor = 15;
	int curColorOld = 15;
    
    if(_setvideomode(_MRES16COLOR) == 0) {
        printf("Sorry! No available screen modes were found!");
    }

    if(!initmouse()) {
        _setvideomode(_DEFAULTMODE);
        printf("No mouse driver detected!");
        return 1;
    }
	
	setmousescreensize();
	showmouse();
	
	drawHUD();
	redrawCurColor(curColor);
	
    while(1) {
		char buff[24];
		lastTick = tick;
		tick = *(unsigned long far *)MK_FP(0x40,0x6c);
		
		if(tick - lastTick >= 1) { 
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
			}
			
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

    return 0;
}
