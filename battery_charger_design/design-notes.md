# Design Notes for Battery Management PSU

## Battery Management IC, BQ2407x

UVLO is 3.3 volts - this is the Vin, not for Vbat. The V

Setting the input current limit of the battery charger: I_IN-MAX = K_ILIM / R_ILIM

### Vout

System Supply Output. OUT provides a regulated output when the input is below the OVP threshold and above the regulation
voltage. When the input is out of the operation range, OUT is connected to V BAT except when SYSOFF is high. Connect OUT
to the system load. Bypass OUT to VSS with a 4.7-μF to 47-μF ceramic capacitor.

In standby mode, Q1 is OFF and Q2 is ON so OUT is connected to the battery input. (If
SYSOFF is high, FET Q2 is off). During this mode, the V OUT(SC2) circuitry is active and monitors for overload
conditions on OUT.

With a source connected, the dynamic power-path management (DPPM) circuitry of the bq2407x monitors the
input current continuously. For the bq24076/78, OUT is regulated to 210 mV above the voltage at BAT. When the
BAT voltage falls below 3.2 V, OUT is clamped to 3.41 V

9.3.4.2 Input Source Not Connected
When no source is connected to the IN input, OUT is powered strictly from the battery. During this mode the
current into OUT is not regulated, similar to Battery Supplement Mode, however the short circuit circuitry is
active. If the OUT voltage falls below the BAT voltage by 250 mV for longer than tDGL(SC2), OUT is turned off. The
short circuit recovery timer then starts counting. After tREC(SC2), OUT turns on and attempts to restart. If the short
circuit remains, OUT is turned off and the counter restarts. This ON/OFF cycle continues until the overload
condition is removed.

9.3.1 Undervoltage Lockout (UVLO)
The bq2407x family remains in power down mode when the input voltage at the IN pin is below the undervoltage
threshold (UVLO).
During the power down mode the host commands at the control inputs (CE, EN1 and EN2) are ignored. The Q1
FET connected between IN and OUT pins is off, and the status outputs CHG and PGOOD are high impedance.
The Q2 FET that connects BAT to OUT is ON. (If SYSOFF is high, Q2 is off). During power down mode, the
VOUT(SC2) circuitry is active and monitors for overload conditions on OUT.

### Charge Enable

Connect CE to a high logic level to suspend charging. When CE is high, OUT is active and
battery supplement mode is still available. Connect CE to a low logic level to enable the battery charger

### Switch on and off, SYSOFF

System Enable Input. Connect SYSOFF high to turn off the FET connecting the battery to the system output. When an
adapter is connected, charging is also disabled. Connect SYSOFF low for normal operation. SYSOFF is internally pulled up
to V BAT through a large resistor (approximately 5 MΩ). Do not leave SYSOFF unconnected to ensure proper operation.
