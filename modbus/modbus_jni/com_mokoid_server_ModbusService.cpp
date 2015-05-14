#define LOG_TAG "ModbusService"
#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include <utils/misc.h>
#include <utils/Log.h>
#include <hardware/hardware.h>
#include <hardware/modbus.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
namespace android {
struct modbus_device_t *modbus_device = NULL;

static void modbus_writeReg(JNIEnv *env, jobject clazz, jcharArray value, jint len, jchar num) {
    jchar *str;
    int i = 0;
    char *array = new char [40];
    str = env->GetCharArrayElements(value, NULL);
    for(i = 0; i < len; i ++) {
        array[i] = str[i];
    }
    ALOGI("Modbus JNI: set value %s to device reg.",array);
    if(!modbus_device) {
        ALOGI("Modbus JNI: device is not open.");
        return ;
    }

        modbus_device->write_reg(modbus_device, array, len);
}

static jint modbus_readReg(JNIEnv *env, jobject clazz, jcharArray value, jint len, jchar num) {
    int i;
    int ret;
    char *str = new char [40];

    ret = modbus_device->read_reg(modbus_device, str, len);
    if(ret == -1) {
        ALOGE("modbus number return error");
        free(str);
        return -1;
    } else if(ret == 0) {
        ALOGE("modbus read null");
        free(str);
        return 0;
    }
    jchar *pArray = (jchar *)calloc(ret, sizeof(jchar));
    if(pArray == NULL) {
        ALOGE("Modbus JNI: calloc pArray error.");
        return false;
    }
    ALOGE("Modbus JNI: read %d numbers.",ret);
    len = ret;

    for(i = 0; i < len; i ++) {
        *(pArray + i) = *(str + i);
    }
    env->SetCharArrayRegion(value, 0, len, pArray);

    free(pArray);
    pArray = NULL;

    if(!modbus_device) {
        ALOGI("Modbus JNI: device is not open.");
        return false;
    }

    ALOGI("Modbus JNI: read %s from device reg.",str);
out:
    free(str);
    str = NULL;
    return len;
}

static inline int modbus_device_open(const hw_module_t *module, struct modbus_device_t **device) {
    return module->methods->open(module, MODBUS_HARDWARE_MODULE_ID, (struct hw_device_t **)device);
}

jboolean modbus_init(JNIEnv *env, jclass clazz) {
    modbus_module_t *module;

    ALOGI("Modbus JNI: initialing...");
    if(hw_get_module(MODBUS_HARDWARE_MODULE_ID, (const struct hw_module_t **)&module) == 0) {
        ALOGI("Modbus JNI: modbus stub found.");
        if(modbus_device_open(&(module->common), &modbus_device) == 0) {
            ALOGI("Modbus JNI:modbus device is open.");
            return 0;
        }
        ALOGE("Modbus JNI: failed to open modbus device.");
        return -1;
    }
    ALOGE("Modbus JNI: failed to get modbus stub module.");
    return -1;
}

static const JNINativeMethod method_table[] = {
    {"init_native", "()Z", (void *)modbus_init},
    {"writeReg_native", "([CI)V", (void *)modbus_writeReg},
    {"readReg_native", "([CI)I", (void *)modbus_readReg},
};

/*int register_android_server_ModbusService(JNIEnv *env) {
    return jniRegisterNativeMethods(env, "com/android/server/ModbusService", method_table, NELEM(method_table));
}*/
/*int register_modbus_hal_jni(JNIEnv *env) {
    static const char *kClassName = "com/mokoid/server/ModbusService";
    jclass clazz;*/

    /*look up the class*/
    /*clazz = env -> FindClass(kClassName);
    if(clazz == NULL) {
        ALOGE("Cant find class %s",kClassName);
        return -1;
    }*/

    /*register the method*/
   /* if(env -> RegisterNatives(clazz, method_table, sizeof(method_table) / sizeof(method_table[0])) != JNI_OK) {
        printf("Failed to registering method %s/n",kClassName);
        return -1;
    }
    return 0;
}*/
/*只能在jdk1.6以上版本*/
/*extern "C" jint JNI_Onload(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint ret = -1;
    ALOGI("JNI_Onload modbus");
    if(vm -> GetEnv((void **)&env,JNI_VERSION_1_6) != JNI_OK) {
        printf("ERROR: GetEnv failed/n");
        goto fail;
    }
    assert(env != NULL);
    if(register_modbus_hal_jni(env) != 0) {
        printf("ERROR: PlatformLibrary native registration failed!\n");
        goto fail;
    }

    ret = JNI_VERSION_1_6;


fail:
    return ret;
}*/
int register_mokoid_server_ModbusService(JNIEnv *env) {
    return jniRegisterNativeMethods(env, "com/mokoid/server/ModbusService",method_table, NELEM(method_table));
}
}
