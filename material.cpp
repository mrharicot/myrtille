#include "material.h"


std::ostream& operator<<(std::ostream& os, const Material &m)
{
    os << "material: " << m.name << " | Kd: " << m.color;
}
