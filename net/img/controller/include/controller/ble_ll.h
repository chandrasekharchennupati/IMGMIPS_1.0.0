#ifndef H_BLELL_
#define H_BLELL_

#include "log/log.h"
uint8_t g_dev_addr[BLE_DEV_ADDR_LEN];
extern struct log ble_ll_log;
#define BLELL_LOG_MODULE (LOG_MODULE_PERUSER + 9)
#define BLELL_LOG(lvl, ...) LOG_ ## lvl(&ble_ll_log, BLELL_LOG_MODULE, __VA_ARGS__)
extern int ble_ll_init(void);
extern int ble_ll_hci_cmd_rx(uint8_t *cmd, void *arg);
extern int ble_ll_hci_acl_rx(struct os_mbuf *om, void *arg);

#endif
