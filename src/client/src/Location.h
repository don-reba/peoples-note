#pragma once

class Location
{
public:

	bool   IsValid;
	double Latitude;
	double Longitude;
	double Altitude;

public:

	Location();
	Location(double latitude, double longitude, double altitude);
};
