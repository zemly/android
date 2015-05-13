package com.example.modbustest;

import android.app.Activity;
import android.os.RemoteException;
import android.os.Bundle;
import android.os.ServiceManager;
import android.view.Menu;
import android.view.MenuItem;
import android.util.Log;
import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Button;
import android.view.View;
import android.view.View.OnClickListener;


import android.os.IModbusService;
import com.mokoid.server.ModbusService;


public class MainActivity extends Activity {
//    private IModbusService ModbusService = null;
    private char[] arry = new char[40];
    private static int len;
    private static final String TAG = "ModbusService";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        /*try {
            Log.i(TAG, "Modbus Service");
            ServiceManager.addService("modbus", new ModbusService());
        } catch (Throwable e) {
            Log.e(TAG, "Failure starting Modbus Service",e);
        }*/
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
//        ModbusService = IModbusService.Stub.asInterface(ServiceManager.getService("modbus"));
        
        Button btn = new Button(this);
        btn.setText("Click");
        btn.setOnClickListener(new OnClickListener() {
            public void onClick(View v){
                initservice();
            }
        });
        setContentView(btn);
    }
    public void initservice() {
        ModbusService ls = new ModbusService();
        if(ls == null) Log.e(TAG, "get service failed.");
        else if(ls != null) Log.e(TAG, "get service suceed.");
        
        
        arry[0] = 0x11;
        arry[1] = 0x01;
        arry[2] = 0x00;
        arry[3] = 0x13;
        arry[4] = 0x00;
        arry[5] = 0x25;
        len = 6;

        String str1 = new String(arry);
        Log.e(TAG, "write:" + str1);
        ls.writeReg(arry,len);
        len = 0;
       // write(arry,len);
        if((len = ls.readReg(arry,len)) <= 0) Log.e(TAG, "read failure.");
        Log.e(TAG, "read: " + arry + ": " + len);
        
    }
    /*public void onClick(View v) {
        TextView tv = new TextView(this);
        tv.setText("Led");
        setContentView(tv);
    }*/
/*    public void write(char[] str, int len) {
        try {
            ModbusService.writeReg(str,len);
        } catch (RemoteException e) {
            Log.e(TAG, "RemoteException in ModbusService.write:",e);
        }
    }
    public boolean read(char[] str,int len) {
        boolean ret = false;
        try {
            ret = ModbusService.readReg(str, len);    
        } catch (RemoteException e) {
            Log.e(TAG, "RemoteException in ModbusService.read:",e);
        }
        return ret;
    }*/
}
