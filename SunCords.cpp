#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <math.h>

using namespace std::chrono;

struct Set
{
	long double time; //days
	long double lat;
	long double lon;
	long double altitude;
	long double azimuth;
};

long double deg2rad(long double x)//good
{
	return x * 3.141592653589793 / 180.0;
}

long double rad2deg(long double x)//good
{
	return x * 180.0 / 3.141592653589793;
}

long double getJDN()//good
{
	milliseconds ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		);
	long double msd = static_cast<long double>(ms.count());
	msd /= 1000;
	msd /= 3600;

	msd /= 24;

	long double jd = 2440587.5 + msd; //correct for Unix epoch 1970-01-01
	return jd;
}

long double getGST(long double jdn)//good
{
	long double UT = 24 * fmod(jdn + .5, 1);
	long double S = (jdn - (UT / 24)) - 2451545.0;
	S /= 36525;
	S = fmod(6.697374558 + 2400.051336*S + .000025862*S*S, 24);
	UT *= 1.002737909;
	S += UT;
	S = fmod(S, 24);
	return S;
}

long double getLST(long double lon, long double gst)
{
	lon /= 15;
	gst += lon;
	return fmod(gst, 24);
}

long double getT(long double jdn)//good
{
	return (jdn - 2415020.0) / 36525;
}

long double getEclipticLongitude(long double T)//good
{
	return fmod(279.6966778 + 36000.76892*T + .0003025*T*T, 360);
}

long double getLonOSun(long double T)//good
{
	return fmod(281.2208444 + 1.719175*T + .000452778*T*T, 360);
}

long double getEccentricity(long double T)//good
{
	return .01675104 - .0000418*T - .000000126*T*T;
}

long double getMeanAnomaly(long double eclipticLongitude, long double lonOSun)//good
{
	return fmod(eclipticLongitude - lonOSun + 360, 360);
}

long double getEcentricAnomaly(long double eccentricity, long double meanAnomaly, long double eclipticLongitude)//good
{
	long double ecentricAnomaly = deg2rad(meanAnomaly);
	long double delta = ecentricAnomaly - eccentricity*sin(ecentricAnomaly) - deg2rad(meanAnomaly);
	long double chE = 0;
	while (abs(delta) > eclipticLongitude)
	{
		chE = delta / (1 - eccentricity*cos(ecentricAnomaly));
		ecentricAnomaly -= chE;
		delta = ecentricAnomaly - eccentricity*sin(ecentricAnomaly) - deg2rad(meanAnomaly);
	}
	return rad2deg(ecentricAnomaly);
}

long double getTrueAnomaly(long double eccentricity, long double ecentricAnomaly)//good
{
	return fmod(rad2deg(2 * atan(sqrt((1 + eccentricity) / (1 - eccentricity))*tan(deg2rad(ecentricAnomaly) / 2))) + 360, 360);
}

long double getEclipticLongitudeSun(long double lonOSun, long double trueAnomaly)//good
{
	return fmod(lonOSun + trueAnomaly + 360, 360);
}

long double getObliquity(long double jdn)//good
{
	long double T = (jdn - 2451544.5) / 36525;
	return 23.43929 + 0.01300417*T + .0000001666667*T*T + .0000005027778*T*T*T;
}

long double getRightAscension(long double eclipticLongitudeSun, long double obliquity)//good
{
	return fmod(rad2deg(atan2(sin(deg2rad(eclipticLongitudeSun))*cos(deg2rad(obliquity)) - tan(deg2rad(0))*sin(deg2rad(obliquity)), cos(deg2rad(eclipticLongitudeSun)))), 360);
}

long double getDeclination(long double eclipticLongitudeSun, long double obliquity)//good
{
	return fmod(rad2deg(asin(sin(deg2rad(obliquity))*sin(deg2rad(eclipticLongitudeSun)))) + 360, 360);
}

long double getHourAngle(long double lst, long double rightAscension)
{
	return fmod(lst - rightAscension / 15 + 24, 24);
}

long double getAltitude(long double hourAngle, long double declination, long double lat)
{
	return rad2deg(asin(sin(deg2rad(declination))*sin(deg2rad(lat)) + cos(deg2rad(declination))*cos(deg2rad(lat))*cos(deg2rad(hourAngle * 15))));
}
long double getAzimuth(long double altitdue, long double hourAngle, long double declination, long double lat)
{
	return fmod(rad2deg(atan2(-cos(deg2rad(declination))*cos(deg2rad(lat))*sin(deg2rad(hourAngle * 15)), sin(deg2rad(declination)) - sin(deg2rad(lat))*sin(deg2rad(altitdue)))) + 360, 360);
}

Set getCords(Set x)
{
	long double lat = x.lat;
	long double lon = x.lon;
	long double jdn = x.time;
	long double GST = getGST(jdn);
	long double LST = getLST(lon, GST);
	long double T = getT(jdn);
	long double eclipticLongitude = getEclipticLongitude(T);
	long double lonOSun = getLonOSun(T);
	long double eccentricity = getEccentricity(T);
	long double meanAnomaly = getMeanAnomaly(eclipticLongitude, lonOSun);
	long double ecentricAnomaly = getEcentricAnomaly(eccentricity, meanAnomaly, eclipticLongitude);
	long double trueAnomaly = getTrueAnomaly(eccentricity, ecentricAnomaly);
	long double eclipticLongitudeSun = getEclipticLongitudeSun(lonOSun, trueAnomaly);
	long double obliquity = getObliquity(jdn);
	long double rightAscension = getRightAscension(eclipticLongitudeSun, obliquity);
	long double declination = getDeclination(eclipticLongitudeSun, obliquity);
	long double hourAngle = getHourAngle(LST, rightAscension);
	x.altitude = getAltitude(hourAngle, declination, lat);
	x.azimuth = getAzimuth(x.altitude, hourAngle, declination, lat);
	return x;
}


