package com.atmostrack.model;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Random;
import java.util.stream.Collectors;

public class SensorList {
    private final List<Sensor> sensors = new ArrayList<>();

    public void init() {
        sensors.clear();
    }

    public void append(Sensor sensor) {
        sensors.add(new Sensor(sensor));
    }

    public boolean isEmpty() {
        return sensors.isEmpty();
    }

    public int size() {
        return sensors.size();
    }

    public List<Sensor> getSensors() {
        return Collections.unmodifiableList(sensors);
    }

    public Sensor getSensorById(int id) {
        for (Sensor sensor : sensors) {
            if (sensor.getId() == id) {
                return sensor;
            }
        }
        return null;
    }

    public SensorList copy() {
        SensorList copy = new SensorList();
        for (Sensor sensor : sensors) {
            copy.append(sensor);
        }
        return copy;
    }

    public String formatCurrentConditions() {
    StringBuilder builder = new StringBuilder();
    builder.append("      City             Temperature  Humidity  Pressure  Wind Speed\n");
    builder.append("----------------------------------------------------------------------\n");
    for (Sensor sensor : sensors) {
        builder.append(String.format("  %-18s %8d F %8d %% %8d %8d MPH\n",
                sensor.getCity(), sensor.getTemperature(), sensor.getHumidity(), sensor.getPressure(), sensor.getWind()));
    }
    builder.append(String.format("Total number of cities: %d\n", sensors.size()));
    return builder.toString();
}

    public String formatCities() {
        StringBuilder builder = new StringBuilder();
        builder.append("\n     Available Cities\n");
        builder.append("    -----------------------\n");
        builder.append(" ID   City              ID   City\n");
        builder.append("------------------------------------------------\n");

        for (int i = 0; i < sensors.size(); i += 2) {
            Sensor left = sensors.get(i);
            builder.append(String.format(" %-3d %-18s", left.getId(), left.getCity()));
            if (i + 1 < sensors.size()) {
                Sensor right = sensors.get(i + 1);
                builder.append(String.format(" %-3d %-18s", right.getId(), right.getCity()));
            }
            builder.append("\n");
        }
        builder.append("------------------------------------------------\n");
        return builder.toString();
    }

    public String formatHottestColdest() {
        if (isEmpty()) {
            return "No sensor data available.\n";
        }

        List<Sensor> sorted = sensors.stream()
                .sorted(Comparator.comparingInt(Sensor::getTemperature))
                .collect(Collectors.toList());

        int topCount = Math.min(5, sorted.size());
        StringBuilder builder = new StringBuilder();
        builder.append("\nThe five hottest cities are:\n");
        builder.append(" City              Temperature\n");
        builder.append("-------------------------------\n");
        for (int j = sorted.size() - 1; j >= sorted.size() - topCount; j--) {
            Sensor sensor = sorted.get(j);
            builder.append(String.format(" %-17s %3d F\n", sensor.getCity(), sensor.getTemperature()));
        }
        builder.append("-------------------------------\n");
        builder.append("The five coldest cities are:\n");
        builder.append(" City              Temperature\n");
        builder.append("-------------------------------\n");
        for (int j = 0; j < topCount; j++) {
            Sensor sensor = sorted.get(j);
            builder.append(String.format(" %-17s %3d F\n", sensor.getCity(), sensor.getTemperature()));
        }
        builder.append("-------------------------------\n");
        return builder.toString();
    }

    public String formatWettestDriest() {
        if (isEmpty()) {
            return "No sensor data available.\n";
        }

        List<Sensor> sorted = sensors.stream()
                .sorted(Comparator.comparingInt(Sensor::getHumidity))
                .collect(Collectors.toList());

        int topCount = Math.min(5, sorted.size());
        StringBuilder builder = new StringBuilder();
        builder.append("\nThe five wettest cities are:\n");
        builder.append(" City              Humidity\n");
        builder.append("-------------------------------\n");
        for (int j = sorted.size() - 1; j >= sorted.size() - topCount; j--) {
            Sensor sensor = sorted.get(j);
            builder.append(String.format(" %-17s %3d%%\n", sensor.getCity(), sensor.getHumidity()));
        }
        builder.append("-------------------------------\n");
        builder.append("The five driest cities are:\n");
        builder.append(" City              Humidity\n");
        builder.append("-------------------------------\n");
        for (int j = 0; j < topCount; j++) {
            Sensor sensor = sorted.get(j);
            builder.append(String.format(" %-17s %3d%%\n", sensor.getCity(), sensor.getHumidity()));
        }
        builder.append("-------------------------------\n");
        return builder.toString();
    }

    public void updateSensors(Random random) {
        if (isEmpty()) {
            return;
        }

        int weatherRoll = random.nextInt(101);
        int event;

        if (weatherRoll <= 25) {
            event = 0;  // Stable - 25%
        } else if (weatherRoll <= 40) {
            event = 1;  // Warming/drying - 15%
        } else if (weatherRoll <= 55) {
            event = 2;  // Cooling/humidifying - 15%
        } else if (weatherRoll <= 70) {
            event = 3;  // Pressure drop, humid, windy - 15%
        } else if (weatherRoll <= 80) {
            event = 4;  // Pressure rise, drying - 10%
        } else if (weatherRoll <= 90) {
            event = 5;  // Humid, pressure drop - 10%
        } else {
            event = 6;  // Severe weather - 10%
        }

        for (Sensor sensor : sensors) {
            int temperature = sensor.getTemperature();
            int humidity = sensor.getHumidity();
            int pressure = sensor.getPressure();
            int wind = sensor.getWind();

            switch (event) {
                case 0:
                    temperature += random.nextInt(3) - 1;
                    humidity += random.nextInt(3) - 1;
                    pressure += random.nextInt(3) - 1;
                    wind += random.nextInt(3) - 1;
                    break;
                case 1:
                    temperature += random.nextInt(5) - 1;
                    humidity -= random.nextInt(5) + 2;
                    pressure += random.nextInt(3);
                    wind += random.nextInt(4) - 1;
                    break;
                case 2:
                    temperature += random.nextInt(5) - 3;
                    humidity += random.nextInt(4) + 2;
                    pressure += random.nextInt(3) - 1;
                    wind += random.nextInt(4);
                    break;
                case 3:
                    temperature += random.nextInt(5) - 2;
                    humidity += random.nextInt(8) + 1;
                    pressure -= random.nextInt(3) + 2;
                    wind += random.nextInt(8) + 5;  // Higher wind range for potential thunderstorms
                    break;
                case 4:
                    temperature += random.nextInt(4) - 1;
                    humidity -= random.nextInt(4) + 1;
                    pressure += random.nextInt(3) + 2;
                    wind += random.nextInt(3) - 2;
                    break;
                case 5:
                    temperature += random.nextInt(5) - 1;
                    humidity += random.nextInt(7) + 4;
                    pressure -= random.nextInt(3) + 1;
                    wind += random.nextInt(6);
                    break;
                case 6:
                    temperature += random.nextInt(13) - 6;
                    humidity += random.nextInt(26) - 5;  // More likely to reach high humidity
                    pressure -= random.nextInt(8) + 3;   // More significant pressure drop
                    wind += random.nextInt(12) + 8;      // Higher wind speeds for thunderstorms
                    break;
                default:
                    break;
            }

            sensor.setTemperature(clamp(temperature, -120, 140));
            sensor.setHumidity(clamp(humidity, 0, 100));
            sensor.setPressure(clamp(pressure, 870, 1085));
            sensor.setWind(clamp(wind, 0, 60));
        }
    }

    public SensorList calculateDeltas(SensorList previousList) {
        SensorList delta = new SensorList();
        if (previousList == null || previousList.isEmpty()) {
            return delta;
        }

        HashMap<Integer, Sensor> previousById = new HashMap<>();
        for (Sensor previous : previousList.getSensors()) {
            previousById.put(previous.getId(), previous);
        }

        for (Sensor current : sensors) {
            Sensor previous = previousById.get(current.getId());
            if (previous != null) {
                Sensor difference = new Sensor(
                        current.getId(),
                        current.getCity(),
                        current.getTemperature() - previous.getTemperature(),
                        current.getHumidity() - previous.getHumidity(),
                        current.getPressure() - previous.getPressure(),
                        current.getWind() - previous.getWind());
                delta.append(difference);
            }
        }
        return delta;
    }

    public Stats computeStats(SensorList deltaList) {
        return Stats.from(this, deltaList);
    }

    public String formatHeatIndexAdvisory() {
        if (isEmpty()) {
            return "\nNo sensor data available.\n\n";
        }

        StringBuilder builder = new StringBuilder();
        builder.append("\n***ADVISORY*** HEAT INDEX:\n");
        builder.append("The following locations are experiencing higher risk\n");
        builder.append("of heat stroke due to elevated temperature and humidity:\n\n");
        builder.append(" City               Actual   Feels Like\n");
        builder.append("--------------------------------------------\n");

        boolean foundAny = false;
        for (Sensor sensor : sensors) {
            if (sensor.getTemperature() >= 88 && sensor.getHumidity() >= 40) {
                double feelsLike = -42.379
                        + 2.04901523 * sensor.getTemperature()
                        + 10.14333127 * sensor.getHumidity()
                        - 0.22475541 * sensor.getTemperature() * sensor.getHumidity()
                        - 0.00683783 * sensor.getTemperature() * sensor.getTemperature()
                        - 0.05481717 * sensor.getHumidity() * sensor.getHumidity()
                        + 0.00122874 * sensor.getTemperature() * sensor.getTemperature() * sensor.getHumidity()
                        + 0.00085282 * sensor.getTemperature() * sensor.getHumidity() * sensor.getHumidity()
                        - 0.00000199 * sensor.getTemperature() * sensor.getTemperature()
                        * sensor.getHumidity() * sensor.getHumidity();

                builder.append(String.format(" %-18s %3d F %8d F\n",
                        sensor.getCity(), sensor.getTemperature(), (int) feelsLike));
                foundAny = true;
            }
        }

        if (!foundAny) {
            builder.append(" No locations currently meet heat-index advisory criteria.\n");
        }
        builder.append("\n");
        return builder.toString();
    }

    public String formatWindChillAdvisory() {
        if (isEmpty()) {
            return "\nNo sensor data available.\n\n";
        }

        StringBuilder builder = new StringBuilder();
        builder.append("\n***ADVISORY*** WIND CHILL:\n");
        builder.append("The following locations are experiencing increased\n");
        builder.append("risk of frostbite due to low temperatures and wind speeds:\n\n");
        builder.append(" City               Actual   Feels Like\n");
        builder.append("--------------------------------------------\n");

        boolean foundAny = false;
        for (Sensor sensor : sensors) {
            if (sensor.getTemperature() <= 40 && sensor.getWind() > 3) {
                double base = sensor.getWind();
                double exponent = 0.16;
                double feelsLike = 35.74
                        + 0.6215 * sensor.getTemperature()
                        - 35.75 * Math.pow(base, exponent)
                        + 0.4275 * sensor.getTemperature() * Math.pow(base, exponent);

                builder.append(String.format(" %-18s %3d F %8d F\n",
                        sensor.getCity(), sensor.getTemperature(), (int) feelsLike));
                foundAny = true;
            }
        }

        if (!foundAny) {
            builder.append(" No locations currently meet wind chill advisory criteria.\n");
        }
        builder.append("\n");
        return builder.toString();
    }

    public String formatForecast(int sensorId, SensorList deltaList) {
        Sensor target = getSensorById(sensorId);
        if (target == null) {
            return String.format("Sensor %d not found in current list.\n", sensorId);
        }

        Sensor delta = deltaList == null ? null : deltaList.getSensorById(sensorId);
        StringBuilder builder = new StringBuilder();
        builder.append(String.format("\nForecast for: %s\n", target.getCity()));
        builder.append("------------------------------------------\n");
        builder.append(String.format("Current temperature: %d F\n", target.getTemperature()));
        builder.append(String.format("Current humidity:    %d%%\n", target.getHumidity()));
        builder.append(String.format("Current pressure:    %d mb\n", target.getPressure()));
        builder.append(String.format("Current wind speed:  %d mph\n", target.getWind()));

        if (delta == null) {
            builder.append(String.format("Sensor %d not found in delta list.\n", sensorId));
            return builder.toString();
        }

        builder.append("\nRecent trends (since last update):\n");
        builder.append(String.format("Temperature change: %+d F\n", delta.getTemperature()));
        builder.append(String.format("Humidity change:    %+d%%\n", delta.getHumidity()));
        builder.append(String.format("Pressure change:    %+d mb\n", delta.getPressure()));
        builder.append(String.format("Wind change:        %+d mph\n", delta.getWind()));

        builder.append("\nForecast:\n");
        if (target.getPressure() > 1020 && target.getHumidity() < 40) {
            builder.append("Clear and stable conditions expected.\n");
        } else if (target.getHumidity() > 85 && delta.getPressure() < 0 && target.getWind() > 20) {
            builder.append("Thunderstorms may be developing.\n");
        } else if (target.getHumidity() > 80 && delta.getPressure() < 0) {
            builder.append("Rain is likely.\n");
        } else if (target.getWind() > 30) {
            builder.append("Windy conditions expected.\n");
        } else if (target.getHumidity() > 60) {
            builder.append("Cloudy or overcast conditions likely.\n");
        } else {
            builder.append("Partly cloudy with no major changes expected.\n");
        }
        builder.append("\n");
        return builder.toString();
    }

    private int clamp(int value, int min, int max) {
        if (value < min) {
            return min;
        }
        if (value > max) {
            return max;
        }
        return value;
    }
}
