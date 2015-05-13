#define LOG_TAG "ModbusStub"

#include <hardware/hardware.h>
#include <hardware/modbus.h>
#include <stdio.h>      
#include <stdlib.h>     
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>     
#include <sys/types.h> 
#include <sys/stat.h>  
#include <cutils/log.h>
#include <termios.h>    /*PPSIX 终端控制定义*/
#include <cutils/atomic.h>
#include "seri.h"

#define DEVICE_NAME "/dev/ttyUSB0"          //驱动文件
#define MODULE_NAME "Modbus"                //模块名
#define MODULE_AUTHOR "LUO"
#define FALSE  -1
#define TRUE   0
#define Vtime 0             //读取超时
#define Vmin  1            //读取字节数

/*设备打开和关闭接口*/
static int modbus_device_open(const struct hw_module_t *module,const char *name,struct hw_device_t **device);
static int modbus_device_close(struct hw_device_t *device);

/*设备访问接口*/
static int modbus_read_reg(struct modbus_device_t *dev, char *reg, char num);
static int modbus_write_reg(struct modbus_device_t *dev, char *reg, char num);

/*模块方法表*/
static struct hw_module_methods_t modbus_module_methods = {
open: modbus_device_open,
};

/*模块实例变量*/
struct modbus_module_t HAL_MODULE_INFO_SYM = {
    common:{
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: MODBUS_HARDWARE_MODULE_ID,
        name: MODULE_NAME,
        author: MODULE_AUTHOR,
        methods: &modbus_module_methods,
    }  
};

static int modbus_device_open(const struct hw_module_t *module, const char *name, struct hw_device_t **device)
{
    struct modbus_device_t *dev;

    dev = (struct modbus_device_t*)malloc(sizeof(struct modbus_device_t));
    if(!dev){
        ALOGE("Modbus Stub:failed to alloc space");
        return -EFAULT;
    }

    memset(dev, 0, sizeof(struct modbus_device_t));
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (hw_module_t *)module;
    dev->common.close = modbus_device_close;
    dev->read_reg = modbus_read_reg;
    dev->write_reg = modbus_write_reg;
    //dev->fd = open( DEVICE_NAME, O_RDWR | O_NOCTTY | O_NDELAY);
       // | O_NOCTTY | O_NDELAY    
    dev->fd = OpenDev( DEVICE_NAME );
    set_speed(dev->fd, B9600);    
    if (set_Parity(dev->fd,8,1,'N') == FALSE)  {
                   ALOGE("Modbus Stub: Set Parity Error/n");
                   //return -1;
         } 

    if (-1 == dev->fd)        
    {   
        ALOGE("Modbus Stub: failed to open /dev/ttyUSB0 -- %s.",strerror(errno));
        free(dev);
        return -EFAULT;
    }      
    *device = &(dev->common);
    ALOGI("Modbus Stub: open /dev/ttyUSB0 successfully.");

    return 0;
}

static int modbus_device_close(struct hw_device_t *device) {
    struct modbus_device_t *modbus_device = (struct modbus_device_t *)device;

    if(modbus_device) {
        close(modbus_device -> fd);
        free(modbus_device);
    }
    return 0;
}

static int modbus_write_reg(struct modbus_device_t *dev, char *reg, char num) {
    int ret;
    unsigned char output[35];
    int len;
    len = FormFrame(reg, output);
    ALOGI("Modbus Stub: set value %s:%d:%d to device reg.",output ,output[0],len);
    ret = write(dev->fd,output,len);
    ALOGI("Modbus Stub: write %d numbers succeed.",ret);
    return ret;
}

static int modbus_read_reg(struct modbus_device_t *dev, char *reg, char num) {
    int ret;
    int i;
    unsigned char output[35];
    ALOGI("Modbus Stub: going to read %d tty.",num);
    if(!reg) {
        ALOGE("Modbus Stub: error reg pointer");
        return -EFAULT;
    }
    //ret = read(dev->fd,reg,8);
    for(i = 0; i < 35; i ++) {
        ret = read(dev->fd,&output[i],1);
        if(ret == 0) ALOGI("Modbus Stub:return end.");
        if(output[i] == 0x0A) break;
    }
    ret = i + 1;
    ALOGI("Modbus Stub: get value %s:%d:%d:%d from devicereg", output, output[ret - 2], output[ret - 1] ,ret);
    ret = Resolve(output, reg);
    ALOGI("Modbus Stub: get modbus number %d:%d from read", reg[0], ret);
    return ret + 1;
}
