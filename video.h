#ifndef __VIDEO_H__
#define __VIDEO_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <sys/time.h>
#include <sys/ioctl.h>
//#include <linux/types.h>
#include <sys/mman.h>
 
#include <bits/types/struct_timespec.h>
#include <bits/types/struct_timeval.h>
 
#include <linux/videodev2.h>

void DisplayAllFormat(int *DeviceID){
	struct v4l2_fmtdesc fmtdesc;
	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	printf("全部支持格式:\n");
	while (ioctl(*DeviceID, VIDIOC_ENUM_FMT, &fmtdesc) != -1)

	{
		printf("\t%d.%s\n", fmtdesc.index + 1, fmtdesc.description);
		fmtdesc.index++;
	}
	printf("读取完成\n");
}

void DisplayNowFormat(int *DeviceID){
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(*DeviceID, VIDIOC_G_FMT, &fmt);

	printf("当前格式:\n\twidth=%d px\n\theight=%d px\n", fmt.fmt.pix.width, fmt.fmt.pix.height);

	struct v4l2_fmtdesc fmtdesc2;
	fmtdesc2.index = 0;
	fmtdesc2.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	while (ioctl(*DeviceID, VIDIOC_ENUM_FMT, &fmtdesc2) != -1)
	{
		if (fmtdesc2.pixelformat & fmt.fmt.pix.pixelformat)
		{
			printf("\t%d.%s\n", fmtdesc2.index + 1,fmtdesc2.description);
			break;
		}
		fmtdesc2.index++;
	}
}

int SetFormat(int *DeviceID,int Width,int Height){
	//设置视频格式
	//如果摄像头不支持我们设置的分辨率格式，则 fmt.fmt.pix.width 会被修改，所以此处建议再次检查 fmt.fmt.pix. 的各种信息
	struct v4l2_format fmt;
	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = Width;
	fmt.fmt.pix.height = Height;
	fmt.fmt.pix.pixelformat = 0;
	fmt.fmt.pix.field = V4L2_FIELD_ANY;
	//设置设备捕获视频的格式 
	if (ioctl(*DeviceID, VIDIOC_S_FMT, &fmt) < 0)
	{
		printf("✖错误:设置视频格式失败\n");
		close(*DeviceID);
		return -1;
	}
	return 0;
}
int SaveFrame(int *DeviceID,int FrameNumber){
	//向驱动申请帧缓存
	int CAP_BUF_NUM = FrameNumber;
	struct v4l2_requestbuffers req;
	memset(&req, 0, sizeof(req));
	req.count = CAP_BUF_NUM;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (ioctl(*DeviceID, VIDIOC_REQBUFS, &req) < 0)
	{
		if (EINVAL == errno)
		{
			printf("✖错误:不支持内存映射\n");
			close(*DeviceID);
			return -2;
		}
		else
		{
			printf("✖错误:内存映射错误,未知原因\n");
			close(*DeviceID);
			return -3;
		}
	}
	else
	{
		printf("内存映射成功\n");
	}
	if (req.count < CAP_BUF_NUM)
	{
		printf("✖错误:缓冲区内存不足\n");
		close(*DeviceID);
		return -4;
	}
	else
	{
		printf("获取缓冲帧数=%d\n", req.count);
	}

	//将帧缓存与本地内存关联
	typedef struct VideoBuffer {   //定义一个结构体来映射每个缓冲帧
		void* start;
		size_t length;
	} VideoBuffer;
	VideoBuffer* buffers = calloc(req.count, sizeof(*buffers));
	struct v4l2_buffer buf;
	for (int numBufs = 0; numBufs < req.count; numBufs++) {//映射所有的缓存
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = numBufs;
		if (ioctl(*DeviceID, VIDIOC_QUERYBUF, &buf) == -1) {//获取到对应index的缓存信息，此处主要利用length信息及offset信息来完成后面的mmap操作。
			printf("✖错误:未知错误,错误ID=%d\n", numBufs);
			free(buffers);
			close(*DeviceID);
			return -5;
		}
		buffers[numBufs].length = buf.length;
		// 转换成相对地址
		buffers[numBufs].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, *DeviceID, buf.m.offset); // #include <sys/mman.h>
		if (buffers[numBufs].start == MAP_FAILED) {
			printf("✖错误:%d映射失败错误,错误ID=%d\n", numBufs, errno);
			free(buffers);
			close(*DeviceID);
			return -6;
		}
		//把缓冲帧加入缓冲队列
		if (ioctl(*DeviceID, VIDIOC_QBUF, &buf) < 0)
		{
			printf("✖错误:BUFFER添加到队列失败%d\n", numBufs);
			free(buffers);
			close(*DeviceID);
			return -7;
		}
	}
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	/* 打开设备视频流 */
	if (ioctl(*DeviceID, VIDIOC_STREAMON, &type) < 0)
	{
		printf("✖错误:数据流打开失败\n");
		free(buffers);
		close(*DeviceID);
		return 0;
	}
	int franeCount = FrameNumber;
	while (franeCount--)
	{
		struct v4l2_buffer capture_buf;
		memset(&capture_buf, 0, sizeof(capture_buf));
		capture_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		capture_buf.memory = V4L2_MEMORY_MMAP;
		/* 将已经捕获好视频的内存拉出已捕获视频的队列 */
		if (ioctl(*DeviceID, VIDIOC_DQBUF, &capture_buf) < 0)
		{
			printf("✖错误:获取缓冲帧%d失败\n", franeCount);
			break;
		}
		else
		{
			char fname[18] = "0.jpeg";
			fname[0] = 48 + franeCount;
			printf("----------*****debug*****----------\n");
			{
				FILE* f = fopen(fname, "ab");
				int wt = fwrite(buffers[capture_buf.index].start, 1, buffers[capture_buf.index].length, f);
				printf("数据流Size=%d\n", wt);
				fclose(f);
			}
			printf("获取缓冲帧%d成功\n", franeCount);
			//把用完的帧重新插回队列
			if (ioctl(*DeviceID, VIDIOC_QBUF, &capture_buf) == -1) {
				printf("✖错误:缓冲帧%d插回队列失败\n", franeCount);
				break;
			}
		}
	}

	//清理资源
	int ret = ioctl(*DeviceID, VIDIOC_STREAMOFF, &type);
	for (int i = 0; i < CAP_BUF_NUM; i++)
	{
		munmap(buffers[i].start, buffers[i].length);
	}
	free(buffers);
	close(*DeviceID);
	return 0;
}
#endif
