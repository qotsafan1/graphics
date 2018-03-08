// Based on NVIDIA's OptiX implementation of 2009.
// Code modified by Jeppe Revall Frisvad, 2013.


#define SOLUTION_CODE

#include <cmath>
#include "CGLA/Vec4f.h"
#include "CGLA/Vec3f.h"
#include "CGLA/Vec2f.h"
#include "GLGraphics/GLHeader.h"
#include "skymodel.h"
#include <thread>
#include <QTime>

using namespace std;
using namespace CGLA;

const float SKY_TONEMAPPING = 0.025f;

PreethamSunSky::PreethamSunSky(unsigned int day, float time, float latitude, float turbidity, float overcast) :
    _day(day),
    _time(time),
    _latitude(latitude),
    _turbidity(turbidity),
    _overcast(overcast),
    _dirty(true)
{
    Vec2f coord = sun_position(day, time, latitude);
    _sun_theta = coord[0];
    _sun_phi = coord[1];
    _up = Vec3f( 0.0f, 0.0f, 1.0f );
}

DirectionalLight PreethamSunSky::get_directional_light(float scale)
{
    init();
    DirectionalLight l;
    l.direction = _sun_dir;
  // Converting radiance to irradiance at the surface of the Earth using the
  // solid angle 6.74e-5 subtended by the solar disk as seen from Earth.
  // The radiance of a directional source modeling the Sun should be equal
  // to the irradiance at the surface of the Earth.
    l.diffuse  = Vec4f(_sun_color*6.74e-5f*scale,1.0f);
    l.specular = Vec4f(0.05f);
    l.ambient  = Vec4f(0.0f);
    return l;
}


// Given a direction vector v sampled on the hemisphere
// over a surface point with the z-axis as its normal,
// this function applies the same rotation to v as is
// needed to rotate the z-axis to the actual normal
// [Frisvad, Journal of Graphics Tools 16, 2012].
inline void rotate_to_normal(const CGLA::Vec3f& normal, CGLA::Vec3f& v)
{
    if(normal[2] < -0.999999f)
  {
    v = CGLA::Vec3f(-v[1], -v[0], -v[2]);
    return;
  }
  const float a = 1.0f/(1.0f + normal[2]);
  const float b = -normal[0]*normal[1]*a;
  v =   CGLA::Vec3f(1.0f - normal[0]*normal[0]*a, b, -normal[0])*v[0]
      + CGLA::Vec3f(b, 1.0f - normal[1]*normal[1]*a, -normal[1])*v[1]
      + normal*v[2];
}

// Given spherical coordinates, where theta is the
// polar angle and phi is the azimuthal angle, this
// function returns the corresponding direction vector
inline CGLA::Vec3f spherical_direction(double sin_theta, double cos_theta, double phi)
{
  return CGLA::Vec3f(sin_theta*cos(phi), sin_theta*sin(phi), cos_theta);
}

void PreethamSunSky::init()
{
  if( !_dirty ) return;

  _dirty = false;

  _c0 = Vec3f (  0.1787f * _turbidity - 1.4630f,
                -0.0193f * _turbidity - 0.2592f,
                -0.0167f * _turbidity - 0.2608f );

  _c1 = Vec3f ( -0.3554f * _turbidity + 0.4275f,
                -0.0665f * _turbidity + 0.0008f,
                -0.0950f * _turbidity + 0.0092f );

  _c2 = Vec3f ( -0.0227f * _turbidity + 5.3251f,
                -0.0004f * _turbidity + 0.2125f,
                -0.0079f * _turbidity + 0.2102f );

  _c3 = Vec3f (  0.1206f * _turbidity - 2.5771f,
                -0.0641f * _turbidity - 0.8989f,
                -0.0441f * _turbidity - 1.6537f );

  _c4 = Vec3f ( -0.0670f * _turbidity + 0.3703f,
                -0.0033f * _turbidity + 0.0452f,
                -0.0109f * _turbidity + 0.0529f );

  const float sun_theta_2 = _sun_theta * _sun_theta;
  const float sun_theta_3 = sun_theta_2 * _sun_theta;

  const float xi = ( 4.0f / 9.0f - _turbidity / 120.0f ) *
                   ( static_cast<float>( M_PI ) - 2.0f * _sun_theta );

  Vec3f zenith;
  // Preetham paper is in kilocandellas -- we want candellas
  zenith[0] = ( ( 4.0453f * _turbidity - 4.9710f) * tan(xi) - 0.2155f * _turbidity + 2.4192f ) * 1000.0f;
  zenith[1] = _turbidity * _turbidity * ( 0.00166f*sun_theta_3 - 0.00375f*sun_theta_2 + 0.00209f*_sun_theta ) +
                           _turbidity * (-0.02903f*sun_theta_3 + 0.06377f*sun_theta_2 - 0.03202f*_sun_theta + 0.00394f) +
                                        ( 0.11693f*sun_theta_3 - 0.21196f*sun_theta_2 + 0.06052f*_sun_theta + 0.25886f);
  zenith[2] = _turbidity * _turbidity * ( 0.00275f*sun_theta_3 - 0.00610f*sun_theta_2 + 0.00317f*_sun_theta ) +
                           _turbidity * (-0.04214f*sun_theta_3 + 0.08970f*sun_theta_2 - 0.04153f*_sun_theta + 0.00516f) +
                                        ( 0.15346f*sun_theta_3 - 0.26756f*sun_theta_2 + 0.06670f*_sun_theta + 0.26688f);

  float cos_sun_theta = cos( _sun_theta );
  const Vec3f one(1.0f);
  const Vec3f c3_s_th = _c3*_sun_theta;
  Vec3f divisor_Yxy = ( one + _c0 * Vec3f(exp(_c1[0]), exp(_c1[1]), exp(_c1[2])) ) *
                      ( one + _c2 * Vec3f(exp(c3_s_th[0]), exp(c3_s_th[1]), exp(c3_s_th[2])) + _c4 * (cos_sun_theta * cos_sun_theta) );
  _inv_divisor_Yxy  = zenith / divisor_Yxy;

  //
  // Direct sunlight
  //
  _sun_color = sunColor();
  _sun_dir = spherical_direction(sin(_sun_theta), cos_sun_theta, _sun_phi);
  rotate_to_normal(_up, _sun_dir);
}

void PreethamSunSky::precomputeTexture(GLuint & cubetex, int size)
{
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    static const Vec3f CUBEMAP_DIRS[6] = {  Vec3f(1,0,0),  Vec3f(-1,0,0), Vec3f(0,1,0), Vec3f(0,-1,0), Vec3f(0,0,1),  Vec3f(0,0,-1)};
    static const Vec3f UPS[6] = {           Vec3f(0,-1,0), Vec3f(0,-1,0), Vec3f(0,0,1), Vec3f(0,0,-1), Vec3f(0,-1,0), Vec3f(0,-1,0)};
    static const Vec3f BASE[6] = {          Vec3f(0,0,-1), Vec3f(0,0,1),  Vec3f(1,0,0), Vec3f(1,0,0),  Vec3f(1,0,0),  Vec3f(-1,0,0)};

    glGenTextures(1, &cubetex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubetex);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

    vector<vector<Vec3f> *> cubemap(6);

    std::cout << "Generating skymap...";
    std::cout.flush();
    for(int k = 0; k < 6; k++)
    {
        cubemap[k] = new vector<Vec3f>(size*size);
        vector<Vec3f> * image = cubemap[k];

        for(int i = 0; i < size; ++i)
        {
            for(int j = 0; j < size; ++j)
            {
                Vec2f real_coord = 2.0f*Vec2f(i, j)/static_cast<float>(size) - Vec2f(1.0f);
                Vec3f vec_in_cubemap = normalize(CUBEMAP_DIRS[k] + BASE[k]*real_coord[0] + UPS[k]*real_coord[1]);
                float cos_up = dot(vec_in_cubemap, _up);
                if(cos_up > -0.05f)
                    (*image)[j*size + i] = skyColor(vec_in_cubemap, false);
                else
                {
                    if(cos_up > -0.15f)
                        (*image)[j*size + i] = (cos_up + 0.15f)*10.0f*skyColor(vec_in_cubemap, false);
                    else
                        (*image)[j*size + i] = Vec3f(0.0f);
                }
            }
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + k, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, &(image->at(0)));
    }
    std::cout << " Done." << endl << flush;
}

// lambda is wavelength in micro meters
float PreethamSunSky::calculateAbsorption( float , float m, float lambda, float turbidity, float k_o, float k_wa )

{
  float alpha = 1.3f;                             // wavelength exponent
  float beta  = 0.04608f * turbidity - 0.04586f;  // turbidity coefficient
  float ell   = 0.35f;                            // ozone at NTP (cm)
  float w     = 2.0f;                             // precipitable water vapor (cm)

  float rayleigh_air   = exp( -0.008735 * m * pow( lambda, -4.08f ) );
  float aerosol        = exp( -beta * m * pow( lambda, -alpha ) );
  float ozone          = k_o  > 0.0f ? exp( -k_o*ell*m )                                         : 1.0f;
  float water_vapor    = k_wa > 0.0f ? exp( -0.2385*k_wa*w*m/pow( 1.0 + 20.07*k_wa*w*m, 0.45 ) ) : 1.0f;

  return rayleigh_air*aerosol*ozone*water_vapor;
}

Vec3f PreethamSunSky::sunColor()
{
  init();

  const float elevation_angle = 93.885f  - rad2deg( _sun_theta );
  if(elevation_angle < 0.0f)
    return Vec3f(0.0f);

  // optical mass
  const float cos_sun_theta = cos( _sun_theta );
  const float m = 1.0f / ( cos_sun_theta + 0.15f * powf( elevation_angle, -1.253f ) );

  float results[38];
  for( int i = 0; i < 38; ++i ) {
    results[i]  = data[i].sun_spectral_radiance * 10000.0f // convert from 1/cm^2 to 1/m^2;
                                                / 1000.0f; // convert from micrometer to nanometer

    results[i] *= calculateAbsorption( _sun_theta, m, data[i].wavelength, _turbidity, data[i].k_o, data[i].k_wa );
  }

  float X = 0.0f, Y = 0.0f, Z = 0.0f;
  for( int i = 0; i < 38; ++i ) {
    X += results[i]*cie_table[i][1] * 10.0f;
    Y += results[i]*cie_table[i][2] * 10.0f;
    Z += results[i]*cie_table[i][3] * 10.0f;
  }

  return XYZ2rgb(Vec3f(X, Y, Z))*SKY_TONEMAPPING; // return result in W/m^2/sr (using a scale of 0.03 to handle dynamic range)
}


Vec3f PreethamSunSky::skyColor( const Vec3f & direction, bool CEL )
{
  init();

  Vec3f overcast_sky_color = Vec3f( 0.0f );
  Vec3f sunlit_sky_color   = Vec3f( 0.0f );

  // Preetham skylight model
  if( _overcast < 1.0f )
  {
    Vec3f ray_direction = direction;
    if(CEL && dot( ray_direction, _sun_dir ) > 0.999 )
      sunlit_sky_color = _sun_color;
    else
    {
        float dir_dot_up = dot( direction, _up );
        if(abs(dir_dot_up) < 1.0e-16f)
            dir_dot_up = 1.0e-16f;
        float inv_dir_dot_up = 1.f / dir_dot_up;
        if(inv_dir_dot_up < 0.f)
        {
            ray_direction = 2.0*dot(-ray_direction, _up)*_up + ray_direction;
            inv_dir_dot_up = -inv_dir_dot_up;
        }

      float cos_gamma = dot(_sun_dir, ray_direction);
      float gamma = acos(cos_gamma);
      Vec3f A =  _c1 * inv_dir_dot_up;
      Vec3f B =  _c3 * gamma;
      Vec3f color_Yxy = ( Vec3f(1.0f) + _c0*Vec3f( exp(A[0]), exp(A[1]), exp(A[2]) ) ) *
                         ( Vec3f(1.0f) + _c2*Vec3f( exp(B[0]), exp(B[1]), exp(B[2]) ) + _c4*cos_gamma*cos_gamma );
      color_Yxy *= _inv_divisor_Yxy;

      Vec3f color_XYZ = Yxy2XYZ( color_Yxy );
      sunlit_sky_color = XYZ2rgb( color_XYZ );
      sunlit_sky_color *= SKY_TONEMAPPING/683.0f; // return result in W/m^2 (using a scale of 0.03 to handle dynamic range)
    }
  }

  // CIE standard overcast sky model
  float Y =  15.0f;
  overcast_sky_color = Vec3f( ( 1.0f + 2.0f * fabs( dot(direction,_up) ) ) / 3.0f * Y ) * SKY_TONEMAPPING/683.0f;

  // return linear combo of the two
  Vec3f color = sunlit_sky_color*(1.0f - _overcast) + overcast_sky_color*_overcast;
  color[0] = max(0.0f, color[0]);
  color[1] = max(0.0f, color[1]);
  color[2] = max(0.0f, color[2]);
  return color;
}

// Approximations for Earth-Sun distance correction factor and solar declination [Spencer 1971]

void PreethamSunSky::spencer(unsigned int day, double& earth_sun_dist, double& declination)
{
  double day_angle = (day - 1)*2.0*M_PI/365.0;
  double cosA = cos(day_angle);
  double sinA = sin(day_angle);
  double cosA_sqr = cosA*cosA;
  double sinA_sqr = sinA*sinA;
  double cos2A = cosA_sqr - sinA_sqr;
  double sin2A = 2.0*sinA*cosA;
  double cos3A = (cosA_sqr - 3.0*sinA_sqr)*cosA;
  double sin3A = (3.0*cosA_sqr - sinA_sqr)*sinA;
  earth_sun_dist = 1.00011 + 0.034221*cosA + 0.001280*sinA + 0.000719*cos2A + 0.000077*sin2A;
  declination = 0.006918 - 0.399912*cosA + 0.070257*sinA - 0.006758*cos2A + 0.000907*sin2A - 0.002697*cos3A + 0.001480*sin3A;
}

// Common equations for computing the sun position [Preetham et al. 1999, Prein and Gayanilo 1992, Milankovitch 1930]
inline Vec2f PreethamSunSky::solar_elevation(double time, double latitude, double declination)
{
  double hour_angle = (time - 12.0)*M_PI/12.0;
  double cos_hour_angle = cos(hour_angle);
  double sin_hour_angle = sign(hour_angle)*sqrt(1.0 - cos_hour_angle*cos_hour_angle);
  double cos_latitude = cos(latitude);
  double sin_latitude = sin(latitude);
  double cos_declination = cos(declination);
  double sin_declination = sin(declination);
  double cos_zenith = sin_latitude*sin_declination + cos_latitude*cos_declination*cos_hour_angle;
  double phi = atan2(cos_declination*sin_hour_angle, cos_latitude*sin_declination + sin_latitude*cos_declination*cos_hour_angle);
  return Vec2f(acos(cos_zenith), phi);
}

Vec2f PreethamSunSky::sun_position(unsigned int day, double time, double latitude)
{
    double tmp, declination;
    spencer(day, tmp, declination);
    return solar_elevation(time, latitude*M_PI/180.0, declination);
}


const float PreethamSunSky::cie_table[38][4] =
{
  {380.f, 0.0002f, 0.0000f, 0.0007f},
  {390.f, 0.0024f, 0.0003f, 0.0105f},
  {400.f, 0.0191f, 0.0020f, 0.0860f},
  {410.f, 0.0847f, 0.0088f, 0.3894f},
  {420.f, 0.2045f, 0.0214f, 0.9725f},

  {430.f, 0.3147f, 0.0387f, 1.5535f},
  {440.f, 0.3837f, 0.0621f, 1.9673f},
  {450.f, 0.3707f, 0.0895f, 1.9948f},
  {460.f, 0.3023f, 0.1282f, 1.7454f},
  {470.f, 0.1956f, 0.1852f, 1.3176f},

  {480.f, 0.0805f, 0.2536f, 0.7721f},
  {490.f, 0.0162f, 0.3391f, 0.4153f},
  {500.f, 0.0038f, 0.4608f, 0.2185f},
  {510.f, 0.0375f, 0.6067f, 0.1120f},
  {520.f, 0.1177f, 0.7618f, 0.0607f},

  {530.f, 0.2365f, 0.8752f, 0.0305f},
  {540.f, 0.3768f, 0.9620f, 0.0137f},
  {550.f, 0.5298f, 0.9918f, 0.0040f},
  {560.f, 0.7052f, 0.9973f, 0.0000f},
  {570.f, 0.8787f, 0.9556f, 0.0000f},

  {580.f, 1.0142f, 0.8689f, 0.0000f},
  {590.f, 1.1185f, 0.7774f, 0.0000f},
  {600.f, 1.1240f, 0.6583f, 0.0000f},
  {610.f, 1.0305f, 0.5280f, 0.0000f},
  {620.f, 0.8563f, 0.3981f, 0.0000f},

  {630.f, 0.6475f, 0.2835f, 0.0000f},
  {640.f, 0.4316f, 0.1798f, 0.0000f},
  {650.f, 0.2683f, 0.1076f, 0.0000f},
  {660.f, 0.1526f, 0.0603f, 0.0000f},
  {670.f, 0.0813f, 0.0318f, 0.0000f},

  {680.f, 0.0409f, 0.0159f, 0.0000f},
  {690.f, 0.0199f, 0.0077f, 0.0000f},
  {700.f, 0.0096f, 0.0037f, 0.0000f},
  {710.f, 0.0046f, 0.0018f, 0.0000f},
  {720.f, 0.0022f, 0.0008f, 0.0000f},

  {730.f, 0.0010f, 0.0004f, 0.0000f},
  {740.f, 0.0005f, 0.0002f, 0.0000f},
  {750.f, 0.0003f, 0.0001f, 0.0000f}
};

const PreethamSunSky::Datum PreethamSunSky::data[] =
{
  {0.38f, 1655.9f, -1.f, -1.f},
  {0.39f, 1623.37f, -1.f, -1.f},
  {0.4f, 2112.75f, -1.f, -1.f},
  {0.41f, 2588.82f, -1.f, -1.f},
  {0.42f, 2582.91f, -1.f, -1.f},
  {0.43f, 2423.23f, -1.f, -1.f},
  {0.44f, 2676.05f, -1.f, -1.f},
  {0.45f, 2965.83f, 0.003f, -1.f},
  {0.46f, 3054.54f, 0.006f, -1.f},
  {0.47f, 3005.75f, 0.009f, -1.f},

  {0.48f, 3066.37f, 0.014f, -1.f},
  {0.49f, 2883.04f, 0.021f, -1.f},
  {0.5f, 2871.21f, 0.03f, -1.f},
  {0.51f, 2782.5f, 0.04f, -1.f},
  {0.52f, 2710.06f, 0.048f, -1.f},
  {0.53f, 2723.36f, 0.063f, -1.f},
  {0.54f, 2636.13f, 0.075f, -1.f},
  {0.55f, 2550.38f, 0.085f, -1.f},
  {0.56f, 2506.02f, 0.103f, -1.f},
  {0.57f, 2531.16f, 0.12f, -1.f},

  {0.58f, 2535.59f, 0.12f, -1.f},
  {0.59f, 2513.42f, 0.115f, -1.f},
  {0.6f, 2463.15f, 0.125f, -1.f},
  {0.61f, 2417.32f, 0.12f, -1.f},
  {0.62f, 2368.53f, 0.105f, -1.f},
  {0.63f, 2321.21f, 0.09f, -1.f},
  {0.64f, 2282.77f, 0.079f, -1.f},
  {0.65f, 2233.98f, 0.067f, -1.f},
  {0.66f, 2197.02f, 0.057f, -1.f},
  {0.67f, 2152.67f, 0.048f, -1.f},

  {0.68f, 2109.79f, 0.036f, -1.f},
  {0.69f, 2072.83f, 0.028f, 0.028f},
  {0.7f, 2024.04f, 0.023f, 0.023f},
  {0.71f, 1987.08f, 0.018f, 0.018f},
  {0.72f, 1942.72f, 0.014f, 0.014f},
  {0.73f, 1907.24f, 0.011f, 0.011f},
  {0.74f, 1862.89f, 0.01f, 0.01f},
  {0.75f, 1825.92f, 0.009f, 0.009f}
};
