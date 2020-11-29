import pandas as pd
import numpy as np

name = 'dbx.csv'

df = pd.read_csv(name)

df['poiID'] = np.arange(len(df))

df.to_csv(name, index=False)