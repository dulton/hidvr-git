//RGB to YUV420 原代码：  RGB2YUV.CPP文件

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//转换矩阵
#define MY(a,b,c) (( a*  0.2989  + b*  0.5866  + c*  0.1145))
#define MU(a,b,c) (( a*(-0.1688) + b*(-0.3312) + c*  0.5000 + 128))
#define MV(a,b,c) (( a*  0.5000  + b*(-0.4184) + c*(-0.0816) + 128))
//大小判断
#define DY(a,b,c) (MY(a,b,c) > 255 ? 255 : (MY(a,b,c) < 0 ? 0 : MY(a,b,c)))
#define DU(a,b,c) (MU(a,b,c) > 255 ? 255 : (MU(a,b,c) < 0 ? 0 : MU(a,b,c)))
#define DV(a,b,c) (MV(a,b,c) > 255 ? 255 : (MV(a,b,c) < 0 ? 0 : MV(a,b,c)))
//只处理352*288文件
#define WIDTH 704
#define HEIGHT 480

//读BMP
void ReadBmp(unsigned char *RGB,FILE *fp)
{
    int i,j;
    unsigned char temp;

    fseek(fp,54, SEEK_SET);

    fread(RGB+WIDTH*HEIGHT*3, 1, WIDTH*HEIGHT*3, fp);//读取
    for(i=HEIGHT-1,j=0; i>=0; i--,j++)//调整顺序
    {
        memcpy(RGB+j*WIDTH*3,RGB+WIDTH*HEIGHT*3+i*WIDTH*3,WIDTH*3);
    }
    
    //顺序调整
    for(i=0; (unsigned int)i < WIDTH*HEIGHT*3; i+=3)
    {
        temp = RGB[i];
        RGB[i] = RGB[i+2];
        RGB[i+2] = temp;
    }
}

//转换函数
void Convert(unsigned char *RGB, unsigned char *YUV)
{
    //变量声明
    unsigned int i,x,y,j;
    unsigned char *Y = NULL;
    unsigned char *U = NULL;
    unsigned char *V = NULL;
    
    Y = YUV;
    U = YUV + WIDTH*HEIGHT;
    V = U + ((WIDTH*HEIGHT)>>2);

    for(y=0; y < HEIGHT; y++){
        for(x=0; x < WIDTH; x++)
        {
            j = y*WIDTH + x;
            i = j*3;
            Y[j] = (unsigned char)(DY(RGB[i], RGB[i+1], RGB[i+2]));

            if(x%2 == 1 && y%2 == 1)
            {
                j = (WIDTH>>1) * (y>>1) + (x>>1);
                //上面i仍有效
                U[j] = (unsigned char)
                       ((DU(RGB[i  ], RGB[i+1], RGB[i+2]) + 
                         DU(RGB[i-3], RGB[i-2], RGB[i-1]) +
                         DU(RGB[i  -WIDTH*3], RGB[i+1-WIDTH*3], RGB[i+2-WIDTH*3]) +
                         DU(RGB[i-3-WIDTH*3], RGB[i-2-WIDTH*3], RGB[i-1-WIDTH*3]))/4);

                V[j] = (unsigned char)
                       ((DV(RGB[i  ], RGB[i+1], RGB[i+2]) + 
                         DV(RGB[i-3], RGB[i-2], RGB[i-1]) +
                         DV(RGB[i  -WIDTH*3], RGB[i+1-WIDTH*3], RGB[i+2-WIDTH*3]) +
                         DV(RGB[i-3-WIDTH*3], RGB[i-2-WIDTH*3], RGB[i-1-WIDTH*3]))/4);
            }

        }
    }
}

//入口
int main(int argc, char* argv[])
{
    char file[255];
    FILE *fp_bmp = NULL;
    FILE *fp_yuv;
    unsigned char *YUV = NULL;
    unsigned char *RGB = NULL;
    unsigned int imgSize = WIDTH * HEIGHT;

	// alloc rgb image space
	RGB = (unsigned char*)(malloc(imgSize * 6));
	memset(RGB, 0, imgSize*6);
	if(!RGB){
		goto err1;
	}
	// alloc yuv image space
    YUV = (unsigned char*)(malloc(imgSize + (imgSize / 2)));
	memset(YUV, 0, imgSize + (imgSize / 2));
	if(!YUV){
		goto err1;
	}

	// read bmp file
	sprintf(file, "%s.bmp", argv[1]);
	fp_bmp = fopen(file, "rb");
	if(!fp_bmp){
		fprintf(stderr, "fopen file \"%s\" err", file);
		goto err2;
	}
	ReadBmp(RGB, fp_bmp);
	// convert bmp to yuv
    Convert(RGB, YUV);

	// write to new file
	sprintf(file, "%s.yuv", argv[1]);
	fp_yuv = fopen(file, "wb");
	if(!fp_yuv){
		fprintf(stderr, "fopen file \"%s\" err", file);
		goto err3;
	}
	fwrite(YUV, 1, imgSize + (imgSize / 2), fp_yuv);

	// return
	fclose(fp_yuv);
	fp_yuv = NULL;
	fclose(fp_bmp);
	fp_bmp = NULL;
	return 0;
   
err3:
	fclose(fp_yuv);
	fp_yuv = NULL;
err2:
	fclose(fp_bmp);
	fp_bmp = NULL;
err1:
    return 1;
}



