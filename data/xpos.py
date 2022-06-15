import fileinput
import csv

with fileinput.input(files=('positions.csv'), inplace=True, mode='r') as f:
    reader = csv.DictReader(f)
    print(",".join(reader.fieldnames))  # print back the headers
    for row in reader:
        # print(row)
        row["X"] = float(row["X"]) * 0.5
        row["Y"] = float(row["Y"]) * 0.5
        print(",".join([str(row["X"]), str(row["Y"])]))