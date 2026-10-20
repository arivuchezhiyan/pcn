/*
 * File: sliding_window_go_back_n.c
 * Assignment 7: Implementation of Go-Back-N (GBN) Sliding Window Protocol in C
 * Description: Implements the Go-Back-N ARQ flow control protocol.
 *              Allows sender to transmit up to N frames before receiving an acknowledgment.
 *              Maintains Send Window [base, next_seq_num], cumulative ACKs,
 *              timer-driven retransmission of all unacknowledged frames on loss.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define WINDOW_SIZE 4
#define TOTAL_FRAMES 10

typedef struct {
    int seq_no;
    char data[32];
    int acked;
} Frame;

int main() {
    srand(time(NULL));

    printf("===================================================================\n");
    printf("     Assignment 7: Go-Back-N (GBN) Sliding Window Protocol in C\n");
    printf("     Window Size (N) = %d | Total Frames to Send = %d\n", WINDOW_SIZE, TOTAL_FRAMES);
    printf("===================================================================\n\n");

    Frame frames[TOTAL_FRAMES];
    for (int i = 0; i < TOTAL_FRAMES; i++) {
        frames[i].seq_no = i;
        snprintf(frames[i].data, sizeof(frames[i].data), "DataPayload-%d", i);
        frames[i].acked = 0;
    }

    int base = 0;
    int next_seq_num = 0;
    int total_transmissions = 0;

    while (base < TOTAL_FRAMES) {
        // Send frames within the current window
        while (next_seq_num < base + WINDOW_SIZE && next_seq_num < TOTAL_FRAMES) {
            printf("[Sender]   Sending Frame #%d (Window [%d, %d])...\n",
                   next_seq_num, base, base + WINDOW_SIZE - 1);
            total_transmissions++;
            next_seq_num++;
        }

        // Simulate acknowledgment for the oldest unacknowledged frame (base)
        printf("\n[Channel]  Verifying transit for Frame #%d...\n", base);
        int loss = (rand() % 100) < 30; // 30% chance of frame or ACK loss

        if (!loss) {
            // Success
            printf("[Receiver] Frame #%d received correctly! Sending Cumulative ACK #%d\n", base, base);
            printf("[Sender]   Received ACK #%d. Sliding window forward by 1 frame.\n\n", base);
            frames[base].acked = 1;
            base++;
        } else {
            // Timeout occurs on Frame #base
            printf("  [Network]  *** Packet / ACK Loss Detected on Frame #%d! ***\n", base);
            printf("[Sender]   *** TIMEOUT for Frame #%d! ***\n", base);
            printf("[Sender]   Go-Back-N: Rolling back next_seq_num from %d to %d\n", next_seq_num, base);
            printf("[Sender]   Retransmitting all frames in window starting from #%d...\n\n", base);
            next_seq_num = base; // Reset sender pointer to window base
        }
    }

    printf("===================================================================\n");
    printf(" Transmission Summary:\n");
    printf(" Total Unique Frames Sent : %d\n", TOTAL_FRAMES);
    printf(" Total Transmissions Made : %d\n", total_transmissions);
    printf(" Protocol Efficiency     : %.2f%%\n", ((float)TOTAL_FRAMES / total_transmissions) * 100);
    printf(" Go-Back-N Protocol simulation finished successfully.\n");
    printf("===================================================================\n");

    return 0;
}
