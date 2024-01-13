#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "report_generator.h"
#include "email_sender.h"
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

      if (temp > 30.0) {
        fprintf(outputFile,"High temperature anomaly detected!: %.2f\n"\n);
    } else if (temp < 30.0) {
        fprintf(outputFile,"A drop in temperature is recorded!:%.2f\n".\n);
    } else {
        fprintf(outputFile,"Temperature is moderate.\n");
    }

    if (precip > 10.0) {
        fprintf(outputFile,"High precipitation anomaly detected!:%.2f\n"\n);
    } else if (precip < 10.0) {
        fprintf(outputFile,"Precipitation anomaly not detected!:%.2f\n"\n);
    }
}
void logAnomaliesToFile(const char *temperature, const char *precipitation, const char *filePath) {
    FILE *anomaliesFile = fopen(filePath, "a");
    if (anomaliesFile == NULL) {
        fprintf(stderr, "Error opening anomalies log file.\n");
        return;
    }

    detectAnomalies(temperature, precipitation, anomaliesFile);

    // Close the anomalies log file
    fclose(anomaliesFile);
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
        


    detectAnomalies(temperature, precipitation,outputFile);



    if (outputFile != NULL) {
        fclose(outputFile);
    }
}
}
int main(void) {
    char temperature[10];
    char humidity[10];
    char precipitation[10];
    char pressure[10];
    char windSpeed[10];
    char windDirection[10];
    //char severeWeather[10];
    FILE *outputFile = fopen("output_log.txt", "w");
    if (outputFile == NULL) {
        fprintf(stderr, "Error opening output file.\n");
        return 1;
    }

    FILE *processedDataFile = fopen("processed_data.txt", "a");
    if (processedDataFile == NULL) {
        fprintf(stderr, "Error opening processed_data file.\n");
        fclose(outputFile);
        return 1;
      }

    CURL *hnd = curl_easy_init();
    if (hnd) {
        curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(hnd, CURLOPT_URL, "https://weatherapi-com.p.rapidapi.com/current.json?q=24.8607%2C67.0011");

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "X-RapidAPI-Key: 59ebf45566msh90bbd47d19191e1p129e89jsn5411c2e62214");
        headers = curl_slist_append(headers, "X-RapidAPI-Host: weatherapi-com.p.rapidapi.com");
        curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, fwrite);
        curl_easy_setopt(hnd, CURLOPT_WRITEDATA, outputFile);

        CURLcode ret = curl_easy_perform(hnd);

        if (ret != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(ret));
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(hnd);

        fclose(outputFile);
    }

    FILE *inputFile = fopen("output_log.txt", "r");
    if (!inputFile) {
        fprintf(stderr, "Failed to open input file.\n");
        return 1;
    }
     FILE *apiResponseFile = fopen("api_response.txt", "a");
    if (apiResponseFile == NULL) {
        fprintf(stderr, "Error opening api_response.txt file.\n");
        fclose(inputFile);
        fclose(processedDataFile);
        return 1;
    }

    // Copy and append content from output_log.txt to api_response.txt
    int ch;
    while ((ch = fgetc(inputFile)) != EOF) {
        fputc(ch, apiResponseFile);
    }



    fseek(inputFile, 0, SEEK_END);
    long fileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    char *fileContent = (char *)malloc(fileSize + 1);
    size_t bytesRead =fread(fileContent, 1, fileSize, inputFile);
    fileContent[bytesRead] = '\0';
    fclose(inputFile);
    fclose(apiResponseFile);
    fclose(processedDataFile);
    fileContent[fileSize] = '\0';

    processData(fileContent,outputFile,temperature, humidity, precipitation,
                pressure, windSpeed, windDirection);

    detectAnomalies(temperature, precipitation, outputFile);

// Call the function from the other file
    logAnomaliesToFile(temperature, precipitation, "anomalies_log.txt");


        // Declare variables for generateReport function
    char reportTemperature[10];
    char reportHumidity[10];
    char reportPrecipitation[10];
    char reportpressure[10];
    char reportwindSpeed[10];
    char reportwindDirection[10];


    // Copy values from the original variables
    strcpy(reportTemperature, temperature);
    strcpy(reportHumidity, humidity);
    strcpy(reportPrecipitation, precipitation);
    strcpy(reportpressure, pressure);
    strcpy(reportwindDirection, windDirection);
    strcpy(reportwindSpeed, windSpeed);

    // Free the allocated memory for fileContent
    free(fileContent);

        // Get the current time
    time_t currentTime = time(NULL);
    struct tm *timeInfo = localtime(&currentTime);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeInfo);

    // Open the report file for writing
    FILE *reportFile = fopen("environmental_report.txt", "w");
    if (reportFile == NULL) {
        fprintf(stderr, "Error opening report file.\n");
        fclose(outputFile);
        return 1;
    }

 
    generateReport(timeStr, reportTemperature, reportHumidity, reportPrecipitation, reportpressure, reportwindSpeed, reportwindDirection, reportFile);

    // Close the report file
    fclose(reportFile);


    // Check the result and handle accordingly...
    const char *to = "khan4506823@cloud.neduet.edu.pk";
    const char *file_path = "environmental_report.txt";

    int result = send_email_with_attachment(to, file_path);

    if (result == 0) {
        printf("Email sent successfully!\n");
    } else {
        printf("Failed to send email. Error code: %d\n", result);
    }


    return 0;

}
