#include "Renderer.h"
#include <stdlib.h>
#include <stdio.h>
int main (int argc, char **argv) {
	unsigned int w = 1920;
	unsigned int h = 1080;
	bool fullscreen = true;
	for (int i = 0;i < argc;i++) {
		if (argv[i][0] == '-'){
			switch(argv[i][1]) {
			case 'f' :
				fullscreen = false;
				break;
			case 'h' :
				h = atoi(&argv[i][2]);
				if (h==0) {
					puts("Invalid height(-h) parameter.");
				}
				break;
			case 'w' :
				w = atoi(&argv[i][2]);
				if (w==0) {
					puts("Invalid width(-w) parameter.");
				}
				break;
			}
		}
	}


	Renderer renderer = Renderer();
	renderer.init(w,h,fullscreen);
	renderer.loop();
    return 0;
}