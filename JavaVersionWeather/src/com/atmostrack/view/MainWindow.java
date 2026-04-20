package com.atmostrack.view;

import java.awt.*;
import java.awt.event.ActionListener;
import java.util.List;
import javax.swing.*;

public class MainWindow extends JFrame {
    private final FrameBackgroundPanel frameBackgroundPanel;
    private final JTextArea displayArea;
    private final JScrollPane scrollPane;
    private final ImagePanel imagePanel;
    private final JButton currentConditionsButton;
    private final JButton alertsButton;
    private final JButton highsLowsButton;
    private final JButton updateButton;
    private final JButton forecastButton;
    private final JButton exitButton;
    private final JComboBox<String> cityComboBox;
    private final JLabel statusLabel;

    public MainWindow() {
        setTitle("AtmosTrack");
        setSize(1100, 1200);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        // Create the frame background panel as the main content pane
        frameBackgroundPanel = new FrameBackgroundPanel();
        frameBackgroundPanel.setLayout(new BorderLayout());
        setContentPane(frameBackgroundPanel);

        displayArea = new JTextArea();
        displayArea.setEditable(false);
        displayArea.setFont(new Font("Monospaced", Font.BOLD, 18));
        displayArea.setLineWrap(true);
        displayArea.setWrapStyleWord(true);
        displayArea.setOpaque(false);
        displayArea.setForeground(Color.BLACK);

        imagePanel = new ImagePanel();
        imagePanel.setLayout(new BorderLayout());
        imagePanel.add(displayArea, BorderLayout.CENTER);

        scrollPane = new JScrollPane(imagePanel);
        scrollPane.setOpaque(true);
        scrollPane.setBackground(Color.WHITE);
        scrollPane.getViewport().setOpaque(true);
        scrollPane.getViewport().setBackground(Color.WHITE);

        currentConditionsButton = new JButton("Current Conditions");
        alertsButton = new JButton("Weather Alerts");
        highsLowsButton = new JButton("Highs & Lows");
        updateButton = new JButton("Update Conditions");
        forecastButton = new JButton("City Forecast");
        exitButton = new JButton("Exit");

        cityComboBox = new JComboBox<>();
        cityComboBox.setPreferredSize(new Dimension(260, 30));

        statusLabel = new JLabel("Ready.");

        initLayout();
    }

    private void initLayout() {
        JPanel headerPanel = new JPanel(new BorderLayout());
        headerPanel.setOpaque(false);
        headerPanel.setBackground(new Color(255, 255, 255, 100));  // Semi-transparent white
        JLabel title = new JLabel("AtmosTrack Weather Dashboard");
        title.setFont(new Font(Font.SANS_SERIF, Font.BOLD, 20));
        title.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        headerPanel.add(title, BorderLayout.WEST);
        frameBackgroundPanel.add(headerPanel, BorderLayout.NORTH);

        JPanel controlPanel = new JPanel();
        controlPanel.setLayout(new BoxLayout(controlPanel, BoxLayout.Y_AXIS));
        controlPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        controlPanel.setOpaque(false);
        controlPanel.setBackground(new Color(255, 255, 255, 100));  // Semi-transparent white
        controlPanel.add(currentConditionsButton);
        controlPanel.add(Box.createRigidArea(new Dimension(0, 10)));
        controlPanel.add(alertsButton);
        controlPanel.add(Box.createRigidArea(new Dimension(0, 10)));
        controlPanel.add(highsLowsButton);
        controlPanel.add(Box.createRigidArea(new Dimension(0, 10)));
        controlPanel.add(updateButton);
        controlPanel.add(Box.createRigidArea(new Dimension(0, 20)));
        controlPanel.add(new JLabel("Select City for Forecast:"));
        controlPanel.add(Box.createRigidArea(new Dimension(0, 5)));
        controlPanel.add(cityComboBox);
        controlPanel.add(Box.createRigidArea(new Dimension(0, 10)));
        controlPanel.add(forecastButton);
        controlPanel.add(Box.createVerticalGlue());
        controlPanel.add(exitButton);

        frameBackgroundPanel.add(controlPanel, BorderLayout.WEST);
        frameBackgroundPanel.add(scrollPane, BorderLayout.CENTER);

        JPanel statusPanel = new JPanel(new BorderLayout());
        statusPanel.setOpaque(true);
        statusPanel.setBackground(Color.WHITE);
        statusPanel.setBorder(BorderFactory.createEmptyBorder(5, 10, 5, 10));
        statusPanel.add(statusLabel, BorderLayout.WEST);
        frameBackgroundPanel.add(statusPanel, BorderLayout.SOUTH);
    }

    public void addCurrentConditionsListener(ActionListener listener) {
        currentConditionsButton.addActionListener(listener);
    }

    public void addAlertsListener(ActionListener listener) {
        alertsButton.addActionListener(listener);
    }

    public void addHighsLowsListener(ActionListener listener) {
        highsLowsButton.addActionListener(listener);
    }

    public void addUpdateListener(ActionListener listener) {
        updateButton.addActionListener(listener);
    }

    public void addForecastListener(ActionListener listener) {
        forecastButton.addActionListener(listener);
    }

    public void addExitListener(ActionListener listener) {
        exitButton.addActionListener(listener);
    }

    public void updateCityList(List<String> cityItems) {
        cityComboBox.removeAllItems();
        for (String item : cityItems) {
            cityComboBox.addItem(item);
        }
    }

    public int getSelectedCityId() {
        Object selected = cityComboBox.getSelectedItem();
        if (selected == null) {
            return -1;
        }
        String text = selected.toString();
        int dashIndex = text.indexOf(" - ");
        if (dashIndex < 0) {
            return -1;
        }
        try {
            return Integer.parseInt(text.substring(0, dashIndex).trim());
        } catch (NumberFormatException e) {
            return -1;
        }
    }

    public void appendToDisplay(String text) {
        displayArea.append(text + "\n");
        displayArea.setCaretPosition(displayArea.getDocument().getLength());
    }

    public void setDisplayText(String text) {
        displayArea.setText(text);
        displayArea.setCaretPosition(displayArea.getDocument().getLength());
    }

    public void setStatusText(String status) {
        statusLabel.setText(status);
    }

    public void setBackgroundImage(String imagePath, float opacity) {
        imagePanel.setBackgroundImage(imagePath, opacity);
    }

    public void setBackgroundOpacity(float opacity) {
        imagePanel.setOpacity(opacity);
    }

    public void clearBackgroundImage() {
        imagePanel.clearImage();
    }

    public void setFrameBackgroundImage(String imagePath, float opacity) {
        frameBackgroundPanel.setBackgroundImage(imagePath, opacity);
    }

    public void setFrameBackgroundOpacity(float opacity) {
        frameBackgroundPanel.setOpacity(opacity);
    }

    public void clearFrameBackgroundImage() {
        frameBackgroundPanel.clearImage();
    }
}
