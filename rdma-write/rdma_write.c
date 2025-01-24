#include "../common.h"

static void rw_post_write(struct config_t *config, const char *message, struct qp_info_t *remote_info)
{
    size_t length = strlen(message) + 1;
    post_operation(config, OP_WRITE, message, remote_info, length);
}

// Server function to handle incoming writes
static void rw_server_loop(struct config_t *config)
{
    while (1) {
        // Post a receive work request to get immediate data
        post_receive(config);
        
        // Wait for completion
        struct ibv_wc wc;
        while (ibv_poll_cq(config->cq, 1, &wc) == 0)
            ;
            
        if (wc.status != IBV_WC_SUCCESS) {
            fprintf(stderr, "Completion error: %s\n", ibv_wc_status_str(wc.status));
            continue;
        }

        // wc.imm_data contains the length sent by the client
        uint32_t received_len = ntohl(wc.imm_data);
        printf("Received (%u bytes): %s\n", received_len, (char *)config->buf);
        fflush(stdout);
    }
}

// These are the mode-specific implementations
int rw_run_server(void)
{
    struct config_t config = {};
    
    if (setup_rdma_connection(&config, NULL, MODE_WRITE, NULL) != RDMA_SUCCESS) {
        return -1;
    }
    
    printf("Write Server ready.\n");
    rw_server_loop(&config);
    
    cleanup_resources(&config);
    return 0;
}

int rw_run_client(const char *server_name)
{
    struct config_t config = {};
    struct qp_info_t remote_info;
    
    if (setup_rdma_connection(&config, server_name, MODE_WRITE, &remote_info) != RDMA_SUCCESS) {
        return -1;
    }
    
    printf("Connected to server. Enter messages (Ctrl+D to stop):\n");
    
    char input[MAX_BUFFER_SIZE];
    while (fgets(input, MAX_BUFFER_SIZE, stdin)) {
        size_t len = strlen(input);
        if (input[len - 1] == '\n') {
            input[--len] = '\0';
        }
        
        if (len == 0) continue;
        
        // Post the write with immediate operation
        rw_post_write(&config, input, &remote_info);
        
        // Wait for completion of the write
        wait_completion(&config);
        
        printf("Message sent successfully\n");
    }

    cleanup_resources(&config);
    return 0;
}
