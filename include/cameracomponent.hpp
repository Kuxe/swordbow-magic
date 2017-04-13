#ifndef CAMERACOMPONENT_HPP
#define CAMERACOMPONENT_HPP

#include <glm/vec4.hpp>

/** Component required by all entites that may be set as source in
	the camera system. CameraComponent should contain all data which
	is used to compute the view matrix (is it first person? Third person?
	Head-bobbin? All these questions should be answered by camera component) **/

struct CameraComponent {
	/** These vectors will be component-wise multiplied with
		base vectors of transform matrix in movecomponent to
		determine where the POV is. **/
	glm::vec4 posoff;
	float yaw;
};


#endif //CAMERACOMPONENT_HPP