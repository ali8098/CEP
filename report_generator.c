// report_generator.c
#include <stdlib.h>
#include <curl/curl.h>
#include "email_sender.h"
#include <stdio.h>

#include "report_generator.h"

void generateReport(const char* timeStr, const char* temperature, const char* humidity,
                    const char* precipitation,const char* pressure,const char* windSpeed
                    ,const char* windDirection, FILE* outputFile) {
    // Check for errors in opening the output file
    if (outputFile == NULL) {
        fprintf(stderr, "Error opening report file.\n");
        return; // Exit the function if an error occurs
    }

    // Add headers and timestamp to the report
    fprintf(outputFile, "-----------------------\n");
    fprintf(outputFile, "Environmental Report\n");
    fprintf(outputFile, "-----------------------\n\n");
    fprintf(outputFile, "Timestamp: %s\n\n", timeStr);

    // Include basic environmental data in the report
    fprintf(outputFile, "Temperature: %s\n", temperature);
    fprintf(outputFile, "Humidity: %s\n", humidity);
    fprintf(outputFile, "Precipitation: %s\n\n", precipitation);
    fprintf(outputFile, "pressure: %s\n\n", pressure);
    fprintf(outputFile, "windspeed: %s\n\n", windSpeed );
    fprintf(outputFile, "wind direction: %s\n\n", windDirection);
    
    // Include anomalies detected, if any
    
    double precip = atof(precipitation);
    // Include anomalies detected, if any
    double temp = atof(temperature);
    if (temp > 35.0 || temp < 25.0) {
      fprintf(outputFile, "Temperature Anomaly:\n");
      fprintf(outputFile, "-----------------------\n");
      if (temp > 35.0) {
          fprintf(outputFile, "High Temperature Anomaly: %s\n", "Detected");
      } else if (temp < 25.0) {
          fprintf(outputFile, "Low Temperature Anomaly: %s\n", "Detected");
      }
      fprintf(outputFile, "-----------------------\n");
  } else {
      fprintf(outputFile, "Temperature Anomaly: %s\n", "Normal");
}

    fflush(outputFile);
   // fclose(outputFile); // Close the report file


}
