#include "CameraTarget.h"

using namespace glm;
using namespace std;

extern int window_width;
extern int window_height;

CameraTarget::CameraTarget(vec3 target, float distance) : Camera()
{
	yaw = 0.0f;
	pitch = 0.0f;
	this->target = target;
	this->distance = distance;
	
	updateViewMatrix();
}

CameraTarget::~CameraTarget()
{

}

void CameraTarget::update(float dt)
{
	mouseDeltaX = mouseSensitivity * (mouseX - mouseLastX);
	mouseDeltaY = mouseSensitivity * (mouseY - mouseLastY);
	mouseLastX = mouseX;
	mouseLastY = mouseY;

	updateViewMatrix();
}

void CameraTarget::updateViewMatrix()
{
	/*vec3 dirVec = normalize(target - position);
	vec3 right = normalize(cross(dirVec, up));
	vec3 newUp = normalize(cross(right, dirVec));

	quat orientation = angleAxis(radians(mouseDeltaY), right) * angleAxis(radians(mouseDeltaX), up);
	position = target + vec3(toMat4(orientation) * vec4(-dirVec, 0.0f)) * distance;

	vec3 newDir = rotate(-dirVec, radians(yaw), up);
	position = target + newDir * distance;*/

	//printf("%f %f \n", yaw, pitch);

	/*mat4 rotation = eulerAngleYXZ(radians(yaw), radians(pitch), 0.0f);
	vec3 translation = vec3(vec4(0.0f, 0.0f, distance, 0.0f) * rotation);
	vec3 newUp = vec3(vec4(up, 0.0f) * rotation);
	position = target + translation;*/

	/*vec3 dirVec = normalize(target - position);
	vec3 right = normalize(cross(dirVec, up));
	vec3 newUp = normalize(cross(right, dirVec));

	position = target + rotate(position - target, radians(mouseDeltaX), up);
	position = target + rotate(position - target, radians(mouseDeltaY), right);*/

	vec3 dirVec = rotate(vec3(0, 0, 1), radians(yaw), up);
	vec3 right = cross(-dirVec, up);

	position = target + rotate(dirVec, radians(pitch), right) * distance;

	matView = lookAt(position, target, up);

	glBindBuffer(GL_UNIFORM_BUFFER, Shader::uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(getMatView()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, Shader::uboLighting);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(vec4), value_ptr(position));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void CameraTarget::mouse(int button, int state)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		stateLookAround = true;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		stateLookAround = false;
	}
}

// Track the delta for the mouse movements
void CameraTarget::mouseMotion(int x, int y)
{
	if (stateLookAround)
	{
		Camera::mouseMotion(x, y);

		// Update yaw and pitch and limit pitch
		yaw += -mouseDeltaX * mouseSensitivity;
		pitch += -mouseDeltaY * mouseSensitivity;
		pitch = clamp(pitch, -89.0f, 89.0f);
	}
}

void CameraTarget::mouseWheel(int dir)
{
	//if (stateLookAround)
	{
		distance += -dir;
		distance = clamp(distance, 0.0f, 100.0f);
	}
}

void CameraTarget::setTarget(glm::vec3 target)
{
	this->target = target;
}

void CameraTarget::setDistance(float distance)
{
	this->distance = distance;
}

void CameraTarget::setOrientation(float yaw, float pitch)
{
	this->yaw = yaw;
	this->pitch = pitch;
}

glm::vec3 CameraTarget::getTarget() const
{
	return target;
}

float CameraTarget::getDistance() const
{
	return distance;
}