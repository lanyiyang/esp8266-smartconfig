package com.example.smartconfig;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

import com.example.smartlink.R;

public class MainActivity extends Activity {

	private EditText txt_ssid;
	private EditText txt_password;
	private Button btn_smartlink;
	private WifiManager wm = null; 
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		wm = (WifiManager) getSystemService(WIFI_SERVICE);
		new UdpThread().start();
		
		btn_smartlink = (Button)findViewById(R.id.btn_smartlink);
		txt_ssid = (EditText)findViewById(R.id.txt_ssid);
		txt_password = (EditText)findViewById(R.id.txt_password);
		btn_smartlink.setText("START");
		txt_ssid.setText(getSSid());

		btn_smartlink.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				int result;
				
				if(btn_smartlink.getText().toString() == "SMARTLINK"){
					if(txt_ssid.getText().toString().equals("")){
						new AlertDialog.Builder(MainActivity.this)
					     .setTitle("错误")
					     .setMessage("请输入SSID")
					     .show();
						return;
					}
					
					btn_smartlink.setText("STOP");
					UdpThread.send(txt_ssid.getText().toString(), txt_password.getText().toString(), 3);
				} else {
					btn_smartlink.setText("SMARTLINK");
				}
			}
		});
	}
	
	private String getSSid(){
		if(wm != null){
			WifiInfo wi = wm.getConnectionInfo();
			if(wi != null){
				String s = wi.getSSID();
				Log.d("SmartlinkActivity",wi.getBSSID()+s.substring(1,s.length()-1));
				if(s.length()>2&&s.charAt(0) == '"'&&s.charAt(s.length() -1) == '"'){
					return s.substring(1,s.length()-1);
				}
			}
		}
		return "";
	}
	
	 //创建选择菜单
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
	    getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
  //处理菜单选择事情
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		Intent intent = new Intent();
		switch(item.getItemId()){
			case R.id.about:
				goAboutActivity();
				break;
				
			default:break;
		}
		return true;
	}
	
	 public void goAboutActivity(){
	    	Intent intent = new Intent();
	    	intent.setClass(this, AboutActivity.class);
	    	startActivity(intent);
	    }
}
