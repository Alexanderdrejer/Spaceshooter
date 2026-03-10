cmake_minimum_required(VERSION 3.10)

project(SpaceShooter LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include path
include_directories(${CMAKE_SOURCE_DIR}/include)

# Create executable from source files
add_executable(space_shooter
    src/space_shooter.cpp
    src/I2Cdriver.cpp
    src/accelerometer.cpp
    src/buttons.cpp
    src/display.cpp
    src/game_controller.cpp
    src/enemy.cpp
    src/bullet.cpp
    src/ship.cpp
)

# If I2Cdriver needs external libraries (e.g., -lrt), link them here:
target_link_libraries(space_shooter PRIVATE SSD1306_OLED_RPI)

