#ifndef NOISEBRUSH_HPP
#define NOISEBRUSH_HPP
#include <glm/vec2.hpp>
#include <random>


/** Generates a point. Several points together will
  * constitute a noise within the radius, with more
  * points closer to the center as the intensity is
  * higher
**/
class NoiseBrush {
private:
    const glm::vec2 centre;
    const int radius;
    const int intensity;
    const int seed;
    std::default_random_engine generator;
    std::normal_distribution<double> standardNormal;
public:

    NoiseBrush(const glm::vec2& centre, const int radius, const int intensity, const int seed = 0);
    const glm::vec2 getPoint();
};



#endif //NOISEBRUSH_HPP
