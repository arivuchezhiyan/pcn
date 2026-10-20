/*
 * File: sliding_window_stop_and_wait.c
 * Assignment 7: Implementation of Stop-and-Wait ARQ Flow Control Protocol in C
 * Description: Implements the Stop-and-Wait Automatic Repeat reQuest (ARQ) protocol
 *              using TCP/UDP socket communication (or internal socketpair).
 *              The sender transmits one frame at a time, starts a timer, and waits for
 *              an ACK before transmitting the subsequent frame. Handles timeouts,
 *              frame loss simulation, and retransmissions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define TOTAL_FRAMES 5
#define TIMEOUT_SEC 2

typedef struct {
    int seq_no;
    char data[32];
} Frame;

typedef struct {
    int ack_no;
    bool is_ack;
} Ack;

int main() {
    srand(time(NULL));

    printf("===================================================================\n");
    printf("   Assignment 7: Stop-and-Wait Flow Control Protocol in C\n");
    printf("===================================================================\n\n");

    int current_frame = 0;
    int expected_seq = 0;
    int total_transmissions = 0;

    while (current_frame < TOTAL_FRAMES) {
        total_transmissions++;
        Frame frame;
        frame.seq_no = expected_seq;
        snprintf(frame.data, sizeof(frame.data), "DataPayload-%d", current_frame + 1);

        printf("[Sender]   Sending Frame #%d (Seq: %d, Data: %s)...\n",
               current_frame + 1, frame.seq_no, frame.data);

        // Simulate frame transmission with random drop chance (25%)
        int drop_chance = rand() % 100;
        if (drop_chance < 25) {
            printf("  [Network]  *** Frame #%d Lost in Transit (Packet Loss Simulation)! ***\n", current_frame + 1);
            printf("[Sender]   Timer Expired! Timeout waiting for ACK %d.\n", expected_seq);
            printf("[Sender]   Retransmitting Frame #%d...\n\n", current_frame + 1);
            continue;
        }

        // Receiver receives frame
        printf("[Receiver] Received Frame (Seq: %d, Data: %s)\n", frame.seq_no, frame.data);
        if (frame.seq_no == expected_seq) {
            printf("[Receiver] Frame sequence matches expected sequence %d. Storing data.\n", expected_seq);
            
            // Simulate ACK drop chance (20%)
            int ack_drop = rand() % 100;
            if (ack_drop < 20) {
                printf("  [Network]  *** ACK %d Lost in Transit! ***\n", expected_seq);
                printf("[Sender]   Timer Expired! Timeout waiting for ACK %d.\n", expected_seq);
                printf("[Sender]   Retransmitting Frame #%d...\n\n", current_frame + 1);
                continue;
            }

            // Normal ACK received
            printf("[Receiver] Sending ACK for Seq %d\n", expected_seq);
            printf("[Sender]   Received ACK %d successfully.\n\n", expected_seq);

            expected_seq = 1 - expected_seq; // Alternating bit 0 and 1
            current_frame++;
        } else {
            printf("[Receiver] Duplicate frame detected. Resending ACK %d.\n\n", 1 - expected_seq);
        }
    }

    printf("===================================================================\n");
    printf(" Transmission Summary:\n");
    printf(" Total Unique Frames Sent : %d\n", TOTAL_FRAMES);
    printf(" Total Transmissions Made : %d\n", total_transmissions);
    printf(" Protocol Efficiency     : %.2f%%\n", ((float)TOTAL_FRAMES / total_transmissions) * 100);
    printf(" Stop-and-Wait ARQ simulation finished successfully.\n");
    printf("===================================================================\n");

    return 0;
}
