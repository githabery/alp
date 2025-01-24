#ifndef IDMX512DELIVERY_HPP
#define IDMX512DELIVERY_HPP

#include <QByteArray>
#include <stdint.h>


class IDMX512Delivery
{
public:
    virtual void writeUniverse(uint32_t universe, const QByteArray& data) = 0;
    static IDMX512Delivery& instance();
};

#endif // IDMX512DELIVERY_HPP
