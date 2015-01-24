/*
 *  Copyright (C) 2014 -2015  By younger
 *
 */

#ifndef __SMARTCONFIG_H__
#define __SMARTCONFIG_H__

#include "ets_sys.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"

#define RECEVEBUF_MAXLEN  90
#define SCAN_TIME  1000

typedef enum _encrytion_mode {
    ENCRY_NONE           = 1,
    ENCRY_WEP,
    ENCRY_TKIP,
    ENCRY_CCMP
} ENCYTPTION_MODE;

struct router_info {
    SLIST_ENTRY(router_info)     next;

    u8 bssid[6];
    u8 channel;
    u8 authmode;

    u16 rx_seq;
    u8 encrytion_mode;
    u8 iv[8];
    u8 iv_check;
};

enum smart_status
{
  SMART_CH_INIT = 0x1,
  SMART_CH_LOCKING = 0x2,
  SMART_CH_LOCKED = 0x4
};


void smartconfig_init(void);

#endif
