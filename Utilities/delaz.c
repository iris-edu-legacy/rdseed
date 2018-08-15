#include <math.h>

#define OK	0
#define ERR	1

static double pi;
static double pi2;
static double fac = 0.993305621334896;
static double twopi;
static double rd;

/* dmax:  returns the maximum value of two input single precision numbers
 *         as a single precision number
 */

double dmax(arg1, arg2)
double arg1, arg2;
{
    return((arg1 > arg2 ? arg1 : arg2));
}

/* geocen:  converts geographic colatitude (in radians) to geocentric colatitude
 *          (also in radians)
 */

double geocen(in_clat)
double in_clat;
{
    return(pi2-atan(fac*cos(in_clat)/dmax(1.e-30,sin(in_clat))));
}

/* geogrf:  converts geocentric colatitude (in radians) to geographic colatitude
 *          (also in radians)
 */
 
double geogrf(in_clat)
double in_clat;
{
    return(pi2-atan(cos(in_clat)/(fac*dmax(1.e-30,sin(in_clat)))));
}

/*
 *	delaz
 *
 *	delaz(folat,folon,ftlat,ftlon,del,az,baz)
 *		float folat,folon,ftlat,ftlon;
 *		float *del, *az, *baz;
 *
 *   This subroutine will compute the distance, azimuth, and back
 *   azimuth (in degrees), given the latitude and longitude (in degrees)
 *   of an origin point and a target point.  (E+,W-; N+,S-)
 */

int delaz(folat,folon,ftlat,ftlon,del,az,baz)
float folat,folon,ftlat,ftlon;
float *del, *az, *baz;
{
        double olat, olon, tlat, tlon;
	double clat,clon,clar,clor,stho,ctho,ctlat,ctlon;
	double ctlar,ctlor,sth,cth,dph,sdph,cdph,delr;
	double azr,bazr;

        /* convert input floating point values to double precision for
           calculations */

        olat = (double)folat;
        olon = (double)folon;
        tlat = (double)ftlat;
        tlon = (double)ftlon;

        pi = acos(-1.0);
        rd = 180.0/pi;
        pi2 = pi/2.0;
        twopi = 2.0 * pi;
	if (olat < -90.0 || olat > 90.0) return(ERR);
	if (olon < -180.0)
		return(ERR);
	else if(olon > 180.0) {
		olon -= 360.0;
		if(olon > 180.0) return(ERR);
	}
	if (tlat < -90.0 || tlat > 90.0) return(ERR);
	if (tlon < -180.0)
		return(ERR);
	else if(tlon > 180.0) {
		tlon -= 360.0;
		if(tlon > 180.0) return(ERR);
	}

        /* eq coords to geocentric radians */

	clat = 90.0 - olat;
	clon = olon;
	if (clon < 0.0) clon += 360.0;
	clar = geocen(clat/rd);
	clor = clon/rd;
	stho = sin(clar);
	ctho = cos(clar);

        /* sta coords to geocentric radians */

	ctlat = 90.0 - tlat;
	ctlon = tlon;
	if (clon < 0.0) ctlon += 360.0;
	ctlar = geocen(ctlat/rd);
	ctlor = ctlon/rd;
	sth = sin(ctlar);
	cth = cos(ctlar);

        /* now calc distance */

	dph = ctlor - clor;
	sdph = sin(dph);
	cdph = cos(dph);
	delr = acos(stho * sth * cdph + ctho * cth);
	*del = (float)(delr*rd);

	/* computer forward azimuth */

	if (sth == 0.0) azr = 0.0;
	else azr = atan2(sdph,stho*cth/sth-ctho*cdph);
	if (azr < 0.0) azr += twopi;
	*az = (float)(azr*rd);

	/* compute back azimuth */

	if (stho == 0.0) bazr = 0.0;
	else bazr = atan2(-sdph,sth*ctho/stho-cth*cdph);
	if (bazr < 0.0) bazr += twopi;
	*baz = (float)(bazr*rd);
	return(OK);
}

/* midpoint:  a function that calculates the midpoint location (in lat, lon)
 *            along a great circle path from (qlat,qlon) to (slat,slon)
 */
 
int midpoint(fqlat,fqlon,fslat,fslon,bplat,bplon)
float fqlat, fqlon, fslat, fslon;
float *bplat, *bplon;
{
    double qlat, qlon, slat, slon;
    float fdel, faz, fbaz;
    double del, az, baz;
    double t0, t1, p1, sphi, cphi, ctheta;
    double qcolat, qcolon, bpcolat, bpcolon;

    /* convert input floating point values to double precision for
       calculations */

    qlat = (double)fqlat;
    qlon = (double)fqlon;
    slat = (double)fslat;
    slon = (double)fslon;

    /* convert input event location into colatitude and colongitude */

    if (qlat < -90.0 || qlat > 90.0) return(ERR);
    if (qlon < -180.0)
        return(ERR);
    else if(qlon > 180.0) {
        qcolon = qlon - 360.0;
        if(qcolon > 180.0) return(ERR);
    }
    else
        qcolon = qlon;

    qcolat = 90.0 - qlat;

    /* calculate the distance and azimuth from the event to the station */

    delaz(fqlat,fqlon,fslat,fslon,&fdel,&faz,&fbaz);

    /* assume distance to bounce point is one half the distance from the event
       to the station. Convert this distance and the azimuth to radians */

    del = (double)fdel/(2.0*rd);
    az = (double)faz/rd;
    
    /* convert qcolat to geocentric coordinates (rather than geographic coords) */

    t0  = geocen(qcolat/rd);

    /* and determine the point that is del radians away from the event location in
       geocentric coords */

    ctheta = sin(del)*sin(t0)*cos(az) + cos(t0)*cos(del);
    t1 = acos(ctheta);
    if(t0 == 0.0)
        p1 = az;
    else if(t1 == 0.0)
        p1 = 0.0;
    else {
        sphi = sin(del)*sin(az)/sin(t1);
        cphi = (cos(del) - cos(t0)*ctheta)/(sin(t0)*sin(t1));
        p1   = qcolon + atan2(sphi,cphi)*rd;
    }

    /* convert back from geocentric radians to geographic degrees */

    bpcolat = geogrf(t1)*rd;

    /* the following assumes that p1 is never > 720.0 */

    if(p1 > 360.0)
        p1 -= 360.0;
    if(p1 < 0.0)
        p1 += 360.0;

    bpcolon = p1;

    /* and convert back to lat/lon from colat/colon */

    *bplat = (float)(90.0 - bpcolat);
    if(bpcolon < 0.0)
        *bplon = (float)(bpcolon + 360.0);
    else
        *bplon = (float)bpcolon;

    return(OK);
}
