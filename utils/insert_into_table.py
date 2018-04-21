import sys
import sqlite3
import csv

db_name = sys.argv[1]
data_file = sys.argv[2]

conn = sqlite3.connect(db_name)
cursor = conn.cursor()

with open(data_file) as csvfile:
    reader = csv.reader(csvfile, delimiter=' ')

    for row in reader:
        cursor.execute('INSERT INTO paths VALUES (?, ?, ?, ?)', (row[0], row[1], row[2], row[3]))

conn.commit()
