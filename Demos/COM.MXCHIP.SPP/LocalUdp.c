/**
  ******************************************************************************
  * @file    LocalTcpServer.c 
  * @author  William Xu
  * @version V1.0.0
  * @date    05-May-2014
  * @brief   This file create a TCP listener thread, accept every TCP client
  *          connection and create thread for them.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MXCHIP Inc. SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2014 MXCHIP Inc.</center></h2>
  ******************************************************************************
  */ 

#include "MICO.h"
#include "MICODefine.h"
#include "MICOAppDefine.h"

#include "SppProtocol.h"
#include "SocketUtils.h"
#include "PlatformUart.h"

#define server_log(M, ...) custom_log("UDP", M, ##__VA_ARGS__)
#define server_log_trace() custom_log_trace("UDP")

static void localTcpClient_thread(void *inFd);
static mico_Context_t *Context;

mico_thread_t   localTcpClient_thread_handler;

void localUdp_thread(void *inContext)
{
  OSStatus err;
  Context = inContext;
  int i;
  int loopBackFd = -1;
  uint8_t *inDataBuffer = NULL;
  uint8_t *outDataBuffer = NULL;
  int len;
  struct sockaddr_t addr;
  fd_set readfds;
  socklen_t addrLen;
  struct timeval_t t;
  int localUdp_fd = -1;


  localUdp_fd = socket( AF_INET, SOCK_DGRM, IPPROTO_UDP );
  require_action(IsValidSocket( localUdp_fd ), exit, err = kNoResourcesErr );
  addr.s_ip = INADDR_ANY;
  addr.s_port = Context->flashContentInRam.appConfig.localUDPPort;
  err = bind(localUdp_fd, &addr, sizeof(addr));
  require_noerr( err, exit );

  inDataBuffer = malloc(wlanBufferLen);
  require_action(inDataBuffer, exit, err = kNoMemoryErr);
  outDataBuffer = malloc(wlanBufferLen);
  require_action(outDataBuffer, exit, err = kNoMemoryErr);

  /*Loopback fd, recv data from other thread */
  loopBackFd = socket( AF_INET, SOCK_DGRM, IPPROTO_UDP );
  require_action(IsValidSocket( loopBackFd ), exit, err = kNoResourcesErr );
  addr.s_ip = IPADDR_LOOPBACK;
  addr.s_port = LOCAL_UDP_LOOPBACK_PORT;
  err = bind( loopBackFd, &addr, sizeof(addr) );
  require_noerr( err, exit );

  t.tv_sec = 20;
  t.tv_usec = 0;

  addr.s_ip = INADDR_BROADCAST;
  addr.s_port = 0;
  
  while(1){

    FD_ZERO(&readfds);
    FD_SET(localUdp_fd, &readfds);
    FD_SET(loopBackFd, &readfds);

    select(1, &readfds, NULL, NULL, &t);

    /*recv UART data using loopback fd*/
    if (FD_ISSET( loopBackFd, &readfds )) {
      len = recv( loopBackFd, outDataBuffer, wlanBufferLen, 0 );
      if(addr.s_port == 0)
        continue;
      sendto(localUdp_fd, outDataBuffer, len, 0, &addr, sizeof(addr));
    }

    /*Read data from tcp clients and process these data using HA protocol */ 
    if (FD_ISSET(localUdp_fd, &readfds)) {
      len = recvfrom(localUdp_fd, inDataBuffer, wlanBufferLen, 0, &addr, &addrLen);
      require_action_quiet(len>0, exit, err = kConnectionErr);
      sppWlanCommandProcess(inDataBuffer, &len, localUdp_fd, Context);
    }
  }

exit:
    server_log("Exit: udp thread exit with err = %d", err);
    if(loopBackFd != -1)
      SocketClose(&loopBackFd);
    SocketClose(&localUdp_fd);
    if(inDataBuffer) free(inDataBuffer);
    if(outDataBuffer) free(outDataBuffer);
    mico_rtos_delete_thread(NULL);
    return;
}

