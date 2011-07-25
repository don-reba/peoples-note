#include "stdafx.h"
#include "Location.h"

Location::Location()
	: IsValid (false)
{
}

Location::Location(double latitude, double longitude, double altitude)
	: IsValid   (true)
	, Latitude  (latitude)
	, Longitude (longitude)
	, Altitude  (altitude)
{
}