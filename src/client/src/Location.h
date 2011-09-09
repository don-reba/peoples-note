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
	Location(double atltitude, double latitude, double longitude);

	void Validate();
};
