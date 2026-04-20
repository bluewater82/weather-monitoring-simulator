package com.atmostrack.model;

public class Sensor {
    private int id;
    private String city;
    private int temperature;
    private int humidity;
    private int pressure;
    private int wind;

    public Sensor() {
        this.city = "";
    }

    public Sensor(int id, String city, int temperature, int humidity, int pressure, int wind) {
        this.id = id;
        this.city = city;
        this.temperature = temperature;
        this.humidity = humidity;
        this.pressure = pressure;
        this.wind = wind;
    }

    public Sensor(Sensor other) {
        this.id = other.id;
        this.city = other.city;
        this.temperature = other.temperature;
        this.humidity = other.humidity;
        this.pressure = other.pressure;
        this.wind = other.wind;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getCity() {
        return city;
    }

    public void setCity(String city) {
        this.city = city;
    }

    public int getTemperature() {
        return temperature;
    }

    public void setTemperature(int temperature) {
        this.temperature = temperature;
    }

    public int getHumidity() {
        return humidity;
    }

    public void setHumidity(int humidity) {
        this.humidity = humidity;
    }

    public int getPressure() {
        return pressure;
    }

    public void setPressure(int pressure) {
        this.pressure = pressure;
    }

    public int getWind() {
        return wind;
    }

    public void setWind(int wind) {
        this.wind = wind;
    }

    @Override
    public String toString() {
        return String.format("%d %s %dF %d%% %dmb %dmph", id, city, temperature, humidity, pressure, wind);
    }
}
