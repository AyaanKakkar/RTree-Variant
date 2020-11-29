import pandas as pd
import numpy as np

name = 'db3.csv'

df = pd.read_csv(name)

df.drop(df[df['poiID'] > 77].index, inplace=True)

df.to_csv(name, index=False)