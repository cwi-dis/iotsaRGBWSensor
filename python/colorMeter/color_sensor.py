import csv
import iotsa
import matplotlib.pyplot as pyplot
import colour
import colour.plotting
import matplotlib.pyplot


class ColorSensor:
    def __init__(self, verbose=False):
        self.data = []
        self.verbose = verbose
        self.device = None

    def read_device(self, device, kwargs):
        self.device = iotsa.IotsaDevice(device, **kwargs)
        api = self.device.getApi('rgbw')
        api_data = api.getAll()
        # Update API data with measurement indication
        print(f"RGB={api_data['r'],api_data['g'],api_data['b']}, White={api_data['w']}, Lux={api_data['lux']}, Temp={api_data['cct']}")
        self.data.append(api_data)

    def read_file(self, filename):
        data = []
        with open(filename, 'r') as fp:
            csv_reader = csv.DictReader(fp, quoting=csv.QUOTE_NONNUMERIC)
            for record in csv_reader:
                data.append(record)
        self.data = self.data + data

    def write_file(self, filename):
        with open(filename, 'w') as fp:
            csv_writer = csv.DictWriter(fp, self.data[0].keys(), quoting=csv.QUOTE_NONNUMERIC)
            csv_writer.writeheader()
            csv_writer.writerows(self.data)
        if self.verbose:
            print(f'write_file: wrote {len(self.data)} records to {filename}')

    def graph(self):
        fig, ax = colour.plotting.plot_planckian_locus_in_chromaticity_diagram_CIE1931([], standalone=False)
        ax.set_title("CCT color")
        cct_measured = self.data[0]['cct']
        x, y = colour.CCT_to_xy(cct_measured)
        matplotlib.pyplot.plot([x], [y], '*', label="Device-converted")
        r = self.data[0]['r']
        g = self.data[0]['g']
        b = self.data[0]['b']
        xyz = colour.RGB_to_XYZ([r, g, b], 'sRGB')
        x, y = colour.XYZ_to_xy(xyz)
        matplotlib.pyplot.plot([x], [y], '.', label="Python-converted")
        ax.legend()
        matplotlib.pyplot.show()
        return
        pd = pandas.DataFrame(self.data)
        pd.v = pandas.to_numeric(pd.v)
        pd.ts = pandas.to_numeric(pd.ts)
        pd.t = pandas.to_datetime(pd.t)
        pd.plot(x='t', y='v')
        pyplot.show()
