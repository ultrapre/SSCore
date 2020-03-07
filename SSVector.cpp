//  SSVector.hpp
//  SSCore
//
//  Created by Tim DeBenedictis on 2/24/20.
//  Copyright © 2020 Southern Stars. All rights reserved.

#include <math.h>
#include "SSVector.hpp"

SSVector::SSVector ( void )
{
    x = y = z = 0.0;
}

SSVector::SSVector ( double x, double y, double z )
{
    this->x = x;
    this->y = y;
    this->z = z;
}

double SSVector::magnitude ( void )
{
    return ( sqrt ( x * x + y * y + z * z ) );
}

double SSVector::normalize ( void )
{
    double s = magnitude();
    if ( s > 0.0 )
        divideBy ( s ); 
    return ( s );
}

SSVector SSVector::add ( SSVector other )
{
    return ( SSVector ( x + other.x, y + other.y, z + other.z ) );
}

SSVector SSVector::subtract ( SSVector other )
{
    return ( SSVector ( x - other.x, y - other.y, z - other.z ) );
}

SSVector SSVector::multiplyBy ( double s )
{
    return ( SSVector ( x * s, y * s, z * s ) );
}

SSVector SSVector::divideBy ( double s )
{
    return ( SSVector ( x / s, y / s, z / s ) );
}

double SSVector::dotProduct ( SSVector other )
{
    return ( x * other.x + y * other.y + z * other.z );
}

SSVector SSVector::crossProduct ( SSVector other )
{
    double cX = y * other.z - z * other.y;
    double cY = z * other.x - x * other.z;
    double cZ = x * other.y - y * other.x;
    
    return ( SSVector ( cX, cY, cZ ) );
}

void SSVector::toSpherical ( SSAngle &lon, SSAngle &lat )
{
    double r = magnitude();
    lat = SSAngle ( asin ( z / r ) );
    lon = SSAngle ( atan2 ( y, x ) ).mod2Pi();
}

void SSVector::toSpherical ( SSAngle &lon, SSAngle &lat, double &r )
{
    r = magnitude();
    lat = SSAngle ( asin ( z / r ) );
    lon = SSAngle ( atan2 ( y, x ) ).mod2Pi();
}

SSVector SSVector::fromSpherical ( SSAngle lon, SSAngle lat, double rad )
{
    double x = rad * cos ( lat.a ) * cos ( lon.a );
    double y = rad * cos ( lat.a ) * sin ( lon.a );
    double z = rad * sin ( lat.a );
    
    return SSVector ( x, y, z );
}

SSVector SSVector::fromSpherical ( SSAngle lon, SSAngle lat )
{
    double x = cos ( lat.a ) * cos ( lon.a );
    double y = cos ( lat.a ) * sin ( lon.a );
    double z = sin ( lat.a );
    
    return SSVector ( x, y, z );
}

SSAngle SSVector::angularSeparation ( SSVector other )
{
    return SSAngle ( asin ( dotProduct ( other ) ) );
}

double SSVector::distance ( SSVector other )
{
    return subtract ( other ).magnitude();
}
