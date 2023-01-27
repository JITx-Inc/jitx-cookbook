# Design Notes for Battery Management PSU

## Battery Management IC, BQ2407x

UVLO is 3.3 volts - this is the Vin, not for Vbat. The V

## Setting current

Setting the input current limit of the battery charger: I_IN-MAX = K_ILIM / R_ILIM

10.2.2.2 Calculations
	10.2.2.2.1 Program the Fast Charge Current (ISET):
		RISET = KISET / ICHG
		KISET = 890 AΩ from the electrical characteristics table.
		RISET = 890 AΩ / 0.8 A = 1.1125 kΩ
		Select the closest standard value, which for this case is 1.13 kΩ. Connect this resistor between ISET (pin 16)
		and VSS.
	10.2.2.2.2 Program the Input Current Limit (ILIM)
		RlLIM = KILIM / II_MAX
		KILIM = 1550 AΩ from the electrical characteristics table.
		RISET = 1550 AΩ / 1.3 A = 1.192 kΩ
		Select the closest standard value, which for this case is 1.18 kΩ. Connect this resistor between ILIM (pin 12) and
		VSS.

### Current going into the battery

9.3.5.1 Charge Current Translator
When the charger is enabled, internal circuits generate a current proportional to the charge current at the ISET
input. The current out of ISET is 1/400 (±10%) of the charge current. This current, when applied to the external
charge current programming resistor, RISET, generates an analog voltage that can be monitored by an external
host to calculate the current sourced from BAT.

- VISET = ICHARGE / 400 × RISET

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


## Checks

#### Battery Connector + Switch

- GND needs to be on the left of the batteries cable when plugging it in - make sure these aren't reversed
- make sure that there is no possible short in either switch position
- switch MK12C02 meets specs - 12V 0.5 or 6V 1.0A (convert this to power and check for meets power specs)
 
#### LDO

- LDO EN needs to be pulled down when off, pulled high when on - make sure not NC
- LDO voltage input range
- LDO - make sure bypass caps are present

#### BQ2407 battery charger 

- bq2047 EP/thermal-pad must be connected to VSS
- check BQ2407 has appropriate bypass caps
	- Bypass OUT to VSS with a 4.7-μF to 47-μF ceramic capacitor
- check BQ2407 input voltage range
	- 
- check BQ2407 output voltage range
- check BQ2407 that ILIM, ITERM have resistance to GND in proper range
	- Connect a 1100-Ω to 8-kΩ resistor from ILIM to VSS 
	- Connect a 590-Ω to 8.9-kΩ resistor from ISET to VSS
- ensure proper resistance on CHG pin on BQ2407 -> Connect CHG to the desired logic voltage rail using a 1kΩ-100kΩ resistor, or use with an LED for visual indication.
- ensure proper resistance on PGOOD pin on BQ2407 -> Connect PGOOD to the desired logic voltage rail using a 1-kΩ to 100-kΩ resistor, or use with an LED for visual indication
- check BQ2407 BAT is bypassed to VSS with cap in proper range
	- Bypass BAT to VSS with a 4.7-μF to 47-μF ceramic capacitor
- check that EN1 and EN2 are following the proper 01 configuration (or, another configuration)
- check BQ2407 SYSOFF is not NC
- check BQ2407 SYSOFF is pulled high/low
- check TMR is either NC or connected to the proper voltage range
- ensure VSS is connected directly to GND
- ensure TS is connected to 10k resistor or 10k thermistor




















