package com.atmostrack;

import com.atmostrack.controller.AtmosTrackController;
import javax.swing.SwingUtilities;

public class Main {
    public static void main(String[] args) {
        SwingUtilities.invokeLater(AtmosTrackController::new);
    }
}
