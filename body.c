#include "video.h"

void main(int argc, char* argv[]){
	int fd = open("/dev/video0", O_RDWR);
	
	DisplayAllFormat(&fd);
	DisplayNowFormat(&fd);
	// SetNowFormat(&fd,600,400);
	SaveFrame(&fd,5);
}
