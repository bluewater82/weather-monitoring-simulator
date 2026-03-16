#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include <math.h>

#define SIZE 50
#define CITY_LEN 64
#define MAX_CITIES 100

/*
* Author - Andre DeHerrera
*
* Weather monitoring program
*
* This program uses fundamental CS concepts such as struct, arrays, functions, 
* sorting, basic arithmetic, and output formatting to simulate a collection of 
* weather sensors and provide basic statistics based on the simulated data.
*
* Future iterations will include temporal data collection so that trends can
* be evaluated from the data to provide weather predictions. Effort will also
* be made to eventually figure out a way to have regional weather patterns
* that feel more realistic to the locations used in the simulation.
*/


/*============================================================================
                SENSOR INITIALIZATION & REFRESH FUNCTIONS
============================================================================*/


/*
* Sensor structs with multiple attributes that will be populated and used for
* simulation. Attributes such as temperature, pressure, and humidity were chosen
* as those will be used to evaluate the weather summaries/predictions. A second
* sensor struct will be used to track changes as conditions are refreshed.
*/
typedef struct {
        int id;
        char city[CITY_LEN];
        int temperature;
        int humidity;
        int pressure;
        int windSpeed;
} Sensor;

typedef struct {
    char city[CITY_LEN];
    int tempDelta;
    int humDelta;
    int pressDelta;
    int windDelta;
} SensorDelta;

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

/*
* initializeSensors
*
* This function performs the initialization of all of the sensor struct
* attributes. The initialization is random and not accurate to real world
* constraints like region-specific conditions, however in the current iteration
* I have made attempts to limit ranges and incorporate the beginnings
* of conditions affecting others, such as pressure values having direct influence
* on wind conditions.
*/
void initializeSensors(Sensor sensors[], char cities[][CITY_LEN], int cityCount) {
    for (int i = 0; i < SIZE; i++){
        
        // City names
        sensors[i].id = i + 1;
        if (i < cityCount) {
            strcpy(sensors[i].city, cities[i]);
        } else {
            snprintf(sensors[i].city, CITY_LEN, "Station_%d", i + 1);
        }

        // Temperature
        sensors[i].temperature = rand() % 120;
        if (sensors[i].temperature < 25 && i % 3 == 0) {
            sensors[i].temperature *= -1;
        }
        if (sensors[i].temperature > 90 && i % 3 == 0) {
            sensors[i].temperature -= 20;
        }

        // Humidity
        double maxHumidity = 100.0 - 1.2*(sensors[i].temperature - 70);
        int roundedMax = maxHumidity;
        if (sensors[i].temperature <= 80) {
            sensors[i].humidity = 15 + rand() % 86;  // 15%-100%
        } else {
            sensors[i].humidity = rand() % roundedMax;
        }

        // Pressure
        sensors[i].pressure = 980 + rand() % 51;

        // Wind Speed
        sensors[i].windSpeed = rand() % 61;
        if (sensors[i].pressure < 995) {
            sensors[i].windSpeed += rand() % 20;
        }
        if (sensors[i].pressure > 1020) {
            sensors[i].windSpeed /= 2;
        }
    }
}

/*
* copySensors
*
* Makes a copy of current sensor array for comparison to new values
* after a weather update has occured.
*/
void copySensors(Sensor new[], Sensor previous[]) {
    for (int i = 0; i < SIZE; i++) {
        new[i] = previous[i];
    }
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
void updateSensors(Sensor new[]) {
    unsigned int event;
    unsigned int weatherRoll = rand() % 101;
    if (weatherRoll <= 34) {         // stable
        event = 0;
    } else if (weatherRoll <= 54) {  // mild warming & drying
        event = 1;
    } else if (weatherRoll <= 69) {  // mild cooling & moistening
        event = 2;
    } else if (weatherRoll <= 79) {  // falling pressure & unsettling
        event = 3;
    } else if (weatherRoll <= 89) {  // rising pressure & clearing
        event = 4;
    } else if (weatherRoll <= 96) {  // humid & unstable
        event = 5;
    } else {                         // severe event
        event = 6;
    }

    switch (event) {
        case 0 : for (int i = 0; i < SIZE; i++) {
            new[i].temperature += (rand() % 2) - 1;
            new[i].humidity += (rand() % 4) - 1;
            if (new[i].humidity > 100) {
                new[i].humidity = 100;
            }
            if (new[i].humidity < 0) {
                new[i].humidity = 0;
            }
            new[i].pressure += (rand() % 2) - 1;
            if (new[i].pressure > 1085) {
                new[i].pressure = 1085;
            }
            if (new[i].pressure < 870) {
                new[i].pressure = 870;
            }
            new[i].windSpeed += (rand() % 3) - 1;
            if (new[i].windSpeed < 0) {
                new[i].windSpeed = 0;
            }
            if (new[i].windSpeed > 60) {
                new[i].windSpeed = 60;
            }
        }
        break;
        case 1 : for (int i = 0; i < SIZE; i++) {
            new[i].temperature += (rand() % 3) + 1;
            new[i].humidity -= (rand() % 5) + 2;
            if (new[i].humidity > 100) {
                new[i].humidity = 100;
            }
            if (new[i].humidity < 0) {
                new[i].humidity = 0;
            }
            new[i].pressure += rand() % 3;
            if (new[i].pressure > 1085) {
                new[i].pressure = 1085;
            }
            if (new[i].pressure < 870) {
                new[i].pressure = 870;
            }
            new[i].windSpeed += (rand() % 4) - 1;
            if (new[i].windSpeed < 0) {
                new[i].windSpeed = 0;
            }
            if (new[i].windSpeed > 60) {
                new[i].windSpeed = 60;
            }
        }
        break;
        case 2 : for (int i = 0; i < SIZE; i++) {
            new[i].temperature -= (1 + rand() % 3);
            new[i].humidity += (rand() % 4) + 2;
            if (new[i].humidity > 100) {
                new[i].humidity = 100;
            }
            if (new[i].humidity < 0) {
                new[i].humidity = 0;
            }
            new[i].pressure += (rand() % 2) - 1;
            if (new[i].pressure > 1085) {
                new[i].pressure = 1085;
            }
            if (new[i].pressure < 870) {
                new[i].pressure = 870;
            }
            new[i].windSpeed += rand() % 4;
            if (new[i].windSpeed < 0) {
                new[i].windSpeed = 0;
            }
            if (new[i].windSpeed > 60) {
                new[i].windSpeed = 60;
            }
        }
        break;
        case 3 : for (int i = 0; i < SIZE; i++) {
            new[i].temperature += (rand() % 4) - 2;
            new[i].humidity += (rand() % 8) + 1;
            if (new[i].humidity > 100) {
                new[i].humidity = 100;
            }
            if (new[i].humidity < 0) {
                new[i].humidity = 0;
            }
            new[i].pressure -= (rand() % 3) + 2;
            if (new[i].pressure > 1085) {
                new[i].pressure = 1085;
            }
            if (new[i].pressure < 870) {
                new[i].pressure = 870;
            }
            new[i].windSpeed += (rand() % 5) + 2;
            if (new[i].windSpeed < 0) {
                new[i].windSpeed = 0;
            }
            if (new[i].windSpeed > 60) {
                new[i].windSpeed = 60;
            }
        }
        break;
        case 4 : for (int i = 0; i < SIZE; i++) {
            new[i].temperature += (rand() % 4) - 1;
            new[i].humidity -= (rand() % 6) - 3;
            if (new[i].humidity > 100) {
                new[i].humidity = 100;
            }
            if (new[i].humidity < 0) {
                new[i].humidity = 0;
            }
            new[i].pressure += (rand() % 3) + 2;
            if (new[i].pressure > 1085) {
                new[i].pressure = 1085;
            }
            if (new[i].pressure < 870) {
                new[i].pressure = 870;
            }
            new[i].windSpeed += (rand() % 3) - 2;
            if (new[i].windSpeed < 0) {
                new[i].windSpeed = 0;
            }
            if (new[i].windSpeed > 60) {
                new[i].windSpeed = 60;
            }
        }
        break;
        case 5 : for (int i = 0; i < SIZE; i++) {
            new[i].temperature += rand() % 4;
            new[i].humidity += (rand() % 7) + 4;
            if (new[i].humidity > 100) {
                new[i].humidity = 100;
            }
            if (new[i].humidity < 0) {
                new[i].humidity = 0;
            }
            new[i].pressure -= (rand() % 3) + 1;
            if (new[i].pressure > 1085) {
                new[i].pressure = 1085;
            }
            if (new[i].pressure < 870) {
                new[i].pressure = 870;
            }
            new[i].windSpeed += rand() % 6;
            if (new[i].windSpeed < 0) {
                new[i].windSpeed = 0;
            }
            if (new[i].windSpeed > 60) {
                new[i].windSpeed = 60;
            }
        }
        break;
        case 6 : for (int i = 0; i < SIZE; i++) {
            new[i].temperature += (rand() % 12) - 6;
            new[i].humidity += (rand() % 26) - 10;
            if (new[i].humidity > 100) {
                new[i].humidity = 100;
            }
            if (new[i].humidity < 0) {
                new[i].humidity = 0;
            }
            new[i].pressure += (rand() % 9) - 4;
            if (new[i].pressure > 1085) {
                new[i].pressure = 1085;
            }
            if (new[i].pressure < 870) {
                new[i].pressure = 870;
            }
            new[i].windSpeed += (rand() % 8) + 5;
            if (new[i].windSpeed < 0) {
                new[i].windSpeed = 0;
            }
            if (new[i].windSpeed > 60) {
                new[i].windSpeed = 60;
            }
        }
        break;
    }

}

/*
* calculateDeltas
*
* This function creates a record of the changes in conditions
* when updating the weather. Those changes in conditions are then
* used to generate more meaningful data in the weather forcast module.
*/
void calculateDeltas(Sensor old[], Sensor new[], SensorDelta deltas[]) {
    for (int i = 0; i < SIZE; i++) {
        strcpy(deltas[i].city, new[i].city);
        deltas[i].tempDelta = new[i].temperature - old[i].temperature;
        deltas[i].humDelta = new[i].humidity - old[i].humidity;
        deltas[i].pressDelta = new[i].pressure - old[i].pressure;
        deltas[i].windDelta = new[i].windSpeed - old[i].windSpeed;
    }
}

/*
* weatherForecase
*
* Takes user selection of city and provides a detailed look
* at the current conditions, latest changes, and ultimately
* provides a rudimentary forecast based on combinations of both
* current and changing conditions.
*/
void weatherForecast(Sensor current[], SensorDelta change[], int chosenIndex) {
    int i = chosenIndex - 1;
    printf("\n Forecast for %s\n", current[i].city);
    printf("-----------------------------\n");

    printf("Current temperature: %d F\n", current[i].temperature);
    printf("Current humidity:    %d%%\n", current[i].humidity);
    printf("Current pressure:    %d mb\n", current[i].pressure);
    printf("Current wind speed:  %d mph\n", current[i].windSpeed);

    printf("\nRecent trends:\n");
    printf("Temperature change: %d F\n", change[i].tempDelta);
    printf("Humidity change:    %d%%\n", change[i].humDelta);
    printf("Pressure change:    %d mb\n", change[i].pressDelta);
    printf("Wind change:        %d mph\n", change[i].windDelta);

    printf("\nForecast:\n");

    if (current[i].pressure > 1020 && current[i].humidity < 40) {
        printf("Clear and stable conditions expected.\n");
    } else if (current[i].humidity > 85 &&
               change[i].pressDelta < 0 &&
               current[i].windSpeed > 20) {
        printf("Thunderstorms may be developing.\n");
    } else if (current[i].humidity > 80 && change[i].pressDelta < 0) {
        printf("Rain is likely.\n");
    } else if (current[i].windSpeed > 30) {
        printf("Windy conditions expected.\n");
    } else if (current[i].humidity > 60) {
        printf("Cloudy or overcast conditions likely.\n");
    } else {
        printf("Partly cloudy with no major changes expected.\n");
    }
    printf("\n");

}


/*============================================================================
                         STATISTICAL OUTPUT FUNCTIONS
============================================================================*/


/*
* printTable
*
* This function pulls a complete list of all sensors/locations and
* displays their current conditions.
* Formatting style is still evolving.
*/
void printTable(Sensor sensors[]) {
    printf("\n Sensor  City            Temperature  Humidity\n");
    printf("-----------------------------------------------\n");

    for (int i = 0; i < SIZE; i++){
            printf(" %-4d %-18s %6d F %8d%%\n",
                sensors[i].id,
                sensors[i].city,
                sensors[i].temperature,
                sensors[i].humidity);
    }

    printf("-----------------------------------------------\n");
}

/*
* printCities
*
* Displays list of cities and their corresponding ID numbers
* for user to choose from for the forecasting function.
*/
void printCities(Sensor sensors[]) {
    printf("\n Available Cities\n");
    printf("-----------------------------------------\n");
    printf(" ID   City              ID   City\n");
    printf("-----------------------------------------\n");

    for (int i = 0; i < SIZE; i += 2) {
        printf(" %-3d %-18s",
            sensors[i].id,
            sensors[i].city);
        if (i + 1 < SIZE) {
            printf(" %-3d %-18s",
                sensors[i+1].id,
                sensors[i+1].city);
        }
        printf("\n");
    }
    printf("-----------------------------------------\n");
}

/*
* printAverages
*
* This function calculates averages through mean calculation.
* It then prints the averages.
*
* Future iterations may incorporate additional statistical
* interpretations such as medians, variance, and standard
* deviations.
*/
void printAverages(Sensor sensors[]) {
    int totalTemp = 0;
    int totalHum = 0;
    double avgTemp;
    double avgHum;

    for (int i = 0; i < SIZE; i++) {
        totalTemp += sensors[i].temperature;
        totalHum += sensors[i].humidity;
    }

    avgTemp = (double) totalTemp/SIZE;
    avgHum = (double) totalHum/SIZE;

    printf(" The average temperature is %.1f F.\n", avgTemp);
    printf(" The average humidity is %.1f%%.\n\n", avgHum);
}


/*
* sortTemps
*
* Utilizes insertion sort algorithm to sort the sensor array
* based on temperature values and then prints the top five
* locations with the highest and lowest temperatures.
*/
void sortTemps(Sensor sensors[]) {

    Sensor tempSorted[SIZE];

    for (int i = 0; i < SIZE; i++) {
    tempSorted[i] = sensors[i];
}
    for (int k = 1; k < SIZE; k++) {
        Sensor key = tempSorted[k];
        int h = k - 1;

        while (h >= 0 && tempSorted[h].temperature > key.temperature) {
        tempSorted[h + 1] = tempSorted[h];
        h--;
        }
        tempSorted[h + 1] = key;
    }
    
    printf("\n The five hottest cities are: \n");
    printf(" City           Temperature\n");
    printf("-------------------------------\n");
    for (int j = SIZE - 1; j > SIZE - 6; j--) {
        printf(" %-17s %3d F\n", tempSorted[j].city, tempSorted[j].temperature);
    }
    printf("-------------------------------\n");
    printf(" The five coldest cities are: \n");
    printf(" City           Temperature\n");
    printf("-------------------------------\n");
    for (int j = 0; j < 5; j++) {
        printf(" %-17s %3d F\n", tempSorted[j].city, tempSorted[j].temperature);
    }
    printf("-------------------------------\n");
}


/*
* sortHum
*
* Utilizes insertion sort algorithm to sort the sensor array
* based on humidity level and then prints the top five locations
* with the highest and lowest levels.
*/
void sortHum(Sensor sensors[]) {

    Sensor humSorted[SIZE];
    for (int i = 0; i < SIZE; i++){
        humSorted[i] = sensors[i];
    }
    for (int k = 1; k < SIZE; k++) {

        Sensor key = humSorted[k];
        int h = k - 1;

        while (h >= 0 && humSorted[h].humidity > key.humidity) {
        humSorted[h + 1] = humSorted[h];
        h--;
        }
        humSorted[h + 1] = key;
    }
    
    printf("\n The five most humid cities are: \n");
    printf(" City            Humidity\n");
    printf("-------------------------------\n");
    for (int j = SIZE - 1; j > SIZE - 6; j--) {
        printf(" %-17s %-3d%%\n", humSorted[j].city, humSorted[j].humidity);
    }
    printf("-------------------------------\n");
    printf(" The five driest cities are: \n");
    printf(" City            Humidity\n");
    printf("-------------------------------\n");
    for (int j = 0; j < 5; j++) {
        printf(" %-17s %-3d%%\n", humSorted[j].city, humSorted[j].humidity);
    }
    printf("-------------------------------\n\n");
}

/*
* heatIndexReport
*
* This function uses the Heat Index formula to calculate what it feels
* like given a relationship between temperature and humidity levels.
* It then prints out warning about the increased felt temperature.
*/
void heatIndexReport(Sensor sensors[]) {
    printf("\n ***ADVISORY*** HEAT INDEX:\n");
    printf("The following locations are experiencing higher risk\n");
    printf("of heat stroke due to elevated temperature and humidity:\n\n");
    printf(" City             Actual   Feels Like\n");
    printf("------------------------------------------\n");
    for (int i = 0; i < SIZE; i++) {
        if (sensors[i].temperature >= 88 && sensors[i].humidity >= 40) {
            double feelsLike = -42.379
                + 2.04901523*(sensors[i].temperature)
                + 10.14333127*(sensors[i].humidity)
                - 0.22475541*(sensors[i].temperature)*(sensors[i].humidity)
                - 0.00683783*(sensors[i].temperature)*(sensors[i].temperature)
                - 0.05481717*(sensors[i].humidity)*(sensors[i].humidity)
                + 0.00122874*(sensors[i].temperature)*(sensors[i].temperature)*(sensors[i].humidity)
                + 0.00085282*(sensors[i].temperature)*(sensors[i].humidity)*(sensors[i].humidity)
                - 0.00000199*(sensors[i].temperature)*(sensors[i].temperature)*(sensors[i].humidity)*(sensors[i].humidity);
            printf(" %-17s %3d F %7d F\n", sensors[i].city, 
                sensors[i].temperature, 
                (int) feelsLike);
        }
    }
    printf("\n\n");
}

/*
* windChillFactor
*
* This function checks for temperature and wind speed combinations that would
* lead to dangerous exposure conditions and then prints the locations in an
* advisory report*/
void windChillFactor(Sensor sensors[]) {
    printf("\n ***ADVISORY*** WIND CHILL:\n");
    printf("The following locations are experiencing increased\n");
    printf("risk of frostbite due to low temperatures and wind speeds:\n\n");
    printf(" City             Actual   Feels Like\n");
    printf("------------------------------------------\n");
    for (int i =0; i < SIZE; i++) {
        double base = sensors[i].windSpeed;
        double exponent = 0.16;
        if (sensors[i].temperature <= 40 && sensors[i].windSpeed > 3) {
            double feelsLike = 35.74
                + 0.6215*(sensors[i].temperature)
                - 35.75*(pow(base, exponent))
                + 0.4275*(sensors[i].temperature)*(pow(base, exponent));
            printf(" %-17s %3d F %7d F\n", sensors[i].city, sensors[i].temperature, (int) feelsLike);
        }
    }
    printf("\n");
}


/*============================================================================
                        USER I/O-RELATED FUNCTIONS
============================================================================*/


/*
* printMenu
*
* This function prints out the menu for user prompting.
*/
void printMenu() {
    printf("Main Menu\n");
    printf("---------\n");
    printf("Please choose an option from the following list:\n");
    printf("1. Current weather conditions\n");
    printf("2. Weather alerts\n");
    printf("3. Highs & Lows\n");
    printf("4. Update current conditions\n");
    printf("5. City forecasts\n");
    printf("6. Exit program\n");
    printf("Type the number of your choice and press Enter.\n\n");
}


/*===========================================================================
                                MAIN PROGRAM
===========================================================================*/


int main(void) {

    Sensor sensors[SIZE];
    SensorDelta deltas[SIZE] = {0};
    Sensor newSensors[SIZE];

    srand(time(NULL));

    char cities[MAX_CITIES][CITY_LEN];
    int cityCount = loadCities("cities.txt", cities, MAX_CITIES);
    int userChoice = 0;
    int userCityChoice = 0;

    initializeSensors(sensors, cities, cityCount);

    printf("\n");
    printf("--------------------------------------\n");
    printf("          Weather App 42,000          \n");
    printf("--------------------------------------\n\n");

    while (userChoice != 6) {
        printMenu();

        scanf("%d", &userChoice);

        switch (userChoice) {
            case 1 :
                printf("----------------------\n");
                printf("  Current Conditions\n");
                printf("----------------------\n");
                printTable(sensors);
                printAverages(sensors);
                printf("\a");
                break;
            case 2 :
                printf("----------------------\n");
                printf("   Weather Alerts\n");
                printf("----------------------\n");
                heatIndexReport(sensors);
                windChillFactor(sensors);
                // wind advisory
                printf("\a");
                break;
            case 3 :
                printf("----------------------\n");
                printf("  Highs & Lows\n");
                printf("----------------------\n");
                sortTemps(sensors);
                sortHum(sensors);
                printf("\a");
                break;
            case 4 :
                copySensors(newSensors, sensors);
                updateSensors(newSensors);
                calculateDeltas(sensors, newSensors, deltas);
                copySensors(sensors, newSensors);
                printf("\n----------------------------------------------\n");
                printf("  ***Weather conditions have been updated***\n");
                printf("----------------------------------------------\n\n");
                printf("\a");
                break;
            case 5 :
                printf("Please enter the ID of the city:\n");
                printCities(sensors);
                scanf("%d", &userCityChoice);
                weatherForecast(sensors, deltas, userCityChoice);
                printf("\a");
                break;
            case 6 :
                printf("\n----------------------\n");
                printf("Exiting program.\n");
                printf("----------------------\n\n");
                break;
            default :
                printf("\nInvalid input. Please choose from the menu.\n");
        }
    }
    return 0;
}
