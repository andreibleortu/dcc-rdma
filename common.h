#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <infiniband/verbs.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#define MAX_BUFFER_SIZE 4096
#define TCP_PORT 18515
#define IB_PORT 1
#define GID_INDEX 1

#define DEBUG 1  // Set to 0 to disable debug messages

#define DEBUG_LOG(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, "[DEBUG][%s:%d] " fmt "\n", \
        __FILE__, __LINE__, ##__VA_ARGS__); } while (0)

#define ERROR_LOG(fmt, ...) \
    fprintf(stderr, "[ERROR][%s:%d] " fmt "\n", \
        __FILE__, __LINE__, ##__VA_ARGS__);

// Add this macro for consistent header guards
#define DEFINE_RDMA_MODE_HEADER(prefix)                                                                                \
	int prefix##_run_client(const char *server_name);                                                                  \
	int prefix##_run_server(void);

// Constants grouping
#define DEFAULT_MAX_WR 10
#define DEFAULT_MAX_SGE 1

// Add these definitions
#define CQ_SIZE 128
#define MAX_INLINE_DATA 256
#define MAX_SGE 4

// Add error handling macros
#define CHECK_NULL(ptr, msg) do { if (!(ptr)) { fprintf(stderr, "%s\n", msg); return RDMA_ERR_RESOURCE; } } while(0)

typedef enum rdma_mode { MODE_SEND_RECV, MODE_WRITE, MODE_READ, MODE_LAMBDA } rdma_mode_t;

typedef enum rdma_op { OP_SEND, OP_WRITE, OP_READ } rdma_op_t;

typedef enum { RDMA_SUCCESS = 0, RDMA_ERR_DEVICE, RDMA_ERR_RESOURCE, RDMA_ERR_COMMUNICATION } rdma_status_t;

struct config_t {
	struct ibv_context *context;
	struct ibv_pd *pd;
	struct ibv_cq *cq;
	struct ibv_qp *qp;
	struct ibv_mr *mr;
	void *buf;
	union ibv_gid gid;
	int sock_fd;
};

// Update QP info struct to remove lid
struct qp_info_t {
	uint32_t qp_num;
	union ibv_gid gid;
	uint64_t addr; // Remote buffer address
	uint32_t rkey; // Remote key for RDMA operations
};

// Error handling
void die(const char *message);
void die_with_cleanup(const char *message, struct config_t *config);

// Resource management
void cleanup_resources(struct config_t *config);

/**
 * @brief Initializes RDMA resources
 * @param config Configuration structure to initialize
 * @param mode RDMA operation mode
 * @return RDMA status code
 */
rdma_status_t init_resources(struct config_t *config, rdma_mode_t mode);

// Queue Pair operations
void modify_qp_to_init(struct ibv_qp *qp, int access_flags);
void modify_qp_to_rtr(struct ibv_qp *qp, uint32_t remote_qpn, union ibv_gid remote_gid);
void modify_qp_to_rts(struct ibv_qp *qp);

// Connection management
void setup_socket(struct config_t *config, const char *server_name);
void exchange_qp_info(
	struct config_t *config, const char *server_name, struct qp_info_t *local_info, struct qp_info_t *remote_info);
void connect_qps(struct config_t *config, const char *server_name, struct qp_info_t *remote_info, rdma_mode_t mode);

// Common operations
void wait_completion(struct config_t *config);
void post_receive(struct config_t *config);

// Unified operation posting function
void post_operation(
	struct config_t *config, rdma_op_t op, const char *data, const struct qp_info_t *remote_info, size_t length);

// Run functions for client/server
int run_client(const char *server_name, rdma_mode_t mode);
int run_server(rdma_mode_t mode);

// Signal handling
extern struct config_t *global_config;
void signal_handler(int signo);
void handle_disconnect(struct config_t *config);  // Add this declaration

// Add helper function declaration
rdma_status_t setup_rdma_connection(struct config_t *config, const char *server_name, 
                                  rdma_mode_t mode, struct qp_info_t *remote_info);

#endif
