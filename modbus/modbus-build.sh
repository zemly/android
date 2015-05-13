. setenv
mmm frameworks/base/services/modbus_jni/
mmm packages/experimental/modbustest/java/
mmm packages/experimental/modbustest
mmm hardware/libhardware/modules/modbus/
adb push out/target/product/tiny4412/obj/lib/modbus.default.so /system/lib/hw/
adb push out/target/product/tiny4412/system/framework/mokoid.jar /system/framework/
adb push out/target/product/tiny4412/obj/lib/libmokoid_runtime.so /system/lib/
adb install -r out/target/product/tiny4412/system/app/modbustest.apk
