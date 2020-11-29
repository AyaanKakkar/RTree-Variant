import pandas as pd
import csv
df = pd.read_csv('db1.csv')

df.to_csv('dbx.csv', quoting=csv.QUOTE_ALL, index=False)