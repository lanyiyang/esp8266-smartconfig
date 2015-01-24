package com.example.smartconfig;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.UnknownHostException;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;


public class UdpThread extends Thread {	
	
	public final static String TAG = "UdpThread";
	static Handler mUdpHandler;
	static DatagramSocket udpSocket = null;
	static DatagramPacket udpsendPacket = null;
	static DatagramPacket udprecPacket = null;
	
	 @Override
	public void run() {
		connect();
		Looper.prepare();
		mUdpHandler = new Handler() {
			 @Override
			public void handleMessage(Message msg) {
                 // 处理收到的消息
				 switch(msg.what)
					{
						case 1:
				
							break;
						case 2:

							break;
						case 3:
							Bundle data = msg.getData(); 
							send(data);
							break;
						
						default:break;
					}
             }
		};
		Looper.loop();
	 }
	 
	 public static void send(String ssid,String pwd, int what){
		Bundle bundle =  new Bundle();
		bundle.putString("ssid", ssid);
		bundle.putString("pwd", pwd);
		
		Message	sendMsg = new Message();	
		sendMsg.setData(bundle);
		sendMsg.what =what;
		mUdpHandler.sendMessage(sendMsg);
	 }
	 
	 private void connect() {  		
	    	try {	
	    		Log.d(TAG,"udp connect");
				udpSocket = new DatagramSocket(4560);
	    	}catch(Exception e){

			}
	 }
	 
	 private void send(Bundle bundle ){
	
			try {
				String ssid = bundle.getString("ssid");
				String pwd = bundle.getString("pwd");
				byte[] sendbuf = MakeIotMooncake_SSID_PWD(ssid,pwd);
				sendbuf = MakeCRC8(sendbuf);
				for(int i =0;i<sendbuf.length;i++){
					int j= sendbuf[i]&0xff;
					Log.e(TAG,"sendBuf="+j);
				}
				byte[] encBuf = SmartLinkEncode(sendbuf);
		        byte[] dummybuf = new byte[18];
		        int delayms = 5;
		        long beginTime = System.currentTimeMillis();
		        
		        udpsendPacket = new DatagramPacket(dummybuf, dummybuf.length);
				udpsendPacket.setData(dummybuf);
				udpsendPacket.setPort(80);
				InetAddress broadcastAddr;		
				broadcastAddr = InetAddress.getByName("255.255.255.255");
				udpsendPacket.setAddress(broadcastAddr);
				
	            while (true)
	            {
	                delayms++;
	                if (delayms > 7) 
	                    delayms = 5;
	                //Log.e(TAG,"encBuf="+ encBuf.length);
	                for (int i = 0; i < encBuf.length; i++)
	                {
	                	udpsendPacket.setLength(encBuf[i]);
	                	udpSocket.send(udpsendPacket);
	                    //Thread.sleep(delayms);
	                }
	                Thread.sleep(200);
	                if ((System.currentTimeMillis() - beginTime)/1000 >= 13)
	                    break;
	           }
			}catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (UnknownHostException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
	 }
	 
	 private byte[] SmartLinkEncode(byte[] src)
     {
         byte[] rtlval = null;
         int curidx = 0;
         rtlval = new byte[src.length * 10];
         byte tmp;
         for (int i = 0; i < src.length; i++)
         {
             rtlval[curidx++] = 0;
             rtlval[curidx++] = 0;
             tmp = (byte)(i & 0xF);
             rtlval[curidx++] = (byte)(tmp+1);//pos_L
             rtlval[curidx++] = (byte)((15-tmp)+1);//~pos_L
             tmp = (byte)((i & 0xF0)>>4);
             rtlval[curidx++] = (byte)(tmp + 1);//pos_H
             rtlval[curidx++] = (byte)((15 - tmp) + 1);//~pos_H
             tmp = (byte)(src[i] & 0xF);
             rtlval[curidx++] = (byte)(tmp + 1);//val_L
             rtlval[curidx++] = (byte)((15 - tmp) + 1);//~val_L
             tmp = (byte)((src[i] & 0xF0)>>4);
             rtlval[curidx++] = (byte)(tmp + 1);//val_H
             rtlval[curidx++] = (byte)((15 - tmp) + 1);//~val_H
         }
         return rtlval;
     }
	 
     byte[] MakeIotMooncake_SSID_PWD(String ssid, String pwd)
     {
         byte[] SSID = ssid.getBytes();
         byte[] PWD =  pwd.getBytes();
         byte[] rtlval = new byte[SSID.length + PWD.length + 1];
         int curidx = 0;
         for (int i = 0; i < SSID.length; i++){
        	 rtlval[curidx++] = SSID[i];
         }
         rtlval[curidx++] = 0x0a;//'\n'  split by '\n'
         for (int i = 0; i < PWD.length; i++){
        	     rtlval[curidx++] = PWD[i];
         }
         return rtlval;
     }
     
     byte[] MakeCRC8(byte[] src)
     { 
         byte crc = calcrc_bytes(src,src.length);
         byte[] rtlval = new byte[src.length + 1];
         for (int i = 0; i < src.length; i++){
        	  rtlval[i] = src[i];
         }
         rtlval[src.length] = crc;
         return rtlval;
     }
     
     byte calcrc_bytes(byte[] p,int len)  
     {  
         byte crc=0;
         int i = 0;
         for (i = 0; i < len;i++ )
	      {
	          crc = (byte)calcrc_1byte(crc^p[i]);
	          int j= crc&0xff;
	          Log.e(TAG,"crc="+j);
	      }  
      return crc;  //若最终返回的crc为0，则数据传输正确  
     }
     
     int calcrc_1byte(int abyte)    
     {    
        int i,crc_1byte;     
        crc_1byte=0;                //设定crc_1byte初值为0  
         for(i = 0; i < 8; i++)    
         {    
           if(((crc_1byte^abyte)&0x01) > 0)    
           {    
                crc_1byte^=0x18;     
                crc_1byte>>=1;    
                crc_1byte|=0x80;    
           }          
           else     
           {
               crc_1byte>>=1;   
           }
            abyte>>=1;          
          }   
          return crc_1byte;   
     }  
}