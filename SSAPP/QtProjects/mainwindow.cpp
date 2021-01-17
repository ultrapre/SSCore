#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTime>
#include <QTimer>
#include <QtConcurrent>
#include <QFuture>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QDesktopServices>
#include "newdownloader.h"
//#include "SSTest.cpp"

//  SSTest.cpp
//  SSCore
//
//  Created by Tim DeBenedictis on 2/24/20.
//  Copyright © 2020 Southern Stars. All rights reserved.

#include <cstdio>
#if defined __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif
#include <iostream>
using namespace std;

#include "SSCoordinates.hpp"
#include "SSOrbit.hpp"
#include "SSPlanet.hpp"
#include "SSStar.hpp"
#include "SSConstellation.hpp"
#include "SSImportHIP.hpp"
#include "SSImportSKY2000.hpp"
#include "SSImportNGCIC.hpp"
#include "SSImportMPC.hpp"
#include "SSImportGJ.hpp"
#include "SSJPLDEphemeris.hpp"
#include "SSTLE.hpp"
#include "SSEvent.hpp"
#include "VSOP2013.hpp"
#include "ELPMPP02.hpp"



QMap<QString, QString> map_nebula;
QStringList fileLocations;
QString cuserDir = QString::fromLocal8Bit(qgetenv("EXTERNAL_STORAGE")) + "/stellarium";
QSettings *conf;


void makeSureDirExistsAndIsWritable(const QString& dirFullPath)
{

    // Check that the dirFullPath directory exists
    QFileInfo uDir(dirFullPath);
    if (!uDir.exists())
    {
        // The modules directory doesn't exist, lets create it.
        qDebug() << "Creating directory " << QDir::toNativeSeparators(uDir.filePath());
        if (!QDir("/").mkpath(uDir.filePath()))
        {
            throw std::runtime_error(QString("Could not create directory: " +uDir.filePath()).toStdString());
        }
        QFileInfo uDir2(dirFullPath);
        if (!uDir2.isWritable())
        {
            throw std::runtime_error(QString("Directory is not writable: " +uDir2.filePath()).toStdString());
        }
    }
    else if (!uDir.isWritable())
    {
        throw std::runtime_error(QString("Directory is not writable: " +uDir.filePath()).toStdString());
    }
}


void exportCatalog ( SSObjectVec &objects, SSCatalog cat, int first, int last )
{
    SSObjectMap map = SSMakeObjectMap ( objects, cat );

    for ( int n = first; n <= last; n++ )
    {
        int i = map[ SSIdentifier ( cat, n ) ];
        if ( i > 0 )
        {
            SSObjectPtr pObj = objects[ i - 1 ];
            cout << pObj->toCSV() << endl;
        }
    }
}

void TestTime ( void )
{
    SSTime now = SSTime::fromSystem();
    SSDate date = now;

    cout << "Current local date is " << format ( "%04hd-%02hd-%0hd", date.year, date.month, date.day ) << endl;
    cout << "Current local time is " << format ( "%02hd:%02hd:%04.1f", date.hour, date.min, date.sec ) << endl;
    cout << "Current local time is " << format ( "%+.2f", date.zone ) << " hours east of UTC" << endl;
    cout << "Current Julian Date is " << format ( "%.6f", now.jd ) << endl;

    cout << "Current working directory is " << getcwd() << endl << endl;

    cout << "Before is "
         << format("%04hd-%02hd-%02.0f %02hd:%02hd:%04.1f",
              date.year,
              date.month,
              floor(date.day),
              date.hour,
              date.min,
              date.sec)
         << endl;
    SSTime time2 = date;
    SSDate date2 = time2;
    cout << "After is "
         << format("%04hd-%02hd-%02.0f %02hd:%02hd:%04.1f",
              date2.year,
              date2.month,
              floor(date2.day),
              date2.hour,
              date2.min,
              date2.sec)
         << endl;
};

void TestSatellites ( string inputDir, string outputDir )
{
    string filename = inputDir + "/SolarSystem/Satellites/visual.txt";
    FILE *file = fopen ( filename.c_str(), "r" );
    if ( ! file )
    {
        cout << "Failed to open " << filename << endl;
        return;
    }

    string outfilename = outputDir + "/ExportedSatellites.tle";
    ofstream outfile ( outfilename );
    if ( ! outfile )
    {
        cout << "Failed to open " << outfilename << endl;
        return;
    }

    SSTLE tle;

    while ( tle.read ( file ) == 0 )
    {
        tle.write ( cout );

        SSVector pos, vel;

        tle.toPositionVelocity ( tle.jdepoch + 10.0, pos, vel );

        cout << format ( "JD %.6f  ", tle.jdepoch + 10.0 );
        cout << format ( "pos %+10.3f %+10.3f %+10.3f  ", pos.x, pos.y, pos.z );
        cout << format ( "vel %+7.3f %+7.3f %+7.3f",   vel.x, vel.y, vel.z ) << endl;

        SSTLE tle1 = tle;
        tle1.fromPositionVelocity ( tle.jdepoch + 10.0, pos, vel );

        SSTLE tle2 = tle1;
        tle1.toPositionVelocity ( tle1.jdepoch - 10.0, pos, vel );
        tle2.fromPositionVelocity ( tle1.jdepoch - 10.0, pos, vel );
        tle2.write ( cout );
        tle2.write ( outfile );
     }
}

void TestSolarSystem ( string inputDir, string outputDir )
{
    SSObjectVec planets;
    int numPlanets = SSImportObjectsFromCSV ( inputDir + "/SolarSystem/Planets.csv", planets );
    cout << "Imported " << numPlanets << " major planets" << endl;
    numPlanets = SSExportObjectsToCSV ( "", planets );

    SSObjectVec moons;
    int numMoons = SSImportObjectsFromCSV ( inputDir + "/SolarSystem/Moons.csv", moons );
    cout << "Imported " << numMoons << " natural satellites" << endl;

    SSObjectVec comets;
    int numComets = SSImportMPCComets ( inputDir + "/SolarSystem/Comets.txt", comets );
    cout << "Imported " << numComets << " MPC comets" << endl;

    SSObjectVec asteroids;
    int numAsteroids = SSImportMPCAsteroids ( inputDir + "/SolarSystem/Asteroids.txt", asteroids );
    cout << "Imported " << numAsteroids << " MPC asteroids" << endl;

    if ( ! outputDir.empty() )
    {
        numMoons = SSExportObjectsToCSV ( outputDir + "/ExportedMoons.csv", moons );
        cout << "Exported " << numMoons << " natural satellites to " << outputDir + "/ExportedMoons.csv" << endl;

        numComets = SSExportObjectsToCSV ( outputDir + "/ExportedComets.csv", comets );
        cout << "Exported " << numComets << " MPC comets to " << outputDir + "/ExportedComets.csv" << endl;

        numAsteroids = SSExportObjectsToCSV ( outputDir + "/ExportedAsteroids.csv", asteroids );
        cout << "Exported " << numAsteroids << " MPC asteroids to " << outputDir + "/ExportedAsteroids.csv" << endl;
    }
}

void TestConstellations ( string inputDir, string outputDir )
{
    SSObjectVec constellations;

    int numCons = SSImportConstellations ( inputDir + "/Constellations/Constellations.csv", constellations );
    cout << "Imported " << numCons << " IAU constellations" << endl;

    int numVerts = SSImportConstellationBoundaries ( inputDir + "/Constellations/Boundaries.csv", constellations );
    cout << "Imported " << numVerts << " IAU constellation boundary vertices" << endl;

    int numLines = SSImportConstellationShapes ( inputDir + "/Constellations/Shapes.csv", constellations );
    cout << "Imported " << numLines << " IAU constellation shape lines" << endl;

    if ( ! outputDir.empty() )
    {
        numCons = SSExportObjectsToCSV ( outputDir + "/ExportedConstellations.csv", constellations );
        cout << "Exported " << numCons << " constellations to " << outputDir + "/ExportedConstellations.csv" << endl;
    }
}

void TestStars ( string inputDir, string outputDir )
{
    SSObjectVec nearest, brightest;

    int numStars = SSImportObjectsFromCSV ( inputDir + "/Stars/Nearest.csv", nearest );
    cout << "Imported " << numStars << " nearby stars" << endl;

    numStars = SSImportObjectsFromCSV ( inputDir + "/Stars/Brightest.csv", brightest );
    cout << "Imported " << numStars << " bright stars" << endl;

    if ( ! outputDir.empty() )
    {
        numStars = SSExportObjectsToCSV ( outputDir + "/ExportedNearbyStars.csv", nearest );
        cout << "Exported " << numStars << " nearby stars to " << outputDir + "/ExportedNearbyStars.csv" << endl;

        numStars = SSExportObjectsToCSV ( outputDir + "/ExportedBrightStars.csv", brightest );
        cout << "Exported " << numStars << " bright stars to " << outputDir + "/ExportedBrightStars.csv" << endl;
    }
}

void TestDeepSky ( string inputDir, string outputDir )
{
    SSObjectVec messier, caldwell;

    int numObjs = SSImportObjectsFromCSV ( inputDir + "/DeepSky/Messier.csv", messier );
    cout << "Imported " << numObjs << " Messier objects" << endl;

    numObjs = SSImportObjectsFromCSV ( inputDir + "/DeepSky/Caldwell.csv", caldwell );
    cout << "Imported " << numObjs << " Caldwell objects" << endl;

    if ( ! outputDir.empty() )
    {
        numObjs = SSExportObjectsToCSV ( outputDir + "/ExportedMessier.csv", messier );
        cout << "Exported " << numObjs << " Messier objects to " << outputDir + "/ExportedMessier.csv" << endl;

        numObjs = SSExportObjectsToCSV ( outputDir + "/ExportedCaldwell.csv", caldwell );
        cout << "Exported " << numObjs << " Caldwell objects to " << outputDir + "/ExportedCaldwell.csv" << endl;
    }
}

void TestJPLDEphemeris ( string inputDir )
{
    SSJPLDEphemeris jpldeph;

    string ephemFile = inputDir + "/SolarSystem/DE438/1950_2050.438";
    if ( ! jpldeph.open ( ephemFile ) )
    {
        cout << "Failed to open " << ephemFile << endl;
        return;
    }

    cout << "Successfully opened " << ephemFile << endl;
    cout << "JED " << jpldeph.getStartJED() << " to " << jpldeph.getStopJED() << endl;

    // for ( int i = 0; i < jpldeph.getConstantNumber(); i++ )
    //    cout << jpldeph.getConstantName ( i ) << " = " << jpldeph.getConstantValue ( i ) << endl;

    double jed = SSTime ( SSDate ( kGregorian, 0.0, 2020, 1, 1.0, 0, 0, 0.0 ) ).getJulianEphemerisDate();
    SSVector pos, vel;

    for ( int id = 0; id <= 10; id++ )
    {
        jpldeph.compute ( id, jed, true, pos, vel );
        cout << format ( "obj %2d  ", id );
        cout << format ( "pos %+12.8f %+12.8f %+12.8f  ", pos.x, pos.y, pos.z );
        cout << format ( "vel %+11.8f %+11.8f %+11.8f", vel.x, vel.y, vel.z ) << endl;
    }

    jpldeph.close();
}

void TestEvents ( SSCoordinates coords, SSObjectVec &solsys )
{
    SSTime now = coords.getTime();

    // Compute Sun/Moon rise/transit/set circumstances

    if ( solsys.size() > 10 )
    {
        SSObjectPtr pSun = solsys[0];
        SSObjectPtr pMoon = solsys[10];

        SSPass sunpass = SSEvent::riseTransitSet ( now, coords, pSun, SSEvent::kSunMoonRiseSetAlt );
        SSPass moonpass = SSEvent::riseTransitSet ( now, coords, pMoon, SSEvent::kSunMoonRiseSetAlt );

        SSDate date = SSDate ( sunpass.rising.time );
        if ( isinf ( sunpass.rising.time ) )
            cout << "Sunrise:  none" << endl;
        else
            cout << "Sunrise:  " << date.format ( "%H:%M:%S" ) << format ( " @ %.1f°", sunpass.rising.azm * SSAngle::kDegPerRad ) << endl;

        date = SSDate ( sunpass.setting.time );
        if ( isinf ( sunpass.setting.time ) )
            cout << "Sunset:   none" << endl;
        else
            cout << "Sunset:   " << date.format ( "%H:%M:%S" ) << format ( " @ %.1f°", sunpass.setting.azm * SSAngle::kDegPerRad ) << endl;

        date = SSDate ( moonpass.rising.time );
        if ( isinf ( moonpass.rising.time ) )
            cout << "Moonrise: none" << endl;
        else
            cout << "Moonrise: " << date.format ( "%H:%M:%S" ) << format ( " @ %.1f°", moonpass.rising.azm * SSAngle::kDegPerRad ) << endl;

        date = SSDate ( moonpass.setting.time );
        if ( isinf ( moonpass.setting.time ) )
            cout << "Moonset:  none" << endl << endl;
        else
            cout << "Moonset:  " << date.format ( "%H:%M:%S" ) << format ( " @ %.1f°", moonpass.setting.azm * SSAngle::kDegPerRad ) << endl << endl;

        SSTime time = SSEvent::nextMoonPhase ( now, pSun, pMoon, SSEvent::kNewMoon );
        date = SSDate ( time );
        cout << "New Moon:       " << date.format ( "%Y/%m/%d %H:%M:%S" ) << endl;

        time = SSEvent::nextMoonPhase ( now, pSun, pMoon, SSEvent::kFirstQuarterMoon );
        date = SSDate ( time );
        cout << "First Quarter:  " << date.format ( "%Y/%m/%d %H:%M:%S" ) << endl;

        time = SSEvent::nextMoonPhase ( now, pSun, pMoon, SSEvent::kFullMoon );
        date = SSDate ( time );
        cout << "Full Moon:      " << date.format ( "%Y/%m/%d %H:%M:%S" ) << endl;

        time = SSEvent::nextMoonPhase ( now, pSun, pMoon, SSEvent::kLastQuarterMoon );
        date = SSDate ( time );
        cout << "Last Quarter:   " << date.format ( "%Y/%m/%d %H:%M:%S" ) << endl << endl;

        // Find Jupiter-Saturn conjunctions in the next year

        SSObjectPtr pJup = solsys[5];
        SSObjectPtr pSat = solsys[6];
        vector<SSEventTime> conjunctions;

        SSEvent::findConjunctions ( coords, pJup, pSat, now, now + 365.25, conjunctions, 10 );
        cout << conjunctions.size() << " Jupiter-Saturn Conjunctions in the next year:" << endl;
        for ( int i = 0; i < conjunctions.size(); i++ )
        {
            date = SSDate ( conjunctions[i].time );
            SSDegMinSec sep = SSAngle ( conjunctions[i].value );
            cout << sep.format ( "%2hd° %2hd' %4.1f\"" ) << " on " << date.format ( "%Y/%m/%d %H:%M:%S" ) << endl;
        }
        cout << endl;
    }

    // Find the ISS

    int i = 0;
    for ( i = 0; i < solsys.size(); i++ )
    {
        SSPlanet *p = SSGetPlanetPtr ( solsys[i] );
        if ( p->getType() == kTypeSatellite )
            if ( p->getIdentifier() == SSIdentifier ( kCatNORADSat, 25544 ) )
                break;
    }

    // If we found it, find and print all ISS passes in the next day.

    coords.setTime ( now );
    if ( i < solsys.size() )
    {
        vector<SSPass> passes;

        int numpasses = SSEvent::findSatellitePasses ( coords, solsys[i], now, now + 1.0, 0.0, passes, 10 );
        cout << numpasses << " ISS passes in the next day:" << endl;
        for ( i = 0; i < numpasses; i++ )
        {
            SSDate date = SSDate ( passes[i].rising.time );
            cout << format ( "Rise:  %02hd:%02hd:%02.0f @ %.1f°", date.hour, date.min, date.sec, passes[i].rising.azm * SSAngle::kDegPerRad ) << endl;

            date = SSDate ( passes[i].transit.time );
            cout << format ( "Peak:  %02hd:%02hd:%02.0f @ %.1f°", date.hour, date.min, date.sec, passes[i].transit.alt * SSAngle::kDegPerRad ) << endl;

            date = SSDate ( passes[i].setting.time );
            cout << format ( "Set:   %02hd:%02hd:%02.0f @ %.1f°", date.hour, date.min, date.sec, passes[i].setting.azm * SSAngle::kDegPerRad ) << endl << endl;
        }
    }
}

void TestEphemeris ( string inputDir, string outputDir )
{
    SSObjectVec solsys;

    SSPlanet::useVSOPELP ( false );
    string ephemFile = inputDir + "/SolarSystem/DE438/1950_2050.438";
    if ( SSJPLDEphemeris::open ( ephemFile ) )
        cout << "Successfully opened JPL DE438 ephemeris file." << endl;
    else
        cout << "Failed to open JPL DE438 ephemeris file " << ephemFile << endl;

    SSImportObjectsFromCSV ( inputDir + "/SolarSystem/Planets.csv", solsys );
    SSImportObjectsFromCSV ( inputDir + "/SolarSystem/Moons.csv", solsys );
    //silasComets
//    SSObjectVec comets;
    int numComets = SSImportMPCComets ( inputDir + "/SolarSystem/CometsLite.txt", solsys );

    cout << "Imported " << solsys.size() << " solar system objects." << endl;

    int nsat = SSImportSatellitesFromTLE ( inputDir + "/SolarSystem/Satellites/visual.txt", solsys );
    cout << "Imported " << nsat << " artificial satellites." << endl;

    int nnames = SSImportMcNames ( inputDir + "/SolarSystem/Satellites/mcnames.txt", solsys );
    cout << "Imported " << nnames << " McCants satellite names." << endl;

    SatFreqMap satfreqs;
    int nfreqs = SSImportSatelliteFrequencyData ( inputDir + "/SolarSystem/Satellites/je9pel.csv", satfreqs );
    cout << "Imported " << nfreqs << " JE9PEL satellite radio frequencies." << endl << endl;

    SSDate date ( kGregorian, 0.0, 2020, 4, 15.0, 0, 0, 0.0 );
    SSTime now = SSTime::fromSystem(); // SSTime ( date );
    now -= 123;

    date = SSDate ( now );
    SSSpherical here = { SSAngle ( SSDegMinSec ( '+', 110, 0, 0 ) ), SSAngle ( SSDegMinSec ( '+', 21, 0, 0 ) ), 0.026 };
    SSCoordinates coords ( now, here );

    cout << format ( "Test Date: %04d/%02hd/%02hd", date.year, date.month, date.day ) << endl;
    cout << format ( "Test Time: %02hd:%02hd:%04.1f", date.hour, date.min, date.sec ) << endl;
    cout << format ( "Test Zone: %+.1f hours", date.zone ) << endl;
    cout << format ( "Test Longitude: %s", SSDegMinSec ( here.lon ).toString().c_str() ) << endl;
    cout << format ( "Test Latitude:  %s", SSDegMinSec ( here.lat ).toString().c_str() ) << endl;
    cout << format ( "Test Altitude:  %.0f m", here.rad * 1000.0 ) << endl << endl;

    coords.setAberration ( true );
    coords.setLightTime ( true );

    TestEvents ( coords, solsys );

    // Compute and print ephemeris of solar system objects.

    coords.setTime ( now );
    for ( int i = 0; i < solsys.size(); i++ )
    {
        SSPlanet *p = SSGetPlanetPtr ( solsys[i] );
        if ( p == nullptr )
            continue;

        // Skip everything except the first 34 solar system objects and the ISS.

        bool skip = i < 34 ? false : true;
        if ( p->getType() == kTypeSatellite )
            if ( p->getIdentifier() == SSIdentifier ( kCatNORADSat, 25544 ) )
                skip = false;

        if ( skip && p->getType() != kTypeComet)
            continue;

        p->computeEphemeris ( coords );
        string con = SSConstellation::identify ( p->getDirection() );
        SSSpherical dir ( p->getDirection() );
        dir = coords.transform ( kFundamental, kEquatorial, dir );
        SSHourMinSec ra ( dir.lon );
        SSDegMinSec dec ( dir.lat );
        double dist = p->getDistance();
        float mag = p->getMagnitude();

        cout << p->getName ( 0 ) << ":" << endl;
        cout << "RA:   " << ra.format ( " %02hdh %02hdm %05.2fs" ) << endl;
        cout << "Dec:  " << dec.format ( "%c%02hd° %02hd' %04.1f\"" ) << endl;
        cout << "Con:  " << con << endl;
        if ( dist > 0.1 )
            cout << "Dist: " << format ( "%.6f AU", dist ) << endl;
        else
            cout << "Dist: " << format ( "%.0f km", dist * coords.kKmPerAU ) << endl;
        cout << "Mag:  " << format ( "%+.2f", mag ) << endl;
        cout << "Illum: " << format ( "%.1f%%", p->illumination() * 100.0 ) << endl;

        // For moons, print RA (X) and Dec (Y) offset in arcsec from primary planet.

        if ( p->getType() == kTypeMoon )
        {
            SSPlanet *primary = SSGetPlanetPtr ( solsys[ p->getIdentifier().identifier() / 100 ] );
            SSSpherical primaryDir ( primary->getDirection() );
            primaryDir = coords.transform ( kFundamental, kEquatorial, primaryDir );
            double x = modpi ( dir.lon - primaryDir.lon ) * SSAngle::kArcsecPerRad * cos ( primaryDir.lat );
            double y = modpi ( dir.lat - primaryDir.lat ) * SSAngle::kArcsecPerRad;
            cout << format ( "X: %+.1f\" Y: %+.1f\"", x, y ) << endl;
        }

        cout << endl;
    }

    SSJPLDEphemeris::close();

    // Compute and print ephemeris information for the 10 nearest stars

    SSObjectVec nearStars;
    int numStars = SSImportObjectsFromCSV ( inputDir + "/Stars/Nearest.csv", nearStars );
    cout << "Imported " << numStars << " nearby stars" << endl;

    coords.setStarParallax ( true );
    coords.setStarMotion ( true );
    coords.setAberration ( true );

    for ( int i = 0; i < nearStars.size(); i++ )
    {
        SSStar *pStar = SSGetStarPtr ( nearStars[i] );
        if ( pStar == nullptr )
            continue;

        // Stop after the first 11 stars.

         if ( i > 10 )
            break;

        pStar->computeEphemeris ( coords );
        string con = SSConstellation::identify ( pStar->getDirection() );
        SSSpherical dir ( pStar->getDirection() );
        dir = coords.transform ( kFundamental, kEquatorial, dir );
        SSHourMinSec ra ( dir.lon );
        SSDegMinSec dec ( dir.lat );
        double dist = pStar->getDistance() / coords.kAUPerParsec;
        float mag = pStar->getMagnitude();

        string name = pStar->getName ( 0 );
        if ( name.empty() )
            name = pStar->getIdentifiers()[0].toString();

        cout << name << ":" << endl;
        cout << "RA:   " << ra.toString() << endl;
        cout << "Dec:  " << dec.toString() << endl;
        cout << "Con:  " << con << endl;
        cout << "Dist: " << format ( "%.3f pc", dist ) << endl;
        cout << "Mag:  " << format ( "%+.2f", mag ) << endl << endl;
    }
}

void TestELPMPP02 ( const string &datadir )
{
    ELPMPP02 elp;

#if ! ELPMPP02_EMBED_SERIES
    elp.readSeries ( datadir );
    cout << "Testing ELPMPP02 using series read from " << datadir << endl;
#else
    cout << "Testing ELPMPP02 using embedded series." << endl;
#endif

    double testjd[5] = { 2444239.5, 2446239.5, 2448239.5, 2450239.5, 2452239.5 };

    for ( int i = 0; i < 5; i++ )
    {
        SSVector pos, vel;
        double jed = testjd[i];
        cout << format ( "JD %.1f", jed ) << endl;
        elp.computePositionVelocity ( jed, pos, vel );
        pos *= SSCoordinates::kKmPerAU;
        vel *= SSCoordinates::kKmPerAU;
        cout << format ( "moon pos: %+13.5f  %+13.5f  %+13.5f km  ", pos.x, pos.y, pos.z );
        cout << format ( "vel: %+13.5f  %+13.5f  %+13.5f km/day", vel.x, vel.y, vel.z ) << endl;
    }

    cout << endl;
}

void TestVSOP2013 ( const string &datadir )
{
    VSOP2013 vsop2013;

#if ! VSOP2013_EMBED_SERIES
    vsop2013.readFile ( datadir + "VSOP2013p1.dat", kMercury );
    vsop2013.readFile ( datadir + "VSOP2013p2.dat", kVenus );
    vsop2013.readFile ( datadir + "VSOP2013p3.dat", kEarth );
    vsop2013.readFile ( datadir + "VSOP2013p4.dat", kMars );
    vsop2013.readFile ( datadir + "VSOP2013p5.dat", kJupiter );
    vsop2013.readFile ( datadir + "VSOP2013p6.dat", kSaturn );
    vsop2013.readFile ( datadir + "VSOP2013p7.dat", kUranus );
    vsop2013.readFile ( datadir + "VSOP2013p8.dat", kNeptune );
    vsop2013.readFile ( datadir + "VSOP2013p9.dat", kPluto );
    cout << "Testing VSOP2013 using series read from " << datadir << endl;
#else
    cout << "Testing VSOP2013 using embedded series." << endl;
#endif

    // These Julian Ephemeris Dates match the test dates in the test cases provided with VSOP2013.

    for ( double jed = 2411545.0; jed <= 2491545.0; jed += 40000.0 )
    {
        SSVector pos, vel;

        cout << format ( "JD %.1f\n", jed );
        for ( int iplanet = 1; iplanet <= 9; iplanet++ )
        {
            vsop2013.computePositionVelocity ( iplanet, jed, pos, vel );
            cout << format ( "planet %d pos: %+14.10f  %+14.10f  %+14.10f AU  ", iplanet, pos.x, pos.y, pos.z );
            cout << format ( "vel: %+13.10f  %+13.10f  %+13.10f AU/day", vel.x, vel.y, vel.z ) << endl;
        }
    }

    cout << endl;
}

void TestPrecession ( void )
{
    SSMatrix p = SSCoordinates::getPrecessionMatrix ( 1219339.078000 );

    printf ( "%+.12f %+.12f %+.12f\n", p.m00, p.m01, p.m02 );
    printf ( "%+.12f %+.12f %+.12f\n", p.m10, p.m11, p.m12 );
    printf ( "%+.12f %+.12f %+.12f\n", p.m20, p.m21, p.m22 );
}

// Android redirects stdout & stderr output to /dev/null. This uses Android logging functions to send
// output to logcat. From https://stackoverflow.com/questions/8870174/is-stdcout-usable-in-android-ndk

#ifdef ANDROID

#include <android/log.h>

#define LOG_TAG "sstest"

class androidbuf : public std::streambuf
{
public:
    enum { bufsize = 128 }; // ... or some other suitable buffer size
    androidbuf() { this->setp(buffer, buffer + bufsize - 1); }

private:
    int overflow(int c)
    {
        if (c == traits_type::eof()) {
            *this->pptr() = traits_type::to_char_type(c);
            this->sbumpc();
        }
        return this->sync()? traits_type::eof(): traits_type::not_eof(c);
    }

    int sync()
    {
        int rc = 0;
        if (this->pbase() != this->pptr()) {
            char writebuf[bufsize+1];
            memcpy(writebuf, this->pbase(), this->pptr() - this->pbase());
            writebuf[this->pptr() - this->pbase()] = '\0';

            rc = __android_log_write(ANDROID_LOG_INFO, LOG_TAG, writebuf) > 0;
            this->setp(buffer, buffer + bufsize - 1);
        }
        return rc;
    }

    char buffer[bufsize];
};

#endif

#if TARGET_OS_IOS || defined ( ANDROID )
#include "SSTest.h"

#define main(argc,argv) _main(argc,argv)

int _main ( int argc, const char *argv[] );

int SSTestMain ( const char *inputpath, const char *outputpath )
{
    const char *argv[] = { "SSTest", inputpath, outputpath };

#ifdef ANDROID
    std::cout.rdbuf ( new androidbuf );
#endif

//    return _main ( 3, argv );
}

#endif

void ExportObjectsToHTM ( const string htmdir, SSObjectVec &objects );

/**
int main ( int argc, const char *argv[] )
{
// This bit of magic makes UTF-8 output with degree characters appear correctly on the Windows console;
// see https://www.codeproject.com/Articles/34068/Unicode-Output-to-the-Windows-Console

#ifdef _WIN32
    UINT oldcp = GetConsoleOutputCP();
    SetConsoleOutputCP ( CP_UTF8 );
#endif

    TestTime();

    if ( argc < 3 )
    {
        cout << "Usage: SSTest <inpath> <outpath>" << endl;
        cout << "inpath: path to SSData directory" << endl;
        cout << "outpath: path to output directory" << endl;
        exit ( -1 );
    }

    string inpath ( argv[1] );
    string outpath ( argv[2] );


    TestELPMPP02 ( "/Users/timmyd/Projects/SouthernStars/Projects/Astro Code/ELPMPP02/Chapront/" );
    TestVSOP2013 ( "/Users/timmyd/Projects/SouthernStars/Projects/Astro Code/VSOP2013/solution/" );
    TestEphemeris ( inpath, outpath );
//  TestPrecession();
//  TestSatellites ( inpath, outpath );
//  TestJPLDEphemeris ( inpath );
//  TestSolarSystem ( inpath, outpath );
//  TestConstellations ( inpath, outpath );
    TestStars ( inpath, outpath );
    TestDeepSky ( inpath, outpath );
/*
    SSObjectVec comets;
    int numcom = importMPCComets ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Comets/MPC/CometEls.txt", comets );
    cout << "Imported " << numcom << " MPC comets" << endl;

    SSObjectVec asteroids;
    int numast = importMPCAsteroids ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Asteroids/MPCORB/MPCORB.DAT", asteroids );
    cout << "Imported " << numast << " MPC asteroids" << endl;

    SSIdentifierNameMap ngcicNameMap;
    SSImportIdentifierNameMap ( "/Users/timmyd/Projects/SouthernStars/Projects/SSCore/CSVData/DeepSky/Names.csv", ngcicNameMap );

    SSObjectVec clusters;
    int numClus = SSImportDAML02 ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Open Clusters/Dias 2016/clusters.txt", ngcicNameMap, clusters );
    cout << "Imported " << numClus << " open clusters" << endl;
    exportCatalog ( clusters );

    SSObjectVec globulars;
    int numGlobs = SSImportMWGC ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Globular Clusters/Harris 2010/mwgc.original.txt", ngcicNameMap, globulars );
    cout << "Imported " << numGlobs << " globular clusters" << endl;
    exportCatalog ( globulars );

    SSObjectVec planNebs;
    int numPlanNebs = SSImportPNG ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Planetary Nebulae/Strasbourg-ESO 1992/main.dat",
                                   "/Users/timmyd/Projects/SouthernStars/Catalogs/Planetary Nebulae/Strasbourg-ESO 1992/dist.dat",
                                   "/Users/timmyd/Projects/SouthernStars/Catalogs/Planetary Nebulae/Strasbourg-ESO 1992/diam.dat",
                                   "/Users/timmyd/Projects/SouthernStars/Catalogs/Planetary Nebulae/Strasbourg-ESO 1992/vel.dat", ngcicNameMap, planNebs );
    cout << "Imported " << numPlanNebs << " planetary nebulae" << endl;
    exportCatalog ( planNebs );

    int numobj = SSImportNGCIC ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Revised NGC-IC 2019/NI2019.txt", ngcicNameMap, clusters, globulars, planNebs, objects );
    cout << "Imported " << numobj << " NGC-IC objects" << endl;
    exportCatalog ( objects );

    exportCatalog ( objects, kCatMessier, 1, 110 );
    exportCatalog ( objects, kCatCaldwell, 1, 110 );
*/
/*
    SSIdentifierMap hipHRMap;
    int n = SSImportHIPHRIdentifiers ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Hipparcos/TABLES/IDENT3.DOC", hipHRMap );
    cout << "Imported " << n << " Hipparcos HR identifiers." << endl;

    SSIdentifierMap hipBayMap;
    n = SSImportHIPBayerIdentifiers ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Hipparcos/TABLES/IDENT4.DOC", hipBayMap );
    cout << "Imported " << n << " Hipparcos Bayer/Flamsteed identifiers." << endl;

    SSIdentifierMap hipGCVSMap;
    n = SSImportHIPGCVSIdentifiers ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Hipparcos/TABLES/IDENT5.DOC", hipGCVSMap );
    cout << "Imported " << n << " Hipparcos GCVS identifiers." << endl;

    SSIdentifierNameMap hipNames;
    n = SSImportHIPNames ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Hipparcos/TABLES/IDENT6.DOC", hipNames );
    cout << "Imported " << n << " Hipparcos star names." << endl;

    SSObjectVec hicStars;
    n = SSImportHIC ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Hipparcos Input Catalog/main.dat", hicStars );
    cout << "Imported " << n << " Hipparcos Input Catalog stars." << endl;

    SSObjectVec hip2Stars;
//  n = SSImportHIP2 ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Hipparcos New Reduction 2007/hip2.dat", hip2Stars );
//  cout << "Imported " << n << " Hipparcos New Reduction stars." << endl;

    SSObjectVec hipStars;
    n = SSImportHIP ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Hipparcos/CATS/HIP_MAIN.DAT", hipHRMap, hipBayMap, hipGCVSMap, hipNames, hicStars, hip2Stars, hipStars );
    cout << "Imported " << n << " Hipparcos stars." << endl;

    SSIdentifierNameMap iauNames;
    n = SSImportIAUStarNames ( "/Users/timmyd/Projects/SouthernStars/Projects/Star Names/IAU-CSN.txt", iauNames );
    cout << "Imported " << n << " IAU star names." << endl;

    SSIdentifierNameMap starNames;
    n = SSImportIdentifierNameMap ( "/Users/timmyd/Projects/SouthernStars/Projects/SSCore/SSData/Stars/Names.csv", starNames );
    cout << "Imported " << n << " star names." << endl;

    SSObjectVec gjACStars;
    n = SSImportGJAC ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Nearby Stars/Accurate Coordinates/table1.dat", hipStars, gjACStars );
    cout << "Imported " << n << " GJ accurate coordinate nearby stars." << endl;
//    exportCatalog ( gjACStars );

    SSObjectVec gjStars;
    n = SSImportGJCNS3 ( "/Users/timmyd/Projects/SouthernStars/Catalogs/Nearby Stars/CNS3/catalog.dat", starNames, gjACStars, gjStars );
    cout << "Imported " << n << " GJ nearby stars." << endl;
//    exportCatalog ( gjStars );

    SSObjectVec skyStars;
    n = SSImportSKY2000 ( "/Users/timmyd/Projects/SouthernStars/Catalogs/SKY2000 Master Star Catalog/ATT_sky2kv5.cat", iauNames, hipStars, gjStars, skyStars );
    cout << "Imported " << n << " SKY2000 stars." << endl;
    // exportCatalog ( skyStars, kCatHR, 1, 9110 );
    // SSExportObjectsToCSV ( "/Users/timmyd/Desktop/SKY2000.csv", skyStars );
    ExportObjectsToHTM ( "/Users/timmyd/Desktop/SKY2000/", skyStars );


#ifdef _WIN32
    SetConsoleOutputCP ( oldcp );
#endif

    return 0;
}
**/

#include "SSHTM.hpp"

void ExportObjectsToHTM ( const string htmdir, SSObjectVec &objects )
{
    vector<float> maglevels = { 6.0, 7.2, 8.4, INFINITY };
    SSHTM htm ( maglevels, htmdir );

    cout << "Stored " << htm.store ( objects ) << " stars." << endl;
    cout << "HTM has " << htm.countRegions() << " regions and " << htm.countStars() << " stars." << endl;
    int n = htm.saveRegions();
    cout << "Exported " << n << " objects." << endl;

    // Finally empty the original object vector

    objects.clear();

    // Now try dumping and reloading regions

    htm.dumpRegions();
    n = htm.loadRegions();
    cout << "HTM has " << htm.countRegions() << " regions and " << htm.countStars() << " stars." << endl;
}

#include <QTimer>

////////////
/// \brief MainWindow::MainWindow
/// \param parent
///
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
#ifdef Q_OS_WIN
    QString winApiPath = getWin32SpecialDirPath(CSIDL_APPDATA);
    if (!winApiPath.isEmpty())
    {
        userDir = winApiPath + "\\Stellarium";
    }
#elif defined(Q_OS_MAC)
    userDir = QDir::homePath() + "/Library/Application Support/Stellarium";
#else
    QStringList perms;
        perms << "android.permission.WRITE_EXTERNAL_STORAGE" << "android.permission.ACCESS_FINE_LOCATION" << "android.permission.READ_EXTERNAL_STORAGE" << "android.permission.ACCESS_COARSE_LOCATION";
        QtAndroid::PermissionResultMap checkPerms = QtAndroid::requestPermissionsSync(perms);
        QHash<QString, QtAndroid::PermissionResult>::iterator i;
        for (i = checkPerms.begin(); i != checkPerms.end(); i++)
        qDebug() << i.key() << ": " << (i.value() == QtAndroid::PermissionResult::Granted);
#endif

    ui->setupUi(this);

    //runtime
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateUI()));
    timer->start(1);

#ifdef Q_OS_ANDROID
    ui->lineEdit->setText(QDir::homePath()+"/SSData");
    ui->lineEdit_2->setText(QDir::homePath()+"");
#endif

//stelsel

    QStringList fileLocations;
    QString cuserDir = QString::fromLocal8Bit(qgetenv("EXTERNAL_STORAGE")) + "/stellarium";
    if (!QFile(cuserDir).exists())
        {
        qWarning() << "User config directory does not exist: " << QDir::toNativeSeparators(cuserDir);
        ui->textBrowser_2->append("User config directory does not exist\r\n");
        }
        try
        {
        makeSureDirExistsAndIsWritable(cuserDir);
        fileLocations.append(cuserDir);  // Higher priority than default dir
        qDebug() << "User config directory " << QDir::toNativeSeparators(cuserDir) << " added to list.";
        }
        catch (std::runtime_error &e)
        {
        ui->textBrowser_2->append("Cannot write to SD card, will not add custom user directory\r\n");
        //ui->textBrowser->append("\r\nCannot write to SD card, will not add custom user directory\r\n");
        qWarning() << "Cannot write to SD card, will not add custom user directory";
        }
    conf = new QSettings(cuserDir+"/config.ini", QSettings::IniFormat);

    ui->comboBox_2->addItem(conf->value("astro/labels_amount").toString());
    ui->comboBox_3->addItem(conf->value("astro/nebula_hints_amount").toString());

    ui->comboBox_2->addItem("0.5");
    ui->comboBox_2->addItem("1");
    ui->comboBox_2->addItem("1.5");
    ui->comboBox_2->addItem("2");
    ui->comboBox_2->addItem("2.5");
    ui->comboBox_2->addItem("3");
    ui->comboBox_2->addItem("3.5");
    ui->comboBox_2->addItem("4");
    ui->comboBox_2->addItem("4.5");
    ui->comboBox_2->addItem("5");
    ui->comboBox_2->addItem("5.5");
    ui->comboBox_2->addItem("6");
    ui->comboBox_2->addItem("6.5");
    ui->comboBox_2->addItem("7");
    ui->comboBox_2->addItem("7.5");
    ui->comboBox_2->addItem("8");
    ui->comboBox_2->addItem("8.5");
    ui->comboBox_2->addItem("9");
    ui->comboBox_2->addItem("9.5");
    ui->comboBox_2->addItem("10");

    ui->comboBox_3->addItem("0.5");
    ui->comboBox_3->addItem("1");
    ui->comboBox_3->addItem("1.5");
    ui->comboBox_3->addItem("2");
    ui->comboBox_3->addItem("2.5");
    ui->comboBox_3->addItem("3");
    ui->comboBox_3->addItem("3.5");
    ui->comboBox_3->addItem("4");
    ui->comboBox_3->addItem("4.5");
    ui->comboBox_3->addItem("5");
    ui->comboBox_3->addItem("5.5");
    ui->comboBox_3->addItem("6");
    ui->comboBox_3->addItem("6.5");
    ui->comboBox_3->addItem("7");
    ui->comboBox_3->addItem("7.5");
    ui->comboBox_3->addItem("8");
    ui->comboBox_3->addItem("8.5");
    ui->comboBox_3->addItem("9");
    ui->comboBox_3->addItem("9.5");
    ui->comboBox_3->addItem("10");


//    QFile loadFile(cuserDir+"/data.json");
    QFile file(cuserDir+"/data.json");
    qDebug()<<"data.json"<<endl;
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
    qDebug()<<"Can't open the file!"<<endl;
    }
    ui->comboBox->clear();
    while(!file.atEnd())
    {
        QByteArray line = file.readLine().replace("\n","");
        QString str(line);
        QString Name1 = str.split("\t")[0];
        QString Name2 = str.split("\t")[1];
        qDebug()<< Name1<<":"<<Name2;
        ui->comboBox->addItem(Name1);
        map_nebula[Name1] = Name2;

        if (Name2 +".dat" == conf->value("astro/nebula_data_file").toString()){
            ui->textBrowser_2->append("Now the data file: "+Name1+"\n");
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateUI(){
    QCoreApplication::processEvents();
    ui->lineEdit_3->setText(QTime::currentTime().toString("hh:mm:ss.zzz"));
}

void MainWindow::on_pushButton_clicked()
{
    string inpath = ui->lineEdit->text().toStdString();
    string outpath = ui->lineEdit_2->text().toStdString();
    TestDeepSky(inpath, outpath);
}

void MainWindow::on_pushButton_2_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    ui->lineEdit->setText(dir);
}

void MainWindow::on_pushButton_3_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    ui->lineEdit_2->setText(dir);
}


void MainWindow::on_pushButton_4_clicked()
{
    string inpath = ui->lineEdit->text().toStdString();
    string outpath = ui->lineEdit_2->text().toStdString();
    TestStars (inpath, outpath);
}

void MainWindow::on_pushButton_5_clicked()
{
    string inpath = ui->lineEdit->text().toStdString();
    string outpath = ui->lineEdit_2->text().toStdString();
    TestEphemeris ( inpath, outpath );
}

void MainWindow::on_pushButton_6_clicked()
{
    string inpath = ui->lineEdit->text().toStdString();
    string outpath = ui->lineEdit_2->text().toStdString();
    TestSatellites ( inpath, outpath );
}

void MainWindow::on_pushButton_7_clicked()
{
    string inpath = ui->lineEdit->text().toStdString();
    string outpath = ui->lineEdit_2->text().toStdString();
    TestSolarSystem( inpath, outpath );
}

void MainWindow::on_pushButton_8_clicked()
{
    string inpath = ui->lineEdit->text().toStdString();
    string outpath = ui->lineEdit_2->text().toStdString();
    TestConstellations( inpath, outpath );
}

void MainWindow::on_pushButton_9_clicked()
{
    string inpath = ui->lineEdit->text().toStdString();
    string outpath = ui->lineEdit_2->text().toStdString();
    TestJPLDEphemeris( inpath);
}

void MainWindow::on_pushButton_10_clicked()
{
    string inpath = ui->lineEdit->text().toStdString();
    string outpath = ui->lineEdit_2->text().toStdString();
    TestELPMPP02( inpath);
}

void MainWindow::on_pushButton_11_clicked()
{
    string inpath = ui->lineEdit->text().toStdString();
    string outpath = ui->lineEdit_2->text().toStdString();
    TestVSOP2013( inpath);
}

void MainWindow::on_pushButton_12_clicked()
{
    TestPrecession();
}

void MainWindow::on_pushButton_13_clicked()
{
    TestTime();
}

//排列判断
bool compareVMagData(SSPlanet * &p1, SSPlanet * &p2)
{
    if (p1->getMagnitude() < p2->getMagnitude())
    {
        return true;
    }
    return false;
}

void TestCometsList( string inputDir, string outputDir )
{
    SSObjectVec solsys;

//    SSPlanet::useVSOPELP ( false );
//    string ephemFile = inputDir + "/SolarSystem/DE438/1950_2050.438";
//    if ( SSJPLDEphemeris::open ( ephemFile ) )
//        cout << "Successfully opened JPL DE438 ephemeris file." << endl;
//    else
//        cout << "Failed to open JPL DE438 ephemeris file " << ephemFile << endl;

    int numComets = SSImportMPCComets ( inputDir + "/SolarSystem/cometelements", solsys );

//    cout << "Imported " << solsys.size() << " solar system objects." << endl;

//    int nsat = SSImportSatellitesFromTLE ( inputDir + "/SolarSystem/Satellites/visual.txt", solsys );
//    cout << "Imported " << nsat << " artificial satellites." << endl;

//    int nnames = SSImportMcNames ( inputDir + "/SolarSystem/Satellites/mcnames.txt", solsys );
//    cout << "Imported " << nnames << " McCants satellite names." << endl;

//    SatFreqMap satfreqs;
//    int nfreqs = SSImportSatelliteFrequencyData ( inputDir + "/SolarSystem/Satellites/je9pel.csv", satfreqs );
//    cout << "Imported " << nfreqs << " JE9PEL satellite radio frequencies." << endl << endl;

    SSDate date ( kGregorian, 0.0, 2020, 4, 15.0, 0, 0, 0.0 );
    SSTime now = SSTime::fromSystem(); // SSTime ( date );
//    now -= 123;

    date = SSDate ( now );
    SSSpherical here = { SSAngle ( SSDegMinSec ( '+', 110, 0, 0 ) ), SSAngle ( SSDegMinSec ( '+', 21, 0, 0 ) ), 0.026 };
    SSCoordinates coords ( now, here );

    cout << format ( "Test Date: %04d/%02hd/%02hd", date.year, date.month, date.day ) << endl;
    cout << format ( "Test Time: %02hd:%02hd:%04.1f", date.hour, date.min, date.sec ) << endl;
    cout << format ( "Test Zone: %+.1f hours", date.zone ) << endl;
    cout << format ( "Test Longitude: %s", SSDegMinSec ( here.lon ).toString().c_str() ) << endl;
    cout << format ( "Test Latitude:  %s", SSDegMinSec ( here.lat ).toString().c_str() ) << endl;
    cout << format ( "Test Altitude:  %.0f m", here.rad * 1000.0 ) << endl << endl;

//    ui->textBrowser->append(QString::fromStdString(format ( "Test Date: %04d/%02hd/%02hd", date.year, date.month, date.day )));

    coords.setAberration ( true );
    coords.setLightTime ( true );

//    TestEvents ( coords, solsys );

    // Compute and print ephemeris of solar system objects.

    coords.setTime ( now );

    QList<SSPlanet*> cometslist;

    for ( int i = 0; i < solsys.size(); i++ )
    {
        SSPlanet *p = SSGetPlanetPtr ( solsys[i] );
        if ( p == nullptr )
            continue;

//        // Skip everything except the first 34 solar system objects and the ISS.

//        bool skip = i < 34 ? false : true;
//        if ( p->getType() == kTypeSatellite )
//            if ( p->getIdentifier() == SSIdentifier ( kCatNORADSat, 25544 ) )
//                skip = false;

//        if ( skip && p->getType() != kTypeComet)
//            continue;

        p->computeEphemeris ( coords );
        string con = SSConstellation::identify ( p->getDirection() );
        SSSpherical dir ( p->getDirection() );
        dir = coords.transform ( kFundamental, kEquatorial, dir );
        SSHourMinSec ra ( dir.lon );
        SSDegMinSec dec ( dir.lat );
        double dist = p->getDistance();
        float mag = p->getMagnitude();
        if (mag<99)
            cometslist.append(p);

//        cout << p->getName ( 0 ) << ":" << endl;
//        cout << "RA:   " << ra.format ( " %02hdh %02hdm %05.2fs" ) << endl;
//        cout << "Dec:  " << dec.format ( "%c%02hd° %02hd' %04.1f\"" ) << endl;
//        cout << "Con:  " << con << endl;
//        if ( dist > 0.1 )
//            cout << "Dist: " << format ( "%.6f AU", dist ) << endl;
//        else
//            cout << "Dist: " << format ( "%.0f km", dist * coords.kKmPerAU ) << endl;
//        cout << "Mag:  " << format ( "%+.2f", mag ) << endl;
//        cout << "Illum: " << format ( "%.1f%%", p->illumination() * 100.0 ) << endl;

//        cout << endl;
    }
    qSort(cometslist.begin(), cometslist.end(), compareVMagData);
    for (int i = 0; i < cometslist.size(); i++){
        SSPlanet *p = SSGetPlanetPtr ( cometslist[i] );
        p->computeEphemeris ( coords );
        string con = SSConstellation::identify ( p->getDirection() );
        SSSpherical dir ( p->getDirection() );
        dir = coords.transform ( kFundamental, kEquatorial, dir );
        SSHourMinSec ra ( dir.lon );
        SSDegMinSec dec ( dir.lat );
        double dist = p->getDistance();
        float mag = p->getMagnitude();

        QList<QString> names;
        int lsz = p->getNames().size();
        for (int i = 0; i < lsz; i++){
            names.push_back(QString::fromStdString(p->getNames()[i]));
        }
//        qDebug() << names;
//        qDebug() << QString::fromStdString(p->getName ( 0 )) << ":" << QString::fromStdString(format ( "%+.2f", mag )) << endl;
//        qDebug()<<"===";


//                cout << p->getName ( 0 ) << ":" << endl;
//                cout << "RA:   " << ra.format ( " %02hdh %02hdm %05.2fs" ) << endl;
//                cout << "Dec:  " << dec.format ( "%c%02hd° %02hd' %04.1f\"" ) << endl;
//                cout << "Con:  " << con << endl;
//                if ( dist > 0.1 )
//                    cout << "Dist: " << format ( "%.6f AU", dist ) << endl;
//                else
//                    cout << "Dist: " << format ( "%.0f km", dist * coords.kKmPerAU ) << endl;
//                cout << "Mag:  " << format ( "%+.2f", mag ) << endl;
    }
}


void MainWindow::on_pushButton_14_clicked()
{
    string inpath = ui->lineEdit->text().toStdString();
    string outpath = ui->lineEdit_2->text().toStdString();
    TestCometsList( inpath, outpath);
}



void MainWindow::CometsListShow( string inputDir, string outputDir )
{
    SSObjectVec solsys;

    int numComets = SSImportMPCComets ( inputDir + "/SolarSystem/cometelements", solsys );

    SSDate date ( kGregorian, 0.0, 2020, 4, 15.0, 0, 0, 0.0 );
    SSTime now = SSTime::fromSystem(); // SSTime ( date );
    date = SSDate ( now );
    SSSpherical here = { SSAngle ( SSDegMinSec ( '+', 110, 0, 0 ) ), SSAngle ( SSDegMinSec ( '+', 21, 0, 0 ) ), 0.026 };
    SSCoordinates coords ( now, here );

    cout << format ( "Test Date: %04d/%02hd/%02hd", date.year, date.month, date.day ) << endl;
    cout << format ( "Test Time: %02hd:%02hd:%04.1f", date.hour, date.min, date.sec ) << endl;
    cout << format ( "Test Zone: %+.1f hours", date.zone ) << endl;
    cout << format ( "Test Longitude: %s", SSDegMinSec ( here.lon ).toString().c_str() ) << endl;
    cout << format ( "Test Latitude:  %s", SSDegMinSec ( here.lat ).toString().c_str() ) << endl;
    cout << format ( "Test Altitude:  %.0f m", here.rad * 1000.0 ) << endl << endl;
    coords.setAberration ( true );
    coords.setLightTime ( true );

    // Compute and print ephemeris of solar system objects.

    coords.setTime ( now );

    QList<SSPlanet*> cometslist;

    for ( int i = 0; i < solsys.size(); i++ )
    {
        SSPlanet *p = SSGetPlanetPtr ( solsys[i] );
        if ( p == nullptr )
            continue;
        p->computeEphemeris ( coords );
//        string con = SSConstellation::identify ( p->getDirection() );
//        SSSpherical dir ( p->getDirection() );
//        dir = coords.transform ( kFundamental, kEquatorial, dir );
//        SSHourMinSec ra ( dir.lon );
//        SSDegMinSec dec ( dir.lat );
//        double dist = p->getDistance();
        float mag = p->getMagnitude();
        if (mag<99)
            cometslist.append(p);
    }
    qSort(cometslist.begin(), cometslist.end(), compareVMagData);

    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"Comet"<<"Mag"<<"con"<<"dist"<<"ra"<<"dec");
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //禁止修改
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);  //设置为可以选中单个
    ui->tableWidget->verticalHeader()->setVisible(false);   //隐藏列表头

    for (int i = 0; i < cometslist.size(); i++){
        SSPlanet *p = SSGetPlanetPtr ( cometslist[i] );
        p->computeEphemeris ( coords );
        string con = SSConstellation::identify ( p->getDirection() );
        SSSpherical dir ( p->getDirection() );
        dir = coords.transform ( kFundamental, kEquatorial, dir );
        SSHourMinSec ra ( dir.lon );
        SSDegMinSec dec ( dir.lat );
        double dist = p->getDistance();
        float mag = p->getMagnitude();

        QList<QString> names;
        int lsz = p->getNames().size();
        for (int i = 0; i < lsz; i++){
            names.push_back(QString::fromStdString(p->getNames()[i]));
        }

        //insert
        if(mag<15){
        ui->tableWidget->setRowCount(i+1);
//        ui->tableWidget->setItem(i,0,new QTableWidgetItem(i+1));
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::fromStdString(p->getName ( 0 ))));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(QString::fromStdString(format ( "%+.2f", mag ))));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(QString::fromStdString(con)));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(QString("%1").arg(dist)));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(QString::fromStdString(ra.toString())));
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(QString::fromStdString(dec.toString())));
        }

//        qDebug() << names;
//        qDebug() << QString::fromStdString(p->getName ( 0 )) << ":" << QString::fromStdString(format ( "%+.2f", mag )) << endl;
//        qDebug()<<"===";
    }
}


void MainWindow::on_pushButton_15_clicked()
{
////    string inpath = ui->lineEdit->text().toStdString();
////    string outpath = ui->lineEdit_2->text().toStdString();
    string inpath = (QDir::homePath()+"/SSData").toStdString();
    string outpath = (QDir::homePath()+"").toStdString();
//    TestCometsList(inpath, outpath);
    CometsListShow(inpath, outpath);
}

void MainWindow::spendtime(){
    QCoreApplication::processEvents();
    string inpath = ui->lineEdit->text().toStdString();
    string outpath = ui->lineEdit_2->text().toStdString();
    TestDeepSky(inpath, outpath);
    inpath = ui->lineEdit->text().toStdString();
        outpath = ui->lineEdit_2->text().toStdString();
        TestStars (inpath, outpath);
        inpath = ui->lineEdit->text().toStdString();
        outpath = ui->lineEdit_2->text().toStdString();
        TestEphemeris ( inpath, outpath );
        inpath = ui->lineEdit->text().toStdString();
        outpath = ui->lineEdit_2->text().toStdString();
        TestSatellites ( inpath, outpath );
        inpath = ui->lineEdit->text().toStdString();
        outpath = ui->lineEdit_2->text().toStdString();
        TestSolarSystem( inpath, outpath );
        inpath = ui->lineEdit->text().toStdString();
        outpath = ui->lineEdit_2->text().toStdString();
        TestConstellations( inpath, outpath );
        inpath = ui->lineEdit->text().toStdString();
        outpath = ui->lineEdit_2->text().toStdString();
        TestJPLDEphemeris( inpath);
        inpath = ui->lineEdit->text().toStdString();
        outpath = ui->lineEdit_2->text().toStdString();
        TestELPMPP02( inpath);
        inpath = ui->lineEdit->text().toStdString();
        outpath = ui->lineEdit_2->text().toStdString();
        TestVSOP2013( inpath);
        TestPrecession();
        TestTime();
        QTime t;
        t.start();
        while(t.elapsed()<10000){
    //        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);  //**注释1**
//            QCoreApplication::processEvents();  //**注释2**
            }
}

void MainWindow::on_pushButton_16_clicked()
{
    QFuture<void> future = QtConcurrent::run(this,&MainWindow::spendtime);
    while(!future.isFinished())
    {
        QApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
    QDir sourceDir(fromDir);
    QDir targetDir(toDir);
    if(!targetDir.exists()){    /**< 如果目标目录不存在，则进行创建 */
        if(!targetDir.mkdir(targetDir.absolutePath()))
            return false;
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList){
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir()){    /**< 当为目录时，递归的进行copy */
            if(!copyDirectoryFiles(fileInfo.filePath(),
                targetDir.filePath(fileInfo.fileName()),
                coverFileIfExist))
                return false;
        }
        else{            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
            if(coverFileIfExist && targetDir.exists(fileInfo.fileName())){
                targetDir.remove(fileInfo.fileName());
            }

            /// 进行文件copy
            if(!QFile::copy(fileInfo.filePath(),
                targetDir.filePath(fileInfo.fileName()))){
                    return false;
            }
        }
    }
    return true;
}

void MainWindow::on_pushButton_17_clicked()
{
    ui->pushButton_17->setText("Copying...");
    ui->pushButton_15->setDisabled(true);
    QCoreApplication::processEvents();
    bool ok = copyDirectoryFiles("assets:/SSData",QDir::homePath()+"/SSData");
    qDebug()<<"silas:copy:"<<ok;
    if (ok)
        ui->textBrowser->append("silas:copy:ok");
    else
        ui->textBrowser->append("silas:copy:false");
    ui->pushButton_17->setText("Copy");
    ui->pushButton_15->setDisabled(false);
    QCoreApplication::processEvents();

}

void MainWindow::on_pushButton_18_clicked()
{

    QDir targetDir(QDir::homePath()+"/SSData/SolarSystem");
    if(!targetDir.exists()){    /**< 如果目标目录不存在，则进行创建 */
        if(!targetDir.mkdir(targetDir.absolutePath()))
            qDebug()<<"create fail";
    }
    ui->pushButton_18->setText("Downloading...");
    ui->pushButton_15->setDisabled(true);
    QCoreApplication::processEvents();
    NewDownloader *d2 = new NewDownloader();
    d2->startdl("http://astro.vanbuitenen.nl/cometelements?format=mpc&mag=obs",QDir::homePath()+"/SSData/SolarSystem/cometelements");
    qDebug()<<"push fin";
    connect(d2->downloadReply,&QNetworkReply::finished,this,&MainWindow::update);
}

void MainWindow::on_pushButton_19_clicked()
{
    QString strPath = QDir::homePath()+"/SSData/SolarSystem/cometelements";
    if (strPath.isEmpty() || !QDir().exists(strPath))//是否传入了空的路径||路径是否存在
        qDebug()<<"delete"<< false;
    QFileInfo FileInfo(strPath);
    bool suc;
    if (FileInfo.isFile())//如果是文件
        suc = QFile::remove(strPath);
    else if (FileInfo.isDir())//如果是文件夹
    {
        QDir qDir(strPath);
        suc = qDir.removeRecursively();
    }
    qDebug()<<"del success:"<<suc;
}

void MainWindow::on_pushButton_20_clicked()
{
    QDir targetDir(QDir::homePath()+"/SSData/SolarSystem");
    if(!targetDir.exists()){    /**< 如果目标目录不存在，则进行创建 */
        if(!targetDir.mkdir(targetDir.absolutePath()))
            qDebug()<<"create fail";
    }
    ui->pushButton_20->setText("Downloading...");
    ui->pushButton_15->setDisabled(true);
    QCoreApplication::processEvents();

    NewDownloader *d2 = new NewDownloader();
    d2->startdl("https://www.minorplanetcenter.net/iau/MPCORB/CometEls.txt",QDir::homePath()+"/SSData/SolarSystem/cometelements");
    qDebug()<<"push fin";
    connect(d2->downloadReply,&QNetworkReply::finished,this,&MainWindow::update);
}


void MainWindow::update()
{
    ui->pushButton_18->setText("DL: GVB");
    ui->pushButton_20->setText("DL: MPC");
    ui->pushButton_15->setDisabled(false);
    QCoreApplication::processEvents();
}

void MainWindow::on_pushButton_21_clicked()
{
    QDesktopServices::openUrl(QUrl(QLatin1String("http://astro.vanbuitenen.nl/comets")));
}

void MainWindow::on_pushButton_22_clicked()
{
    QDesktopServices::openUrl(QUrl(QLatin1String("https://cobs.si/")));
}



void MainWindow::selector(QString filename,QString namefile){

    conf->setValue("astro/nebula_data_file", filename);
    conf->setValue("astro/nebula_name_file", namefile);
    ui->textBrowser_2->append("set to "+filename+"\r\n");

    if (ui->comboBox_2->currentText()!=conf->value("astro/labels_amount").toString()){
        conf->setValue("astro/labels_amount",ui->comboBox_2->currentText());
    }
    if (ui->comboBox_3->currentText()!=conf->value("astro/nebula_hints_amount").toString()){
        conf->setValue("astro/nebula_hints_amount",ui->comboBox_3->currentText());
    }

//	QFile file1(cuserDir+"/nebulae/default/"+filename);
//	QFile filex1(cuserDir+"/nebulae/default/ngc2000.dat");

//	QFile file2(cuserDir+"/nebulae/default/"+namefile);
//	QFile filex2(cuserDir+"/nebulae/default/ngc2000names.dat");

    QFile::remove(cuserDir+"/nebulae/default/ngc2000.dat");
    QFile::remove(cuserDir+"/nebulae/default/ngc2000names.dat");
    if(QFile::copy(cuserDir+"/nebulae/default/"+filename, cuserDir+"/nebulae/default/ngc2000.dat")){

        qDebug()<<"Copyed filename! "<< endl;
    }
    else{
        qDebug()<<"Can't open the file1! "<< endl;
    }
    if(QFile::copy(cuserDir+"/nebulae/default/"+namefile, cuserDir+"/nebulae/default/ngc2000names.dat")){
        qDebug()<<"Copyed namefile! "<< endl;
    }
    else{
        qDebug()<<"Can't open the file2! "<< endl;
    }


//	if(!file1.open(QIODevice::ReadOnly))
//	{
//	    qDebug()<<"Can't open the file1! "<<file1<< endl;
//	}
//	else if(!filex1.open(QIODevice::WriteOnly)){
//	    qDebug()<<"Can't open the file1x! "<<filex1<< endl;
//	}
//	else{
//		filex1.write(file1.readAll());
//		filex1.close();
//		file1.close();
//		qDebug()<<"Fin file2! "<<file1<< endl;
//	}

//	if(!file2.open(QIODevice::ReadOnly))
//	{
//	    qDebug()<<"Can't open the file2! "<<file1<<endl;
//	}
//	else if(!filex2.open(QIODevice::WriteOnly)){
//	    qDebug()<<"Can't open the file2x! "<<filex2<< endl;
//	}
//	else{
//		filex2.write(file2.readAll());
//		filex2.close();
//		file2.close();
//		qDebug()<<"Fin file2!"<<endl;
//	}
}


void MainWindow::on_pushButton_23_clicked()
{
    QString filename = ui->comboBox->currentText();
    filename = map_nebula[filename];
    if(!ui->radioButton->isChecked())
        filename += "_";
    QString namefile = filename;
    filename += ".dat";
    namefile += "names.dat";
    selector(filename,namefile);

}
