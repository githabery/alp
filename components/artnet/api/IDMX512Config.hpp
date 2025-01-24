#ifndef IDMX512CONFIG_HPP
#define IDMX512CONFIG_HPP

#include <vector>
#include <string>
#include <stdint.h>

class IDMX512Config
{
public:
    virtual std::vector<std::string> outputs() = 0;
    virtual bool openOutput(std::string output, uint32_t universe) = 0;
    virtual void closeOutput(std::string output, uint32_t universe) = 0;
    static IDMX512Config& instance();
};

#endif // IDMX512CONFIG_HPP
