import csv
import re


class PinData:
    def __init__(self, pin_name, ball_numbers, group):
        self.pin_name = pin_name
        self.ball_numbers = ball_numbers
        self.group = group

    def __str__(self):
        return f"[ {self.pin_name} | {', '.join(self.ball_numbers)} | Left | {self.group} ]"


def parse_csv(file_name):
    pin_data_list = []
    current_group = None

    with open(file_name, newline='') as csvfile:
        reader = csv.reader(csvfile)
        for index, row in enumerate(reader):
            if index > 340:
                break

            if row[0] and not row[1] and row[0] != "Pin Description" and not row[0].startswith('H3'):
                current_group = row[0]

            if not row or not row[0] or not row[1]:
                continue


            ball_number = re.match(r'^[A-Z]+\d+$', row[0])
            pin_name = re.match(r'^[A-Z]+\d*$', row[1])

            if ball_number and pin_name:
                ball_number = re.sub(r'(\d+)', r'[\1]', row[0])
                pin_name = re.sub(r'(\d+)', r'[\1]', row[1])
                ball_numbers = [ball_number]
                if '"' in row[0]:
                    ball_numbers = [re.sub(r'(\d+)', r'[\1]', bn) for bn in row[0].strip('"').split(',')]

                pin_data = PinData(pin_name, ball_numbers, current_group)
                pin_data_list.append(pin_data)

    return pin_data_list


def write_stanza_file(pin_data_list, output_file):
    with open(output_file, 'w') as f:
        f.write("#use-added-syntax(jitx)\n")
        f.write("defpackage allwinner-h3 :\n")
        f.write("  import core\n")
        f.write("  import collections\n")
        f.write("  import math\n")
        f.write("  import jitx\n")
        f.write("  import jitx/commands\n")
        f.write("  import ocdb/utils/box-symbol\n\n")
        f.write('public pcb-component component :\n')
        f.write('  manufacturer = "Allwinner"\n')
        f.write('  mpn = "H3"\n')
        f.write("  pin-properties :\n")
        f.write("    [pin:Ref | pads:Int ... | side:Dir|  bank: Ref ]\n")

        for pin_data in pin_data_list:
            f.write(f"    {str(pin_data)}\n")


if __name__ == "__main__":
    input_file = "allwinnerH3.csv"
    output_file = "allwinner-h3.stanza"
    pin_data_list = parse_csv(input_file)
    write_stanza_file(pin_data_list, output_file)
