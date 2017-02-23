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

#define BLE_IMG_LL_TASK_PRIO              1
#define BLE_IMG_LL_STACK_SIZE             (OS_STACK_ALIGN(288))


struct os_task ble_img_ll_task;
bssnz_t os_stack_t ble_img_ll_stack[BLE_IMG_LL_STACK_SIZE];
extern int ble_hs_rx_data(struct os_mbuf *om);

//#define FILE_NAME "/home/krishnab/bt_krishna_p4/ensigma/sw/connectivity/whisper/MAIN/bt/support/HCI_TRANSPORT/source/bash/topDevice"
#define FILE_NAME "/home/krishnab/bt_p4_240117/ensigma/sw/connectivity/whisper/DEV/BASE/commsStacks/bt/support/HCI_TRANSPORT/source/bash/topDevice"

int gFd;

//static struct log_handler ble_ll_log_console_handler;
struct log ble_ll_log;

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
    uint16_t codeValue; 
    uint8_t ogf;
    uint16_t ocf;
    int flags = 0;
    int rc;

    fd = open(FILE_NAME, O_RDONLY);

    while (1) {

       flags = fcntl(fd, F_GETFL, 0);
       fcntl(fd, F_SETFL, flags & (~O_NONBLOCK));
       noBytes = read(fd, &opCode, sizeof(opCode));

       BLELL_LOG(DEBUG,"No. of Bytes read from Device %d \n",noBytes);
       BLELL_LOG(DEBUG,"RECVD EVENT VALUE %x\n", opCode[0]);
       printf("No. of Bytes read from Device %d \n",noBytes);
//       printf("RECVD EVENT VALUE %x %x %x %x %x %x %x\n", opCode[0], opCode[1], opCode[2], opCode[3], opCode[4],opCode[5],opCode[6]);
printf("No. of Bytes read from Device %d %x %x %x %x %x %x %x\n",noBytes, opCode[0], opCode[1], opCode[2], opCode[3], opCode[4], opCode[5], opCode[6]);

    if(opCode[1] == 0xff || opCode[1] == 0x00)
    {
      opCode[0] = 0x02;
      opCode[1] = 0x48;
    }
    codeValue = le16toh(opCode);
    ocf = BLE_HCI_OCF(codeValue);
    ogf = BLE_HCI_OGF(codeValue);
    printf("OCF %x OGF %x >>>>> \n", ocf, ogf);
       if(opCode[0] == H4_ACL)
       {
          BLELL_LOG(DEBUG,"acl Data RECEIVED\n");
          printf("ACL DATA RECEIVED \n");
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
//          printf("EVENT RECEIVED \n");
          evbuf = ble_hci_trans_buf_alloc(BLE_HCI_TRANS_BUF_EVT_HI);
          if(evbuf)
          {
             memcpy(evbuf, opCode + 1, noBytes - 1);
             ble_hci_trans_ll_evt_tx(evbuf);
//             ble_hci_transport_ctlr_event_send(evbuf);
          }
          else
             assert(evbuf == 0);
       }
    }
    close(fd);
}

int ble_ll_init(void)
{
//   log_init();
//   log_console_handler_init(&ble_ll_log_console_handler);
   log_register("ble_ll", &ble_ll_log, &log_console_handler, NULL, LOG_SYSLEVEL);

   os_task_init(&ble_img_ll_task, "ble_img_ll", ble_img_ll_task_handler,
                 NULL, 3, OS_WAIT_FOREVER,
                 ble_img_ll_stack, BLE_IMG_LL_STACK_SIZE);
   ble_hci_trans_cfg_ll(ble_ll_hci_cmd_rx, NULL, ble_ll_hci_acl_rx, NULL);
   return 0;
}

