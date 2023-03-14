# USB-C Cable Tester PCB Cookbook Recipe

This tutorial guides you in creating a USB-C cable testing PCB in JITX from the ground up. With step-by-step instructions, even individuals with no prior experience in JITX can learn to define hardware in software and generate a PCB using code.

## Getting started
##### Prerequisites
To start, we'll need to setup JITX. We can follow this tutorial to get setup: https://docs-testing.jitx.com/faq/installationinstructions.html

Then, we need to create a new project. So let's run VSCode, select the JITX extension, and click "New Project".

Now we're ready to start designing the board.

> NOTE: The code below is not the entire design. Refer to the [Github repo](https://github.com/JITx-Inc/jitx-cookbook/tree/main/usb_c_cable_tester) for a full working design. Below, we describe the design flow and thought process behind designing this board.

##### Intended functionality
Let's start by considering what we want our design to be able to do. It needs to to test every connection in a USB-C cable, and provide feedback about what connections exist, and what connections don't exist, or are broken. To do that, we'll design a circuit board which:

- connects to both ends of a USB-C cable
- sends a signal into the pins of one connector of the cable and should be present on the pins of the other connector
- measures that signal using an LED indicator for every individual USB-C pin. If the LED doesn't light up, it means that the cable has a bad connection

##### Implementation plan

To accomplish the above, we need a few components:

- 2x USB-C connectors, we'll use the [
1054500101 USB-C connector](https://www.lcsc.com/product-detail/span-style-background-color-ff0-USB-span-span-style-background-color-ff0-Connectors-span_MOLEX-1054500101_C134092.html)
- a coin cell battery clip (or similiar battery holder), we'll use the [CR2032 SMD coin cell clip](https://datasheet.lcsc.com/lcsc/1811061923_Q-J-CR2032-BS-6-1_C70377.pdf)
- a number of indicator LEDs

Considering this is a simple design, we'll use a 2 layer board, with all components on the same side for ease of PCBA.

##### Resources

This board is fully open source, available here: [USB-C Cable Tester Recipe](https://github.com/JITx-Inc/jitx-cookbook/tree/main/usb_c_cable_tester)

Clone [this repo](https://github.com/JITx-Inc/jitx-cookbook) and open the subfolder `usb_c_cable_tester` in VSCode to follow along.

## Components

We've identified some specific components that we want to use, namely the [
1054500101 USB-C Connector](https://www.lcsc.com/product-detail/span-style-background-color-ff0-USB-span-span-style-background-color-ff0-Connectors-span_MOLEX-1054500101_C134092.html) and the [CR2032-BS-6-1 Coin Cell Battery Clip](https://jlcpcb.com/partdetail/QJ-CR2032_BS_61/C70377), alongside some other components.

JITX has built-in support for many basic components like capacitors, resistors, and LED's, so we'll be able to source those as we need them while designing. For the specific ICs we're using, we'll need to either find an existing component definition, or make that ourselves.

##### Battery Clip

To power the design, we'll need an on-board battery. Since this is a low-power, low-cost, rarely used board, we want to use a battery than will last a long time. A 3V lithium coin cell is a good choice here. Fortunately, [OCDB](https://github.com/JITx-Inc/open-components-database) already contains a definition of a coin cell battery clip, the [CR2032-BS-6-1](https://jlcpcb.com/partdetail/QJ-CR2032_BS_61/C70377). We found this by opening OCDB in VS Code, pressing `Ctrl + Shift + F`, and then searching for "battery".

Let's go ahead an instantiate that battery clip holder inside our top level `pcb-module`. We can create it, connect it, and place it in a few lines of code:
```
  ; create a coin cell battery
  val battery-capacity = 90.0 ;set the capacity of battery, in mAh
  inst battery : ocdb/components/q-n-j/CR2032-BS-6-1/component(battery-capacity); create a battery with the specified capacity
  net (POWER battery.power.vdd); connect positive of battery to the POWER net
  net (GND battery.power.gnd); connect negative of battery to the GND net
  val battery-y = board-height / 3.5; programmatically calculate an x and y position of the battery based on the board-shape
  val battery-x = board-width / -7.0; 
  place(battery) at loc(battery-x, battery-y, 90.0) on Top; place the battery on the top of the board at the calculated position
```

##### LEDs

We also want to build a circuit generator that picks specific colors of LEDs, so we'll use the Everlight Electronics family of 0603 Chip LEDs, which all have similar curves for luminous intensity versus current. For example, here's the [red Everlight Electronics 0603 LED](https://datasheet.lcsc.com/lcsc/1810010118_Everlight-Elec-19-21-R6C-FP1Q2L-3T_C93128.pdf).

For now, we'll simply identify that we want to use a few different colors of LEDs in the same family, and create a component generator that selects the correct LED based on our color parameter:
```
public pcb-component led-component (color:LED-COLORS) :
  name = "LED-maker-ROYGBIV"
  description = "Everlight Elec 0603 various color SMD LEDs"
  manufacturer = "Everlight Elec"
  reference-prefix = "LED"
  property(self.package) = "0603"

  ; properties shared by all LEDs
  property(self.max-current) = 25.0e-3
  property(self.rated-temperature) = 85.0

  ; properties different among LEDs
  switch(color) :
    RED:
      mpn = "19-21/R6C-FP1Q2L/3T"
      property(self.LCSC) = "C93128"
      property(self.datasheet) = "https://datasheet.lcsc.com/lcsc/1810010118_Everlight-Elec-19-21-R6C-FP1Q2L-3T_C93128.pdf"
      property(self.li) = min-max(45.0e-3, 112.0e-3)
      property(self.vf) = min-max(1.7, 2.3)
    ORANGE:
      mpn = "19-217UYOC/S3077/TR8"
      property(self.LCSC) = "C264474"
      property(self.datasheet) = "https://datasheet.LCSC.com/lcsc/1912111437_Everlight-Elec-19-217UYOC-S3077-TR8_C264474.pdf"
      property(self.li) = min-max(28.0e-3, 72.0e-3)
      property(self.vf) = min-max(1.7, 2.4)
    YELLOW:
      mpn = "19-21/Y2C-CN1P2B/3T"
      property(self.LCSC) = "C131265"
      property(self.datasheet) = "https://datasheet.LCSC.com/lcsc/1811141628_Everlight-Elec-19-21-Y2C-CN1P2B-3T_C131265.pdf"
      property(self.li) = min-max(28.5e-3, 72.0e-3)
      property(self.vf) = min-max(1.75, 2.35)
    GREEN:
      mpn = "19-217/GHC-YN1P2B18X/3T"
      property(self.LCSC) = "C2980184"
      property(self.datasheet) = "https://datasheet.LCSC.com/lcsc/2202101201_Everlight-Elec-19-21-GHC-YR1S2-3T_C2973113.pdf"
      property(self.li) = min-max(28.5e-3, 72.0e-3)
      property(self.vf) = min-max(2.7, 3.7)
    BLUE:
      mpn = "19-213/BHC-AP2Q2E/3T"
      property(self.LCSC) = "C474027"
      property(self.datasheet) = "https://datasheet.LCSC.com/lcsc/1912251011_Everlight-Elec-19-213-BHC-AP2Q2E-3T_C474027.pdf"
      property(self.li) = min-max(57.0e-3, 112.0e-3)
      property(self.vf) = min-max(2.75, 3.65)

  pin-properties :
    [pin:Ref    | pads:Int ... | side:Dir ]
    [a          | 1            | Up       ]
    [c          | 2            | Down     ]

  assign-symbol(diode-sym(DiodeLED))
  assign-landpattern(lp-led-maker)

```

Now all we have to do is change the color argument, and we will instantiate the correct LED - no need to keep loading datasheets and searching suppliers, we just do that once, turn it into code, and reuse the same component with no extra effort.
##### USB-C Connector

We also need two USB-C connectors that the two ends of our female-to-female USB-C cable will plug into. One thing to ensure is that the USB-C connector that we select doesn't internally bridge connections, as we want to be able to test each connection in the USB-C cable individually.

We originally selected the [U263-241N-4BQC11-1](https://www.lcsc.com/product-detail/USB-Connectors_XKB-Connectivity-U263-241N-4BQC11-1_C381139.html) connector,  but it was out of stock by the time we went to order. JITX made it easy to swap that out for a different in-stock connector, the Molex [1054500101](https://www.lcsc.com/product-detail/span-style-background-color-ff0-USB-span-span-style-background-color-ff0-Connectors-span_MOLEX-1054500101_C134092.html) USB-C Connector.

Now that the USB-C connector is in our project, we can instantiate it and programmatically place it at the board's edge:
```
  ; create USB connectors
  val usb-x-shift = 4.0
  val usb-y-shift = -7.0
  ; OLD CONNECTOR -> public inst in-usb : ocdb/components/xkb/U263-241N-4BQC11-1/component
  public inst in-usb : components/USB-C-1054500101/component
  place(in-usb) at loc((-1.0 * width(board-shape) / 2.0) + usb-x-shift, usb-y-shift, -90.0) on Top
  ; OLD CONNECTOR -> public inst out-usb : ocdb/components/xkb/U263-241N-4BQC11-1/component
  public inst out-usb : components/USB-C-1054500101/component
  place(out-usb) at loc((width(board-shape) / 2.0) - usb-x-shift, usb-y-shift, 90.0) on Top
```
![](images/img1.png)

We downloaded this [USB-C connector from SnapEDA](https://www.snapeda.com/parts/1054500101/Molex/view-part) and imported it directly into our project.

## Design 
##### Circuit Generator - LED indicator modules
We want all of our LEDs to have the same brightness, but each LED will need a different driving current to achieve an identical brightness. In the past, we might have worked this out on paper or in a spreadsheet. 

We can incorporate these calculations directly into the code and reuse that everytime we instantiate a new LED. So, let's make a circut generator which creates an LED and a properly selected ballast resistor. We'll choose the ballast resistor given a color and a required brightness.

First, we'll create a module that accepts a color and an input voltage: 

```
; the LED and ballast resistor module
public pcb-module module (color:LED-COLORS, voltage-in:Toleranced) :
```
Then, we create an LED of the given color:
```
  inst led : components/LED-maker-ROYGB/led-component(color)
```
We then create a ballast resistor of the proper resistance for a given brightness (here, 50 millicandellas):

```
  ; create ballast resistor
  val brightness = 50.0e-3 ; candella
  val ballast-resistance = compute-ballast-resistor(led, voltage-in, brightness);
  inst ballast-res : database-part(["category" => "resistor", "resistance" => ballast-resistance, "case" => ["0402"], "_exist" => ["vendor_part_numbers.lcsc"]])
```

The automation capability here comes from two function calls. First, we wrote a custom function that maps from an LED parameters, input voltage, and brightness to a ballast resistor resistance:

```
defn compute-ballast-resistor (led:JITXObject, voltage:Toleranced, brightness:Double) -> Double : ; led is an led-component, brightness is in candellas
  ; nominal luminous intensity of this LED
  val nli = center-value(property(led.li))
  val vf = center-value(property(led.vf))

  ; map luminous intensity to current
  ; Datasheet gives data in tables, so use piecewise linear model to find value
  val my-pwl = PWL([
      [0.02 * nli, 1.0e-3] 
      [0.06 * nli, 2.0e-3]
      [0.5 * nli, 10.0e-3]
      [1.0 * nli, 20.0e-3]
    ])
  val computed-current = my-pwl[brightness]

  ; get the required resistance, given the input voltage and the desired current
  val computed-resistance = (voltage - typ(vf)) / typ(computed-current)
  ; turn the resistor into the closest standard value resistor by passing in the resistance that we computed
  closest-std-val(typ-value(computed-resistance), 5.0)
```

Then we used the built-in JITX parts database to parametrically search and find a resistor that matches our resistance, package size, and supplier needs:
```
database-part(["category" => "resistor", "resistance" => ballast-resistance, "case" => ["0402"], "_exist" => ["vendor_part_numbers.lcsc"]])
```

The above `database-part` query contains _millions of parts_ that you can parametrically define using category, value, manufacturer, MPN, etc. This system can greatly increase your productivity by automatically finding parts and importing their information (MPN, manufacturer, VPN, etc.). We can filter based on live data about components stock from various sources, so our design only includes components that are guaranteed to be available.

This generates a `pcb-module` with LED and properly tuned ballast resistor, already connected and ready to drop into our design. Now in the future, whenever we need a brightness controlled, colored indicator LED, we can reuse this module.

![](images/img2.png)

##### Board Setup
We've now created many components and circuit generators that will serve as the foundation of our design. In the top level module, let's now setup our board, connect things together, and add a bit of organization so our design is ready for export.

First, we create a `pcb-module`, define our input/output pins, add version label text, and place a logo on the silkscreen:
```
pcb-module usb-c-cable-tester-module :
  ; define pins to expose from our module
  pin gnd
  pin power

  ; define nets
  net GND (gnd)
  net POWER (power)

  ; draw a version number and date on the board silkscreen
  val mydate:String = get-time-string("%Y-%m-%d")
  inst version-label  : ocdb/artwork/board-text/version-silkscreen(to-string("USB-C Cable Tester | Version 0.1 %_" % [get-time-string("%Y-%m-%d")])) ; make a version label text on silkscreen
  place(version-label) at loc(0.0, height(board-shape) / 2.0 - 1.0) on Bottom ; place the version label text at specified location

  ; setup logo
  inst logo : ocdb/artwork/jitx-logo/logo(10.0)
  place(logo) at loc(10.0, -5.0, 0.0) on Bottom
```

##### Naming test points based on connected net - circuit design introspection

Often, we want to have more control of our components and design. The goal is to automate engineering best practices like having testpoints with informative names.

```
; place a testpoint
defn my-testpoint-strap (tp:JITXObject, tp-name:String, diameter:Double) -> JITXObject:
  inside pcb-module :
    ...
```

First, we created the testpad with `public inst tp-pad : gen-testpad(diameter)`. We then connect it to the net or pin provided using `net (tp-pad.p tp)`. Finally, we set the value of the testpad and place its label in a convenient location:
```
    ; name the test point
    value-label(tp-pad) = Text(tp-name, 1.0, W, loc(1.2, 0.0))
    ; val my-label = Text(tp-name, 1.0, W, loc(1.5, 0.0))
    inst my-label : ocdb/artwork/board-text/text(tp-name, 1.0, 0.0)
    place(my-label) at loc(1.2, -0.4, 0.0) on Top (relative-to tp-pad)
    tp-pad
```
We now have a testpoint creation function, which we can reuse anywhere we need a testpoint, in this project or in others.

##### Connecting Everything
We already setup the board, instantiated and connected the battery clip and USB-C connector, and wrote circuit generators, as discussed above.

Now, let's connect everything together into a single design.

Since we want to test every individual connection in the USB-C cable, let's create a net for every pin in our output-usb connector:

```
  ; make nets for all of the pins on the usb C that we want to test
  net VBUS (out-usb.usb-c-pinout.VBUS) ; for each pin of the USB, we make a net
  net GND-usb (out-usb.usb-c-pinout.GND)
  net SHIELD (out-usb.usb-c-pinout.shield)
  net SBU2 (out-usb.usb-c-pinout.SBU2)
  net SBU1 (out-usb.usb-c-pinout.SBU1)
  net DN2 (out-usb.usb-c-pinout.DN2)
  net DP2 (out-usb.usb-c-pinout.DP2)
  net DN1 (out-usb.usb-c-pinout.DN1)
  net DP1 (out-usb.usb-c-pinout.DP1)
  net CC2 (out-usb.usb-c-pinout.CC2)
  net CC1 (out-usb.usb-c-pinout.CC1)
  net SSTXP1 (out-usb.usb-c-pinout.SSTXP1)
  net SSTXN1 (out-usb.usb-c-pinout.SSTXN1)
  net SSRXP1 (out-usb.usb-c-pinout.SSRXP1)
  net SSRXN1 (out-usb.usb-c-pinout.SSRXN1)
  net SSTXP2 (out-usb.usb-c-pinout.SSTXP2)
  net SSTXN2 (out-usb.usb-c-pinout.SSTXN2)
  net SSRXP2 (out-usb.usb-c-pinout.SSRXP2)
  net SSRXN2 (out-usb.usb-c-pinout.SSRXN2)
```

However, that's a tad cumbersome. We can do the exact same thing in a loop:
```
  for p in pins(out-usb) do :
    val pin-ref = ref(p)
    val pin-name = tail(pin-ref, ref-length(pin-ref) - 1)
    make-net(to-symbol(pin-name), [p])
```

That's much cleaner and more extendable.

We then order the pins of the USB based on their physical location, map those pins to the nets we just created, and generate a test LED module for every pin:
```
  ; get all pins of the USB, and order them based on how they physically appear
  val ordered-pins = get-pins-physically-ordered(out-usb)

  ; convert from ordered list of pins to ordered list of nets
  val ordered-nets = map(get-named-net{self, _}, ordered-pins)

  ; generate an array of test LEDs for each net we just made
  generate-test-leds-array(ordered-nets, GND, property(battery.power.vdd.voltage))
```

We connect all the pins of the left (power in) USB-C connector to the POWER net:
```
  ; connect all pins of in-usb to power
  net (POWER pins(in-usb))
```
Then, we specify that we want our power nets to use specific power symbols in the schematic to make it easier to visualize:

```
  ; set symbols of some nets
  symbol(GND) = ocdb/utils/symbols/ground-sym
  symbol(POWER) = ocdb/utils/symbols/supply-sym
```

Finally, we programmatically place mounting holes on the board, sized for M2 screws:
```
  ; add mounting holes to corners of the board
  add-mounting-holes(board-shape, "M2", [])
```

## Checks

JITX comes with an array of powerful checks we can run on our design. After adding just a few lines of code to describe our components, JITX can run checks including power checks, digital GPIO checks, rated-temperature, operating point checks, etc. For this simple design, we'll focus mostly on PowerPin checks, which ensure that either the components or modules which we define as requiring a specific input voltage are connected to voltage supplies providing the correct voltage.

Consider the LED module that was generated by our circuit generator (described above). It's created expecting a specific input voltage on its power pin. In order to represent that in a way that JITX checks can understand, we define the input pin of the LED module as a `PowerPin`:
```
; the LED and ballast resistor module
public pcb-module module (color:LED-COLORS, voltage-in:Toleranced) :
  pin in
  
  ...
  
  ; specify our input voltage range
  property(in.power-pin) = PowerPin(voltage-in);
```
Notice that the above module uses a `Toleranced` value, which allows for the voltage-in value to accept a range of possible input values via a `Toleranced` type.

For many designs, this is all we need to do in order to run power checks. However, since our USB-C tester board has a "virtual component" (the actual USB cable), we'll need to model this in a test rig that so voltages propogate across that cable. Let's create a cable module and hook it up so that our voltage can propogate and checks can run.

##### Bundles and test rig

JITX supports bundles, which make it simple to handle connectors, buses, or anything where there are a number of connections that are grouped (bundled) together.

Above, we mentioned the need to create a virtual USB cable component that allows us to connect the USB-C connector on one side of the PCB to the USB-C connector on the other side. This can be done if our USB-C connector component implements a USB-C `bundle`. The virtual cable will then simply connect two bundles together to simulate a cable.

To do so, in our definition of the USB-C connector, we'll define the pins to breakout a USB-C bundle:
```
public pcb-component component :
  port usb-c-pinout : usb-c-full-bundle
  pin-properties :
    [pin:Ref | pads:Ref ...            | side:Dir | electrical-type:String]
    [usb-c-pinout.SSRXN2 | A10 | Left | "Bidirectional"]
    [usb-c-pinout.SSRXP2 | A11 | Left | "Bidirectional"]
    [usb-c-pinout.GND    | A12 A1 B12 B1 | Left | "PowerIn"]
    [usb-c-pinout.SSTXP1 | A2 | Left | "Bidirectional"]
    [usb-c-pinout.SSTXN1 | A3 | Left | "Bidirectional"]
    [usb-c-pinout.CC1    | A5 | Left | "Bidirectional"]
    [usb-c-pinout.DP1    | A6 | Left | "Bidirectional"]
    [usb-c-pinout.DN1    | A7 | Left | "Bidirectional"]
    [usb-c-pinout.SBU1   | A8 | Left | "Bidirectional"]
    [usb-c-pinout.VBUS   | A9 A4 B9 B4 | Left | "PowerIn"]
    [usb-c-pinout.SSRXN1 | B10 | Right | "Bidirectional"]
    [usb-c-pinout.SSRXP1 | B11 | Right | "Bidirectional"]
    [usb-c-pinout.SSTXP2 | B2 | Right | "Bidirectional"]
    [usb-c-pinout.SSTXN2 | B3 | Right | "Bidirectional"]
    [usb-c-pinout.CC2    | B5 | Right | "Bidirectional"]
    [usb-c-pinout.DP2    | B6 | Right | "Bidirectional"]
    [usb-c-pinout.DN2    | B7 | Right | "Bidirectional"]
    [usb-c-pinout.SBU2   | B8 | Right | "Bidirectional"]
    [usb-c-pinout.shield | S4 S3 S2 S1 | Left | "Passive"]
```

Since that component now implements the `usb-c-full-bundle`, we can create a usb-cable module in `main.stanza` that connects those two bundles together:
```
; a component that models a usb cable
public pcb-module usb-c-cable-module:
  port in-usb-bundle : ocdb/utils/bundles/usb-c-full-bundle
  port out-usb-bundle : ocdb/utils/bundles/usb-c-full-bundle
  net (in-usb-bundle out-usb-bundle)
```

Notice that connecting a multi-pin bundle together is as simple as netting the bundles together.

And finally, let's create a top level test rig `pcb-module` that includes that USB-C cable module and plugs in to both connectors of our tester board - thus closing the circuit and allowing our power propogation checks to run!
```
; a rig that plugs a USB cable into both connectors on the main usb cable testing board - this is created to allow us to run checks
public pcb-module test-rig :
  inst design-under-test : usb-c-cable-tester-module
  inst usb-c-cable : usb-c-cable-module
  net IN (design-under-test.in-usb.usb-c-pinout usb-c-cable.in-usb-bundle)
  net OUT (design-under-test.out-usb.usb-c-pinout usb-c-cable.out-usb-bundle)
  check-design(self)
```

Now if we change our testing to call to run on the `test-rig`, we run checks on all of the LED modules, with power propogated through the cable:
```
; Run checks
run-check-on-design(test-rig)
```
Now that we've specified this, we can use the built-in JITX pin checks to get power checks on all of our LED modules:

```
  ; run a check on the module power pins
  ocdb/utils/pin-checks/all/check-pins(self)
```

And we get many successful power pin checks:

![](images/img3.png)

Note that there are 3 incomplete checks. It's good to examine which checks ran and which didn't, in case any of them are easily fixable. Taking a look, we can see that these checks are incomplete because they lack the rated-temperature property (on the logo and USB-C connectors). This isn't a big problem, so we can safely ignore them:

![](images/img4.png)

## Layout + Order
##### Export to KiCad

Now that we're happy with the layout, we can export the design straight into KiCad.

First, let's go into `helpers.stanza`, set KiCad as our CAD tool, and add a mapping so that the custom LCSC field get's exported with our components:
```
val export-field-mapping = [
  "LCSC" => "LCSC"
  "lcsc" => "LCSC"
  "vendor_part_numbers.lcsc" => "LCSC"
]
defn export-to-cad () :
  set-paper(ANSI-A4)
  set-export-backend(`kicad)
  export-cad(export-field-mapping)
```
Then we build the project with `Ctrl + Enter`, and export with `export-design()` in our REPL (terminal), and a KiCad project is generated in the "CAD" directory.

##### Layout

Let's open up KiCad, open the project, and open the schematic + board viewers. The first thing to do is to click the "Update PCB with changes made to schematic" button in the board view to ensure everything has synced.

Then, layout your board exactly as you would normally in Kicad.

##### Export from Kicad, order from JLCPCB

Install this plugin in Kicad to manage your components from LCSC and generate your Gerbers, BOM, and CPL: https://github.com/Bouni/kicad-jlcpcb-tools

Upload your design to JLCPCB and order it:
![](images/img5.png)
![](images/img6.png)
![](images/img7.png)

## Conclusion

We've now successfully designed a PCB using JITX. You can use this as a reference project, copying this workflow to design your own systems. To learn more, check out the other recipes in [the JITX Cookbook Recipes repo](https://github.com/JITx-Inc/jitx-cookbook) and read through the [tutorials](https://docs.jitx.com/tutorials/index.html) in the JITX docs.
