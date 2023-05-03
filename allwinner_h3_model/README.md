# Allwinner H# model
Parsing a (noisy) CSV auto-extracted from a PDF datasheet into a JITX component model for the Allwinner H3.

## Methods

Upload datasheet from https://linux-sunxi.org/File:Allwinner_H3_Datasheet_V1.2.pdf into Google Drive. Open with Google Docs, copy out pin assignment and pin mux table into Google Sheets. Download csv, get: allwinnerH3.csv

Generate a python file using GPT4. Initial prompt:

```
I would like you to write  a python file to parse a csv named allwinnerH3.csv, store data about pin names, ball numbers and groups in a class, and then and write out a file named allwinner-h3.stanza using that data.

The CSV has ball numbers in the first column, and pin names in the second column. Ball names have letters followed by a number. Example ball names are T17 AA20 P20. Add square brackets around numbers in these names. For example AA20 should be stored as AA[20]. Pin names are made of letters and sometimes numbers. Example pin names are SDQ0, PA15, LINEINL. Add square brackets around numbers in those names. For example SDQ0 should be stored as SDQ[0]

There can be more than one ball number per pin name. In this case those ball numbers are enclosed in quotes "".  

Each set of pin names has a group, which is defined by a preceding row that looks like "USB,,,,,,,,," here USB is a group name that should be saved in the class of every pin name in the following rows before the next group.

Many lines in this CSV are invalid, ignore them. Invalid lines do not have ball numbers and pin names matching the stated pattern. Only the first 332 lines of this csv could be valid.

The front matter of the file to write out is 
#use-added-syntax(jitx)
defpackage allwinner-h3 :
  import core
  import collections
  import math
  import jitx
  import jitx/commands
  import ocdb/utils/box-symbol

public pcb-component component :
  manufacturer = "Allwinner"
  mpn = "H3"
  pin-properties :
    [pin:Ref | pads:Int ... | side:Dir|  bank: Ref ]

Then write out the entries of all the pin names in the format:
  [ pin name | ball names | Left | group name]
```

Generates:  gpt_csv2stanza.py

Debug regexs using GPT, until only problematic rows remaining are:
['H10,J10,J11,J12,K1 0,K11,K12,L10,L11, L12,L13, L14', 'VDD-SYS', '-', 'P', '-', '-', '-', '', '', '']
['A21,AA1,G8,H12, H15, J13,J16, J9, K13, K14,K15, K16, K7,K8,K9,L15,L8,L9, M10,M11,M12, M13,M14,M15,M7, M8,M9,N10,N11, N12,N13,N14,N15, N7,N9,P10,P11,P12 ,.P13,P14,P15,R10, R11,R12,R13,R14, R9,T11', 'GND', '-', 'G \nide', '- ntia', 'l\n-', '-', '', '', '']

Edit those rows in csv. 
```
K1 0 => K10
```
```
P12,.P13, => P12,P13,
```

Now we need the leave out list for the package (looks random). Analyze the pin list. Prompt for GPT:
```
Now I need to analyze which pins from the ball numbers are missing. These pins are in a grid with rows as strings and columns as numbers. A[1] is the top left pin. The rows are not just alphabetical, you must determine the used strings by analyzing the populated pins. Print out a list of which strings are used as row names.

Write a python function to print out which pins are missing from the grid
```

## Creating the landpattern

https://www.aliexpress.com/i/3256804488327033.html?gatewayAdapt=4itemAdapt

