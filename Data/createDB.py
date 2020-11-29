import pandas as pd
import numpy as np

db = 'db1.csv'
outFile = 'db2.csv'

df = pd.read_csv(db)

desc = df[df.columns[-1]].values

ids = np.arange(len(desc))

poiID = df[df.columns[0]].values


df = pd.DataFrame({'id' : ids, 'doc' : desc, 'POI_ID': poiID})

df.to_csv(outFile, index=False)

        