#include <stdio.h>
#include <string.h>
#include "anomaly_detector.h"
void detectAnomaliesFromFile(const char *filePath) {
    FILE *inputFile = fopen(filePath, "r");
    if (!inputFile) {
        fprintf(stderr, "Failed to open input file: %s\n", filePath);
        return;
    }

    FILE *outputFile = fopen("anomaly_report.txt", "w");
    if (!outputFile) {
        fprintf(stderr, "Failed to open output file for anomalies.\n");
        fclose(inputFile);
        return;
    }

    char temperature[10];
    char precipitation[10];

    //while (fscanf(inputFile, "Temperature: %s\nPrecipitation: %s\n", temperature, precipitation) == 2) {
    while (fscanf(inputFile, "temp_c: %s\nprecipitation: %s\n", temperature, precipitation) == 2) {
    printf("Temperature: %s, Precipitation: %s\n", temperature, precipitation);
    // Rest of your code...


        double temp = atof(temperature);
        double precip = atof(precipitation);

        if (temp > 30.0) {
            fprintf(outputFile, "High temperature anomaly detected!\n");
        } else if (temp < 30.0) {
            fprintf(outputFile, "A drop in temperature is recorded.\n");
        }

        if (precip > 10.0) {
            fprintf(outputFile, "High precipitation anomaly detected!\n");
        }
    }

    fclose(inputFile);
    fclose(outputFile);
}

