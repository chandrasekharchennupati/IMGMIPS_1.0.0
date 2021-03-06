#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "os/os.h"
#include "bsp/bsp.h"

/* BLE */
#include "nimble/hci_common.h"
#include "nimble/ble_hci_trans.h"
#include "controller/ble_ll.h"

#define H4_CMD                   1
#define H4_ACL                   2
#define H4_EVENT                 4

#define BLE_IMG_LL_STACK_SIZE             (OS_STACK_ALIGN(288))


struct os_task ble_img_ll_task;
bssnz_t os_stack_t ble_img_ll_stack[BLE_IMG_LL_STACK_SIZE];
extern int ble_hs_rx_data(struct os_mbuf *om);

#define FILE_NAME "/dev/ttyHS0"
//#define FILE_NAME "/home/krishnab/bt_krishna_p4/ensigma/sw/connectivity/whisper/MAIN/bt/support/HCI_TRANSPORT/source/bash/topDevice"
//#define FILE_NAME "/home/krishnab/bt_p4_240117/ensigma/sw/connectivity/whisper/DEV/BASE/commsStacks/bt/support/HCI_TRANSPORT/source/bash/topDevice"

int gFd;

//static struct log_handler ble_ll_log_console_handler;
struct log ble_ll_log;

int
ble_ll_hci_send_noop(void)
{
    int rc;
    uint8_t *evbuf;
    uint16_t opcode;

    evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
    if (evbuf) {
        /* Create a command complete event with a NO-OP opcode */
        opcode = 0;
        evbuf[0] = BLE_HCI_EVCODE_COMMAND_COMPLETE;
        evbuf[1] = 3;
        evbuf[2] = 1;
        htole16(evbuf + 3, opcode);
        ble_hci_trans_ll_evt_tx(evbuf);
        rc = BLE_ERR_SUCCESS;
    } else {
        rc = BLE_ERR_MEM_CAPACITY;
    }

    return rc;
}

/**
 * BLE_ll test task
 *
 * @param arg
 */
static void
ble_img_ll_task_handler(void *arg)
{

    int noBytes = 0;
    int fd;
    uint8_t *evbuf;
    struct os_mbuf *om;
    uint8_t opCode[256] = {0};
    int flags = 0;
    int rc;

    ble_ll_hci_send_noop();
    fd = open(FILE_NAME, O_RDONLY);

    while (1) {

       flags = fcntl(fd, F_GETFL, 0);
       fcntl(fd, F_SETFL, flags & (~O_NONBLOCK));
       noBytes = read(fd, &opCode, sizeof(opCode));

       BLELL_LOG(DEBUG,"No. of Bytes read from Device %d \n",noBytes);
       BLELL_LOG(DEBUG,"RECVD EVENT VALUE %x\n", opCode[0]);

       if(opCode[0] == H4_ACL)
       {
          BLELL_LOG(DEBUG,"acl Data RECEIVED\n");
          om = os_msys_get_pkthdr(0, 0);
          if(om != NULL)
          {
             rc = os_mbuf_copyinto(om, 0, opCode + 1, noBytes - 1);
             if(!rc)
                ble_hs_rx_data(om);
             else
                BLELL_LOG(DEBUG,"ACL DATA copy to MBUF FAILED \n");
          }
          else
          {
             BLELL_LOG(DEBUG,"ACL BUFFER ALLOCATION FAILED \n");
             assert(om == 0);
          }
       }
       else // EVENT Received
       {
          evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
          if(evbuf)
          {
             memcpy(evbuf, opCode + 1, noBytes - 1);
             ble_hci_trans_ll_evt_tx(evbuf);
          }
          else
             assert(evbuf == 0);
       }
    }
    close(fd);
}

int ble_ll_init(void)
{
    log_register("ble_ll", &ble_ll_log, &log_console_handler, NULL, LOG_SYSLEVEL);

    os_task_init(&ble_img_ll_task, "ble_img_ll", ble_img_ll_task_handler,
                 NULL, MYNEWT_VAL(BLE_LL_PRIO), OS_WAIT_FOREVER,
                 ble_img_ll_stack, BLE_IMG_LL_STACK_SIZE);
    ble_hci_trans_cfg_ll(ble_ll_hci_cmd_rx, NULL, ble_ll_hci_acl_rx, NULL);
    return 0;
}

