#include "Renderer.h"

int main (int argc, char **argv) {
	Renderer renderer = Renderer();
	renderer.init(1920,1080);
	renderer.loop();
    return 0;
}