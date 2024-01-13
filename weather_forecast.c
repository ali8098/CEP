// weather.c
#include "weather.h"
#include <stdio.h>
#include <curl/curl.h>
#include <string.h> // Include the string.h header for strstr function

int getWeatherForecast() {
    // The libcurl code from your original example
    CURL *hnd = curl_easy_init();
    if (!hnd) {
        fprintf(stderr, "Failed to initialize curl\n");
        return 1;
    }

    // Set HTTP method and URL
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_URL, "https://weatherapi-com.p.rapidapi.com/forecast.json?q=Karachi&days=3");

    // Set custom headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "X-RapidAPI-Key: 59ebf45566msh90bbd47d19191e1p129e89jsn5411c2e62214");
    headers = curl_slist_append(headers, "X-RapidAPI-Host: weatherapi-com.p.rapidapi.com");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    // Open a file for writing the weather data
    FILE *file = fopen("weather_forecast.txt", "w");
    if (!file) {
        fprintf(stderr, "Failed to open file for writing\n");
        curl_easy_cleanup(hnd);
        curl_slist_free_all(headers);
        return 1;
    }
    // Set the file as the write callback
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, file);

    // Perform the HTTP request
    CURLcode ret = curl_easy_perform(hnd);
    if (ret != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(ret));
        fclose(file);  // Close the file before returning
        return 1;
    }

    // Clean up resources
    curl_easy_cleanup(hnd);
    curl_slist_free_all(headers);
    fclose(file);  // Close the file after successful fetch

    return 0;
}

// Function to parse and print the weather forecast for the next 7 days
void parseWeatherData(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file %s for reading\n", filename);
        return;
    }

    FILE *outputFile = fopen("shortlisted_weather.txt", "w");
    if (!outputFile) {
        perror("Failed to open file shortlisted_weather.txt for writing");
        fclose(file);
        return;
    }

    char line[1024];
    int dayCount = 0;

    while (fgets(line, sizeof(line), file)) {
        // Find the date field
        if (strstr(line, "\"date\":\"")) {
            char date[20];
            sscanf(line, "\"date\":\"%19[^\"]\"", date);

            // Print forecast information
            fprintf(outputFile, "Day %d: %s\n", ++dayCount, date);

            // Find and print additional information (customize as needed)
            while (fgets(line, sizeof(line), file) && !strstr(line, "}")) {
                if (strstr(line, "\"maxtemp_c\":")) {
                    double maxTemp;
                    sscanf(line, "\"maxtemp_c\":%lf,", &maxTemp);
                    fprintf(outputFile, "Max Temperature: %.1f°C\n", maxTemp);
                } else if (strstr(line, "\"mintemp_c\":")) {
                    double minTemp;
                    sscanf(line, "\"mintemp_c\":%lf,", &minTemp);
                    fprintf(outputFile, "Min Temperature: %.1f°C\n", minTemp);
                }
                // Add more fields as needed
            }

            // Stop after processing 7 days
            if (dayCount == 7) {
                break;
            }
        }
    }

    fclose(file);
    fclose(outputFile);
}
