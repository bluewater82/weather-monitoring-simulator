#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <math.h>

#define SIZE 50
#define CITY_LEN 64
#define MAX_CITIES 100


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
    printf("Sensor ID       City       Temperature   Humidity   Pressure    Wind Speed\n");
    printf("--------------------------------------------------------------------------\n");
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
    unsigned int event;
    unsigned int weatherRoll = rand() % 101;

    Node *curr = list->header->next;

    while (curr != list->header) {
        switch (event) {
            case 0:   // stable
                curr->data.temperature += (rand() % 2) - 1;
                curr->data.humidity += (rand() % 4) - 1;
                curr->data.pressure += (rand() % 2) - 1;
                curr->data.wind += (rand() % 3) - 1;
                break;

            case 1:   // mild warming & drying
                curr->data.temperature += (rand() % 3) + 1;
                curr->data.humidity -= (rand() % 5) + 2;
                curr->data.pressure += rand() % 3;
                curr->data.wind += (rand() % 4) - 1;
                break;

            case 2:   // mild cooling & moistening
                curr->data.temperature -= (1 + rand() % 3);
                curr->data.humidity += (rand() % 4) + 2;
                curr->data.pressure += (rand() % 2) - 1;
                curr->data.wind += rand() % 4;
                break;

            case 3:   // falling pressure & unsettling
                curr->data.temperature += (rand() % 4) - 2;
                curr->data.humidity += (rand() % 8) + 1;
                curr->data.pressure -= (rand() % 3) + 2;
                curr->data.wind += (rand() % 5) + 2;
                break;

            case 4:   // rising pressure & clearing
                curr->data.temperature += (rand() % 4) - 1;
                curr->data.humidity -= (rand() % 6) - 3;
                curr->data.pressure += (rand() % 3) + 2;
                curr->data.wind += (rand() % 3) - 2;
                break;

            case 5:   // humid & unstable
                curr->data.temperature += rand() % 4;
                curr->data.humidity += (rand() % 7) + 4;
                curr->data.pressure -= (rand() % 3) + 1;
                curr->data.wind += rand() % 6;
                break;

            case 6:   // severe event
                curr->data.temperature += (rand() % 12) - 6;
                curr->data.humidity += (rand() % 26) - 10;
                curr->data.pressure += (rand() % 9) - 4;
                curr->data.wind += (rand() % 8) + 5;
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

    printf("\nForecast for %s\n", target->data.city);
    printf("-----------------------------\n");

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

    printf("\nRecent trends:\n");
    printf("Temperature change: %+d F\n", tarDelta->data.temperature);
    printf("Humidity change:    %+d%%\n", tarDelta->data.humidity);
    printf("Pressure change:    %+d mb\n", tarDelta->data.pressure);
    printf("Wind change:        %+d mph\n", tarDelta->data.wind);

    printf("\nForecast:\n");

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
    printf("\nAvailable Cities\n");
    printf("------------------------------------------------\n");
    printf(" ID   City              ID   City\n");
    printf("------------------------------------------------\n");

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
//                              Main Program
//=============================================================================


int main(void) {

    int id = 0;
    srand(time(NULL));

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
    printList(&currentList);

    updateSensors(&currentList);

    SensorList delta;
    initSensorList(&delta);

    printCities(&currentList);

    return 0;

}