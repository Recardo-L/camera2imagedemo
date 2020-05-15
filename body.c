#include "video.h"
void CutArgv(char *param,char *argv,const int pos){
	int n = pos;int idx;
	int slen = strlen(argv);
	while(n--){ *argv++; }
	// printf("*debug*\n");
	while((*param++ = *argv++)!= '\0');
	for(idx=0;idx<slen-pos;idx++){
		param[idx] = argv[idx+pos];
	}
	// printf("*debug*\n");
}
int main(int argc, char* argv[]){
	char CameraDev[64]={0x00}, FileName[64]={0x00},SW[16]={0x00},SH[16]={0x00},SN[16]={0x00};
	int FrameNumber=0, FrameWidth=0, FrameHeight=0;
	int n = argc;
	while(n-- > 0){
		/*
		if(CheckArgv(argv[n],"camera=",7) || CheckArgv(argv[n],"Camera=",7) || CheckArgv(argv[n],"CAMERA=",7)){
			CutArgv(CameraDev,argv[n],7);
			continue;
		}
		if(CheckArgv(argv[n],"filename=",9) || CheckArgv(argv[n],"FileName=",9) || CheckArgv(argv[n],"FILENAME=",9)){
			CutArgv(FileName,argv[n],9);
			continue;
		}
		if(CheckArgv(argv[n],"width=",6) || CheckArgv(argv[n],"Width=",6) || CheckArgv(argv[n],"WIDTH=",6)){
			CutArgv(SW,argv[n],6);
			FrameWidth = atoi(SW);
			continue;
		}
		if(CheckArgv(argv[n],"height=",7) || CheckArgv(argv[n],"Height=",7) || CheckArgv(argv[n],"HEIGHT=",7)){
			CutArgv(SH,argv[n],7);
			FrameHeight = atoi(SH);
			continue;
		}
		if(CheckArgv(argv[n],"framenumber=",12) || CheckArgv(argv[n],"FrameNumber=",12) || CheckArgv(argv[n],"FRAMENUMBER=",12)){
			CutArgv(SN,argv[n],12);
			FrameNumber = atoi(SN);
			continue;
		}
		*/
		// printf("%s\n",argv[n]);
		if(!strncmp(argv[n],"camera=",7) || !strncmp(argv[n],"Camera=",7) || !strncmp(argv[n],"CAMERA=",7)){
			CutArgv(CameraDev,argv[n],7);
			continue;
		}else if(!strncmp(argv[n],"c=",2) || !strncmp(argv[n],"C=",2)){
			CutArgv(CameraDev,argv[n],2);
			continue;
		}
		// ----------
		if(!strncmp(argv[n],"filename=",9) || !strncmp(argv[n],"FileName=",9) || !strncmp(argv[n],"FILENAME=",9)){
			CutArgv(FileName,argv[n],9);
			continue;
		}else if(!strncmp(argv[n],"f=",2) || !strncmp(argv[n],"F=",2)){
			CutArgv(FileName,argv[n],2);
			continue;
		}
		// -----------
		if(!strncmp(argv[n],"width=",6) || !strncmp(argv[n],"Width=",6) || !strncmp(argv[n],"WIDTH=",6)){
			CutArgv(SW,argv[n],6);
			FrameWidth = atoi(SW);
			continue;
		}else if(!strncmp(argv[n],"w=",2) || !strncmp(argv[n],"W=",2)){
			CutArgv(SW,argv[n],2);
			FrameWidth = atoi(SW);
			continue;
		}
		// ----------
		if(!strncmp(argv[n],"height=",7) || !strncmp(argv[n],"Height=",7) || !strncmp(argv[n],"HEIGHT=",7)){
			CutArgv(SH,argv[n],7);
			FrameHeight = atoi(SH);
			continue;
		}else if(!strncmp(argv[n],"h=",2) || !strncmp(argv[n],"H=",2)){
			CutArgv(SH,argv[n],2);
			//printf(">>%s\n",SH);
			FrameHeight = atoi(SH);
			continue;
		}
		// -----------
		if(!strncmp(argv[n],"framenumber=",12) || !strncmp(argv[n],"FrameNumber=",12) || !strncmp(argv[n],"FRAMENUMBER=",12)){
			CutArgv(SN,argv[n],12);
			FrameNumber = atoi(SN);
			continue;
		}else if(!strncmp(argv[n],"n=",2) || !strncmp(argv[n],"N=",2)){
			CutArgv(SN,argv[n],2);
			FrameNumber = atoi(SN);
			continue;
		}
	}
	int fd = open(CameraDev, O_RDWR);
	// printf("Camera=%s\nFilename=%s\nSize=%d*%d\nFrameNumber=%d\n",CameraDev,FileName,FrameWidth,FrameHeight,FrameNumber);
	if(fd < 0 || FrameNumber < 0 || FrameWidth <= 0 || FrameHeight<= 0){
		printf("输入参数说明\n");
		printf("指定硬件设备:\n\tc=  or  C=  or  camera=  or  Camera=  or  CAMERA=\n");
		printf("指定储存文件名:\n\tf=  or  F=  or  filename=  or  FileName=  or  FILENAME=\n");
		printf("指定帧宽度:\n\tw=  or  W=  or  width=  or  Width=  or  WIDTH=\n****硬件不支持则设置无效****\n");
		printf("指定帧高度:\n\th=  or  H=  or  height=  or  Height=  or  HEIGHT=\n****硬件不支持则设置无效****\n");
		printf("指定缓冲帧数目:\n\tn=  or  N=  or  framenumber=  or  FrameNumber=  or  FRAMENUMBER=\n****设置数目建议低于10且大于5****\n");
		return -1;
	}else{
		printf("Camera=%s\nFilename=%s\nSize=%d*%d\nFrameNumber=%d\n",CameraDev,FileName,FrameWidth,FrameHeight,FrameNumber);
	}
	// char *filename = "0.raw";
	// printf("--->%d\n",fd);
	DisplayAllFormat(&fd);
	DisplayNowFormat(&fd);
	SetFormat(&fd,FrameWidth,FrameHeight);
	SaveFrame(&fd,FrameNumber,FileName);
	return 0;
}
