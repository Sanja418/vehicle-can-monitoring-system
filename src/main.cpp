#include <array>
#include <chrono>
#include <thread>

#include "dashboard_node.hpp"
#include "engine_node.hpp"

int main()
{
    EngineNode engine{0};
    DashboardNode dashboard;

    std::array<std::uint16_t, 5> rpmValues{800, 1500, 2500, 3200, 900};

    for (std::uint16_t rpm : rpmValues)
    {
        engine.setRpm(rpm);

        CanFrame frame = engine.createFrame();
        dashboard.receiveFrame(frame);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
