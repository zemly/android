package com.mokoid.server;
import android.content.Context;
import android.os.IModbusService;
import android.util.Slog;
public class ModbusService extends IModbusService.Stub {

    private static final String TAG = "ModbusService";
    static {
//        System.load("/system/lib/libmokoid_runtime.so");
        try {
            System.load("/system/lib/libmokoid_runtime.so");
        } catch (UnsatisfiedLinkError ule) {
            Slog.i("ModbusService","cant load library mokoid_runtime");
        }
    } /*catch (UnsatisfiedLinkError ule) {
        Slog.i("ModbusService","cant load library mokoid_runtime");
    }*/
    public ModbusService() {
        Slog.i("ModbusService","go to get modbus Stub...");
        /*try {
            System.load("/system/lib/libmokoid_runtime.so");
        } catch (UnsatisfiedLinkError ule) {
            Slog.i("ModbusService","cant load library mokoid_runtime");
        }*/
        init_native();
    }
    public void writeReg(char str[], int len) {
        Slog.i("ModbusService","go to write modbus Stub...");
        writeReg_native(str,len);
    }
    public int readReg(char str[], int len) {
        Slog.e("ModbusService","go to read modbus Stub...");
        return readReg_native(str,len);
    }

    private static native boolean init_native();
    private static native void writeReg_native(char[] str, int len);
    private static native int readReg_native(char[] str, int len);
}
