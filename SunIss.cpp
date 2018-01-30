// SunIss.cpp : Defines the entry point for the console application.
//
#define _USE_MATH_DEFINES
#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <math.h>
#include <string>

using namespace std::chrono;
using namespace std;

long double mew = 398600;
long double PI = 3.14159265358979323846;

struct Set
{
	long double time; //days
	long double lat; 
	long double lon;
	long double altitude;
	long double azimuth;
};

struct TLE
{
	string satName;
	string satNum;
	char classification;
	string launchYear;
	string launchNumOfYear;
	string pieceOfTheLaunch;
	int epochYear;
	long double epochDoY;
	long double firstTimeDerivative;
	long double secondTimeDerivative;
	long double BSTAR;
	string elementSetNumber;
	long double inclination;
	long double rightAssentionOfAccendingNode;
	long double ecintricity;
	long double argumentOfPerigee;
	long double meanAnomaly;
	long double meanMotion;
	int revNumAtEpoch;
	long double semiMajorAxis;
	long double perigee;
};

long double getJDN();
Set getCords(Set x);


string toDMS(long double ang)
{
	int D = static_cast<int>(ang);
	int M = static_cast<int>(60*(ang-D));
	double S = 60 * ((60 * (ang - D)) - M);
	string ret = to_string(D) + "D " + to_string(M) + "M " + to_string(S) + "S";
	return ret;
}

string toDHMS(long double time, long double jdn)
{
	int days = static_cast<int>(time + .5);
	long double ctime = fmod(24*((time + .5) - days) + 20,24);
	long double delta = time - jdn -(4/24);
	int D = static_cast<int>(delta);
	int H = static_cast<int>(ctime);
	int M = static_cast<int>(60 * (ctime - H));
	double S = 60 * (60 * (ctime - H) - M);
	string ret = to_string(D) + "D " + to_string(H) + ":" +to_string(M) + "M " + to_string(S) + "S";
	return ret;
}
string toDHMS(long double time)
{
	long double ctime = fmod(24 * time, 24);
	long double delta = time;
	int D = static_cast<int>(delta);
	int H = static_cast<int>(ctime);
	int M = static_cast<int>(60 * (ctime - H));
	double S = 60 * (60 * (ctime - H) - M);
	string ret = to_string(D) + ":" + to_string(H) + ":" + to_string(M) + ":" + to_string(S);
	return ret;
}

TLE decodeTLE(string TLE0, string TLE1, string TLE2)
{
	TLE ret;
	ret.satName = TLE0;
	ret.satNum = TLE1.substr(2, 5);
	ret.classification = TLE1.at(7);
	ret.launchYear = TLE1.substr(9, 2);
	ret.launchNumOfYear = TLE1.substr(11, 3);
	ret.pieceOfTheLaunch = TLE1.substr(14, 3);
	ret.epochYear = stoi(TLE1.substr(18, 2)) + 2000;
	ret.epochDoY = stod(TLE1.substr(20, 12));
	ret.firstTimeDerivative = stod(TLE1.substr(33, 10)) * 2;
	ret.secondTimeDerivative = ((stod(TLE1.substr(44, 6)) / 100000) * pow(10, stoi(TLE1.substr(50, 2)))) * 6;
	ret.BSTAR = ((stod(TLE1.substr(53, 6)) / 100000) * pow(10, stoi(TLE1.substr(59, 2))));
	ret.elementSetNumber = TLE1.substr(64, 4);
	ret.inclination = stod(TLE2.substr(8, 8));
	ret.rightAssentionOfAccendingNode = stod(TLE2.substr(17, 8));
	ret.ecintricity = (stod(TLE2.substr(27, 7)) / 10000000);
	ret.argumentOfPerigee = stod(TLE2.substr(34, 8));
	ret.meanAnomaly = stod(TLE2.substr(43, 8));
	ret.meanMotion = stold(TLE2.substr(52, 11));
	ret.revNumAtEpoch = stoi(TLE2.substr(63, 5));
	return ret;
}

void printDTLE(TLE x)
{
	cout << "Name of Satellite: " << x.satName << endl;
	cout << "Satellite Number: " << x.satNum << endl;
	cout << "Classification (U=Unclassified): " << x.classification << endl;
	cout << "Launch Year: " << x.launchYear << endl;
	cout << "Launch Number of the Year: " << x.launchNumOfYear << endl;
	cout << "Piece of the Launch: " <<	x.pieceOfTheLaunch << endl;
	cout << "Epoch Date: " << x.epochYear << ":" << toDHMS(x.epochDoY) << endl;
	cout << "Ballistic Coefficient: " << x.firstTimeDerivative << endl;
	cout << "Second Derivative of Mean Motion: " << x.secondTimeDerivative << endl;
	cout << "BSTAR Drag Term: " << x.BSTAR << endl;
	cout << "Element Set Number: " << x.elementSetNumber << endl;
	cout << "Inclination (degrees): " << x.inclination << endl;
	cout << "Right Ascension of the Ascending Node (degrees): " << x.rightAssentionOfAccendingNode << endl;
	cout << "Eccentricity: " << x.ecintricity << endl;
	cout << "Argument of Perigee (degrees): " << x.argumentOfPerigee << endl;
	cout << "Mean Anomaly (degrees): " << x.meanAnomaly << endl;
	cout << "Mean Motion: " << x.meanMotion << endl;
	cout << "Revolution Number: " << x.revNumAtEpoch << endl;

}

TLE getsemiMajorAxis(TLE x)
{
	x.semiMajorAxis = pow(mew, 1.0 / 3.0) / pow(2 * x.meanMotion*PI / 86400, 2.0 / 3.0);
	return x;
}

TLE getPerigeeApogee(TLE x)
{
	x.perigee = x.semiMajorAxis*(1 - x.ecintricity) - 6371;
	return x;
}

int main()
{	/*
	cout << setprecision(16);
	cout << "Enter TLE:" << endl;
	string  TLE0, TLE1, TLE2;
	getline(cin, TLE0);
	getline(cin, TLE1);
	getline(cin, TLE2);
	TLE1 = TLE1.substr(4);
	TLE2 = TLE2.substr(4);


	//cout << TLE0 << endl << TLE1 << endl << TLE2 << endl;
	TLE ISS = decodeTLE(TLE0, TLE1, TLE2);
	printDTLE(ISS);
	*/
	long double lat = 39.128988;
	long double lon = -84.517678;
	long double jdn = getJDN();
	Set x = { jdn, lat,lon,0,0 };
	

	cout << setprecision(16);
	cout << jdn << endl;
	for (double i = 0; i < 100; i++)
	{

		x = getCords(x);
		cout <<"Time: " << toDHMS(x.time, jdn) << " Altitude: "<< toDMS(x.altitude) << " Azimuth: " << toDMS(x.azimuth) << endl;
		x.time += 10/ 1440.0;
	}
	
    return 0;
}

