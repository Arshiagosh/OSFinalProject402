#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFSIZE 4096

void error(char *msg) {
    perror(msg);
    exit(1);
}

typedef struct {
    char *source_path;
    char *dest_address;
    int split_size;
} config_t;

void *transfer(void *arg) {
    config_t *config = (config_t *) arg;
    
    // Open the source file
    FILE *src_file = fopen(config->source_path, "rb");
    if (!src_file) {
        fprintf(stderr, "Error opening source file: %s\n", config->source_path);
        pthread_exit(NULL);
    }
    
    // Create the socket for transfer
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        error("Error creating socket");
    }
    
    // Connect to the destination address
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_aton(config->dest_address, &dest_addr.sin_addr);
    dest_addr.sin_port = htons(9999);
    if (connect(socket_fd, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) == -1) {
        error("Error connecting to destination address");
    }
    
    // Send the file name to the destination system
    char *filename = strrchr(config->source_path, '/') + 1;
    send(socket_fd, filename, strlen(filename) + 1, 0);
    
    // Send the file size to the destination system
    fseek(src_file, 0L, SEEK_END);
    long int file_size = ftell(src_file);
    fseek(src_file, 0L, SEEK_SET);
    char size_buf[32];
    snprintf(size_buf, sizeof(size_buf), "%ld", file_size);
    send(socket_fd, size_buf, strlen(size_buf) + 1, 0);
    
    // Send the file data to the destination system in chunks
    char data[BUFSIZE];
    while (file_size > 0) {
        int chunk_size = file_size < config->split_size ? file_size : config->split_size;
        fread(data, sizeof(char), chunk_size, src_file);
        if (send(socket_fd, data, chunk_size, 0) == -1) {
            error("Error sending file data");
        }
        file_size -= chunk_size;
    }
    
    // Close the socket and file
    close(socket_fd);
    fclose(src_file);
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s [source_path] [destination_address] [file splitting size]\n", argv[0]);
        exit(1);
    }
    
    // Parse the command line arguments
    config_t config;
    config.source_path = argv[1];
    config.dest_address = argv[2];
    config.split_size = atoi(argv[3]);
    
    // Create a thread for each file being transferred
    pthread_t threads[argc - 1];
    for (int i = 1; i < argc - 1; i++) {
        config_t *thread_config = malloc(sizeof(config_t));
        memcpy(thread_config, &config, sizeof(config_t));
        thread_config->source_path = argv[i];
        if (pthread_create(&threads[i - 1], NULL, transfer, thread_config) == -1) {
            error("Error creating thread");
        }
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < argc - 2; i++) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}