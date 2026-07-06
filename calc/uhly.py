import ephem
from datetime import datetime, timezone, timedelta
import numpy as np


planets = ["mercury","venus","moon","mars","jupiter","saturn","uranus","neptune","europa","ganymede","callisto","io","titan","enceladus","rhea","dione","tethys","mimas","phobos","deimos","polaris","sirius","betelgeuse","rigel","antares","vega","altair","deneb","spica","arcturus"]

def planet_coordinate(planet_name, lati, long, elev, time):
        # Create observer first
        observer = ephem.Observer()
        observer.lat = str(lati)
        observer.lon = str(long)
        observer.elevation = elev
        observer.date = time

        try:
                # Try to use it as a star first
                planet = ephem.star(planet_name)
                planet.compute(observer)

        except KeyError:
                # If it's not a star, try as a planet
                try:
                        planet = getattr(ephem, planet_name)()
                        planet.compute(observer)
                except AttributeError:
                        return None, None, None

        # Calculate altitude and azimuth in degrees
        alt = float(planet.alt) * 180.0 / ephem.pi
        az = float(planet.az) * 180.0 / ephem.pi

        # Try to get magnitude
        try:
                mag = planet.mag
        except AttributeError:
                mag = 0.0

        return [round(alt,2), round(az,2), mag]






def getCoordinateArray(planet_name,lati,long,elev, stepAngle):

        now = datetime.now(timezone.utc)
        dt = datetime.strptime(now.strftime("%Y-%m-%d %H:%M:%S"), "%Y-%m-%d %H:%M:%S")

        observer = ephem.Observer()
        observer.lat = str(lati)
        observer.long = str(long)
        observer.elevation = (elev)

        # Set the observer's date and time
        observer.date = dt

        angleArray = np.array(planets.index(planet_name.lower()))
        planet_name = planet_name.capitalize()


        alt_az1 = planet_coordinate(planet_name,lati,long,elev, observer.date)[:2]
        observer.date = dt + timedelta(seconds=100)
        alt_az2 = planet_coordinate(planet_name,lati,long,elev, observer.date)[:2]


        for angleIndex in range(2):
                time = datetime.strptime(now.strftime("%Y-%m-%d %H:%M:%S"), "%Y-%m-%d %H:%M:%S") + (timedelta(seconds=10))
                angleArray = np.append(angleArray, planet_coordinate(planet_name,lati,long,elev, time)[0:2])

        return (angleArray.astype(np.float32))#.astype(np.float16)

