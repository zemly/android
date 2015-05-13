package android.os;

interface IModbusService {
    void writeReg(inout char[] str, int len);
    int readReg(inout char[] str, int len);
}
