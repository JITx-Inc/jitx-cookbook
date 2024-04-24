# The JITX Cookbook

Here you'll find a number of recipes (example projects) that will show you what JITX can do while serving as a base for your next project.

### How to Use

1. Install VSCode and JITX extension [How to install JITX](https://docs.jitx.com/faq/installationinstructions.html)
2. Open to root cookbook folder in VSCode with `File -> Open Folder -> (Select jitx-cookbook folder)`
3. Open `main.stanza` in the subfolder of the project that you want to run and press "Ctrl+Enter"

You've now built the recipe! Checkout the tutorial and documentation to learn more about JITX: [JITX Docs](https://docs.jitx.com/)

### Recipes

Click any of the links below to access a detailed walkthrough of how they were designed in JITX.

| Recipe                                                                 | What It Can Do                                                                                                                               |
| -------------                                                          | -------------                                                                                                                                |
| [Battery Charger](./battery_charger_design/)                           | LiPo battery charger, LDO voltage regulator, USB-C connector, JST battery connector, resistors, capacitors, detailed checks, export to KiCad |
| [USB-C Cable Tester](./usb_c_cable_tester/)                            | Test points, programmatic parts placement, USB-C connectors, LEDs, coin cell battery, circuit introspection                                  |
| [BLE-mote Wireless IOT Sensor Board](./ble_mote_esp32_iot_board/)      | Microcontroller, pin assignment (supports/requires), parametric design, design optimization, copper pour, net-classes, stackup               |
| [Hacking Conference Badge](./hacking_conference_badge/)                | A software-defined electronics playground - learn JITX by designing your own conference badge                                                |
| [Example of a reusable regulator design (TPS62933DRLR)](./regulator-example/) | Regulator fed from USB-C PD capable of delivering high current |
| [Example of a reusable keyboard layout generator](./keyboard/) | Keyboard generator which generates layout which is quickly implementable  |
| [Ambiq processor along with a few peripherals](./ambiq_demo/) | System based on Ambiq Apollo 4 Blue processor with wireless charging MIPI-DSI, octal SPI, 6-axis IMU peripherals which demonstrate pin assignment and SI constraints|
### What is a recipe?

These recipes are fully functional projects built in JITX to help show you what JITX can do, teach you how to do what you need in JITX, and to serve as reference or template projects for your next design. Recipes can be thought of as JITX example projects, reference designs, recipes, project blueprints, and/or design templates.
