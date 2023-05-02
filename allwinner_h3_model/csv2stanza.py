import csv
import re

class Pin:
    def __init__(self, pin_name, ball_numbers, group):
        self.pin_name = pin_name
        self.ball_numbers = ball_numbers
        self.group = group

    def __str__(self):
        return f"[ {self.pin_name} | {', '.join(self.ball_numbers)} | Left | {self.group}]"

def process_ball_numbers(ball_numbers):
    return [re.sub(r"(\d+)$", r"[\1]", bn) for bn in ball_numbers]

def process_pin_name(pin_name):
    return re.sub(r"(\d+)$", r"[\1]", pin_name)

def find_missing_pins(pin_data_list):
    # Extract all row names and column numbers from the pin data list
    row_names = set()
    col_numbers = set()
    populated_pins = set()

    for pin_data in pin_data_list:
        for ball_number in pin_data.ball_numbers:
            match = re.match(r"([A-Z]+)\[(\d+)\]", ball_number)
            if match:
                row_name, col_number = match.groups()
                print(ball_number, row_name, col_number)
                row_names.add(row_name)
                col_numbers.add(int(col_number))
                populated_pins.add((row_name, int(col_number)))

    # Sort row names and column numbers
    row_names = sorted(list(row_names), key=lambda x: (len(x), x))
    print(row_names)
    col_numbers = sorted(list(col_numbers))

    # Find missing pins
    missing_pins = []
    for row_name in row_names:
        for col_number in col_numbers:
            if (row_name, col_number) not in populated_pins:
                missing_pins.append(f"{row_name}[{col_number}]")

    return missing_pins

def parse_csv(file_path):
    pins = []
    current_group = ""

    with open(file_path, "r") as csvfile:
        csvreader = csv.reader(csvfile)

        for i, row in enumerate(csvreader):
            if i >= 335:  # Ignore lines after the 335th row
                break

            if len(row) < 1:  # Ignore invalid lines
                continue

            ball_number, pin_name = row[:2]

            if pin_name == "Pin Name" or pin_name == "Function" or pin_name == "Type":
                continue  # Ignore header lines

            valid_ball_number = re.search(r"^([A-Z]+\s*\d+\s*(?:,\s*[A-Z]+\s*\d+\s*)*)$", ball_number) or re.search(r'^"[A-Z\d]+', ball_number)
            valid_pin_name = re.search(r"^[A-Z]+([-_]*[A-Z]*\d*[A-Z]*)*$", pin_name)


            if (ball_number != "Pin Description" and not ball_number.startswith("H3") and
                    not valid_ball_number and not valid_pin_name and ball_number.strip() != ""):
                current_group = ball_number.replace(' ', "-")
                continue


            if valid_ball_number and valid_pin_name:
                ball_numbers = re.findall(r"[A-Z]+\d+", ball_number)
                ball_numbers = process_ball_numbers(ball_numbers)
                pin_name = process_pin_name(pin_name)
                pin = Pin(pin_name, ball_numbers, current_group)
                pins.append(pin)

    return pins

def write_stanza_file(pins, output_file_path):


    missing_pins = find_missing_pins(pins)
    print("Missing pins:", missing_pins)

    with open(output_file_path, "w") as f:
        f.write("#use-added-syntax(jitx)\n")
        f.write("defpackage allwinner-h3 :\n")
        f.write("  import core\n")
        f.write("  import collections\n")
        f.write("  import math\n")
        f.write("  import jitx\n")
        f.write("  import jitx/commands\n")
        f.write("  import ocdb/utils/box-symbol\n\n")
        f.write("public pcb-component component :\n")
        f.write('  manufacturer = "Allwinner"\n')
        f.write('  mpn = "H3"\n')
        f.write("  pin-properties :\n")

        for pin in pins:
            f.write(f"    {str(pin)}\n")
        f.write('  make-box-symbol()\n')




def main():
    input_file = "allwinnerH3.csv"
    output_file = "allwinner-h3.stanza"

    pins = parse_csv(input_file)
    write_stanza_file(pins, output_file)

if __name__ == "__main__":
    main()
