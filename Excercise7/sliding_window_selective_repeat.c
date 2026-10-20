/*
 * File: sliding_window_selective_repeat.c
 * Assignment 7: Implementation of Selective Repeat ARQ Protocol in C
 * Description: Implements Selective Repeat ARQ flow control protocol.
 *              Both sender and receiver maintain sliding windows of size N.
 *              Receiver independently ACKs each valid frame and buffers out-of-order frames.
 *              Sender selectively retransmits only the lost frames, optimizing bandwidth.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define WINDOW_SIZE 4
#define TOTAL_FRAMES 10

typedef struct {
    int seq_no;
    char data[32];
    bool acked;
} SenderFrame;

typedef struct {
    bool received;
    char data[32];
} ReceiverBuffer;

int main() {
    srand(time(NULL));

    printf("===================================================================\n");
    printf("   Assignment 7: Selective Repeat ARQ Protocol in C\n");
    printf("   Window Size (N) = %d | Total Frames to Send = %d\n", WINDOW_SIZE, TOTAL_FRAMES);
    printf("===================================================================\n\n");

    SenderFrame sender_window[TOTAL_FRAMES];
    ReceiverBuffer receiver_buf[TOTAL_FRAMES];

    for (int i = 0; i < TOTAL_FRAMES; i++) {
        sender_window[i].seq_no = i;
        snprintf(sender_window[i].data, sizeof(sender_window[i].data), "DataPayload-%d", i);
        sender_window[i].acked = false;
        receiver_buf[i].received = false;
    }

    int send_base = 0;
    int next_seq_num = 0;
    int total_transmissions = 0;

    while (send_base < TOTAL_FRAMES) {
        // Send all available frames in current sender window
        while (next_seq_num < send_base + WINDOW_SIZE && next_seq_num < TOTAL_FRAMES) {
            if (!sender_window[next_seq_num].acked) {
                printf("[Sender]   Transmitting Frame #%d...\n", next_seq_num);
                total_transmissions++;
            }
            next_seq_num++;
        }

        // Process frames in current window
        for (int i = send_base; i < next_seq_num && i < TOTAL_FRAMES; i++) {
            if (!sender_window[i].acked) {
                int drop = (rand() % 100) < 25; // 25% drop rate
                if (!drop) {
                    printf("[Receiver] Frame #%d successfully received & buffered. Sending Selective ACK #%d\n", i, i);
                    receiver_buf[i].received = true;
                    sender_window[i].acked = true;
                } else {
                    printf("  [Network]  *** Frame #%d Lost! Will selectively retransmit only #%d. ***\n", i, i);
                }
            }
        }

        // Advance sender window base past consecutive acknowledged frames
        while (send_base < TOTAL_FRAMES && sender_window[send_base].acked) {
            printf("[Sender]   Frame #%d fully confirmed. Sliding window forward!\n", send_base);
            send_base++;
        }

        // Retransmit only unacknowledged frames within current window
        if (send_base < TOTAL_FRAMES) {
            printf("\n[Sender]   Checking unacked frames in window [%d to %d]:\n",
                   send_base, send_base + WINDOW_SIZE - 1);
            for (int j = send_base; j < next_seq_num && j < TOTAL_FRAMES; j++) {
                if (!sender_window[j].acked) {
                    printf("[Sender]   Selective Retransmission: Retransmitting Frame #%d ONLY\n", j);
                    total_transmissions++;
                    // Retransmit attempt
                    if ((rand() % 100) < 80) { // High success on retransmit
                        printf("[Receiver] Frame #%d received on retry! Sending Selective ACK #%d\n", j, j);
                        receiver_buf[j].received = true;
                        sender_window[j].acked = true;
                    }
                }
            }
            // Advance base again if possible
            while (send_base < TOTAL_FRAMES && sender_window[send_base].acked) {
                printf("[Sender]   Frame #%d confirmed. Window slides forward!\n", send_base);
                send_base++;
            }
        }
        printf("\n");
    }

    printf("===================================================================\n");
    printf(" Transmission Summary:\n");
    printf(" Total Unique Frames Sent : %d\n", TOTAL_FRAMES);
    printf(" Total Transmissions Made : %d\n", total_transmissions);
    printf(" Protocol Efficiency     : %.2f%%\n", ((float)TOTAL_FRAMES / total_transmissions) * 100);
    printf(" Selective Repeat ARQ finished successfully.\n");
    printf("===================================================================\n");

    return 0;
}
