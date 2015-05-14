package com.example.modbustest;

import com.example.modbustest.R;
import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.os.RemoteException;
import android.util.Log;
import android.widget.TextView;
import android.view.View.OnClickListener;
import android.os.ServiceManager;
import android.os.IModbusService;
import com.mokoid.server.ModbusService;
public class MainActivity extends Activity {
	private char[] arry = new char[40];
    private static int len;
    private static final String TAG = "ModbusService";
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		final Button OkButton = (Button) findViewById(R.id.button1);
		final Button CancelButton = (Button) findViewById(R.id.button2);
		final EditText adr=(EditText)findViewById(R.id.editText1);			//获取地址文本框对象
		final EditText fun=(EditText)findViewById(R.id.editText2);	
		final EditText ahigh=(EditText)findViewById(R.id.editText3);	
		final EditText alow=(EditText)findViewById(R.id.editText4);	
		final EditText dhigh=(EditText)findViewById(R.id.editText5);	
		final EditText dlow=(EditText)findViewById(R.id.editText6);	
		final EditText out=(EditText)findViewById(R.id.editText7);	
		OkButton.setOnClickListener(//OnClickListener为View的内部接口，其实现者负责监听鼠标点击事件
				new View.OnClickListener(){ 
					public void onClick(View v){ 					//重写onClick方法
						  ModbusService ls = new ModbusService();
					        if(ls == null) Log.e(TAG, "get service failed.");
					        else if(ls != null) Log.e(TAG, "get service suceed.");
					        
						arry[0]=(char)Integer.parseInt(adr.getText().toString());
						arry[1]=(char)Integer.parseInt(fun.getText().toString());
						arry[2]=(char)Integer.parseInt(ahigh.getText().toString());
						arry[3]=(char)Integer.parseInt(alow.getText().toString());
						arry[4]=(char)Integer.parseInt(dhigh.getText().toString());
						arry[5]=(char)Integer.parseInt(dlow.getText().toString());
						len=6;
				        String str1 = new String(arry);
				        Log.e(TAG, "write:" + str1);
				        ls.writeReg(arry,len);
				        len = 0;
				       // write(arry,len);
				        if((len = ls.readReg(arry,len)) <= -1) {
                            Log.e(TAG, "read failure."); 
                            out.append("read error");
                            return;
                        }
                        if(len == 0) {
                            out.append("write succeed!");
                            return ;
                        }
                        int arry0 = (int)arry[0];
				        Log.e(TAG, "read: " + arry + ": " + len + ": " + arry0);
						out.setText("");
						int j;
						for(int i=0;i<len;i++)
						{
							j =(int) arry[i];
                            String ads=Integer.toString(j);
                            out.append(ads+" ");
						}
					//	String ads=new String(str3);
					//	out.append(ads );
					//	Log.e(TAG, "read: " + str3 + ": " + len);
					} 
				}); 
		CancelButton.setOnClickListener(//OnClickListener为View的内部接口，其实现者负责监听鼠标点击事件
				new View.OnClickListener(){ 
					public void onClick(View v){ 		//重写onClick方法
						adr.setText("");				//清空地址文本框内容
						fun.setText("");				//清空功能文本框内容
						ahigh.setText("");				
						alow.setText("");			
						dhigh.setText("");				
						dlow.setText("");	
						out.setText("");	
					} 
				}); 
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
}
