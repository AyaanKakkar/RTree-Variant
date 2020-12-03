'''
This code will scrape and generate POIs for datbase 1 (POI (description, (x,y), bitmap)) 
This will also initialise the Events array with regular opening nd closing times for monday to sunday
Events (description, Start time, End time, bitmap)
Table: POIid eventID
Temp: Storing this database as csv --> transfer to mysql db

'''


urls = ["https://en.m.wikivoyage.org/wiki/Delhi/Central_Delhi",
	"https://en.m.wikivoyage.org/wiki/Delhi/South_East_Delhi",
	"https://en.m.wikivoyage.org/wiki/Delhi/New_Delhi",
	"https://en.m.wikivoyage.org/wiki/Delhi/South_Delhi",
	"https://en.m.wikivoyage.org/wiki/Delhi/Western_Delhi",
	"https://en.m.wikivoyage.org/wiki/Delhi/Eastern_Delhi"]



# urls = ["https://en.m.wikivoyage.org/wiki/Delhi/New_Delhi"]
from bs4 import BeautifulSoup
import requests
import csv
import random
from bitmap import BitMap
# import mysql.connector

# mydb = mysql.connector.connect(
# 	  host="localhost",
# 	  user="user",
# 	  passwd="password",
# 	  database="enav"
# 	)

csv_file1 = open('db1.csv', 'w')

csv_writer1 = csv.writer(csv_file1)
csv_writer1.writerow(['poiID', 'latitude', 'longitude', 'bitmap'])

csv_file2 = open('db3.csv', 'w')

csv_writer2 = csv.writer(csv_file2)
csv_writer2.writerow(['poiID', 'eventID', 'startTime', 'endTime', 'bitmap'])

poiID = 0
eventID = 0
startTimeList = [480, 600, 800] # minutes after 12 midnight
endTimeLIst = [1080, 1200, 1380]

eventDesc = 'Regular opening timings'

categories_dict = {
	'Get in': 0,
	'Geting around': 1,
	'Get around': 1,
	'See': 2,
	'Buy': 3,
	'Sleep': 4,
	'Do': 5,
	'Eat': 6,
	'Drink': 7,
	'Connect': 8,
	'Understand': 9
}

for url in urls:
	print(url)
	source = requests.get(url).text
	soup = BeautifulSoup(source, 'lxml')
	article = soup.find('div', class_='mw-parser-output')

	categories = article.find_all('h2')
	sections = article.find_all('section')
	sections.pop(0)
	# print(len(sections))
	# print(len(categories))
	assert len(sections)==len(categories)

	for i in range(len(categories)):
		category = categories[i].find('span', class_='mw-headline').text
		category_bitmap = BitMap(10)
		category_bitmap.set(categories_dict[category])
		bitmap_number = categories_dict[category]

		places = sections[i].find_all('bdi', class_='vcard')

		for y in places:
			try:
				name = y.find('span', class_='fn org listing-name').text
			except:
				name = "NA"

			try:
				latitude = y.find('span', class_='noprint listing-coordinates').span.find('abbr', class_='latitude').text
			except:
				latitude = "NA"

			try:
				longitude = y.find('span', class_='noprint listing-coordinates').span.find('abbr', class_='longitude').text
			except:
				longitude = "NA"



			try:
				text = y.find('bdi', class_='note listing-content').text
			except:
				text = "NA"

			r = random.randint(0, 2)
			startTime = startTimeList[r]
			endTime = endTimeLIst[r]

			# (['poiID', 'name', 'latitude', 'longitude', 'bitmap', 'desc'])
			# (['poiID', 'eventID', 'startTime', 'endTime', 'desc'])
			# csv_writer1.writerow([poiID, latitude, longitude, bitmap_number])
			# csv_writer2.writerow([poiID, eventID, startTime, endTime, bitmap_number])

			csv_writer1.writerow([poiID, latitude, longitude, category_bitmap])
			csv_writer2.writerow([poiID, eventID, startTime, endTime, category_bitmap])

			# mycursor = mydb.cursor()
			# sql = "insert into Table1 values('%d', '%s', '%f', '%f', '%s', '%s')"%(poiID, name, latitude, longitude, category_bitmap, text)
			# mycursor.execute(sql)
			# mydb.commit()

			# mycursor = mydb.cursor()
			# sql = "insert into Table2 values('%d', '%d', '%d', '%d', '%s', '%s')"%(poiID, eventID, startTime, endTime, category_bitmap, eventDesc)
			# mycursor.execute(sql)
			# mydb.commit()

			poiID += 1
			eventID += 1

csv_file1.close()
csv_file2.close()
print(poiID)
print(eventID)