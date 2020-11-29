import pandas as pd

pd.read_csv('db1.csv').dropna().to_csv('dbx.csv', index=False)