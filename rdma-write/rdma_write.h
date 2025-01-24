#ifndef RDMA_WRITE_H
#define RDMA_WRITE_H

#include "../common.h"
DEFINE_RDMA_MODE_HEADER(rw)
void rw_post_write(struct config_t* config, void* local_buf, struct qp_info_t* remote_info);
int rw_run_client(const char* server_name);
int rw_run_server(void);

#endif // RDMA_WRITE_H
