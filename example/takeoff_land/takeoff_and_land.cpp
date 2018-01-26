//
// Simple example to demonstrate how to use DroneCore.
//
// Author: Julian Oes <julian@oes.ch>

#include <chrono>
#include <cstdint>
#include <dronecore/action.h>
#include <dronecore/dronecore.h>
#include <dronecore/telemetry.h>
#include <iostream>
#include <thread>

using namespace dronecore;

#define ERROR_CONSOLE_TEXT "\033[31m" //Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" //Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m"  //Restore normal console colour

int main(int /*argc*/, char ** /*argv*/)
{
    DroneCore dc;

    bool discovered_device = false;

    DroneCore::ConnectionResult connection_result = dc.add_udp_connection();

    if (connection_result != DroneCore::ConnectionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Connection failed: "
                  << DroneCore::connection_result_str(connection_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    std::cout << "Waiting to discover device..." << std::endl;
    dc.register_on_discover([&discovered_device](uint64_t uuid) {
        std::cout << "Discovered device with UUID: " << uuid << std::endl;
        discovered_device = true;
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a device after around 2 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    if (!discovered_device) {
        std::cout << ERROR_CONSOLE_TEXT << "No device found, exiting." << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // We don't need to specify the UUID if it's only one device anyway.
    // If there were multiple, we could specify it with:
    // dc.device(uint64_t uuid);
    Device &device = dc.device();

    auto telemetry = std::make_shared<Telemetry>(&device);
    auto action = std::make_shared<Action>(&device);

    // We want to listen to the altitude of the drone at 1 Hz.
    const Telemetry::Result set_rate_result = telemetry->set_rate_position(1.0);
    if (set_rate_result != Telemetry::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Setting rate failed:" << Telemetry::result_str(
                      set_rate_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }


    // Set up callback to monitor altitude while the vehicle is in flight
    telemetry->position_async([](Telemetry::Position position) {
        std::cout << TELEMETRY_CONSOLE_TEXT // set to blue
                  << "Altitude: " << position.relative_altitude_m << " m"
                  << NORMAL_CONSOLE_TEXT // set to default color again
                  << std::endl;
    });


    // Check if vehicle is ready to arm
    if (telemetry->health_all_ok() != true) {
        std::cout << ERROR_CONSOLE_TEXT << "Vehicle not ready to arm" << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Arm vehicle
    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = action->arm();

    if (arm_result != Action::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Arming failed:" << Action::result_str(
                      arm_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Take off
    std::cout << "Taking off..." << std::endl;
    const Action::Result takeoff_result = action->takeoff();
    if (takeoff_result != Action::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Takeoff failed:" << Action::result_str(
                      takeoff_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Let it hover for a bit before landing again.
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Landing..." << std::endl;
    const Action::Result land_result = action->land();
    if (land_result != Action::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Land failed:" << Action::result_str(
                      land_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // We are relying on auto-disarming but let's keep watching the telemetry for a bit longer.
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Finished..." << std::endl;
    return 0;
}
