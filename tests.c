#include "common.h"
#include "send-receive/send_receive.h"
#include "rdma-write/rdma_write.h"
#include "rdma-read/rdma_read.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>

void print_usage() {
    printf("Usage:\n");
    printf("  Server mode:\n");
    printf("    ./rdma send              - Run send-receive server\n");
    printf("    ./rdma write             - Run RDMA write server\n");
    printf("    ./rdma read              - Run RDMA read server\n");
    printf("    ./rdma lambda            - Run Lambda server\n");
    printf("\n");
    printf("  Client mode:\n");
    printf("    ./rdma send <host>       - Run send-receive client\n");
    printf("    ./rdma write <host>      - Run RDMA write client\n");
    printf("    ./rdma read <host>       - Run RDMA read client\n");
    printf("    ./rdma lambda <host>     - Run Lambda client\n");
}

static void setup_signal_handlers(void) {
    struct sigaction sa = {
        .sa_handler = signal_handler,
        .sa_flags = 0
    };
    sigemptyset(&sa.sa_mask);
    
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

extern struct config_t *global_config;

int main(int argc, char *argv[]) {
    // Add input validation
    if (argc < 2 || argc > 3) {
        print_usage();
        return 1;
    }

    setup_signal_handlers();

    struct config_t config = {};
    global_config = &config;  // Initialize global config pointer

    const char *mode = argv[1];
    const char *host = argc > 2 ? argv[2] : NULL;
    rdma_mode_t rdma_mode;

    // Determine mode
    if (strcmp(mode, "send") == 0) {
        rdma_mode = MODE_SEND_RECV;
    } else if (strcmp(mode, "write") == 0) {
        rdma_mode = MODE_WRITE;
    } else if (strcmp(mode, "read") == 0) {
        rdma_mode = MODE_READ;
    } else if (strcmp(mode, "lambda") == 0) {
        rdma_mode = MODE_LAMBDA;
    } else {
        printf("Unknown mode: %s\n", mode);
        print_usage();
        return 1;
    }

    // Print program configuration
    printf("\n=== RDMA Communication Program Started ===\n");
    printf("Mode: %s (%s)\n", mode, host ? "Client" : "Server");
    printf("Configuration:\n");
    printf("  Buffer size: %d bytes\n", MAX_BUFFER_SIZE);
    printf("  IB port: %d\n", IB_PORT);
    printf("  GID index: %d\n", GID_INDEX);
    printf("  TCP port: %d\n", TCP_PORT);
    fflush(stdout);

    // Run appropriate mode
    int result;
    if (host) {
        printf("Connecting to %s...\n", host);
        result = run_client(host, rdma_mode);
        if (result != 0) {
            fprintf(stderr, "Client operation failed with error code: %d\n", result);
        }
    } else {
        printf("Starting server...\n");
        result = run_server(rdma_mode);
        if (result != 0) {
            fprintf(stderr, "Server operation failed with error code: %d\n", result);
        }
    }
    
    global_config = NULL;  // Clear global config pointer
    return result;
}
