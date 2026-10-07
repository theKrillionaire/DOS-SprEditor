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

void redrawScreen() {
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
		for(y = 0; y < 16; y++) {
			//int x2 = i % 16; 
			//int y2 = i / 16;
			if(c > 14) { c = 0; }
			
			_setcolor(c);
			_rectangle( _GFILLINTERIOR,
				87 + x * 12,
				3	 + y * 12,
				87 + x * 12 + 12,
				3 + y * 12 + 12
			);
			c++;
		}
	}
	
	_setcolor(15);
	_rectangle(_GBORDER, 86, 2, 280, 196);
	
	showmouse();
}

int main() {
	unsigned long tick = 0;
    unsigned long lastTick = 0;

    int mouseX = 0;
    int mouseY = 0;
    int mouseButtons = 0;
    
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
	
	redrawScreen();
	
    while(1) {
		lastTick = tick;
		tick = *(unsigned long far *)MK_FP(0x40,0x6c);
		
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

    return 0;
}
