import csv
import iotsa
import matplotlib.pyplot as pyplot
import pandas

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
        return
        pd = pandas.DataFrame(self.data)
        pd.v = pandas.to_numeric(pd.v)
        pd.ts = pandas.to_numeric(pd.ts)
        pd.t = pandas.to_datetime(pd.t)
        pd.plot(x='t', y='v')
        pyplot.show()
