#ifndef SEND_RECEIVE_H
#define SEND_RECEIVE_H

#include "../common.h"

// Server function - runs the send-receive server
int sr_run_server(void);

// Client function - connects to server and handles send-receive operations
int sr_run_client(const char *server_name);
void sr_post_send(struct config_t *config, const char *message);

DEFINE_RDMA_MODE_HEADER(sr)

#endif // SEND_RECEIVE_H
