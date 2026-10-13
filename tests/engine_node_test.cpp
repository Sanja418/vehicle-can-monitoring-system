#include "engine_node.hpp"

#include <iostream>

#include "rpm_message.hpp"
#include "temperature_message.hpp"

int main()
{
    EngineNode engine{1500};

    CanFrame rpmFrame = engine.createFrame();

    if (!isRpmFrame(rpmFrame) || readRpm(rpmFrame) != 1500)
    {
        std::cerr << "Initial RPM is incorrect\n";
        return 1;
    }

    engine.setTemperature(-10);

    CanFrame temperatureFrame = engine.createTemperatureFrame();

    if (!isTemperatureFrame(temperatureFrame) || readTemperature(temperatureFrame) != -10 ||
        readRpm(engine.createFrame()) != 1500)
    {
        std::cerr << "Temperature update or independent RPM is incorrect\n";
        return 1;
    }

    engine.setRpm(3200);

    if (readRpm(engine.createFrame()) != 3200 ||
        readTemperature(engine.createTemperatureFrame()) != -10)
    {
        std::cerr << "RPM update changed the wrong data\n";
        return 1;
    }

    engine.setTemperature(90);

    if (readTemperature(engine.createTemperatureFrame()) != 90 ||
        readRpm(engine.createFrame()) != 3200)
    {
        std::cerr << "Temperature update changed the wrong data\n";
        return 1;
    }

    std::cout << "Engine tests passed\n";
    return 0;
}
