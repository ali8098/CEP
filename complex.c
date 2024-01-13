
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h> // Add this line for malloc and free
#include "report_generator.h"
#include "email_sender.h"

// Rest of your code...


//#include "anomaly_detector.h"
int findFieldValue(const char *text, const char *field, char *value, size_t valueSize) {
    const char *start = strstr(text, field);

    if (start == NULL) {
        fprintf(stderr, "Field %s not found in text.\n", field);
        return 0;
    }

    // Move the pointer to the value part
    start = strchr(start, ':');
    if (start == NULL) {
        fprintf(stderr, "Invalid text format.\n");
        return 0;
    }

    // Skip any whitespace
    while (*start != '\0' && (*start == ':' || *start == ' ' || *start == '\t' || *start == '\n' || *start == '\r')) {
        start++;
    }

    // Check for the start of a string or number
    if (*start == '"' || (*start >= '0' && *start <= '9') || *start == '-') {
        sscanf(start, "%[^,}]", value);
        return 1;
    } else {
        fprintf(stderr, "Invalid text value format.\n");
        return 0;
    }
}

void detectAnomalies(const char *temperature, const char *precipitation, FILE *outputFile) {
    double temp = atof(temperature);
    double precip = atof(precipitation);

      if (temp > 35.0) {
        fprintf(outputFile,"High temperature anomaly detected!\n");
    } else if (temp < 20.0) {
        fprintf(outputFile,"A drop in temperature is recorded.\n");
    } else {
        fprintf(outputFile,"Temperature is moderate.\n");
    }

    if (precip > 10.0) {
        fprintf(outputFile,"High precipitation anomaly detected!\n");
    } else if (precip < 10.0) {
        fprintf(outputFile,"Precipitation anomaly not detected.\n");
    }
}
void processData(const char *fileContent, FILE*outputFile, char *temperature, char *humidity, char *precipitation,
                  char *pressure, char *windSpeed, char *windDirection) {


    findFieldValue(fileContent, "\"temp_c\"", temperature, sizeof(temperature));
    findFieldValue(fileContent, "\"humidity\"", humidity, sizeof(humidity));
    findFieldValue(fileContent, "\"precip_mm\"", precipitation, sizeof(precipitation));
    findFieldValue(fileContent, "\"pressure_mb\"", pressure, sizeof(pressure));
    findFieldValue(fileContent, "\"wind_kph\"", windSpeed, sizeof(windSpeed));
    findFieldValue(fileContent, "\"wind_dir\"", windDirection, sizeof(windDirection));

    char condition[50];
    findFieldValue(fileContent, "\"text\"", condition, sizeof(condition));
    int isSevereWeather = (strcmp(condition, "Thunderstorm") == 0 || strcmp(condition, "Rain") == 0);

    // Get the current time
    time_t currentTime = time(NULL);
    struct tm *timeInfo = localtime(&currentTime);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeInfo);


    outputFile = fopen("processed_data.txt", "w");
    if (outputFile) {
        fprintf(outputFile, "Temperature: %s\n", temperature);
        fprintf(outputFile, "Humidity: %s\n", humidity);
        fprintf(outputFile, "Precipitation: %s\n", precipitation);
        fprintf(outputFile, "Pressure: %s\n", pressure);
        fprintf(outputFile, "Wind Speed: %s\n", windSpeed);
        fprintf(outputFile, "Wind Direction: %s\n", windDirection);
        fprintf(outputFile, "Severe Weather: %s\n", isSevereWeather ? "Yes" : "No");
        fprintf(outputFile, "\n");
        //fclose(outputFile);


    detectAnomalies(temperature, precipitation,outputFile);


//    fclose(outputFile);
    if (outputFile != NULL) {
        fclose(outputFile);
    }
}
}
