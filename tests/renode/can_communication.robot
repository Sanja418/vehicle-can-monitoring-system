*** Test Cases ***
Should Receive All Telemetry
    Execute Command    include @${CURDIR}/../../firmware/can_demo/two_nodes.resc
    Create Terminal Tester    sysbus.uart4    machine=Receiver    timeout=2    defaultPauseEmulation=true

    Wait For Line On Uart    Received RPM: 1500
    Wait For Line On Uart    Received temperature: 90 C
    Wait For Line On Uart    Received speed: 60 km/h

Should Detect Interruption And Recover
    Execute Command    include @${CURDIR}/../../firmware/can_demo/two_nodes.resc
    Create Terminal Tester    sysbus.uart4    machine=Receiver    timeout=3    defaultPauseEmulation=true


    Wait For Line On Uart    Received RPM: 1500
    Wait For Line On Uart    Received temperature: 90 C
    Wait For Line On Uart    Received speed: 60 km/h


    Execute Command    mach set "Sender"
    Execute Command    cpu IsHalted true


    Wait For Line On Uart    Dashboard: RPM data stale
    Wait For Line On Uart    Dashboard: temperature data stale
    Wait For Line On Uart    Dashboard: speed data stale


    Execute Command    cpu IsHalted false


    Wait For Line On Uart    Received RPM: 1500
    Wait For Line On Uart    Received temperature: 90 C
    Wait For Line On Uart    Received speed: 60 km/h

Should Warn Only At High Temperature
    Execute Command    include @${CURDIR}/../../firmware/can_demo/two_nodes.resc
    Create Terminal Tester    sysbus.uart4    machine=Receiver    timeout=2    defaultPauseEmulation=true

    Wait For Line On Uart    Received temperature: 90 C

    Execute Command    mach set "Sender"
    Execute Command    sysbus WriteWord `sysbus GetSymbolAddress "simulatedTemperature"` 109

    Wait For Line On Uart    Received temperature: 109 C
    Should Not Be On Uart    WARNING: engine temperature too high    timeout=0.6

    Execute Command    sysbus WriteWord `sysbus GetSymbolAddress "simulatedTemperature"` 110

    Wait For Line On Uart    Received temperature: 110 C
    Wait For Line On Uart    WARNING: engine temperature too high

    Execute Command    sysbus WriteWord `sysbus GetSymbolAddress "simulatedTemperature"` 90

    Wait For Line On Uart    Received temperature: 90 C
    Should Not Be On Uart    WARNING: engine temperature too high    timeout=0.6
Should Track RPM Freshness Independently
    Execute Command    include @${CURDIR}/../../firmware/can_demo/two_nodes.resc
    Create Terminal Tester    sysbus.uart4    machine=Receiver    timeout=3    defaultPauseEmulation=true

    Wait For Line On Uart    Received RPM: 1500
    Wait For Line On Uart    Received temperature: 90 C
    Wait For Line On Uart    Received speed: 60 km/h

    Execute Command    mach set "Sender"
    Execute Command    sysbus WriteByte `sysbus GetSymbolAddress "rpmTransmissionEnabled"` 0

    Wait For Line On Uart    Dashboard: RPM data stale
    Wait For Line On Uart    Received temperature: 90 C
    Wait For Line On Uart    Received speed: 60 km/h

    Should Not Be On Uart    Dashboard: temperature data stale    timeout=0.6
    Should Not Be On Uart    Dashboard: speed data stale    timeout=0.6

    Execute Command    sysbus WriteByte `sysbus GetSymbolAddress "rpmTransmissionEnabled"` 1

    Wait For Line On Uart    Received RPM: 1500
    Wait For Line On Uart    Received temperature: 90 C
    Wait For Line On Uart    Received speed: 60 km/h
Should Reject RPM With Wrong Length
    Execute Command    include @${CURDIR}/../../firmware/can_demo/two_nodes.resc
    Create Terminal Tester    sysbus.uart4    machine=Receiver    timeout=3    defaultPauseEmulation=true

    Wait For Line On Uart    Received RPM: 1500
    Wait For Line On Uart    Received temperature: 90 C
    Wait For Line On Uart    Received speed: 60 km/h

    Execute Command    mach set "Sender"
    Execute Command    sysbus WriteByte `sysbus GetSymbolAddress "simulatedRpmLength"` 1

    Wait For Line On Uart    Dashboard: RPM data stale
    Should Not Be On Uart    Received RPM:    timeout=0.6

    Execute Command    sysbus WriteByte `sysbus GetSymbolAddress "simulatedRpmLength"` 2

    Wait For Line On Uart    Received RPM: 1500
