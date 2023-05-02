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

def parse_csv(file_path):
    pins = []
    current_group = ""

    with open(file_path, "r") as csvfile:
        csvreader = csv.reader(csvfile)

        for i, row in enumerate(csvreader):
            if i >= 335:  # Ignore lines after the 340th row
                break

            if len(row) < 2:  # Ignore invalid lines
                continue

            ball_number, pin_name = row[:2]

            if pin_name == "Pin Name" or pin_name == "Function" or pin_name == "Type":
                continue  # Ignore header lines

            valid_ball_number = re.search(r"^[A-Z]+\d+$", ball_number) or re.search(r'^"[A-Z\d]+', ball_number)
            valid_pin_name = re.search(r"^[A-Z]+\d*$", pin_name)

            if not valid_ball_number and not valid_pin_name:
                current_group = ball_number
                continue

            if valid_ball_number and valid_pin_name:
                ball_numbers = re.findall(r"[A-Z]+\d+", ball_number)
                ball_numbers = process_ball_numbers(ball_numbers)
                pin_name = process_pin_name(pin_name)
                pin = Pin(pin_name, ball_numbers, current_group)
                pins.append(pin)

    return pins

def write_stanza_file(pins, output_file_path):
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

def main():
    input_file = "allwinnerH3.csv"
    output_file = "allwinner-h3.stanza"

    pins = parse_csv(input_file)
    write_stanza_file(pins, output_file)

if __name__ == "__main__":
    main()
