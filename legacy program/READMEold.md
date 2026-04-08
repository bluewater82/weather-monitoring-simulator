# AtmosTrack: Regional Weather Simulator

A C-based weather monitoring simulation that models environmental sensor data across multiple cities and provides statistical analysis, alerts, and simple forecasting.

This project was designed as a learning exercise in fundamental computer science concepts including structs, linked lists, arrays, file I/O, sorting algorithms, simulation modeling, and modular program design.

The program generates simulated weather data for a network of sensors, allowing the user to analyze the data through a menu-driven interface, and produces simple weather forecasts based on recent environmental trends.


## Overview

AtmosTrack simulates a network of 50 weather sensors located across different cities. Each sensor records environmental conditions including:

- Temperature
- Humidity
- Atmospheric pressure
- Wind speed

The program generates initial conditions, then allows the user to:

- View current weather conditions
- Analyze statistical summaries
- Detect extreme weather conditions
- Update the simulation with new weather events
- Generate forecasts based on changing trends

Weather conditions evolve using a **weighted probabilistic model**, meaning that changes in conditions resemble realistic weather behavior rather than purely random fluctuations.


## Features
### Sensor Network Simulation

The program simulates a distributed network of weather stations using C structs. The network is structured as a linked list.

Each sensor contains:

```
typedef struct {
    int id;
    char city[CITY_LEN];
    int temperature;
    int humidity;
    int pressure;
    int windSpeed;
} Sensor;
```
Cities are loaded from a text file to give each sensor a realistic location name.

### Weather Event System

Weather updates are driven by a **probabilistic event model**.

Possible weather events include:

- Stable conditions
- Mild warming and drying
- Mild cooling and moistening
- Falling pressure systems
- Rising pressure systems
- Humid unstable systems
- Severe weather events

Each event modifies environmental variables with controlled ranges to prevent unrealistic drift.

### Weather Trend Tracking

The program tracks how conditions change between updates using a secondary struct:

```
typedef struct {
    char city[CITY_LEN];
    int tempDelta;
    int humDelta;
    int pressDelta;
    int windDelta;
} SensorDelta;
```
This allows the system to *analyze recent trends* rather than relying solely on current conditions.

### Statistical Analysis

The program calculates useful weather summaries including:

- Average temperature
- Average humidity
- Top 5 hottest cities
- Top 5 coldest cities
- Most humid locations
- Driest locations

Sorting is implement using an **insertion sort algorithm**, demostrating the fundamental algorithmic technique.

### Weather Alerts

The program idendifies potentially dangerous environmental conditions:

- Heat Index Advisory - Uses official heat index formula to estimate perceived temperature when both heat and humidity are high.
- Wind Chill Advisory - Uses the official wind chill formula to calculate the perceived temperature when wind and cold conditions are combined.

These alerts highlight locations where exposure may be hazardous.

### Forecast Engine

Users can request a forecast for any city listed in the program.

The forecast system analyzes:

- Current atmospheric pressure
- Humidity levels
- Wind speed
- Recent trends

Example forecast outputs include:

- Clear and stable conditions
- Thunderstorm development
- Rain likely
- Cloudy or overcast
- Windy conditions

Although this is a simplified model, the system demonstrates how multiple environmental variable can be combined to infer likely weather outcomes.

## Program Menu

The program is controlled through a menu-driven interface.

```
Main Menu
---------
1. Current weather conditions
2. Weather alerts
3. Highs & Lows
4. Update current conditions
5. City forecasts
6. Exit program
```

## Example Workflow

1. Program initializes sensors with simulated data
2. User views current conditions
3. Weather alerts highlight dangerous areas
4. User updates the simulation
5. Environmental trends are calculated
6. Forecasts can be generated for individual cities
7. User can loop through any option until exiting program

## Technologies Used

Language:

 - C

 Standard Libraries:

 - stdio.h
 - stdlib.h
 - string.h
 - math.h
 - limits.h
 - time.h

 Concepts demonstrated:

 - Struct-based data modeling
 - File input/output
 - Randomized simulation
 - Probabilistic event modeling
 - Sorting algorithms
 - Statistical analysis
 - Menu-driven user interfaces
 - Modular function design

## Input File

The program reads city names from a simple text file:
```
cities.txt
```
Example:
```
Albuquerque
Denver
Seattle
Chicago
Phoenix
```
There are currently 100 city names stored in the file, but if the number of sensors were raised above the number of cities provided in the file the program will automatically generate names such as:
```
Station_153
Station_ 154
```

## Compilation

Although a compiled artifact is already provided, users can manually compile using GCC:
```
gcc weatherProgram.c -o weather
```
Run the program:
```
./weather
```

## Example Output

Example table:
```
Sensor  City            Temperature  Humidity
-----------------------------------------------
1       Albuquerque          74 F      42%
2       Denver               61 F      55%
3       Seattle              58 F      81%
...
```
Example forecast:
```
Forecast for Denver

Current temperature: 61 F
Current humidity: 55%
Current pressure: 1015 mb
Current wind speed: 12 mph

Recent trends:
Temperature change: -2 F
Humidity change: +4%
Pressure change: -3 mb
Wind change: +5 mph

Forecast:
Rain is likely.
```

## Future Improvements

Planned enhancements include:

- Time-series weather tracking
- Regional climate modeling
- Improved forecasting logic
- Median, variance, and standard deviation analysis
- Wind advisory detection/alerts
- More realistic atmospheric models
- Persistent data logging
- Graphical visualization tools

## Learning Goals

This project was built to reinforce core programming skills including:

- Struct-based data modeling
- Modular program design
- Data analysis from simulated systems
- Implementation of classical algorithms
- Designing scalable command-line programs

## Author

- Andre DeHerrera
- Computer Science Major
- University of New Mexico

