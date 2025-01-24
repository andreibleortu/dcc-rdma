#include "../common.h"

static void rd_post_read(struct config_t *config, uint64_t remote_offset, size_t length, struct qp_info_t *remote_info)
{
    remote_info->addr += remote_offset;
    post_operation(config, OP_READ, NULL, remote_info, length);
    remote_info->addr -= remote_offset;  // Reset the address for future operations
}

// Server loop for read operations
static void rd_server_loop(struct config_t *config)
{
    printf("Enter text to store: ");
    fflush(stdout);
    
    char input[MAX_BUFFER_SIZE];
    if (fgets(input, MAX_BUFFER_SIZE, stdin)) {
        size_t len = strlen(input);
        if (input[len - 1] == '\n')
            input[--len] = '\0';
        memcpy(config->buf, input, len + 1);
        printf("Waiting for client read requests...\n");
        while (1)
            sleep(1);
    }
}

int rd_run_server(void)
{
    struct config_t config = {};
    
    if (setup_rdma_connection(&config, NULL, MODE_READ, NULL) != RDMA_SUCCESS) {
        return -1;
    }
    
    printf("Read Server ready.\n");
    rd_server_loop(&config);
    
    cleanup_resources(&config);
    return 0;
}

int rd_run_client(const char *server_name)
{
    struct config_t config = {};
    struct qp_info_t remote_info;
    
    if (setup_rdma_connection(&config, server_name, MODE_READ, &remote_info) != RDMA_SUCCESS) {
        return -1;
    }
    
    printf("Connected to server.\n");
    printf("Enter character range to read (format: start_pos end_pos):\n");
    printf("Example: 0 5 to read first 6 characters\n");
    
    char input[MAX_BUFFER_SIZE];
    while (fgets(input, MAX_BUFFER_SIZE, stdin)) {
        int start, end;
        if (sscanf(input, "%d %d", &start, &end) == 2) {
            if (start < 0 || end < start || end >= MAX_BUFFER_SIZE) {
                printf("Invalid range. start must be >= 0, end must be >= start and < %d\n", MAX_BUFFER_SIZE);
                continue;
            }
            size_t read_len = end - start + 1;
            rd_post_read(&config, start, read_len, &remote_info);
            wait_completion(&config);
            printf("Read data (%zu bytes from position %d): %.*s\n", 
                   read_len, start, (int)read_len, (char *)config.buf);
        } else {
            printf("Invalid input. Please enter two numbers: start_pos end_pos\n");
        }
    }

    cleanup_resources(&config);
    return 0;
}
