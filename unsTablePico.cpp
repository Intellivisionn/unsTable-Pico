#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"


bool connectWiFi(const char *ssid, const char *password)
{
    if (cyw43_arch_init())
    {
        printf("failed to initialise\n");
        return false;
    }

    cyw43_arch_enable_sta_mode();

    printf("Connecting to WiFi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 10000)) //check if it failed
    {
        printf("Failed to connect to WiFi\n");
        return false;
    }
    return true;
}

int main()
{
    stdio_init_all();

    const char *ssid = "78463212";
    const char *password = "78463212";

    while (!connectWiFi(ssid, password))
    {
        printf("Retrying...\n");
        sleep_ms(1000);
    }
    printf("Connected to WiFi\n");

}