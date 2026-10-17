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
