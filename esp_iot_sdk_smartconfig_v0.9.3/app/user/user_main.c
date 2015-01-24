/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
*******************************************************************************/
#include "smartconfig.h"

/******************************************************************************
 * FunctionName : system_init_done
 * Description  : after system init done ,start smartconfig
 * Parameters   : none
 * Returns      : none
*******************************************************************************/

void ICACHE_FLASH_ATTR
system_init_done(void)
{
  smartconfig_init();
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
  uart_init(115200,115200);
	wifi_set_opmode(STATION_MODE);
	system_init_done_cb(system_init_done);
}

