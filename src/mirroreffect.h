/**
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef MIRROREFFECT_H
#define MIRROREFFECT_H


/*!
  \class MirrorEffect
  \brief Makes different mirror effect for the camera viewfinder frame data. Used from MyVideoSurface.
*/
class MirrorEffect
{
public: // Data types

    enum MirrorTransform {
        None,
        HorizontalWave,
        VerticalWave,
        Bubbles,
        InvBubbles,
        Spiral,
        Ripple,
        Spike,
        Tile,
        Dither
    };

    class ImageProperties
    {
    public:
        ImageProperties() : m_data(0), m_width(0), m_height(0), m_pitch(0) {}
        ~ImageProperties() { if (m_data) delete[] m_data; }

        unsigned int *m_data;   // Pointer to memory where the image starts
        int m_width;
        int m_height;
        int m_pitch;            // How many pixels a one row of the image contains (Not necessearly same as width)
    };

public:
    MirrorEffect();
    ~MirrorEffect();

public:
        // Set the source image as a memory reference for the transformation
    void setSource(unsigned int *data, int width, int height, int pitch,
                   bool rotate90degrees = false, bool flipY = false);

        // Set the target image as a memory reference. To this target, transformation's
        // results will be placed.
    void setTarget(unsigned int *data, int width, int height, int pitch);

        // When highQuality is true, linear resampling is done instead of nearest pixel
    void setHighQuality(bool highQuality);
    bool highQuality() const;

        // Set the current transform and it's attributes.
    void setMirrorTransform(MirrorTransform transform,
                            float power = 1.0f,
                            float size = 1.0f);

        // Apply a single transformation from the source to the target defined
        // outside of this class.
    bool process();

protected:
        // Process a single row of pixels with nearest-pixel sampling
    void processLine(unsigned int *t, unsigned int *t_target, int *srcCoords);

        // Process a single row of pixels with linear-resampling
    void processLineHQ(unsigned int *t, unsigned int *t_target, int *srcCoords);

        // (Re)sets the transform map with the attributes provided
    void recreateTransform(MirrorTransform transform, float power, float size);

        // Makes sure the actual memory for the transform-map is in order, according to
        // current settings.
    void recreateTransformMap(int width, int height);

protected: // Data
    /*
     * Source co-ordinate map
     * Each pixel has 3 integers making the total size of the map
     * width * height * 3. First two attributes of the map are 18-14 fixedpoint coordinates
     * of the source pixel FROM WHERE this pixel should take it's color. The coordinates
     * are ABSOLUTE (and therefore relative to source image's current dimensions) => they
     * must be recreated each time when source's or target's dimensions change.
     * The third attribute of each pixel is a "shine" value. Just a single integer telling
     * how much white should be added to this pixel when it's resampled (This makes the mirror
     * effect looks little bit better). The shine-value is used only with highQuality sampling.
     */
    int *m_transMap;

    MirrorTransform m_selectedTransform;
    MirrorTransform m_currentTransform;
    float m_selectedTransformPower;
    float m_selectedTransformSize;
    float m_currentTransformPower;
    float m_currentTransformSize;
    bool m_highQuality;
    ImageProperties m_sourceProperties;
    ImageProperties m_targetProperties;
    ImageProperties m_sourcePropertiesRotated;
};

#endif // MIRROREFFECT_H
