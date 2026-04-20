package com.atmostrack.model;

import java.util.List;

public class Stats {
    private double avgTemp;
    private double avgHumidity;
    private double avgPressure;
    private double avgWind;

    private int minTemp;
    private int maxTemp;
    private String minCity = "";
    private String maxCity = "";

    private double avgTempChange;
    private double avgHumidityChange;
    private double avgPressureChange;
    private double avgWindChange;

    private int warmingCities;
    private int coolingCities;
    private int risingPressureCities;
    private int fallingPressureCities;
    private int increasingWindCities;

    public static Stats from(SensorList currentList, SensorList deltaList) {
        Stats stats = new Stats();

        if (currentList == null || currentList.isEmpty()) {
            return stats;
        }

        List<Sensor> sensors = currentList.getSensors();
        stats.minTemp = Integer.MAX_VALUE;
        stats.maxTemp = Integer.MIN_VALUE;

        int sumTemp = 0;
        int sumHumidity = 0;
        int sumPressure = 0;
        int sumWind = 0;

        for (Sensor sensor : sensors) {
            int temp = sensor.getTemperature();
            sumTemp += temp;
            sumHumidity += sensor.getHumidity();
            sumPressure += sensor.getPressure();
            sumWind += sensor.getWind();

            if (temp < stats.minTemp) {
                stats.minTemp = temp;
                stats.minCity = sensor.getCity();
            }
            if (temp > stats.maxTemp) {
                stats.maxTemp = temp;
                stats.maxCity = sensor.getCity();
            }
        }

        int count = sensors.size();
        stats.avgTemp = (double) sumTemp / count;
        stats.avgHumidity = (double) sumHumidity / count;
        stats.avgPressure = (double) sumPressure / count;
        stats.avgWind = (double) sumWind / count;

        if (deltaList == null || deltaList.isEmpty()) {
            return stats;
        }

        int sumTempChange = 0;
        int sumHumidityChange = 0;
        int sumPressureChange = 0;
        int sumWindChange = 0;
        int deltaCount = 0;

        for (Sensor sensor : deltaList.getSensors()) {
            sumTempChange += sensor.getTemperature();
            sumHumidityChange += sensor.getHumidity();
            sumPressureChange += sensor.getPressure();
            sumWindChange += sensor.getWind();

            if (sensor.getTemperature() > 0) {
                stats.warmingCities++;
            } else if (sensor.getTemperature() < 0) {
                stats.coolingCities++;
            }
            if (sensor.getPressure() > 0) {
                stats.risingPressureCities++;
            } else if (sensor.getPressure() < 0) {
                stats.fallingPressureCities++;
            }
            if (sensor.getWind() > 0) {
                stats.increasingWindCities++;
            }
            deltaCount++;
        }

        if (deltaCount > 0) {
            stats.avgTempChange = (double) sumTempChange / deltaCount;
            stats.avgHumidityChange = (double) sumHumidityChange / deltaCount;
            stats.avgPressureChange = (double) sumPressureChange / deltaCount;
            stats.avgWindChange = (double) sumWindChange / deltaCount;
        }

        return stats;
    }

    public String formatStats() {
        StringBuilder builder = new StringBuilder();
        builder.append("Regional Weather Summary\n");
        builder.append("--------------------------------------------------\n");
        builder.append(String.format("Average Temp:          %.1f F\n", avgTemp));
        builder.append(String.format("Average Humidity:      %.1f%%\n", avgHumidity));
        builder.append(String.format("Average Pressure:      %.1f mb\n", avgPressure));
        builder.append(String.format("Average Wind:          %.1f mph\n", avgWind));
        builder.append("\n");
        builder.append(String.format("Hottest City:          %s (%d F)\n", maxCity, maxTemp));
        builder.append(String.format("Coldest City:          %s (%d F)\n", minCity, minTemp));
        builder.append("\n");
        builder.append(String.format("Average Temp Change:   %+.1f F\n", avgTempChange));
        builder.append(String.format("Average Humidity Chg:  %+.1f%%\n", avgHumidityChange));
        builder.append(String.format("Average Pressure Chg:  %+.1f mb\n", avgPressureChange));
        builder.append(String.format("Average Wind Change:   %+.1f mph\n", avgWindChange));
        builder.append("\n");
        builder.append(String.format("Warming Cities:        %d\n", warmingCities));
        builder.append(String.format("Cooling Cities:        %d\n", coolingCities));
        builder.append(String.format("Rising Pressure:       %d\n", risingPressureCities));
        builder.append(String.format("Falling Pressure:      %d\n", fallingPressureCities));
        builder.append(String.format("Increasing Wind:       %d\n", increasingWindCities));
        builder.append("--------------------------------------------------\n");
        return builder.toString();
    }
}
