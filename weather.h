// weather.h
#ifndef WEATHER_H
#define WEATHER_H

#include <stdio.h>
#include <curl/curl.h>

int getWeatherForecast();


// Function declaration for parsing weather data
void parseWeatherData(const char *filename);

#endif  // WEATHER_H
