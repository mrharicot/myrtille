#include "material.h"


std::ostream& operator<<(std::ostream& os, const Material &m)
{
    os << "material: " << m.name << std::endl;
    os << "Kd: "      << m.color << std::endl;
    os << "Ke: "   << m.emission << std::endl;
}
