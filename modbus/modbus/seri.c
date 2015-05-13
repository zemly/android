#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include <errno.h>
#include <unistd.h>     
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <unistd.h>     /*Unix 标准函数定义*/
#include <sys/types.h> 
#include     <sys/stat.h>  
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/
#include     "seri.h"
 
#define FALSE  -1
#define TRUE   0
#define Vtime 0             //读取超时
#define Vmin  1            //读取字节数





/*
说明:将用户输入的数据信息组织成为modbus发送数据格式
输入:pInBuf-输入数据信息，结构为1B从机地址+1B功能代码+2B寄存器或线圈起始地址+2B寄存器或线圈个数+数据内容（如果是读则没有）
	pOutBuf-输出的帧格式指针，有调用者开辟空间，一般来说35B足够
输出:
	返回值为帧的长度
*/
unsigned char FormFrame(unsigned char *pInBuf,unsigned char *pOutBuf)
{
	unsigned char frameLen=0;
	unsigned char inPos=0;
	unsigned char lrc=0;
	int i=0;
	int byteCnt=0;
	unsigned char tmp;

	//前面的数据区域都是一样的,包括起始-从机地址-功能码-寄存器/线圈地址
	*(pOutBuf+frameLen++)=':';
	for(i=0;i<4;i++)
	{
		tmp=*(pInBuf+inPos++);
		UcharToAsc(tmp,pOutBuf+frameLen);
		frameLen+=2;
	}

	switch(*(pInBuf+1))
	{
	case 01:	//读线圈
		byteCnt=2;
		break;
	case 04:	//读寄存器		
		byteCnt=2;
		break;
	case 05:	//强置单个线圈
		byteCnt=2;
		break;
	case 06:	//预置一个寄存器
		byteCnt=2;
		break;
	case 15:	//预置多个线圈
		byteCnt=2+1+*(pInBuf+inPos+2);	//线圈个数2B+数据字节数1B+线圈数据字节
		break;
	case 16:	//预置多个寄存器
		byteCnt=2+1+*(pInBuf+inPos+2);	//寄存器个数2B+数据字节数1B+寄存器数据字节
		break;
	default:
		pOutBuf=NULL;
		return frameLen;
	}

	//复制所有的附加数据
	for(i=0;i<byteCnt;i++)
	{
		tmp=*(pInBuf+inPos++);
		UcharToAsc(tmp,pOutBuf+frameLen);
		frameLen+=2;
	}
	//后面一样的数据区域
	lrc=LRC(pOutBuf+1,(unsigned short)(frameLen-1));

	*(pOutBuf+frameLen++)=lrc/16+(lrc/16>=10?('A'-0xA):'0');			//LRC校验
	*(pOutBuf+frameLen++)=lrc%16+(lrc%16>=10?('A'-0xA):'0');
	*(pOutBuf+frameLen++)=0xD;					//结束标识
	*(pOutBuf+frameLen++)=0xA;
	
	return frameLen;
}

/*
说明：
	这里，仅对功能码最高位做了检查，没有检查LRC校验码
	只处理了读一个寄存器和最多16个线圈
输入参数：
	pInBuf-收到的Modbus帧
输出参数：
	返回值：0-操作出错，100-操作正常，其它-实际读取的字节数
	pOutBuf-最多2B字节，数量由返回字节数决定
*/
unsigned char Resolve(unsigned char *pInBuf,unsigned char *pOutBuf)
{
	unsigned char datStartPos=0;
	unsigned char funcNo;
	unsigned char unitByteNum=0;

	AscToUchar(&funcNo,pInBuf+3);
	//数据读取出错，功能返回高位为1
	if(funcNo>24)
	{
		*pOutBuf='\0';
		return 0;
	}
	switch(funcNo)
	{
	case 01:	//读线圈
	case 04:	//读寄存器
		AscToUchar(&unitByteNum,pInBuf+5);
		datStartPos=1+2+2+2;
		AscToUchar(pOutBuf,pInBuf+datStartPos);
		if(2==unitByteNum)
			AscToUchar(pOutBuf+1,pInBuf+datStartPos+2);
		break;
	default:
		*pOutBuf='\0';
		return 100;
	}
	//提取线圈和寄存器数据
	
	return unitByteNum;
}

//计算LRC
unsigned char LRC(unsigned char *auchMsg, unsigned short usDataLen)
{
	unsigned char uchLRC = 0 ; /*初始化LRC字符 */
	while (usDataLen--) /*通过数据缓冲器*/
		uchLRC += *auchMsg++ ; /*加缓冲器字节无进位*/
	return ((unsigned char)(-((char)uchLRC))) ; /*返回二进制补码*/
} 

void UcharToAsc(unsigned char tmp,unsigned char *asc)
{
	*asc=(tmp)/16+(tmp/16>=10?('A'-0xA):'0');			
	*(asc+1)=tmp%16+(tmp%16>=10?('A'-0xA):'0');
}

void AscToUchar(unsigned char *tmp,unsigned char *asc)
{
	*tmp=(*asc-(*asc>='A'?'A':'0'))*16;
	*tmp+=*(asc+1)-(*(asc+1)>='A'?'A':'0');
}

/**
*@brief  设置串口通信速率
*@param  fd     类型 int  打开串口的文件句柄
*@param  speed  类型 int  串口速度
*@return  void
*/
int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
                   B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400, 
                            19200,  9600, 4800, 2400, 1200,  300, };
void set_speed(int fd, int speed){
         int   i;
         int   status;
         struct termios   Opt;
         tcgetattr(fd, &Opt);
         for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) {
                   if  (speed == name_arr[i]) {    
                            tcflush(fd, TCIOFLUSH);    
                            cfsetispeed(&Opt, speed_arr[i]); 
                            cfsetospeed(&Opt, speed_arr[i]);  
                            status = tcsetattr(fd, TCSANOW, &Opt); 
                            if  (status != 0) {       
                                     perror("tcsetattr fd"); 
                                     return;    
                            }   
                            tcflush(fd,TCIOFLUSH);  
                   } 
         }
}
/**
*@brief   设置串口数据位，停止位和效验位
*@param  fd     类型  int  打开的串口文件句柄
*@param  databits 类型  int 数据位   取值 为 7 或者8
*@param  stopbits 类型  int 停止位   取值为 1 或者2
*@param  parity  类型  int  效验类型 取值为N,E,O,,S
*/
int set_Parity(int fd,int databits,int stopbits,int parity)
{
         struct termios options;
         if  ( tcgetattr( fd,&options)  !=  0) {
                    ALOGE("Modbus Stub: SetupSerial 1");    
                   return(FALSE); 
         }
         options.c_cflag &= ~CSIZE;
         options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
         options.c_oflag  &= ~OPOST;   /*Output*/
 
         switch (databits) /*设置数据位数*/
         {   
         case 7:                
                   options.c_cflag |= CS7;
                   break;
         case 8:    
                   options.c_cflag |= CS8;
                   break;  
         default:   
                    ALOGE("Modbus Stub: Unsupported data size"); return (FALSE); 
         }
switch (parity)
{  
         case 'n':
         case 'N':   
                   options.c_cflag &= ~PARENB;   /* Clear parity enable */
                   options.c_iflag &= ~INPCK;     /* Enable parity checking */
                   break; 
         case 'o':  
         case 'O':    
                   options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/ 
                   options.c_iflag |= INPCK;             /* Disnable parity checking */
                   break; 
         case 'e': 
         case 'E':  
                   options.c_cflag |= PARENB;     /* Enable parity */   
                   options.c_cflag &= ~PARODD;   /* 转换为偶效验*/    
                   options.c_iflag |= INPCK;       /* Disnable parity checking */
                   break;
         case 'S':
         case 's':  /*as no parity*/  
             options.c_cflag &= ~PARENB;
                   options.c_cflag &= ~CSTOPB;break; 
         default:  
                    ALOGE("Modbus Stub: Unsupported parity/n");   
                   return (FALSE); 
         } 
/* 设置停止位*/ 
switch (stopbits)
{  
         case 1:   
                   options.c_cflag &= ~CSTOPB; 
                   break; 
         case 2:   
                   options.c_cflag |= CSTOPB; 
            break;
         default:   
                     ALOGE("Modbus Stub: Unsupported stop bits/n"); 
                    return (FALSE);
}
/* Set input parity option */
if (parity != 'n')  
         options.c_iflag |= INPCK;
tcflush(fd,TCIFLUSH);
options.c_cc[VTIME] = Vtime; /* 设置超时15 seconds*/  
options.c_cc[VMIN] = Vmin; /* define the minimum bytes data to be readed*/
if (tcsetattr(fd,TCSANOW,&options) != 0)  
{
          ALOGE("Modbus Stub: SetupSerial 3");  
         return (FALSE); 
}
return (TRUE); 
}
/**********************************************************************
代码说明：使用串口一测试的，发送的数据是字符，
但是没有发送字符串结束符号，所以接收到后，后面加上了结束符号
**********************************************************************/
 
/*********************************************************************/
int OpenDev(char *Dev)
{
         int     fd = open( Dev, O_RDWR );
        //| O_NOCTTY | O_NDELAY         
         if (-1 == fd)        
         {                        
                    ALOGE("Modbus Stub: Can't Open Serial Port");
                   return -1;            
         }      
         else  
                   return fd;
}
void getcardinfo(char *buff){
         int fd,i;
         int nread,count=0,nwrite;
         unsigned char input[10];
         unsigned char output[35];
         int len;
         char tempbuff[13];
         char *dev  = "/dev/ttyUSB0"; //串口1
         fd = OpenDev(dev);
         set_speed(fd,9600);
         if (set_Parity(fd,8,1,'N') == FALSE)  {
                    ALOGE("Modbus Stub: Set Parity Error/n");
                   //return -1;
         }

         //测试读线圈
         input[0] = 0x11;
         input[1] = 0x01;
         input[2] = 0x00;
         input[3] = 0x13;
         input[4] = 0x00;
         input[5] = 0x25;
         len = FormFrame(input,output);
          ALOGE("Modbus Stub: the data is going to send:%s\n",output);
         nwrite = write(fd, output, len);
         if(nwrite < len)  ALOGE("Modbus Stub: write data error:%d",nwrite);
         for(i = 0; i < 17; i ++)
         {
            nread = read(fd, &output[i], 1);
         }
             ALOGE("Modbus Stub: %s",output);
         while (1) //循环读取数据
         {  
                   count=0;
                   //sleep(5000);
                   while(1)
                   {
                            if(nread = read(fd, tempbuff, 13));
                            {
                                 ALOGE("Modbus Stub: /nLen %d/n",nread);
                                 ALOGE("Modbus Stub: %s\n",tempbuff);
                            }
//                            else printf("cant get 13 buf\n");
                            if(count==13)
                            {
                                    // buff[count+1] = '/0';  
                            //printf( "/n%s", buff);
                                     break;
                            }
                   }
                   //break;
         }
         //return buff;
         close(fd);
         pthread_exit(NULL);
         //close(fd); 
         // exit (0);
}

/*int main(void)
{
    int fd;
    char *buf;
    unsigned char input[10];
    unsigned char otput[35];
    fd = OpenDev("/dev/ttyUSB0");
    set_speed(fd, B9600);
    set_Parity(fd,8,1,'N');
    getcardinfo(buf);
}*/
