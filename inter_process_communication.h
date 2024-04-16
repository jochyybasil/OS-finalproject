#pragma once

#define MAX_MESSAGES 10
#define MESSAGE_SIZE 5

#define MAX_MESSAGE_QUEUE_SIZE 10
#define MAX_MESSAGE_SIZE 5


// Data structures
struct Message{
    int sender_id;
    char content[MAX_MESSAGE_SIZE];
};

struct MessageQueue {
    struct Message messages[MAX_MESSAGE_QUEUE_SIZE];
    int front;
    int rear;
    int count;
};

void init_ipc();
void send_message(int sender_id, const char* content);
struct Message receive_message();
void broadcast_message(int sender_id, const char* content);
void write_to_shared_memory(const char* data);
char* read_from_shared_memory();
void cleanup_ipc();

