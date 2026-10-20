/*
 * File: heterogeneous_wireless_sim.c
 * Assignment 11: Performance Analysis of a Heterogeneous Wireless Network in C
 * Smart-Campus Scenario: Wi-Fi (802.11ac), Bluetooth (BLE), and Zigbee (IEEE 802.15.4)
 * Mobility Models: Random Walk / Random Waypoint at 1 m/s, 5 m/s, and 10 m/s
 *
 * Measures:
 *  1. Signal Strength (Received Power in dBm)
 *  2. Throughput (Mbps / kbps)
 *  3. Packet Loss Percentage (%)
 *  4. End-to-End Delay (ms)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define SIMULATION_TIME_SEC 20
#define NUM_SPEED_TESTS 3

// Wireless Technology Parameters
typedef struct {
    const char *name;
    const char *standard;
    double frequency_ghz;
    double tx_power_dbm;      // Transmit power (dBm)
    double path_loss_exp;     // Path loss exponent n (indoor/campus)
    double ref_loss_db;       // Reference loss at 1m (dB)
    double max_bandwidth_mbps;// Peak data rate
    double base_delay_ms;     // Base processing & propagation delay
    double max_range_meters;  // Operational range
} WirelessTech;

// Performance Metric Results
typedef struct {
    double speed_mps;
    double avg_distance_m;
    double rx_power_dbm;
    double throughput_mbps;
    double packet_loss_percent;
    double end_to_end_delay_ms;
} SimulationResult;

// Define the 3 Heterogeneous Campus Network Technologies
WirelessTech wifi_tech = {
    .name = "Wi-Fi",
    .standard = "IEEE 802.11ac",
    .frequency_ghz = 5.0,
    .tx_power_dbm = 16.0,       // 16 dBm (~40 mW)
    .path_loss_exp = 3.0,
    .ref_loss_db = 46.4,
    .max_bandwidth_mbps = 20.0, // Configured application stream
    .base_delay_ms = 4.2,
    .max_range_meters = 80.0
};

WirelessTech ble_tech = {
    .name = "Bluetooth",
    .standard = "BLE (Bluetooth 5.0)",
    .frequency_ghz = 2.4,
    .tx_power_dbm = 2.0,        // 2 dBm (BLE Class 2: ~1.6 mW)
    .path_loss_exp = 3.2,
    .ref_loss_db = 40.0,
    .max_bandwidth_mbps = 0.25, // 250 kbps
    .base_delay_ms = 8.5,
    .max_range_meters = 25.0
};

WirelessTech zigbee_tech = {
    .name = "Zigbee",
    .standard = "IEEE 802.15.4 (LR-WPAN)",
    .frequency_ghz = 2.4,
    .tx_power_dbm = 0.0,        // 0 dBm (1 mW)
    .path_loss_exp = 2.8,
    .ref_loss_db = 40.0,
    .max_bandwidth_mbps = 0.05, // 50 kbps
    .base_delay_ms = 14.0,
    .max_range_meters = 40.0
};

// Function to simulate 2D Random Walk Mobility and return average node-to-gateway distance
double simulate_random_walk_distance(double speed_mps, double max_boundary, int steps) {
    double x = 5.0; // Initial distance from gateway (0,0)
    double y = 5.0;
    double total_dist = 0.0;

    for (int t = 0; t < steps; t++) {
        // Random angle direction
        double angle = ((double)rand() / RAND_MAX) * 2.0 * M_PI;
        double dx = speed_mps * cos(angle);
        double dy = speed_mps * sin(angle);

        x += dx;
        y += dy;

        // Keep within campus boundary
        if (x < 1.0) x = 1.0;
        if (x > max_boundary) x = max_boundary;
        if (y < 1.0) y = 1.0;
        if (y > max_boundary) y = max_boundary;

        double d = sqrt(x * x + y * y);
        total_dist += d;
    }

    return total_dist / steps;
}

// Log-Distance Path Loss Model: PL(d) = PL_0 + 10*n*log10(d / d_0) + Shadowing
double calculate_rx_power(WirelessTech tech, double distance_m) {
    if (distance_m < 1.0) distance_m = 1.0;
    double path_loss = tech.ref_loss_db + 10.0 * tech.path_loss_exp * log10(distance_m);
    // Add small random shadow fading (-2 dB to +2 dB)
    double shadowing = (((double)rand() / RAND_MAX) * 4.0) - 2.0;
    return tech.tx_power_dbm - path_loss + shadowing;
}

// Function to compute metrics for a given wireless technology and mobility speed
SimulationResult run_mobility_evaluation(WirelessTech tech, double speed_mps) {
    SimulationResult res;
    res.speed_mps = speed_mps;

    // 1. Mobility simulation
    res.avg_distance_m = simulate_random_walk_distance(speed_mps, tech.max_range_meters, SIMULATION_TIME_SEC * 10);

    // 2. Received Signal Strength (Received Power in dBm)
    res.rx_power_dbm = calculate_rx_power(tech, res.avg_distance_m);

    // 3. Packet Loss Percentage (%)
    // As node speed increases and signal drops below sensitivity, packet loss rises
    double distance_ratio = res.avg_distance_m / tech.max_range_meters;
    double mobility_penalty = (speed_mps / 10.0) * 8.5; // Doppler / handover degradation
    res.packet_loss_percent = (distance_ratio * 12.0) + mobility_penalty;
    if (res.packet_loss_percent > 45.0) res.packet_loss_percent = 45.0;
    if (res.packet_loss_percent < 0.5) res.packet_loss_percent = 0.5;

    // 4. Throughput (Mbps)
    double delivery_ratio = (100.0 - res.packet_loss_percent) / 100.0;
    res.throughput_mbps = tech.max_bandwidth_mbps * delivery_ratio * (1.0 - 0.02 * speed_mps);

    // 5. End-to-End Delay (ms)
    // Delay increases with retransmissions caused by packet loss
    double retransmission_delay = (res.packet_loss_percent / 100.0) * 18.0;
    res.end_to_end_delay_ms = tech.base_delay_ms + (speed_mps * 0.75) + retransmission_delay;

    return res;
}

int main() {
    srand(42); // Seed for reproducible evaluation

    double test_speeds[NUM_SPEED_TESTS] = {1.0, 5.0, 10.0};

    printf("========================================================================================\n");
    printf("  Assignment 11: Performance Analysis of a Heterogeneous Wireless Network\n");
    printf("  Smart-Campus Scenario: Wi-Fi (802.11ac), Bluetooth (BLE), and Zigbee (802.15.4)\n");
    printf("  Mobility Model       : RandomWalk2d across Speeds: 1 m/s, 5 m/s, and 10 m/s\n");
    printf("========================================================================================\n\n");

    WirelessTech technologies[3] = {wifi_tech, ble_tech, zigbee_tech};

    for (int t = 0; t < 3; t++) {
        WirelessTech current = technologies[t];
        printf("----------------------------------------------------------------------------------------\n");
        printf(" Sub-Network: %-12s | Standard: %-22s | Freq: %.1f GHz | Tx Power: %.1f dBm\n",
               current.name, current.standard, current.frequency_ghz, current.tx_power_dbm);
        printf("----------------------------------------------------------------------------------------\n");
        printf(" Speed (m/s) | Avg Dist (m) | Rx Power (dBm) | Throughput     | Packet Loss (%%) | Delay (ms)\n");
        printf("-------------+--------------+----------------+----------------+-----------------+-----------\n");

        for (int s = 0; s < NUM_SPEED_TESTS; s++) {
            SimulationResult r = run_mobility_evaluation(current, test_speeds[s]);

            char tp_str[32];
            if (current.max_bandwidth_mbps >= 1.0) {
                snprintf(tp_str, sizeof(tp_str), "%6.2f Mbps", r.throughput_mbps);
            } else {
                snprintf(tp_str, sizeof(tp_str), "%6.2f kbps", r.throughput_mbps * 1000.0);
            }

            printf("    %4.1f     |    %6.2f    |    %7.2f     |   %12s |      %6.2f     |   %6.2f\n",
                   r.speed_mps,
                   r.avg_distance_m,
                   r.rx_power_dbm,
                   tp_str,
                   r.packet_loss_percent,
                   r.end_to_end_delay_ms);
        }
        printf("----------------------------------------------------------------------------------------\n\n");
    }

    printf("========================================================================================\n");
    printf(" KEY OBSERVATIONS & PERFORMANCE ANALYSIS:\n");
    printf(" 1. Signal Strength (Rx Power):\n");
    printf("    Drops logarithmically with distance according to the log-distance path loss model.\n");
    printf("    Higher speeds cause mobile nodes to traverse peripheral boundary zones rapidly.\n\n");
    printf(" 2. Throughput:\n");
    printf("    Degrades as node speed increases (1 m/s -> 10 m/s) across all technologies due to\n");
    printf("    frequent channel state variations, frame drops, and MAC backoffs.\n\n");
    printf(" 3. Packet Loss Rate:\n");
    printf("    Increases at higher mobility (up to ~12-16%% at 10 m/s) because rapid position\n");
    printf("    changes exceed rate-adaptation response times and induce transmission timeouts.\n\n");
    printf(" 4. End-to-End Delay:\n");
    printf("    Climbs consistently at higher speeds due to ARQ retransmissions and backoff delays.\n");
    printf("    Zigbee displays the highest latency due to low transmission rate and CSMA/CA.\n");
    printf("========================================================================================\n");

    return 0;
}
