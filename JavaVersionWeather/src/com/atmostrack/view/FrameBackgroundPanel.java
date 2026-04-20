package com.atmostrack.view;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import javax.swing.*;

public class FrameBackgroundPanel extends JPanel {
    private BufferedImage backgroundImage;
    private float opacity = 0.1f;
    private String imagePath;

    public FrameBackgroundPanel() {
        setOpaque(true);
    }

    public void setBackgroundImage(String imagePath, float opacity) {
        this.imagePath = imagePath;
        this.opacity = Math.max(0.0f, Math.min(1.0f, opacity));
        loadImage();
    }

    public void setOpacity(float opacity) {
        this.opacity = Math.max(0.0f, Math.min(1.0f, opacity));
        repaint();
    }

    public float getOpacity() {
        return opacity;
    }

    private void loadImage() {
        try {
            if (imagePath != null && !imagePath.isEmpty()) {
                // Convert path to absolute resource path (must start with /)
                String resourcePath = imagePath.startsWith("/") ? imagePath : "/" + imagePath;
                
                // Try to load as classpath resource (works in JAR)
                java.net.URL imageUrl = getClass().getResource(resourcePath);
                if (imageUrl != null) {
                    backgroundImage = ImageIO.read(imageUrl);
                    System.out.println("Frame background image loaded successfully from JAR.");
                    repaint();
                } else {
                    // Fallback to file loading for development
                    File imgFile = new File(imagePath);
                    System.out.println("Attempting to load frame background: " + imgFile.getAbsolutePath());
                    if (imgFile.exists()) {
                        backgroundImage = ImageIO.read(imgFile);
                        System.out.println("Frame background image loaded successfully from file.");
                        repaint();
                    } else {
                        System.err.println("Frame background image not found: " + imgFile.getAbsolutePath());
                        backgroundImage = null;
                    }
                }
            } else {
                backgroundImage = null;
            }
        } catch (IOException e) {
            System.err.println("Error loading frame background image: " + imagePath);
            e.printStackTrace();
            backgroundImage = null;
        }
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);

        if (backgroundImage != null) {
            Graphics2D g2d = (Graphics2D) g;
            g2d.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
            g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, opacity));
            g2d.drawImage(backgroundImage, 0, 0, getWidth(), getHeight(), this);
            g2d.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 1.0f));
        }
    }

    public void clearImage() {
        backgroundImage = null;
        imagePath = null;
        repaint();
    }
}
