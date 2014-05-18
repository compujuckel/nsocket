#ifndef NSOCKET_H
#define NSOCKET_H

#include <os.h>

typedef void* nn_ch_t;
typedef void* nn_nh_t;
typedef void* nn_oh_t;

int ns_init();
int ns_connect(char* host, short port);
int ns_send(void* buf, unsigned int len);
int ns_recv(void* buf, unsigned int len);
void ns_set_timeout(unsigned int timeout_ms);
unsigned int ns_get_timeout();
unsigned int ns_get_pktsize();
void ns_stop();

#endif