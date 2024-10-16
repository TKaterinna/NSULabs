import asyncio
import aiohttp

from keys import *

async def fetch_locations(query: str):
    url = f"https://graphhopper.com/api/1/geocode?q={query}&locale=ru&limit=7&key={GRAPH_HOPPER_API_KEY}"
    async with aiohttp.ClientSession() as session:
        async with session.get(url) as response:
            data = await response.json()
            return data.get('hits', [])

async def fetch_weather(lat, lon):
    url = f"http://api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&appid={OPENWEATHERMAP_API_KEY}&units=metric&lang=ru"
    async with aiohttp.ClientSession() as session:
        async with session.get(url) as response:
            data = await response.json()
            return data

async def fetch_interesting_places(lat, lon, radius=1000):
    url = f"https://api.opentripmap.com/0.1/en/places/radius?radius={radius}&lon={lon}&lat={lat}&limit=5&rate=3h&apikey={OPENTRIPMAP_API_KEY}"
    async with aiohttp.ClientSession() as session:
        async with session.get(url) as response:
            data = await response.json()
            return data.get('features', [])

async def fetch_place_description(xid):
    url = f"https://api.opentripmap.com/0.1/en/places/xid/{xid}?apikey={OPENTRIPMAP_API_KEY}"
    async with aiohttp.ClientSession() as session:
        async with session.get(url) as response:
            data = await response.json()
            return data

async def main():
    query = input("Введите название локации: ")
    
    locations = await fetch_locations(query)
    if len(locations) == 0:
        print("Локации не найдены")
        return
    
    print("Найденные локации:")
    for i, location in enumerate(locations):
        loc: str = str()
        if location.get('name'):
            loc += f"{location['name']}   "
        if location.get('country'):
            loc += f"{location['country']}   "
        if location.get('city'):
            loc += f"{location['city']}   "
        if location.get('street'):
            loc += f"{location['street']}   "
        if location.get('housenumber'):
            loc += f"{location['housenumber']}   "
        print(f"{i + 1}. {loc}")
    
    choice_str = input("Выберите локацию (номер): ")
    try:
        choice = int(choice_str) - 1
    except ValueError:
        print("Не число")
        return
    
    if choice < 0 or choice >= len(locations):
        print("Значение вне допустимого диапазона")
        return
    
    selected_location = locations[choice]
    lat, lon = selected_location['point']['lat'], selected_location['point']['lng']
    
    wthr_plc = await asyncio.gather(fetch_weather(lat, lon), fetch_interesting_places(lat, lon))
    weather = wthr_plc[0]
    places = wthr_plc[1]
    
    place_descriptions = []
    if len(places) == 0:
        print("Интересные места не найдены")
    else:
        place_descriptions = await asyncio.gather(*[fetch_place_description(place['properties']['xid']) for place in places])

    wthr: str = str()
    if weather.get('weather'):
        if len(weather['weather']) > 0:
            if weather['weather'][0].get('description'):
                wthr += f"{weather['weather'][0]['description']}   "
    if weather.get('main'):
        if weather['main'].get('temp'):
            wthr += f"temp = {weather['main']['temp']}   "
    print(f"Погода в {selected_location['name']}: {wthr}")

    if len(places):
        print("Интересные места:")
    for i in range(len(places)):
        place = places[i]
        plc: str = str()
        if place.get('properties'):
            if place['properties'].get('name'):
                plc += f"name: {place['properties']['name']}   "
            if place['properties'].get('dist'):
                plc += f"dist = {place['properties']['dist']}   "
            if place['properties'].get('rate'):
                plc += f"rate = {place['properties']['rate']}   "
        print(f"{plc}")

        data = place_descriptions[i]
        descr: str = str()
        if data.get('name'):
            descr += f"Название: {data['name']}\n"
        if data.get('wikipedia_extracts'):
            if data['wikipedia_extracts'].get('text'):
                descr += f"Описание: {data['wikipedia_extracts']['text']}"
        print(f"{descr}")
        print("-" * 40)        

if __name__ == "__main__":
    asyncio.run(main())