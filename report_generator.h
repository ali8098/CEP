// report_generator.h

#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H
#include <curl/curl.h>
#include <stdio.h>
void generateReport(const char* timeStr, const char* temperature, const char* humidity,
                    const char* precipitation, const char* pressure, const char* windSpeed,
                     const char* windDirection, FILE* outputFile);

#include <stdio.h>

// Function to detect anomalies in temperature and precipitation
void detectAnomaliesFromFile(const char *filePath);


#endif // REPORT_GENERATOR_H
