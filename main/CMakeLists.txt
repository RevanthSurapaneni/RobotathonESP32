set(srcs
        "main.c"
        "arduino_main.cpp"
        "color.cpp"
        "motor.cpp"
        "line.cpp"
)

set(requires "bluepad32" "bluepad32_arduino" "arduino" "btstack" "callbacks")

idf_component_register(SRCS "motor.cpp" "color.cpp" "line.cpp" "${srcs}"
                    INCLUDE_DIRS "."
                    REQUIRES "${requires}")
