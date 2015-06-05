#include "noisebrush.hpp"

NoiseBrush::NoiseBrush(const glm::vec2& centre, const int radius, const int intensity, const int seed) :
        centre(centre),
        radius(radius),
        intensity(intensity),
        seed(seed),
        generator(std::default_random_engine(seed)) {

    standardNormal = std::normal_distribution<double>(0.0, 1.0);
}

const glm::vec2 NoiseBrush::getPoint() {
    return centre + glm::vec2 { standardNormal(generator) * radius, standardNormal(generator) * radius};
}
