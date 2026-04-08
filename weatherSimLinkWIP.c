#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <math.h>

#define SIZE 50
#define CITY_LEN 64
#define MAX_CITIES 100

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define RESET   "\033[0m"


//=============================================================================
//                                Structures
//=============================================================================


/*
* Sensor
*
* Structure for individual weather sensor.
*/
typedef struct {
    int id;
    char city[CITY_LEN];
    int temperature;
    int humidity;
    int pressure;
    int wind;
} Sensor;

/*
* Node
*
* Node structure for the linked list that will connect all sensors.
*/
typedef struct Node {
    Sensor data;
    struct Node *next;
    struct Node *prev;
} Node;

/*
* SensorList
*
* Wrapper for linked list. Contains sentinel node and tracks size of
* the list.
*/
typedef struct {
    Node *header;
    int size;
} SensorList;

/*
* Stats
*
* Structure for data used in statistical analysis
*/
typedef struct {
    double avgTemp;
    double avgHumidity;
    double avgPressure;
    double avgWind;

    int minTemp;
    int maxTemp;
    char minCity[CITY_LEN];
    char maxCity[CITY_LEN];

    double avgTempChange;
    double avgHumidityChange;
    double avgPressureChange;
    double avgWindChange;

    int warmingCities;
    int coolingCities;
    int risingPressureCities;
    int fallingPressureCities;
    int increasingWindCities;
} Stats;


//=============================================================================
//                              City Importing
//=============================================================================


/*
* stripNewline
*
* fgets() function was loading in newline chars. This is a helper function
* that works within my loadCities function to remove that unwanted newline.
*/
void stripNewline(char *str) {
    str[strcspn(str, "\r\n")] = '\0';
}

/*
* loadCities
*
* This function uses an iterative loop to pull lines of characters from
* a simple .txt file with dozens of city names that are used to give
* a more real feel to the Sensor structs than just ID numbers. I chose to
* use the fgets function here to prevent buffer overflow.
*/
int loadCities(const char *filename, char cities[][CITY_LEN], int maxCities) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: could not open %s\n", filename);
        return 0;
    }

    int count = 0;

    while (count < maxCities && fgets(cities[count], CITY_LEN, fp) != NULL) {
        stripNewline(cities[count]);

        if (strlen(cities[count]) > 0) {
            count++;
        }
    }
    fclose(fp);
    return count;
}


//-----------------------------------------------------------------------------
//                               List Functions
//-----------------------------------------------------------------------------


/*
* initSensorArray
*
* Function initializes wrapper's header and size to get rid of garbage.
*/
void initSensorList(SensorList *list) {
    list->header = malloc(sizeof(Node));
    if (list->header == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    list->header->next = list->header;
    list->header->prev = list->header;
    list->size = 0;
}

/*
* appendList
*
* Function inserts new node at end of existing linked list.
*/
void appendList(SensorList *list, Sensor x) {
    Node *newNode = malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    newNode->data = x;

    newNode->next = list->header;
    newNode->prev = list->header->prev;

    list->header->prev->next = newNode;
    list->header->prev = newNode;

    if (list->size == 0) {
        list->header->next = newNode;
    }

    list->size++;
}

/*
* makeSensor
*
* Function to initialze sensor structs. Base normal ranges for each 
* feature are used, as well as some normalization methods and dependency
* to simulate more realistic conditions.
*/
Sensor makeSensor(const char *name, int *id) {
    Sensor s;

    // ID assignment
    s.id = *id;
    (*id)++;

    // City name assignment
    strncpy(s.city, name, CITY_LEN-1);
    s.city[CITY_LEN] = '\0';

    // Temperature normalization & some negative values
    s.temperature = rand() % 120;
    if (s.temperature < 25 && s.id % 3 == 0) {
        s.temperature *= -1;
    }
    if (s.temperature > 90 && s.id % 3 == 0) {
        s.temperature -= 20;
    }

    // Humidity normalization with temperature dependence
    double maxHumidity = 100.0 - 1.2*(s.temperature - 70);
    int roundedMax = maxHumidity;
    if (s.temperature <= 80) {
        s.humidity = 15 + rand() % 86;
    } else {
        s.humidity = rand() % roundedMax;
    }
    
    // Pressure
    s.pressure = 980 + rand() % 51;

    // Pressure-influenced wind speeds
    s.wind = rand() % 61;
    if (s.pressure < 995) {
        s.wind += rand() % 20;
    }
    if (s.pressure > 1020) {
        s.wind /= 2;
    }
    return s;
}

/*
* generateList
*
* Combines makeSensor() with appendList() and utilizes a simple while 
* loop to create a list of sensor nodes.
*/
void generateList(SensorList *list, char cities[][CITY_LEN], int cityCount, int *id) {
    int limit = cityCount < SIZE ? cityCount : SIZE;

    for (int i = 0; i < limit; i++) {
        Sensor s = makeSensor(cities[i], id);
        appendList(list, s);
    }
}

/*
* printList
*
* Function that traverses the existing list and prints out formatted values.
*/
void printList(SensorList *list) {
    Node *current = list->header->next;
    printf(YELLOW "Sensor ID       City       Temperature   Humidity   Pressure    Wind Speed\n" RESET);
    printf(YELLOW "--------------------------------------------------------------------------\n" RESET);
    while (current != list->header) {
        printf("%3d     %16s     %4d F     %5d%%     %7d     %5d MPH\n",
        current->data.id, current->data.city, current->data.temperature,
        current->data.humidity, current->data.pressure, current->data.wind);
        current = current->next;
    }
    printf("Total number of cities: %d\n", list->size);
}

/*
* copyList
*
* Function used to copy Sensor data before mutation in order to compare 
* previous vs current states for weather predictions.
*/
SensorList copyList(const SensorList *list) {
    SensorList copiedList;
    initSensorList(&copiedList);

    Node *current = list->header->next;
    while (current != list->header) {
        appendList(&copiedList, current->data);
        current = current->next;
    }
    return copiedList;
}

/*
* freeList
*
* Frees memory allocation for defunct lists after repeated calls to 
* progress/update the sensor data.
*/
void freeList(SensorList *list) {
    Node *current = list->header->next;

    while (current != list->header) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }

    free(list->header);
    list->header = NULL;
    list->size = 0;
}

/*
* clamp
*
* Small helper for use with updateSensors.
*/
int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

/*
* updateSensors
*
* This function creates a weighted roll for refreshing the current weather
* conditions. Base on the roll, different types of adjustments will be
* made to the sensor attributes. This provides a more realistic approach
* to updating weather patterns instead of just randoming assigning values.
* Floors and ceilings have been incorporated to prevent drifting to unreal
* extremes over many refreshes.
*/
void updateSensors(SensorList *list) {
    if (list == NULL || list->header == NULL || list->size == 0) {
        return;
    }

    unsigned int event;
    unsigned int weatherRoll = rand() % 101;

    if (weatherRoll <= 34) {          // stable
        event = 0;
    } else if (weatherRoll <= 54) {   // mild warming & drying
        event = 1;
    } else if (weatherRoll <= 69) {   // mild cooling & moistening
        event = 2;
    } else if (weatherRoll <= 79) {   // falling pressure & unsettling
        event = 3;
    } else if (weatherRoll <= 89) {   // rising pressure & clearing
        event = 4;
    } else if (weatherRoll <= 96) {   // humid & unstable
        event = 5;
    } else {                          // severe event
        event = 6;
    }

    Node *curr = list->header->next;

    while (curr != list->header) {
        switch (event) {
            case 0:   // stable: slight local wobble in either direction
                curr->data.temperature += (rand() % 3) - 1;   // -1 to +1
                curr->data.humidity    += (rand() % 3) - 1;   // -1 to +1
                curr->data.pressure    += (rand() % 3) - 1;   // -1 to +1
                curr->data.wind        += (rand() % 3) - 1;   // -1 to +1
                break;

            case 1:   // mild warming & drying: mostly warmer, but a few flat/slightly cooler
                curr->data.temperature += (rand() % 5) - 1;   // -1 to +3
                curr->data.humidity    -= (rand() % 5) + 2;   // -2 to -6
                curr->data.pressure    += rand() % 3;         // 0 to +2
                curr->data.wind        += (rand() % 4) - 1;   // -1 to +2
                break;

            case 2:   // mild cooling & moistening: mostly cooler, but a few flat/slightly warmer
                curr->data.temperature += (rand() % 5) - 3;   // -3 to +1
                curr->data.humidity    += (rand() % 4) + 2;   // +2 to +5
                curr->data.pressure    += (rand() % 3) - 1;   // -1 to +1
                curr->data.wind        += rand() % 4;         // 0 to +3
                break;

            case 3:   // falling pressure & unsettling
                curr->data.temperature += (rand() % 5) - 2;   // -2 to +2
                curr->data.humidity    += (rand() % 8) + 1;   // +1 to +8
                curr->data.pressure    -= (rand() % 3) + 2;   // -2 to -4
                curr->data.wind        += (rand() % 5) + 2;   // +2 to +6
                break;

            case 4:   // rising pressure & clearing: fixed drying logic
                curr->data.temperature += (rand() % 4) - 1;   // -1 to +2
                curr->data.humidity    -= (rand() % 4) + 1;   // -1 to -4
                curr->data.pressure    += (rand() % 3) + 2;   // +2 to +4
                curr->data.wind        += (rand() % 3) - 2;   // -2 to 0
                break;

            case 5:   // humid & unstable
                curr->data.temperature += (rand() % 5) - 1;   // -1 to +3
                curr->data.humidity    += (rand() % 7) + 4;   // +4 to +10
                curr->data.pressure    -= (rand() % 3) + 1;   // -1 to -3
                curr->data.wind        += rand() % 6;         // 0 to +5
                break;

            case 6:   // severe event
                curr->data.temperature += (rand() % 13) - 6;  // -6 to +6
                curr->data.humidity    += (rand() % 26) - 10; // -10 to +15
                curr->data.pressure    += (rand() % 9) - 4;   // -4 to +4
                curr->data.wind        += (rand() % 8) + 5;   // +5 to +12
                break;
        }

        curr->data.humidity = clamp(curr->data.humidity, 0, 100);
        curr->data.pressure = clamp(curr->data.pressure, 870, 1085);
        curr->data.wind = clamp(curr->data.wind, 0, 60);

        curr = curr->next;
    }
}

/*
* calculateDeltas
*
* This function creates a record of the changes in conditions
* when updating the weather. Those changes in conditions are then
* used to generate more meaningful data in the weather forcast module.
*/
void calculateDeltas(const SensorList *prevList, const SensorList *newList, SensorList *delta) {
    Node *currPrev = prevList->header->next;
    Node *currNew = newList->header->next;

    while (currPrev != prevList->header && currNew != newList->header) {
        Sensor d;

        d.id = currNew->data.id;

        strncpy(d.city, currNew->data.city, CITY_LEN - 1);
        d.city[CITY_LEN - 1] = '\0';

        d.temperature = currNew->data.temperature - currPrev->data.temperature;
        d.humidity = currNew->data.humidity - currPrev->data.humidity;
        d.pressure = currNew->data.pressure - currPrev->data.pressure;
        d.wind = currNew->data.wind - currPrev->data.wind;

        appendList(delta, d);

        currPrev = currPrev->next;
        currNew = currNew->next;
    }
}

/*
* weatherForecast
*
* Takes user selection of city and provides a detailed look
* at the current conditions, latest changes, and ultimately
* provides a rudimentary forecast based on combinations of both
* current and changing conditions.
*/
void weatherForecast(SensorList *currentList, SensorList *delta, int sensor) {
    Node *target = currentList->header->next;

    while (target != currentList->header) {
        if (target->data.id == sensor) {
            break;
        }
        target = target->next;
    }

    if (target == currentList->header) {
        printf("Sensor %d not found in current list.\n", sensor);
        return;
    }

    printf(YELLOW "\nForecast for: %s\n", target->data.city, RESET);
    printf(YELLOW "------------------------------------------\n" RESET);

    printf("Current temperature: %d F\n", target->data.temperature);
    printf("Current humidity:    %d%%\n", target->data.humidity);
    printf("Current pressure:    %d mb\n", target->data.pressure);
    printf("Current wind speed:  %d mph\n", target->data.wind);

    Node *tarDelta = delta->header->next;

    while (tarDelta != delta->header) {
        if (tarDelta->data.id == sensor) {
            break;
        }
        tarDelta = tarDelta->next;
    }

    if (tarDelta == delta->header) {
        printf("Sensor %d not found in delta list.\n", sensor);
        return;
    }

    printf(YELLOW "\nRecent trends (since last update):\n" RESET);
    printf("Temperature change: %+d F\n", tarDelta->data.temperature);
    printf("Humidity change:    %+d%%\n", tarDelta->data.humidity);
    printf("Pressure change:    %+d mb\n", tarDelta->data.pressure);
    printf("Wind change:        %+d mph\n", tarDelta->data.wind);

    printf(YELLOW "\nForecast:\n" RESET);

    if (target->data.pressure > 1020 && target->data.humidity < 40) {
        printf("Clear and stable conditions expected.\n");
    } else if (target->data.humidity > 85 &&
               tarDelta->data.pressure < 0 &&
               target->data.wind > 20) {
        printf("Thunderstorms may be developing.\n");
    } else if (target->data.humidity > 80 && tarDelta->data.pressure < 0) {
        printf("Rain is likely.\n");
    } else if (target->data.wind > 30) {
        printf("Windy conditions expected.\n");
    } else if (target->data.humidity > 60) {
        printf("Cloudy or overcast conditions likely.\n");
    } else {
        printf("Partly cloudy with no major changes expected.\n");
    }

    printf("\n");
}

/*
* printCities
*
* Displays list of cities and their corresponding ID numbers
* for user to choose from for the forecasting function.
*/
void printCities(SensorList *list) {
    printf(YELLOW "\n     Available Cities\n");
    printf(YELLOW "    -----------------------\n" RESET);
    printf(YELLOW " ID   City              ID   City\n" RESET);
    printf(YELLOW "------------------------------------------------\n" RESET);

    Node *curr = list->header->next;

    while (curr != list->header) {
        // Left column
        printf(" %-3d %-18s",
               curr->data.id,
               curr->data.city);

        curr = curr->next;

        // Right column (only if another node exists)
        if (curr != list->header) {
            printf(" %-3d %-18s",
                   curr->data.id,
                   curr->data.city);
            curr = curr->next;
        }

        printf("\n");
    }

    printf("------------------------------------------------\n");
}


//=============================================================================
//                         Statistical Functions
//=============================================================================


/*
* sortTemps
*
* Utilizes insertion sort algorithm to sort the sensor array
* based on temperature values and then prints the top five
* locations with the highest and lowest temperatures.
*/
void sortTemps(const SensorList *list) {
    if (list == NULL || list->header == NULL || list->size == 0) {
        printf("No sensor data available.\n");
        return;
    }

    int count = list->size;
    Sensor *tempSorted = malloc(count * sizeof(Sensor));
    if (tempSorted == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    Node *curr = list->header->next;
    int i = 0;

    while (curr != list->header && i < count) {
        tempSorted[i] = curr->data;
        curr = curr->next;
        i++;
    }

    for (int k = 1; k < count; k++) {
        Sensor key = tempSorted[k];
        int h = k - 1;

        while (h >= 0 && tempSorted[h].temperature > key.temperature) {
            tempSorted[h + 1] = tempSorted[h];
            h--;
        }
        tempSorted[h + 1] = key;
    }

    int topCount = (count < 5) ? count : 5;

    printf(YELLOW "\nThe five hottest cities are:\n" RESET);
    printf(YELLOW " City              Temperature\n" RESET);
    printf(YELLOW "-------------------------------\n" RESET);
    for (int j = count - 1; j >= count - topCount; j--) {
        printf(" %-17s %3d F\n", tempSorted[j].city, tempSorted[j].temperature);
    }

    printf(YELLOW "-------------------------------\n" RESET);
    printf(YELLOW "The five coldest cities are:\n" RESET);
    printf(YELLOW " City              Temperature\n" RESET);
    printf(YELLOW "-------------------------------\n" RESET);
    for (int j = 0; j < topCount; j++) {
        printf(" %-17s %3d F\n", tempSorted[j].city, tempSorted[j].temperature);
    }
    printf("-------------------------------\n");

    free(tempSorted);
}

/*
* sortHumid
*
* Utilizes insertion sort algorithm to sort the sensor array
* based on temperature values and then prints the top five
* locations with the highest and lowest temperatures.
*/
void sortHumid(const SensorList *list) {
    if (list == NULL || list->header == NULL || list->size == 0) {
        printf("No sensor data available.\n");
        return;
    }

    int count = list->size;
    Sensor *humSorted = malloc(count * sizeof(Sensor));
    if (humSorted == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    Node *curr = list->header->next;
    int i = 0;

    while (curr != list->header && i < count) {
        humSorted[i] = curr->data;
        curr = curr->next;
        i++;
    }

    for (int k = 1; k < count; k++) {
        Sensor key = humSorted[k];
        int h = k - 1;

        while (h >= 0 && humSorted[h].humidity > key.humidity) {
            humSorted[h + 1] = humSorted[h];
            h--;
        }
        humSorted[h + 1] = key;
    }

    int topCount = (count < 5) ? count : 5;

    printf(YELLOW "\nThe five wettest cities are:\n" RESET);
    printf(YELLOW " City              Humidity\n" RESET);
    printf(YELLOW "-------------------------------\n" RESET);
    for (int j = count - 1; j >= count - topCount; j--) {
        printf(" %-17s %3d%%\n", humSorted[j].city, humSorted[j].humidity);
    }

    printf(YELLOW "-------------------------------\n" RESET);
    printf(YELLOW "The five driest cities are:\n" RESET);
    printf(YELLOW " City              Humidity\n" RESET);
    printf(YELLOW "-------------------------------\n" RESET);
    for (int j = 0; j < topCount; j++) {
        printf(" %-17s %3d%%\n", humSorted[j].city, humSorted[j].humidity);
    }
    printf("-------------------------------\n");

    free(humSorted);
}

/*
* computeStats
*
* Function that performs some statistical analysis on the sensor data 
* to provide useful (simulated) information for the user.
*/
void computeStats(const SensorList *currentList, const SensorList *deltaList, Stats *stats) {
    if (currentList == NULL || currentList->header == NULL || currentList->size == 0) {
        return;
    }

    Node *curr = currentList->header->next;

    int sumTemp = 0;
    int sumHumidity = 0;
    int sumPressure = 0;
    int sumWind = 0;
    int count = 0;

    stats->minTemp = INT_MAX;
    stats->maxTemp = INT_MIN;

    while (curr != currentList->header) {
        int temp = curr->data.temperature;

        sumTemp += curr->data.temperature;
        sumHumidity += curr->data.humidity;
        sumPressure += curr->data.pressure;
        sumWind += curr->data.wind;

        if (temp < stats->minTemp) {
            stats->minTemp = temp;
            strncpy(stats->minCity, curr->data.city, CITY_LEN - 1);
            stats->minCity[CITY_LEN - 1] = '\0';
        }

        if (temp > stats->maxTemp) {
            stats->maxTemp = temp;
            strncpy(stats->maxCity, curr->data.city, CITY_LEN - 1);
            stats->maxCity[CITY_LEN - 1] = '\0';
        }

        count++;
        curr = curr->next;
    }

    stats->avgTemp = (double)sumTemp / count;
    stats->avgHumidity = (double)sumHumidity / count;
    stats->avgPressure = (double)sumPressure / count;
    stats->avgWind = (double)sumWind / count;

    stats->avgTempChange = 0.0;
    stats->avgHumidityChange = 0.0;
    stats->avgPressureChange = 0.0;
    stats->avgWindChange = 0.0;

    stats->warmingCities = 0;
    stats->coolingCities = 0;
    stats->risingPressureCities = 0;
    stats->fallingPressureCities = 0;
    stats->increasingWindCities = 0;

    if (deltaList == NULL || deltaList->header == NULL || deltaList->size == 0) {
        return;
    }

    Node *delt = deltaList->header->next;

    int sumTempChange = 0;
    int sumHumidityChange = 0;
    int sumPressureChange = 0;
    int sumWindChange = 0;
    int deltaCount = 0;

    while (delt != deltaList->header) {
        sumTempChange += delt->data.temperature;
        sumHumidityChange += delt->data.humidity;
        sumPressureChange += delt->data.pressure;
        sumWindChange += delt->data.wind;

        if (delt->data.temperature > 0) {
            stats->warmingCities++;
        } else if (delt->data.temperature < 0) {
            stats->coolingCities++;
        }

        if (delt->data.pressure > 0) {
            stats->risingPressureCities++;
        } else if (delt->data.pressure < 0) {
            stats->fallingPressureCities++;
        }

        if (delt->data.wind > 0) {
            stats->increasingWindCities++;
        }

        deltaCount++;
        delt = delt->next;
    }

    if (deltaCount > 0) {
        stats->avgTempChange = (double)sumTempChange / deltaCount;
        stats->avgHumidityChange = (double)sumHumidityChange / deltaCount;
        stats->avgPressureChange = (double)sumPressureChange / deltaCount;
        stats->avgWindChange = (double)sumWindChange / deltaCount;
    }
}

/*
* printStats
*
* Print function for displaying regional stats.
*/
void printStats(const Stats *stats) {
    printf(YELLOW "\nRegional Weather Summary\n" RESET);
    printf(YELLOW "--------------------------------------------------\n" RESET);
    printf("Average Temp:          %.1f F\n", stats->avgTemp);
    printf("Average Humidity:      %.1f%%\n", stats->avgHumidity);
    printf("Average Pressure:      %.1f mb\n", stats->avgPressure);
    printf("Average Wind:          %.1f mph\n", stats->avgWind);
    printf("\n");
    printf("Hottest City:          %s (%d F)\n", stats->maxCity, stats->maxTemp);
    printf("Coldest City:          %s (%d F)\n", stats->minCity, stats->minTemp);
    printf("\n");
    printf("Average Temp Change:   %+0.1f F\n", stats->avgTempChange);
    printf("Average Humidity Chg:  %+0.1f%%\n", stats->avgHumidityChange);
    printf("Average Pressure Chg:  %+0.1f mb\n", stats->avgPressureChange);
    printf("Average Wind Change:   %+0.1f mph\n", stats->avgWindChange);
    printf("\n");
    printf("Warming Cities:        %d\n", stats->warmingCities);
    printf("Cooling Cities:        %d\n", stats->coolingCities);
    printf("Rising Pressure:       %d\n", stats->risingPressureCities);
    printf("Falling Pressure:      %d\n", stats->fallingPressureCities);
    printf("Increasing Wind:       %d\n", stats->increasingWindCities);
    printf("--------------------------------------------------\n\n");
}

/*
* heatIndexReport
*
* This function uses the Heat Index formula to calculate what it feels
* like given a relationship between temperature and humidity levels.
* It then prints out warning about the increased felt temperature.
*/
void heatIndexReport(const SensorList *list) {
    if (list == NULL || list->header == NULL || list->size == 0) {
        printf("\nNo sensor data available.\n\n");
        return;
    }

    Node *curr = list->header->next;
    int foundAny = 0;

    printf(RED "\n***ADVISORY*** HEAT INDEX:\n" RESET);
    printf(YELLOW "The following locations are experiencing higher risk\n" RESET);
    printf(YELLOW "of heat stroke due to elevated temperature and humidity:\n\n" RESET);
    printf(YELLOW " City               Actual   Feels Like\n" RESET);
    printf(YELLOW "--------------------------------------------\n" RESET);

    while (curr != list->header) {
        if (curr->data.temperature >= 88 && curr->data.humidity >= 40) {
            double feelsLike = -42.379
                + 2.04901523 * curr->data.temperature
                + 10.14333127 * curr->data.humidity
                - 0.22475541 * curr->data.temperature * curr->data.humidity
                - 0.00683783 * curr->data.temperature * curr->data.temperature
                - 0.05481717 * curr->data.humidity * curr->data.humidity
                + 0.00122874 * curr->data.temperature * curr->data.temperature * curr->data.humidity
                + 0.00085282 * curr->data.temperature * curr->data.humidity * curr->data.humidity
                - 0.00000199 * curr->data.temperature * curr->data.temperature
                  * curr->data.humidity * curr->data.humidity;

            printf(" %-18s %3d F %8d F\n",
                   curr->data.city,
                   curr->data.temperature,
                   (int)feelsLike);

            foundAny = 1;
        }

        curr = curr->next;
    }

    if (!foundAny) {
        printf(" No locations currently meet heat-index advisory criteria.\n");
    }

    printf("\n");
}

/*
* windChillFactor
*
* This function checks for temperature and wind speed combinations that would
* lead to dangerous exposure conditions and then prints the locations in an
* advisory report*/
void windChillFactor(const SensorList *list) {
    if (list == NULL || list->header == NULL || list->size == 0) {
        printf("\nNo sensor data available.\n\n");
        return;
    }

    Node *curr = list->header->next;
    int foundAny = 0;

    printf(RED "\n***ADVISORY*** WIND CHILL:\n" RESET);
    printf(YELLOW "The following locations are experiencing increased\n" RESET);
    printf(YELLOW "risk of frostbite due to low temperatures and wind speeds:\n\n" RESET);
    printf(YELLOW " City               Actual   Feels Like\n" RESET);
    printf(YELLOW "--------------------------------------------\n" RESET);

    while (curr != list->header) {
        if (curr->data.temperature <= 40 && curr->data.wind > 3) {

            double base = curr->data.wind;
            double exponent = 0.16;

            double feelsLike = 35.74
                + 0.6215 * curr->data.temperature
                - 35.75 * pow(base, exponent)
                + 0.4275 * curr->data.temperature * pow(base, exponent);

            printf(" %-18s %3d F %8d F\n",
                   curr->data.city,
                   curr->data.temperature,
                   (int)feelsLike);

            foundAny = 1;
        }

        curr = curr->next;
    }

    if (!foundAny) {
        printf(" No locations currently meet wind chill advisory criteria.\n");
    }

    printf("\n");
}


//=============================================================================
//                        USER I/O-RELATED FUNCTIONS
//=============================================================================


/*
* printMenu
*
* This function prints out the menu for user prompting.
*/
void printMenu() {
    printf(CYAN "                    Main Menu\n\n" RESET);

    printf("  1. Current weather conditions\n");
    printf("  2. Weather alerts\n");
    printf("  3. Highs & Lows\n");
    printf("  4. Update current conditions\n");
    printf("  5. City forecasts\n");
    printf("  6. Exit program\n\n");
    printf("Type the number of your choice and press Enter.\n\n");
}


//=============================================================================
//                              Main Program
//=============================================================================


int main(void) {

    int id = 0;
    srand(time(NULL));
    int userChoice = 0;
    int userCityChoice = 0;

    SensorList currentList;
    initSensorList(&currentList);

    char cities[MAX_CITIES][CITY_LEN];
    int cityCount = loadCities("cities.txt", cities, MAX_CITIES);
    if (cityCount == 0) {
        printf("No cities were loaded.\n");
        return(1);
    }

    generateList(&currentList, cities, cityCount, &id);
    SensorList previous = copyList(&currentList);

    SensorList delta;
    initSensorList(&delta);

    Stats stats;


    printf("\n");
    printf(CYAN "====================ATMOSTRACK====================\n" RESET);
    printf(CYAN "       Regional Weather Simulation System\n\n\n" RESET);

    while (userChoice != 6) {
        printMenu();

        scanf("%d", &userChoice);

        switch (userChoice) {
            case 1 :

                printf(CYAN "                        Regional Conditions\n" RESET);
                printf(CYAN "               ------------------------------------\n\n" RESET);
                printList(&currentList);
                computeStats(&currentList, &delta, &stats);
                printStats(&stats);
                printf("\a");
                break;
            case 2 :
                printf(RED "             Weather Alerts\n" RESET);
                printf(RED "         ----------------------\n" RESET);
                heatIndexReport(&currentList);
                windChillFactor(&currentList);
                printf("\a");
                break;
            case 3 :
                printf(CYAN "         Highs & Lows\n" RESET);
                printf(CYAN "    ----------------------\n" RESET);
                sortTemps(&currentList);
                sortHumid(&currentList);
                printf("\a");
                break;
            case 4 :
                updateSensors(&currentList);
                freeList(&delta);
                initSensorList(&delta);
                calculateDeltas(&previous, &currentList, &delta);
                freeList(&previous);
                previous = copyList(&currentList);
                printf(CYAN "    ***Weather conditions have been updated***\n" RESET);
                printf(CYAN "--------------------------------------------------\n\n" RESET);
                printf("\a");
                break;
            case 5 :
                printf("Please enter the ID of the city:\n");
                printCities(&currentList);
                scanf("%d", &userCityChoice);
                weatherForecast(&currentList, &delta, userCityChoice);
                printf("\a");
                break;
            case 6 :
                printf(CYAN "        Exiting program.\n" RESET);
                printf(CYAN "     ----------------------\n\n" RESET);
                break;
            default :
                printf("\nInvalid input. Please choose from the menu.\n");
        }
    }
    freeList(&currentList);
    freeList(&previous);
    freeList(&delta);
    return 0;
}