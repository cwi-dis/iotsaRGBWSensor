import sys
import os
import argparse
from .color_sensor import ColorSensor

def main():
    parser = argparse.ArgumentParser(description="Get data from iotsaRGBWSenspr")
    parser.add_argument("-d", "--device", action="store", metavar="HOST", help="Hostname or IP to get data from")
    parser.add_argument("-D", "--devarg", nargs="*", default=[], metavar="NAME=VALUE", help="iotsa arguments")
    parser.add_argument("-g", "--graph", action="store_true", help="Graph data")
    parser.add_argument("-i", "--input", nargs="*", default=[], metavar="FILE", help="Get data from CSV file in stead of from device")
    parser.add_argument("-o", "--output", action="store", metavar="FILE", help="Store data to file as CSV")
    parser.add_argument("-m", "--merge", action="store_true", help="Merge CSV data into output file in stead of appending")
    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose messages")
    args = parser.parse_args()
    sensor = ColorSensor(verbose=args.verbose)
    if args.device:
        kwargs = {}
        for pair in args.devarg:
            k, v = pair.split('=')
            kwargs[k] = v
        sensor.read_device(args.device, kwargs)
    for a in args.input:
        sensor.read_file(a)
    if args.merge:
        if not args.output:
            print(f'{parser.prog}: --merge requires --output')
        else:
            sensor.read_file(args.output)
    if args.output:
        sensor.write_file(args.output)
    if args.output:
        sensor.write_file(args.output)
    if args.graph:
        sensor.graph()

if __name__ == '__main__':
    main()