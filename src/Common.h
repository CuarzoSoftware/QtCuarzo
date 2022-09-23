#ifndef COMMON_H
#define COMMON_H

#include <qpa/qplatformmenu.h>
#include <Heaven/Heaven-Client.h>
#include <QDebug>

struct HNData
{
    hn_client *heaven = nullptr;
};

struct HNObjectRef
{
    void *object = nullptr;
    quintptr tag;
};

inline hn_pixel *qIconToHNPixels(const QIcon &icon, u_int32_t *width, u_int32_t *height)
{
    hn_pixel *pixels = NULL;

    QImage img = icon.pixmap(icon.actualSize(QSize(64, 64))).toImage();
    int totalPixels = img.width()*img.height();

    if(totalPixels > 0)
    {
        *width = img.width();
        *height = img.height();
        pixels = new hn_pixel[totalPixels];
        u_int32_t i = 0;

        for(u_int32_t x = 0; x < *width; x++)
        {
            for(u_int32_t y = 0; y < *height; y++)
            {
                pixels[i] = (hn_pixel)qAlpha(img.pixel(x,y));
                i++;
            }
        }
    }

    return pixels;
}

#endif // COMMON_H
