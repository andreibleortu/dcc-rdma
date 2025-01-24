#include "../common.h"

void sr_post_send(struct config_t *config, const char *message)
{
    post_operation(config, OP_SEND, message, NULL, strlen(message) + 1);
}

// Server loop for send-receive operations
static void sr_server_loop(struct config_t *config)
{
    while (1) {
        post_receive(config);
        wait_completion(config);
        printf("Received: %s\n", (char *)config->buf);
        fflush(stdout);
        
        sr_post_send(config, "ACK");
        wait_completion(config);
    }
}

int sr_run_server(void)
{
    struct config_t config = {};
    
    if (setup_rdma_connection(&config, NULL, MODE_SEND_RECV, NULL) != RDMA_SUCCESS) {
        return -1;
    }
    
    printf("Send-Receive Server ready.\n");
    sr_server_loop(&config);
    
    cleanup_resources(&config);
    return 0;
}

int sr_run_client(const char *server_name)
{
    struct config_t config = {};
    struct qp_info_t remote_info;
    
    if (setup_rdma_connection(&config, server_name, MODE_SEND_RECV, &remote_info) != RDMA_SUCCESS) {
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
        
        sr_post_send(&config, input);
        wait_completion(&config);
        
        // Wait for ACK
        post_receive(&config);
        wait_completion(&config);
        printf("Server acknowledged\n");
    }

    cleanup_resources(&config);
    return 0;
}
