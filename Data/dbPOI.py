import mysql.connector
import pandas as pd

mydb = mysql.connector.connect(
  host="localhost",
  user="root",
  password="ayan",
  database="enav"
)

bitmapSize = 16

mycursor = mydb.cursor()
mycursor.execute('DROP TABLE IF EXISTS POI')
mycursor.execute("CREATE TABLE POI (id INT PRIMARY KEY, name VARCHAR(255), latitude DECIMAL(12,8),longitude DECIMAL(12,8),bitmap varchar(100),des varchar(2000))")

df = pd.read_csv('dbx.csv').values

a = []

for i in df:
    x = list(i)
    for z in range(len(x)):
        if isinstance(x[z], str):
            x[z] = x[z].replace('"', '\\"').replace("'", "\\'")
    a.append(tuple(x))


sql = 'INSERT INTO POI(id, name, latitude, longitude, bitmap, des) VALUES (%d, "%s", %f, %f, "%d", "%s")'

for i in a:
    temp = sql % i
    mycursor.execute(temp)

mydb.commit()





