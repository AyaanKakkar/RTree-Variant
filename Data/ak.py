import pandas as pd

df = pd.read_csv('db1.csv')

df.dropna(inplace=True)
df.to_csv('db1.csv', index=False)

ids = df['poiID'].values

dt = pd.read_csv('db3.csv')

dt = dt[dt['poiID'].isin(ids)]

dt.to_csv('db3.csv')