# Set c-ares variables
set(DRONECORESERVER_CARES_SOURCE_DIR "${DRONECORESERVER_EXTERNAL_SOURCE_DIR}/cares")
set(DRONECORESERVER_CARES_BINARY_DIR "${DRONECORESERVER_EXTERNAL_BINARY_DIR}/cares")
set(ENV{DRONECORESERVER_CARES_BINARY_DIR} ${DRONECORESERVER_CARES_BINARY_DIR})

# Actually build c-ares
include(cmake/helpers/build_target.cmake)
build_target(${DRONECORESERVER_CARES_SOURCE_DIR} ${DRONECORESERVER_CARES_BINARY_DIR})

# Update CMAKE_PREFIX_PATH accordingly
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}:${DRONECORESERVER_CARES_BINARY_DIR}")
set(ENV{CMAKE_PREFIX_PATH} ${CMAKE_PREFIX_PATH})
