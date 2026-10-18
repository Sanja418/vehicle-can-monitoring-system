#include "can_driver.hpp"
#include "monitoring_config.hpp"
#include "register_access.hpp"
#include "rpm_message.hpp"
#include "speed_message.hpp"
#include "temperature_message.hpp"
#include "timer.hpp"
#include "uart.hpp"
volatile unsigned int counter = 0;
#ifndef CAN_RECEIVER
alignas(4) volatile std::int16_t simulatedTemperature = 90;
alignas(4) volatile bool rpmTransmissionEnabled = true;
alignas(4) volatile std::uint8_t simulatedRpmLength = 2;
#endif

int main()
{
    initUart();
    initTimer();

    if (!initCan())
    {
        writeUart("CAN1 initialization failed!\r\n");

        while (true)
        {
        }
    }

#ifdef CAN_RECEIVER
    writeUart("CAN receiver ready!\r\n");

    bool hasRpm = false;
    bool rpmStaleReported = false;
    unsigned short lastRpm = 0;
    unsigned int lastRpmTime = 0;

    bool hasTemperature = false;
    bool temperatureStaleReported = false;
    std::int16_t lastTemperature = 0;
    unsigned int lastTemperatureTime = 0;

    bool hasSpeed = false;
    bool speedStaleReported = false;
    unsigned short lastSpeed = 0;
    unsigned int lastSpeedTime = 0;

    while (true)
    {
        CanFrame frame{};

        if (receiveCanFrame(frame))
        {
            unsigned int receivedAt = readMilliseconds();

            if (isRpmFrame(frame))
            {
                lastRpm = readRpm(frame);
                lastRpmTime = receivedAt;
                hasRpm = true;
                rpmStaleReported = false;

                writeUart("Received RPM: ");
                writeNumber(lastRpm);
                writeUart("\r\n");
            }
            else if (isTemperatureFrame(frame))
            {
                lastTemperature = readTemperature(frame);
                lastTemperatureTime = receivedAt;
                hasTemperature = true;
                temperatureStaleReported = false;

                writeUart("Received temperature: ");

                int magnitude = lastTemperature;

                if (magnitude < 0)
                {
                    writeUart("-");
                    magnitude = -magnitude;
                }

                writeNumber(static_cast<unsigned short>(magnitude));
                writeUart(" C\r\n");

                if (lastTemperature >= ENGINE_OVERHEAT_THRESHOLD_C)
                {
                    writeUart("WARNING: engine temperature too high\r\n");
                }
            }
            else if (isSpeedFrame(frame))
            {
                lastSpeed = readSpeed(frame);
                lastSpeedTime = receivedAt;
                hasSpeed = true;
                speedStaleReported = false;

                writeUart("Received speed: ");
                writeNumber(lastSpeed);
                writeUart(" km/h\r\n");
            }
        }

        unsigned int now = readMilliseconds();

        if (hasRpm && !rpmStaleReported && now - lastRpmTime >= 2000u)
        {
            writeUart("Dashboard: RPM data stale\r\n");
            rpmStaleReported = true;
        }

        if (hasTemperature && !temperatureStaleReported && now - lastTemperatureTime >= 2000u)
        {
            writeUart("Dashboard: temperature data stale\r\n");
            temperatureStaleReported = true;
        }

        if (hasSpeed && !speedStaleReported && now - lastSpeedTime >= 2000u)
        {
            writeUart("Dashboard: speed data stale\r\n");
            speedStaleReported = true;
        }
    }

#else
    writeUart("CAN periodic sender ready!\r\n");

    bool firstTransmission = true;
    unsigned int lastSendTime = 0;

    while (true)
    {
        unsigned int now = readMilliseconds();

        if (firstTransmission || now - lastSendTime >= 500u)
        {
            firstTransmission = false;
            lastSendTime = now;

            bool rpmRequested = true;

            if (rpmTransmissionEnabled)
            {
                CanFrame rpmFrame = createRpmFrame(1500);
                rpmFrame.length = simulatedRpmLength;

                rpmRequested = sendCanFrame(rpmFrame);
            }

            bool temperatureRequested = sendCanFrame(createTemperatureFrame(simulatedTemperature));

            bool speedRequested = sendCanFrame(createSpeedFrame(60));

            if (rpmRequested && temperatureRequested && speedRequested)
            {
                writeUart("Periodic transmission requested!\r\n");
            }
            else
            {
                writeUart("CAN transmission request failed!\r\n");
            }
        }

        counter = counter + 1;
    }
#endif
}
