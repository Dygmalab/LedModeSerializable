add_library(LED_effects INTERFACE)

target_include_directories(LED_effects
        INTERFACE
        ./src
)

target_link_libraries(LED_effects
        INTERFACE
        LEDManagement
)