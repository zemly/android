#ifndef ANROID_MODBUS_INTERFACE_H
#define ANROID_MODBUS_INTERFACE_H

__BEGIN_DECLS

/*定义模块ID*/
#define MODBUS_HARDWARE_MODULE_ID "modbus"

struct modbus_module_t {
    struct hw_module_t common;
};

/*硬件接口结构体*/
struct modbus_device_t{
    struct hw_device_t common;
    int fd;             //驱动的文件描述符
    int (*read_reg)(struct modbus_device_t* dev, char *reg, char num);
    int (*write_reg)(struct modbus_device_t* dev, char *reg, char num);
};



__END_DECLS


#endif
