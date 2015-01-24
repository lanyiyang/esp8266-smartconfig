/******************************************************************************
 * Copyright 2014-2015  (By younger)
 *
 * FileName: smartconfig.c
 *
 * Description: smartconfig demo on sniffer
 *
 * Modification history:
 *     2015/1/24, v1.0 create this file.
*******************************************************************************/
#include "smartconfig.h"
#include "user_interface.h"

uint8 smart_lock_flag=0;
uint8 smart_mac[6];
uint8 smart_channel=-1;
uint8 smart_status = SMART_CH_INIT;
os_timer_t channel_timer;
uint8 current_channel;
uint16 channel_bits;

SLIST_HEAD(router_info_head, router_info) router_list;

void wifi_scan_done(void *arg, STATUS status);
void smartconfig_end(uint8* buf);

uint8 ICACHE_FLASH_ATTR
calcrc_1byte(uint8 onebyte)    
{    
    uint8 i,crc_1byte;     
    crc_1byte=0;               
    for(i = 0; i < 8; i++)    
     {    
       if(((crc_1byte^onebyte)&0x01)) 
       {    
         crc_1byte^=0x18;     
         crc_1byte>>=1;    
         crc_1byte|=0x80;    
       }else{
         crc_1byte>>=1; 
       }     
             
       onebyte>>=1;          
      }   
    return crc_1byte;   
}   

uint8 ICACHE_FLASH_ATTR
calcrc_bytes(uint8 *p,uint8 len)  
{  
  uint8 crc=0;  
  while(len--)
  {  
    crc=calcrc_1byte(crc^(*p));
    p++;
  }  
  os_printf("[smart] calcrc_bytes: %02x \n",crc);
  return crc;
}

/******************************************************************************
 * FunctionName : smart_config_decode
 * Description  :decode the packet.
 * Parameters   :  pOneByte -- the packet buf.
 * Returns      : none
*******************************************************************************/
int ICACHE_FLASH_ATTR
smart_config_decode(uint8* pOneByte)
{
    int i = 0;
    uint8 pos=0,val=0;
    uint8 n0,n1;
    static uint8 smart_recvbuf_len=0;    
    static uint8 smart_recvbuf[RECEVEBUF_MAXLEN];
    
    if( !(0==pOneByte[0]&&0==pOneByte[1]) ) return -9;
    
    n0 = (pOneByte[2]-1);
    n1 = (pOneByte[3]-1);
    if( (n1+n0) != 15 ) return -1;
    
    pos = (n0&0xf);
    n0 = (pOneByte[4]-1);
    n1 = (pOneByte[5]-1);
    if( (n1+n0) != 15 ) return -2;
    
    pos |= (n0&0xf)<<4;
    n0 = (pOneByte[6]-1);
    n1 = (pOneByte[7]-1);
    if( (n1+n0) != 15 ) return -3;

    val = (n0&0xf);
    n0 = (pOneByte[8]-1);
    n1 = (pOneByte[9]-1);
    if( (n1+n0) != 15 ) return -4;

    val |= (n0&0xf)<<4;
    if( (int)pos >= (int)RECEVEBUF_MAXLEN ) return -5;

    if( smart_recvbuf[pos] == val && smart_recvbuf_len>2 ){//begin crc8
        if(calcrc_bytes(smart_recvbuf,smart_recvbuf_len) == smart_recvbuf[smart_recvbuf_len] ){
            smart_recvbuf[smart_recvbuf_len+1]='\0';
            
            smartconfig_end(smart_recvbuf);
            
            return 0;
        }
        
      os_printf("[smart]:%s\n",smart_recvbuf);
    }else{
    
      smart_recvbuf[pos] = val;
      os_printf("[%02x]=%02x\n",pos,val);
      if( pos > smart_recvbuf_len ){
        smart_recvbuf_len = pos;
      }
      return SMART_CH_LOCKED;
   }
    
    return 99;
}

/******************************************************************************
 * FunctionName : smart_analyze
 * Description  : smart_analyze the packet.
 * Parameters   :  buf -- the packet buf.
                            len -- the packet len.
                            channel -- the packet channel.
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR 
smart_analyze(uint8* buf,int len,int channel)
{
  int funret;
  static uint32 smart_lock_time=0;
  static uint8  smart_onbyte_Idx=0;
  static uint8  smart_onbyte[10];
  //1.th the packet len is under 16.
  if( len > 16 ) return;
  if( (system_get_time() - smart_lock_time) > 70000)
  {
    smart_status =SMART_CH_INIT; 
  }
  if(SMART_CH_INIT == smart_status && len == 0)
  {
    if(smart_lock_flag == 0)
    {
      os_memcpy(smart_mac,buf,6);
    }
    
    smart_channel=channel;
    smart_lock_time=system_get_time();
    smart_status = SMART_CH_LOCKING;
    smart_onbyte_Idx = 0;
    smart_onbyte[smart_onbyte_Idx++]=len;
  
  }else if( SMART_CH_LOCKING == smart_status ){
  
      if( os_memcmp(smart_mac,buf,6) == 0 ) {
          if( len == 0){
            smart_onbyte[smart_onbyte_Idx++]=len;
            smart_status = SMART_CH_LOCKED;
          }else{
            smart_status =SMART_CH_INIT; 
            smart_onbyte_Idx=0;
          }
          smart_lock_time=system_get_time();
      }
      
  }else if( SMART_CH_LOCKED == smart_status ){
  
        if( os_memcmp(smart_mac,buf,6) == 0 ){
            if( len == 0 ){
              if(smart_lock_flag == 0){
                os_memcpy(smart_mac,buf,6);
               }
              smart_channel=channel;
              smart_status = SMART_CH_LOCKING;
              smart_onbyte_Idx = 0;
              smart_onbyte[smart_onbyte_Idx++]=len;
              os_printf("[smart] resync  into SMART_CH_LOCKING\n");
            }else{
              smart_onbyte[smart_onbyte_Idx++]=len; 
            }
            smart_lock_time=system_get_time();
        }
        
  }
   
  if(smart_onbyte_Idx>=10)
  {
    funret = smart_config_decode(smart_onbyte);
    os_printf("[smart] Decode ret=%d\n",funret);
    if( SMART_CH_LOCKED == funret )
    {
      smart_status = SMART_CH_LOCKED ;
      smart_lock_flag = 1;
    }
    smart_status = SMART_CH_INIT; 
    smart_onbyte_Idx=0;
  }
}


/******************************************************************************
 * FunctionName : wifi_promiscuous_rx
 * Description  : sniffer callback.
 * Parameters   :  buf -- the packet buf.
                            len -- the packet len.
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
wifi_promiscuous_rx(uint8 *buf, uint16 len)
{
  uint16 i;

  struct router_info *info = NULL;
  //os_printf("[smart] len = %d,buf = %02x %02x\n",len, buf[0],buf[1]);

  //1.th check the bssid and channel of the packet. 
  SLIST_FOREACH(info, &router_list, next) {
    if ((buf[1] & 0x02) == 0x02) {  
      if (os_memcmp(info->bssid, buf + 10, 6) == 0) {
        if (current_channel - 1 != info->channel) { // check channel
          return;
        } else {
          break;
        }
      }
    }
  }

  if (info == NULL) {
    return;
  }
  
  //2.th 0x08 and 0x88 is wifi Data frame
  if ((buf[0] == 0x08 || buf[0] == 0x88) && (buf[1] & 0x02) == 0x02) {
    if (info->rx_seq != (*(uint16 *)(buf + 22)) >> 4) {
      info->rx_seq = (*(uint16 *)(buf + 22)) >> 4;

      if (info->encrytion_mode != 0) {
        if ((buf[0] & 0x80) == 0) {
          len -= 24;
        } else {
          len -= 26;
        }
        if (info->encrytion_mode == ENCRY_NONE) {
          len -= 40;
        } else if (info->encrytion_mode == ENCRY_WEP){
          len = len - 40 - 4 - 4;
        } else if (info->encrytion_mode == ENCRY_TKIP) {
          len = len - 40 - 12 - 8;
        } else if (info->encrytion_mode == ENCRY_CCMP) {
          len = len - 40 - 8 - 8;
        }
        
        //4.th check the packet len. 
        //os_printf(MACSTR ",len --- %d\n", MAC2STR(buf + 10), len);
        smart_analyze(buf+10,len,info->channel);
        
      } else {
        if (info->authmode == AUTH_OPEN) {
          info->encrytion_mode = ENCRY_NONE;
          os_printf(MACSTR ", channel %d, mode %d\n", MAC2STR(info->bssid), info->channel, info->encrytion_mode);
        } else if (info->authmode == AUTH_WEP) {
          info->encrytion_mode = ENCRY_WEP;
          os_printf(MACSTR ", channel %d, mode %d\n", MAC2STR(info->bssid), info->channel, info->encrytion_mode);
        } else {
        
          //3.th set the bssid encrytion_mode by the packet
          if (info->iv_check == 0) {
            if (buf[0] == 0x08) {
              os_memcpy(info->iv, buf + 24, 8);
            } else if (buf[0] == 0x88) {
              os_memcpy(info->iv, buf + 26, 8);
            }
            info->iv_check = 1;
          } else {
            uint8 *local_iv;
            if (buf[0] == 0x08) {
              local_iv = buf + 24;
            } else if (buf[0] == 0x88) {
              local_iv = buf + 26;
            }
            if (info->iv[2] == local_iv[2] && local_iv[2] == 0) {
              info->encrytion_mode = ENCRY_CCMP;
              os_printf(MACSTR ", channel %d, mode %d\n", MAC2STR(info->bssid), info->channel, info->encrytion_mode);
            } else {
              info->encrytion_mode = ENCRY_TKIP;
              os_printf(MACSTR ", channel %d, mode %d\n", MAC2STR(info->bssid), info->channel, info->encrytion_mode);
            }
          }
        }
      }
    }
  }
}


/******************************************************************************
 * FunctionName : channel_timer_cb
 * Description  : change channel to sniffer the current packet.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
channel_timer_cb(void *arg)
{
  uint8 i;
  //1.th If find one channel send smartdata,lock on this.
  if( smart_channel > 0 && smart_lock_flag == 1)
  {
    wifi_set_channel(smart_channel);
    os_printf("[smart] locked Smartlink channel=%d\n",smart_channel);
    return;
  }

  //2.th scan channel by timer 
  for (i = current_channel; i < 14; i++) {
    if ((channel_bits & (1 << i)) != 0) {
      current_channel = i + 1;
      wifi_set_channel(i);
       os_printf("[smart] current channel2 %d---%d\n", i, system_get_time());
      os_timer_arm(&channel_timer, SCAN_TIME, 0);
      break;
    }
  }

  if (i == 14) {
    current_channel = 1;
    for (i = current_channel; i < 14; i++) {
      if ((channel_bits & (1 << i)) != 0) {
        current_channel = i + 1;
        wifi_set_channel(i);
         os_printf("[smart] current channel3%d---%d\n", i, system_get_time());
        os_timer_arm(&channel_timer, SCAN_TIME, 0);
        break;
      }
    }
  }
}

/******************************************************************************
 * FunctionName : wifi_scan_done
 * Description  : after scan done ,get the ap info, start scan channel
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
wifi_scan_done(void *arg, STATUS status)
{
  uint8 ssid[33];

  channel_bits = 0;
  current_channel = 1;

  struct router_info *info = NULL;
  
  //1.th check the router list. 
  while((info = SLIST_FIRST(&router_list)) != NULL){
    SLIST_REMOVE_HEAD(&router_list, next);

    os_free(info);
  }

  //2.th get the bss info. 
  if (status == OK) {
    uint8 i;
    struct bss_info *bss = (struct bss_info *)arg;

    while (bss != NULL) {
      os_memset(ssid, 0, 33);

      if (os_strlen(bss->ssid) <= 32) {
        os_memcpy(ssid, bss->ssid, os_strlen(bss->ssid));
      } else {
        os_memcpy(ssid, bss->ssid, 32);
      }

      if (bss->channel != 0) {
        struct router_info *info = NULL;

        os_printf("[smart] ssid %s, channel %d, authmode %d, rssi %d\n",
            ssid, bss->channel, bss->authmode, bss->rssi);
        channel_bits |= 1 << (bss->channel);

        info = (struct router_info *)os_zalloc(sizeof(struct router_info));
        info->authmode = bss->authmode;
        info->channel = bss->channel;
        os_memcpy(info->bssid, bss->bssid, 6);

        SLIST_INSERT_HEAD(&router_list, info, next);
      }
      bss = STAILQ_NEXT(bss, next);
    }

    for (i = current_channel; i < 14; i++) {
      if ((channel_bits & (1 << i)) != 0) {
        current_channel = i + 1;
        wifi_set_channel(i);
        os_printf("[smart] current channel1 %d---%d\n", i, system_get_time());
        break;
      }
    }
    
    //3.th start sniffer and scan channel. 
    wifi_promiscuous_enable(1);
    wifi_set_promiscuous_rx_cb(wifi_promiscuous_rx);

    os_timer_disarm(&channel_timer);
    os_timer_setfn(&channel_timer, channel_timer_cb, NULL);
    os_timer_arm(&channel_timer, SCAN_TIME, 0);
  } else {
    os_printf("[smart] err, scan status %d\n", status);
  }
}

/******************************************************************************
 * FunctionName : smartconfig_init
 * Description  : smartconfig_init
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
smartconfig_init(void)
{
  //1.th init the router info 
	SLIST_INIT(&router_list);
  
  //2.th scan wifi 
	wifi_station_scan(NULL,wifi_scan_done);
}


/******************************************************************************
 * FunctionName : smartconfig_end
 * Description  : smartconfig_end
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
smartconfig_end(uint8* buf)
{
  wifi_promiscuous_enable(0);

  os_printf("*************************************\n");
  os_printf("Get the smartconfig result:\n");
  os_printf("%s\n",buf);
  os_printf("*************************************\n");
}

