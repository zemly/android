#ifndef ANDROID_MODBUS_INTERFACE_H
#define ANDROID_MODBUS_INTERFACE_H

__BEGIN_DECLS


unsigned char FormFrame(unsigned char *pInBuf,unsigned char *pOutBuf);
unsigned char Resolve(unsigned char *pInBuf,unsigned char *pOutBuf);
unsigned char LRC(unsigned char *auchMsg, unsigned short usDataLen);
void UcharToAsc(unsigned char tmp,unsigned char *asc);
void AscToUchar(unsigned char *tmp,unsigned char *asc);
void set_speed(int fd, int speed);
int set_Parity(int fd,int databits,int stopbits,int parity);
int OpenDev(char *Dev);
void getcardinfo(char *buff);


__END_DECLS

#endif
