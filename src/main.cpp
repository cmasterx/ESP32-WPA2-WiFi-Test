#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <esp_wpa2.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "credentials.h"

/*

// Create a credentials.h within the same directory as this file
// with the following contents inside:

#pragma once

#define EAP_IDENTITY            "username@email.edu"
#define EAP_ANONYMOUS_IDENTITY  EAP_IDENTITY
#define EAP_PASSWORD            "password"
const static char CERTIFICATE[] = "CERTIFICATE"

*/

const char *ssid = "eduroam";

void setup()
{
    Serial.begin(9600);
    if (!SPIFFS.begin()) {
        Serial.println("ERROR: Failed to mount SPIFFS. Rebooting...");
        delay(333);
        ESP.restart();
    }

    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);

    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wifi_sta_wpa2_ent_set_ca_cert((uint8_t *)CERTIFICATE, strlen(CERTIFICATE) + 1);
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); 
    esp_wifi_sta_wpa2_ent_enable(&config);
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
        Serial.print(F("."));
    }

}

void loop()
{
    Serial.println("CONNECTED!");
    HTTPClient request;
    request.begin("https://raw.githubusercontent.com/cmasterx/MiOrigin/main/firmware/binaries/firmware.bin");
    if (request.GET()) {
        Serial.println("Get request successful");

        unsigned long stopwatch = millis();
        auto &stream = request.getStream();
        size_t totalBytesRead = 0;
        while (stream.available()) {
            char buffer[512];
            size_t sizeRead = stream.readBytes(buffer, sizeof(buffer) / sizeof(buffer[0]));
            Serial.println(buffer);
            totalBytesRead += sizeRead;
        }

        Serial.printf("Download took %.3fs to complete with %d bytes read", (millis() - stopwatch) / 1000.0f, totalBytesRead);
    }

    Serial.println("\n\nDONE! Restarting GET request in 5 seconds...");
    delay(5000);
    // put your main code here, to run repeatedly:
}