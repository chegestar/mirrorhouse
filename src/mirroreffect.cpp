/**
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include "mirroreffect.h"

#include <QDebug>
#include <math.h>


/*!
  \class MirrorEffect
  \brief Makes different mirror effect for the camera viewfinder frame data. Used from MyVideoSurface.
*/


/*!
  Constructor.
*/
MirrorEffect::MirrorEffect()
    : m_transMap(0),
      m_selectedTransform(None),
      m_currentTransform(None),
      m_selectedTransformPower(0.0f),
      m_selectedTransformSize(0.0f),
      m_currentTransformPower(0.0f),
      m_currentTransformSize(0.0f),
      m_highQuality(true)
{

}


/*!
  Destructor.
*/
MirrorEffect::~MirrorEffect()
{
    recreateTransformMap(0, 0);

    // Set the source and the target data to zero to prevent unwanted deletion.
    m_sourceProperties.m_data = 0;
    m_targetProperties.m_data = 0;
}


/*!
  Note that the source must be set before calling process().
  The 90degree rotation is customly added to support the harmattan's
  wrongly oriented camera. It's not used for any other purpose.
*/
void MirrorEffect::setSource(unsigned int *data,
                             int width,
                             int height,
                             int pitch,
                             bool rotate90degrees /* = false */,
                             bool flipY /* = false */)
{
    if (m_sourceProperties.m_width * m_sourceProperties.m_height != width * height) {
        // Must be recreated
        m_currentTransform = None;
    }

    if (!rotate90degrees) {
            // When source can be used without the rotation, just directly
            // place the source image's data into the according capsule.
        m_sourceProperties.m_data = data;
        m_sourceProperties.m_width = width;
        m_sourceProperties.m_height = height;
        m_sourceProperties.m_pitch = pitch;
    }
    else {
            // If source image need to be rotated, a temporary memory is
            // required which wil contain the source image's data rotated
            // 90 degrees. The rotation is done manually few lines below.
        if (!m_sourcePropertiesRotated.m_data
                || m_sourcePropertiesRotated.m_width != height
                || m_sourcePropertiesRotated.m_height != width)
        {
                // The temporary rotation buffer size is incorrect,
                // It must be recreated.
            qDebug() << "MirrorEffect::setSource(): Recreating temporary rotated buffer...";
            m_sourcePropertiesRotated.m_width = height;
            m_sourcePropertiesRotated.m_height = width;

            if (m_sourcePropertiesRotated.m_data)
                delete[] m_sourcePropertiesRotated.m_data;

            m_sourcePropertiesRotated.m_data =
                    new unsigned int[height * width];
        }

            // This loop does the manual 90 degree rotation from sourceData, to
            // sourceDataRotated either with Y-flip or without it.
        for (int y = 0; y < height; y++) {
            if (!flipY) {
                unsigned int *t = m_sourcePropertiesRotated.m_data + (height - 1- y);
                unsigned int *s = data + pitch * y;
                unsigned int *s_target = s + width;

                while (s != s_target) {
                    *t = *s;
                    s++;
                    t += m_sourcePropertiesRotated.m_width;
                }
            }
            else {
                unsigned int *t =
                        m_sourcePropertiesRotated.m_data + (height - 1 - y)
                        + m_sourcePropertiesRotated.m_width
                        * (m_sourcePropertiesRotated.m_height - 1);

                unsigned int *s = data + pitch * y;
                unsigned int *s_target = s + width;

                while (s != s_target) {
                    *t = *s;
                    s++;
                    t -= m_sourcePropertiesRotated.m_width;
                }
            }
        }

        m_sourceProperties.m_data = m_sourcePropertiesRotated.m_data;
        m_sourceProperties.m_width = m_sourcePropertiesRotated.m_width;
        m_sourceProperties.m_pitch = m_sourcePropertiesRotated.m_width;
        m_sourceProperties.m_height = m_sourcePropertiesRotated.m_height;
    }
}


/*!
  Straight forward setter function which places the target image's attributes
  into the according capsule targetImage of the MirrorEffect - class.

  Note that the target must be set before calling process().

*/
void MirrorEffect::setTarget(unsigned int *data, int width, int height, int pitch)
{
    if (m_targetProperties.m_width != width
            || m_targetProperties.m_height != height)
    {
        qDebug() << "MirrorEffect::setTarget(): Target dimensions changed to"
                 << width << "x" << height;

        // Release the map since it must be updated.
        recreateTransformMap(0, 0);
    }

    m_targetProperties.m_data = data;
    m_targetProperties.m_width = width;
    m_targetProperties.m_height = height;
    m_targetProperties.m_pitch = pitch;
}


/*!
  Enables/disables high quality setting.
*/
void MirrorEffect::setHighQuality(bool highQuality)
{
    m_highQuality = highQuality;
}


/*!
  Returns the high quality setting.
*/
bool MirrorEffect::highQuality() const
{
    return m_highQuality;
}


/*!
  Sets the mirror transform properties.
*/
void MirrorEffect::setMirrorTransform(MirrorTransform transform,
                                      float power /* = 1.0f */,
                                      float size /* = 1.0f */)
{
    m_selectedTransform = transform;
    m_selectedTransformPower = power;
    m_selectedTransformSize = size;
}


/*!
  Executes the transform from the source to the target. Returns true if
  successful, false otherwise.
*/
bool MirrorEffect::process()
{
    // Don't continue if source or target is not set
    if (!m_sourceProperties.m_data || !m_targetProperties.m_data)
        return false;

    if (m_transMap == 0) {
        qDebug() << "MirrorEffect::process(): Transmap zeroed, recreate.";
        recreateTransformMap(m_targetProperties.m_width,
                             m_targetProperties.m_height);
    }

    // The transform needs to be (re)created
    if (m_currentTransform != m_selectedTransform
            || m_currentTransformPower != m_selectedTransformPower
            || m_currentTransformSize != m_selectedTransformSize)
    {
        qDebug() << "MirrorEffect::process(): Recreating transform...";

        /*
        // Uncomment this block to enable the full debug printing.
        if (m_currentTransform != m_selectedTransform) qDebug() << "Transform changed";
        if (m_currentTransformPower != m_selectedTransformPower) qDebug() << "Power changed";
        if (m_currentTransformSize != m_selectedTransformSize) qDebug() << "Size changed";
        */

        recreateTransform(m_selectedTransform,
                          m_selectedTransformPower,
                          m_selectedTransformSize);
    }

    for (int y = 0; y < m_targetProperties.m_height; y++) {
        if (!m_highQuality) {
            processLine(m_targetProperties.m_data + m_targetProperties.m_pitch * y,
                        m_targetProperties.m_data + m_targetProperties.m_pitch * y
                        + m_targetProperties.m_width,
                        m_transMap + m_targetProperties.m_width * y * 3);
        }
        else {
            processLineHQ(m_targetProperties.m_data + m_targetProperties.m_pitch * y,
                          m_targetProperties.m_data + m_targetProperties.m_pitch * y
                          + m_targetProperties.m_width,
                          m_transMap + m_targetProperties.m_width * y * 3);
        }
    }

    return true;
}


/*!
  Sample ("copy") the source image contained by m_sourceProperties to the row beginning at
  unsigned int *t and ending at unsigned int *t_target. Copy source pixels from the coordinates
  defined by srcCoords[0] and scrCoords[1] (18/14 fixedpoint).

  The >>14 (same as dividing the number with 16384) used by the function takes only the real-part
  of the fixed-point number and uses it directly.
*/
void MirrorEffect::processLine(unsigned int *t,
                               unsigned int *t_target,
                               int *srcCoords)
{
    unsigned int *sourceData = m_sourceProperties.m_data;
    int sourcePropertiesPitch = m_sourceProperties.m_pitch;
    while (t != t_target) {
        *t = sourceData[(srcCoords[1] >> 14)
                         * sourcePropertiesPitch
                         + (srcCoords[0] >> 14)];
        t++;
        srcCoords += 3;
    }
}


/*!
  This is HIGHLY optimized function for doing the same task as function above but with full,
  4-component linear resampling (with one bit accuracylost).
*/
void MirrorEffect::processLineHQ(unsigned int *t,
                                 unsigned int *t_target,
                                 int *srcCoords)
{
    int x(0);
    int y(0);
    unsigned int temp(0);
    unsigned int mask(0);
    unsigned int *pos(0);

    unsigned int *sourceData = m_sourceProperties.m_data;
    unsigned int sourceDataPitch = m_sourceProperties.m_pitch;

    while (t != t_target) {
            // Place the source x and y coordinates (18/14 - fixedpoint) to the temporary attributes
        x = srcCoords[0];
        y = srcCoords[1];

            // Seek the initial place of the source image by using only real-parts of the source
            // coordinates. sourceImage[ pitch * (y/16384) + (x/16383) ] is the startingpoint.
        pos = sourceData + sourceDataPitch
                * (y >> 14) + (x >> 14);

            // Mask only the decimal part of the source coordinate and round it to 7-bits.
            // After this the source coordinate "fractions" are between 0-and 127
            // Don't think of them as coordinates anymore, only fractions of
            // coordinates.
        x = ((x & 16383) >> 7);
        y = ((y & 16383) >> 7);

            // Temp is used as negative x just to limit the number of calculations
            // it should be named "negx", but we don't want to add any more values
            // into the stack to make sure it doesn't end.
        temp = 128 - x;

        /*
           The basic one dimensional interpolation goes:
           resampled_pixel = (pixelonleft*x + pixelonright*negx ) / 128

           When adding second component (y), we must do second x interpolation, and
           interpolate between those two with y:

           toppixel = (pixelontopleft*x + pixelontopright*negx) / 128;
           bottompixel = (pixelonbottomleft*x + pixelonbottomright*negx) / 128;
           finalpixel = toppixel*y + bottompixel*negy) / 128

           Obviously, this must be done to each of the components R,G,B and A to
           get the actual colour.

           The following code interpolates between four 4component RGBA-pixels
           pos[0], pos[1], pos[pitch] and pos[pitch+1] with x,y,negx and negy.

           The reason we are using 7bit interpolation is that we can combine
           two of the colour components with the same calculation without getting
           any overflows. For example:

           pixel = (RGBAPixel & 0x00FF00FF) leaves only red and green components in
           the pixel. When we multiply this value with any 7bit number (from 0 to 127)
           the resulting value will not be above 0xFF00FF00. This means than when
           shifting the result back (dividing it with 128), we have the multiplied
           values directly at the same places (with some trash possibly from the
           multiplications but we can get rid of those easily with a simple AND).


           So when doing,

           pixel = ( ( (RGBAPixel & 0x00FF00FF) * mul ) / 128 ) & 0x00FF00FF

           we can multiply 2 components at the same time (at this example, those would
           be red and green).

           Obviously, all of the divisions are replaced with according shift-operands
           to get good performance.

           The resulting pixel of the resample will be place directly into the target
           at 't'.

        */

        *t = ((((((((((pos[0] & 0x00FF00FF) * (temp)) + ((pos[1] & 0x00FF00FF)
             * (x))) >> 7) & 0x00FF00FF) * (128 - y))
             + ((((((pos[sourceDataPitch] & 0x00FF00FF) * (temp))
             + ((pos[sourceDataPitch] & 0x00FF00FF) * (x))) >> 7)
             & 0x00FF00FF) * y)) >> 7) & 0x00FF00FF) | (((((((((((pos[0] >> 8)
             & 0x00FF00FF) * (temp)) + (((pos[1] >> 8) & 0x00FF00FF) * (x)))
             >> 7) & 0x00FF00FF) * (128 - y)) +
             (((((((pos[sourceDataPitch] >> 8) & 0x00FF00FF)
             * (temp)) + (((pos[sourceDataPitch + 1] >> 8)
             & 0x00FF00FF) * (x))) >> 7) & 0x00FF00FF) * y)) >> 7)
             & 0x00FF00FF) << 8));

        // In the definition of srcCoords earlier, it was said that the third value
        // Of the srcCoords is a "shine"-value which should be added into the final,
        // resampled colour.
        // The following code creates RGBA (shine, shine, shine, 0) value,
        // and adds it into the pixel at 't' using common temporary
        // attributes temp and mask for the purpose.
        // If you like playing with bits, think of the section with
        // paper and you will see it works ;) Only with one bit
        // of accuracylost.
        // This is the fastest way of adding two RGB - pixels together
        // that i'm aware of.
        if (srcCoords[2] > 0) {
            temp = ((*t & 0xFEFEFEFE) >> 1)
                + (((srcCoords[2] | (srcCoords[2] << 8)
                     | (srcCoords[2] << 16)) & 0xFEFEFEFE) >> 1);
            mask = (temp & 0x80808080);
            temp |= (mask - (mask >> 7));
            *t = ((temp & 0x7F7F7F7F) << 1);
        }

            // This pixel of the row is processed, move on to the next one.
        t++;
        srcCoords += 3;
    }
}


/*!
  (Re)create the transform of the (member srcCoords) with provided attributes.
  Function places the source coordinates from where the target pixel should be
  taken from the sourceimage. And the third "shine"-value as well.

  Note, this method is not designed for real-time use. The user should make sure
  it is not used very often. (MirrorHouse uses it only when the mirror or the camera
  changes).
*/
void MirrorEffect::recreateTransform(MirrorTransform transform, float power, float size)
{
    qDebug() << "MirrorEffect::recreateTransform()";

    float fTemp;
    float a;
    float fx;
    float fy;
    float fz;
    float nx;
    float ny;
    float nz;

    int xInc = ((m_sourceProperties.m_width) << 14) / m_targetProperties.m_width;
    int yInc = ((m_sourceProperties.m_height) << 14) / m_targetProperties.m_height;
    int maxX = ((m_sourceProperties.m_width-1) << 14) - 1;
    int maxY = ((m_sourceProperties.m_height-1) << 14) - 1;

    float pixelMul =
            (float)(m_sourceProperties.m_width
                    + (float)m_sourceProperties.m_height)
            * 2000.0f / m_selectedTransformSize;

    int *t = m_transMap;
    int sy = 0;

    for (int y = 0; y < m_targetProperties.m_height; y++) {
        int sx = 0;

        for (int x = 0; x < m_targetProperties.m_width; x++) {
            switch (transform) {
            default:
            case None: {
                fx = 0.0f;
                fy = 0.0f;
                break;
            }
            case Dither: {
                fx = -1.0f + (float)(rand() & 255) / 128.0f;
                fy = -1.0f + (float)(rand() & 255) / 128.0f;
                fx *= power;
                fy *= power;
                break;
            }
            case Tile: {
                fx = ((float)x / (float)m_targetProperties.m_width);
                fy = ((float)y / (float)m_targetProperties.m_height);
                fx *= size;
                fy *= size;
                fx -= floor(fx);
                fy -= floor(fy);
                fx = (fx - 0.5f) * 2.0f;
                fy = (fy - 0.5f) * 2.0f;
                fx *= power;
                fy *= power;
                break;
            }
            case Spike: {
                fx = ((float)x / (float)m_targetProperties.m_width - 0.5f) * 2.0f;
                fy = ((float)y / (float)m_targetProperties.m_height - 0.5f) * 2.0f;
                fTemp = sqrtf(fx * fx + fy * fy);
                fx /= fTemp;
                fy /= fTemp;
                fTemp = (1.0f - fTemp);

                if (fTemp < 0.0f)
                    fTemp = 0.0f;

                fTemp *= power;
                fx *= fTemp;
                fy *= fTemp;
                break;
            }
            case Ripple: {
                fx = ((float)x / (float)m_targetProperties.m_width - 0.5f) * 2.0f;
                fy = ((float)y / (float)m_targetProperties.m_height - 0.5f) * 2.0f;
                fTemp = sqrtf(fx * fx + fy * fy);
                fx /= fTemp;
                fy /= fTemp;
                fx = fx * sinf(fTemp * size * 3.14159f * 2.0f) * power;
                fy = fy * sinf(fTemp * size * 3.14159f * 2.0f) * power;
                break;
            }
            case Spiral: {
                fx = ((float)x / (float)m_targetProperties.m_width - 0.5f) * 2.0f;
                fy = ((float)y / (float)m_targetProperties.m_height - 0.5f) * 2.0f;
                fTemp = sqrtf(fx * fx + fy * fy);
                a = atan2(fy, fx);
                a += (sqrtf(2.0f) - fTemp) * 20.0f * power;
                fx = -fx/4 + sinf(a) * fTemp;
                fy = -fy/4 + cosf(a) * fTemp;
                fx *= size;
                fy *= size;
                break;
            }
            case Bubbles: {
                fx = ((float)x / (float)m_targetProperties.m_width - 0.5f) * 2.0f;
                fy = ((float)y / (float)m_targetProperties.m_height - 0.5f) * 2.0f;
                fTemp = sqrtf(fx * fx + fy * fy);
                fTemp *= fTemp * fTemp;
                fTemp = 1.0f - fTemp;

                if (fTemp < 0.0f)
                    fTemp = 0.0f;

                fx = (sinf((float)x / (float)m_targetProperties.m_width
                           * 3.14159f * 2.0f * m_selectedTransformSize)) * power;
                fy = (sinf((float)y/(float)m_targetProperties.m_height
                           * 3.14159f * 2.0f * m_selectedTransformSize)) * power;
                break;
            }
            case InvBubbles: {
                fx = ((float)x / (float)m_targetProperties.m_width - 0.5f) * 2.0f;
                fy = ((float)y / (float)m_targetProperties.m_height - 0.5f) * 2.0f;
                // fTemp = (sqrtf(fx * fx + fy * fy) / sqrtf(2.0f));
                fTemp = sqrtf(fx * fx + fy * fy);
                fTemp *= fTemp * fTemp;
                fTemp = 1.0f - fTemp;

                if (fTemp < 0.0f)
                    fTemp = 0.0f;

                fx = (cosf((float)x / (float)m_targetProperties.m_width
                           * 3.14159f * 2.0f * m_selectedTransformSize)) * power;
                fy = (cosf((float)y / (float)m_targetProperties.m_height
                           * 3.14159f * 2.0f * m_selectedTransformSize)) * power;
                fx *= fTemp;
                fy *= fTemp;
                break;
            }
            case VerticalWave: {
                fx = 0.0f;
                fy = (sinf((float)y / (float)m_targetProperties.m_height
                           * 3.14159f * 2.0f * m_selectedTransformSize)) * power;
                break;
            }
            case HorizontalWave: {
                fx = (sinf((float)x / (float)m_targetProperties.m_width
                           * 3.14159f * 2.0f * m_selectedTransformSize)) * power;
                fy = 0.0f;
                break;
            }
            } // switch(transform)

            fz = sqrtf(3.0f - (fx * fx + fy * fy));

            // Calculate reflection mul TODO: Calculate WHAT???
            fTemp = sqrtf(fx * fx + fy * fy + fz * fz);
            nx = fx / fTemp;
            ny = fy / fTemp;
            nz = fz / fTemp;

            fTemp = nx * -0.57f + ny * -0.57f + nz * 0.57f;

            fTemp = fTemp - 0.5f;
            fTemp *= 2.0f;

            if (fTemp < 0.0f)
                fTemp = 0.0f;

            fTemp = fTemp * fTemp * fTemp;

            if (fTemp > 1.0f)
                fTemp = 1.0f;


            t[2] = (int)(fTemp * 100.0f);

            // Place the transform co-ordinate into the array
            t[0] = sx + (int)(fx * pixelMul);
            t[1] = sy + (int)(fy * pixelMul);

            if (t[0] < 0)
                t[0] = 0;

            if (t[1] < 0)
                t[1] = 0;

            if (t[0] > maxX)
                t[0] = maxX;

            if (t[1] > maxY)
                t[1] = maxY;

            sx += xInc;
            t += 3;
        }

        sy += yInc;
    }

    m_currentTransform = transform;
    m_currentTransformPower = power;
    m_currentTransformSize = size;
}


/*!
 Cleaning
*/
void MirrorEffect::recreateTransformMap(int width, int height)
{
    if (m_transMap) {
        delete[] m_transMap;
    }

    m_currentTransform = None;
    m_currentTransformPower = 0.0f;

    if (width >= 1 && height >= 1) {
        m_transMap = new int[width * height * 3];
    }
}
