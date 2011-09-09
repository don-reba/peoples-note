#include "stdafx.h"
#include "Location.h"

Location::Location()
	: IsValid (false)
{
}

Location::Location(double altitude, double latitude, double longitude)
	: IsValid   (true)
	, Altitude  (altitude)
	, Latitude  (latitude)
	, Longitude (longitude)
{
}

void Location::Validate()
{
	IsValid = Altitude != 0.0 || Latitude != 0.0 || Longitude != 0.0;
}
