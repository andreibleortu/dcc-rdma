#ifndef LAMBDA_H
#define LAMBDA_H

#include "../common.h"
#include "../rdma-write/rdma_write.h"
#include "../rdma-read/rdma_read.h"
#include "../send-receive/send_receive.h"
#include <fcntl.h>
#include <dlfcn.h>

#define LAMBDA_MAX_FUNCTION_NAME 128
#define LAMBDA_MAX_CODE_SIZE (1024 * 1024 * 3)  // 1MB
#define LAMBDA_MAX_INPUT_SIZE (MAX_BUFFER_SIZE)
#define LAMBDA_MAX_OUTPUT_SIZE (MAX_BUFFER_SIZE)

typedef int (*lambda_fn)(void* input, size_t input_size, void* output, size_t* output_size);

struct lambda_metadata {
    char function_name[LAMBDA_MAX_FUNCTION_NAME];
    size_t code_size;
    size_t input_size;
    uint64_t entry_offset;
};

struct lambda_memory_regions {
    void* code_region;
    void* input_region;
    void* output_region;
    struct ibv_mr* code_mr;
    struct ibv_mr* input_mr;
    struct ibv_mr* output_mr;
};

struct lambda_config {
    struct config_t data_qp;   // For code and data transfer (RDMA Write)
    struct config_t ctrl_qp;   // For control messages (Send/Recv)
};

// RDMA operations for lambda mode
int post_lambda_send(struct config_t* config, void* buf);
int post_lambda_receive(struct config_t* config);
int post_lambda_write(struct config_t* config, void* buf, struct qp_info_t* remote_info);

// Server/Client entry points
int lambda_run_server(void);
int lambda_run_client(const char* server_name);

DEFINE_RDMA_MODE_HEADER(lambda)

#endif