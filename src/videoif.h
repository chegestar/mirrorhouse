/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#ifndef VIDEOIF_H
#define VIDEOIF_H

/*!
  \class VideoIF
  \brief MyVideoSurface asks to MirrorItem update the mirror when the effect is done
*/
class VideoIF
{
public:
    virtual void updateVideo() = 0;
};

#endif // VIDEOIF_H
