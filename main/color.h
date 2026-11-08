#pragma once

void setupColorSensor();
void startColorTrack();

enum class ColorId {
    None,
    Red,
    Green,
    Blue,
    Yellow,
};

// Returns true only when the robot has reached the first color again.
bool colorTrackStep();