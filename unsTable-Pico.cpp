#include <cstdio>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

// Replace with your WiFi credentials
#define WIFI_SSID "The LAN of freedom"
#define WIFI_PASSWORD "244466666"

// Function to connect to WiFi
bool connect_to_wifi(const char* ssid, const char* password) {
    printf("Initializing WiFi driver...\n");
    // Initialize the WiFi driver
    if (cyw43_arch_init()) {
        printf("Failed to initialize WiFi!\n");
        return false;
    }
    
    printf("Starting WiFi in STA mode...\n");
    // Start WiFi in STA (station) mode
    cyw43_arch_enable_sta_mode();

    printf("Connecting to WiFi...\n");
    int result = cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 10000);
    if (result) {
        printf("Failed to connect to WiFi, error code: %d\n", result);
        cyw43_arch_deinit();
        return false;
    }
    
    printf("Successfully connected to WiFi: %s\n", ssid);
    return true;
}

int main() {
    stdio_init_all(); // Initialize I/O
    printf("Hello, Pico!\n");

    if (connect_to_wifi(WIFI_SSID, WIFI_PASSWORD)) {
        printf("Ready to use the network!\n");
    } else {
        printf("WiFi connection failed.\n");
    }

    while (true) {
        sleep_ms(1000); // Keep the program alive
    }
}