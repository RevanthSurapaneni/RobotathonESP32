set(srcs
        "main.c"
        "arduino_main.cpp"
        "controller_callbacks.cpp"
        "color.cpp"
        "motor.cpp"
        "line.cpp"
)

set(requires "bluepad32" "bluepad32_arduino" "arduino" "btstack")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "."
                    REQUIRES "${requires}")
