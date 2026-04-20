package com.atmostrack.controller;

import com.atmostrack.model.Sensor;
import com.atmostrack.model.SensorList;
import com.atmostrack.model.Stats;
import com.atmostrack.view.MainWindow;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import javax.swing.SwingUtilities;

public class AtmosTrackController {
    private final MainWindow mainWindow;
    private final SensorList currentList;
    private SensorList previousList;
    private SensorList deltaList;
    private int nextSensorId = 0;
    private final Random random = new Random();

    // Image configuration - adjust these values as needed
    private static final String IMAGES_DIR = "images/";
    private static final String FRAME_BACKGROUND_IMAGE = IMAGES_DIR + "background.png";
    private static final float FRAME_BACKGROUND_OPACITY = 0.6f;  // Adjust frame background opacity (0.0 = transparent, 1.0 = opaque)
    private static final float FORECAST_IMAGE_OPACITY = 0.6f;  // Adjust forecast image opacity here (0.0 = transparent, 1.0 = opaque)

    // Panel background images for different display states
    private static final String STARTUP_IMAGE = IMAGES_DIR + "startup.png";
    private static final String CONDITIONS_IMAGE = IMAGES_DIR + "conditions.png";
    private static final String HIGHS_LOWS_IMAGE = IMAGES_DIR + "highs_lows.png";
    private static final String UPDATE_IMAGE = IMAGES_DIR + "update.png";
    private static final String ALERTS_IMAGE = IMAGES_DIR + "alerts.png";

    public AtmosTrackController() {
        mainWindow = new MainWindow();
        currentList = new SensorList();
        previousList = new SensorList();
        deltaList = new SensorList();

        initModel();
        initListeners();
        mainWindow.setVisible(true);
    }

    private void initModel() {
        List<String> cities = loadCities("cities.txt");
        if (cities.isEmpty()) {
            cities = defaultCities();
        }

        currentList.init();
        int limit = Math.min(cities.size(), 50);
        for (int i = 0; i < limit; i++) {
            currentList.append(makeSensor(cities.get(i)));
        }

        previousList = currentList.copy();
        deltaList = currentList.calculateDeltas(previousList);
        mainWindow.updateCityList(makeCityItems(currentList));
        mainWindow.setDisplayText("Initialized " + currentList.size() + " weather sensors.\nUse the buttons on the left to explore the data.");
        mainWindow.setStatusText("Ready.");
        
        // Load frame background image
        mainWindow.setFrameBackgroundImage(FRAME_BACKGROUND_IMAGE, FRAME_BACKGROUND_OPACITY);
        
        // Set startup background image
        mainWindow.setBackgroundImage(STARTUP_IMAGE, FORECAST_IMAGE_OPACITY);
    }

    private void initListeners() {
        mainWindow.addCurrentConditionsListener(e -> displayWeatherConditions());
        mainWindow.addAlertsListener(e -> displayWeatherAlerts());
        mainWindow.addHighsLowsListener(e -> displayHighsAndLows());
        mainWindow.addUpdateListener(e -> updateCurrentConditions());
        mainWindow.addForecastListener(e -> displaySelectedCityForecast());
        mainWindow.addExitListener(e -> System.exit(0));
    }

    private void displayWeatherConditions() {
        StringBuilder builder = new StringBuilder();
        builder.append("Regional Conditions\n");
        builder.append(currentList.formatCurrentConditions());
        Stats stats = currentList.computeStats(deltaList);
        builder.append(stats.formatStats());
        mainWindow.setDisplayText(builder.toString());
        mainWindow.setBackgroundImage(CONDITIONS_IMAGE, FORECAST_IMAGE_OPACITY);
        mainWindow.setStatusText("Showing current conditions.");
    }

    private void displayWeatherAlerts() {
        StringBuilder builder = new StringBuilder();
        builder.append("Weather Alerts\n");
        builder.append(currentList.formatHeatIndexAdvisory());
        builder.append(currentList.formatWindChillAdvisory());
        mainWindow.setDisplayText(builder.toString());
        mainWindow.setBackgroundImage(ALERTS_IMAGE, FORECAST_IMAGE_OPACITY);
        mainWindow.setStatusText("Showing weather alerts.");
    }

    private void displayHighsAndLows() {
        StringBuilder builder = new StringBuilder();
        builder.append("Highs & Lows\n");
        builder.append(currentList.formatHottestColdest());
        builder.append(currentList.formatWettestDriest());
        mainWindow.setDisplayText(builder.toString());
        mainWindow.setBackgroundImage(HIGHS_LOWS_IMAGE, FORECAST_IMAGE_OPACITY);
        mainWindow.setStatusText("Showing highs and lows.");
    }

    private void updateCurrentConditions() {
        previousList = currentList.copy();
        currentList.updateSensors(random);
        deltaList = currentList.calculateDeltas(previousList);
        mainWindow.updateCityList(makeCityItems(currentList));
        mainWindow.setDisplayText("*** Weather conditions have been updated ***\n\nUse Current Conditions to view the latest data.");
        mainWindow.setBackgroundImage(UPDATE_IMAGE, FORECAST_IMAGE_OPACITY);
        mainWindow.setStatusText("Conditions updated.");
    }

    private void displaySelectedCityForecast() {
        int sensorId = mainWindow.getSelectedCityId();
        if (sensorId < 0) {
            mainWindow.setStatusText("Please select a city from the dropdown.");
            return;
        }
        mainWindow.setDisplayText(currentList.formatForecast(sensorId, deltaList));
        
        // Determine forecast condition and set appropriate background image
        String forecastImage = determineForecastImage(sensorId);
        if (!forecastImage.isEmpty()) {
            mainWindow.setBackgroundImage(forecastImage, FORECAST_IMAGE_OPACITY);
        } else {
            mainWindow.clearBackgroundImage();
        }
        
        mainWindow.setStatusText("Showing forecast for sensor " + sensorId + ".");
    }

    private String determineForecastImage(int sensorId) {
        Sensor target = currentList.getSensorById(sensorId);
        if (target == null) {
            return "";
        }

        Sensor delta = deltaList == null ? null : deltaList.getSensorById(sensorId);
        if (delta == null) {
            return "";
        }

        // Map forecast conditions to image file names
        if (target.getPressure() > 1020 && target.getHumidity() < 40) {
            return IMAGES_DIR + "clear.png";
        } else if (target.getHumidity() > 85 && delta.getPressure() < 0 && target.getWind() > 20) {
            return IMAGES_DIR + "thunderstorm.png";
        } else if (target.getHumidity() > 80 && delta.getPressure() < 0) {
            return IMAGES_DIR + "rain.png";
        } else if (target.getWind() > 30) {
            return IMAGES_DIR + "windy.png";
        } else if (target.getHumidity() > 60) {
            return IMAGES_DIR + "cloudy.png";
        } else {
            return IMAGES_DIR + "partly_cloudy.png";
        }
    }

    private List<String> makeCityItems(SensorList list) {
        List<String> items = new ArrayList<>();
        for (Sensor sensor : list.getSensors()) {
            items.add(sensor.getId() + " - " + sensor.getCity());
        }
        return items;
    }

    private Sensor makeSensor(String name) {
        int temperature = random.nextInt(120);
        if (temperature < 25 && nextSensorId % 3 == 0) {
            temperature *= -1;
        }
        if (temperature > 90 && nextSensorId % 3 == 0) {
            temperature -= 20;
        }

        int humidity;
        double expectedMaxHumidity = 100.0 - 1.2 * (temperature - 70);
        if (temperature <= 80) {
            humidity = 15 + random.nextInt(86);
        } else {
            int roundedMax = Math.max(1, (int) expectedMaxHumidity);
            humidity = random.nextInt(roundedMax);
        }

        int pressure = 980 + random.nextInt(51);
        int wind = random.nextInt(61);
        if (pressure < 995) {
            wind += random.nextInt(20);
        }
        if (pressure > 1020) {
            wind /= 2;
        }

        Sensor sensor = new Sensor(nextSensorId, name, temperature, humidity, pressure, wind);
        nextSensorId++;
        return sensor;
    }

    private List<String> loadCities(String filename) {
        List<String> cityNames = new ArrayList<>();
        Path path = Paths.get(filename);
        if (!Files.exists(path)) {
            path = Paths.get(System.getProperty("user.dir"), filename);
        }

        if (Files.exists(path)) {
            try {
                Files.lines(path, StandardCharsets.UTF_8)
                        .map(String::trim)
                        .filter(line -> !line.isEmpty())
                        .forEach(cityNames::add);
            } catch (IOException e) {
                // fallback to built-in list
            }
        }

        return cityNames;
    }

    private List<String> defaultCities() {
        return List.of(
                "Seattle", "Portland", "San Francisco", "Los Angeles", "San Diego",
                "Phoenix", "Las Vegas", "Denver", "Dallas", "Houston",
                "Miami", "Atlanta", "New York", "Boston", "Chicago",
                "Minneapolis", "Detroit", "Philadelphia", "Washington", "Baltimore",
                "Charlotte", "Nashville", "New Orleans", "Kansas City", "Salt Lake City",
                "Boise", "Phoenix", "Albuquerque", "Tucson", "Reno",
                "Sacramento", "Oakland", "San Jose", "Indianapolis", "Columbus",
                "Cleveland", "Pittsburgh", "Milwaukee", "Cincinnati", "St. Louis",
                "Orlando", "Tampa", "Jacksonville", "Memphis", "Austin",
                "Oklahoma City", "Louisville", "Raleigh", "Richmond", "Anchorage"
        );
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(AtmosTrackController::new);
    }
}
