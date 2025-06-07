#ifndef IDMX512DELIVERY_HPP
#define IDMX512DELIVERY_HPP

#include <QByteArray>


class IDMX512Delivery
{
public:
    virtual void pulse (int channel) = 0;
    virtual bool isPulseReady () = 0;
    virtual void setColor (int channel, int r, int g, int b) = 0;
    virtual void setBrightness (int channel, int brightness) = 0;
    static IDMX512Delivery& instance();
};

#endif // IDMX512DELIVERY_HPP
