#include "Transform.h"

#include <glm\ext.hpp>
//for m_LastUpdateTime
#include "TimeHandler.h"
#include "Camera.h"

//global Root Transform of project, can be set at run time
static Transform* m_RootTransform;
//a list of all Transforms made before there was a root Transform
static std::vector<Transform*> m_TransformsBeforeRoot;

Transform::Transform(bool a_AddToRoot) {
	if (a_AddToRoot) {
		if (m_RootTransform != nullptr) {
			setParent(m_RootTransform);
		} else {
			m_TransformsBeforeRoot.push_back(this);
		}
	}
	m_Name = "Untitled";
}

Transform::Transform(std::string a_Name, bool a_AddToRoot) {
	if (a_AddToRoot) {
		if (m_RootTransform != nullptr) {
			setParent(m_RootTransform);
		} else {
			m_TransformsBeforeRoot.push_back(this);
		}
	}
	m_Name = a_Name;
}

Transform::~Transform() {
	setParent(nullptr);
	for (size_t i = 0; i < m_Children.size(); i++) {
		m_Children[i]->setParent(nullptr);
	}
}

Transform::Transform(const Transform & a_Copy) {
	m_Position = a_Copy.m_Position;
	m_Rotation = a_Copy.m_Rotation;
	m_Scale = a_Copy.m_Scale;

	m_Name = a_Copy.m_Name + " - Copy";

	setParent(a_Copy.m_Parent);

	setDirty();
}

Transform & Transform::operator=(const Transform & a_Copy) {
	m_Name = a_Copy.m_Name + " - Copy";
	//copy's all data
	m_Position = a_Copy.m_Position;
	m_Rotation = a_Copy.m_Rotation;
	m_Scale = a_Copy.m_Scale;

	//copy's all parents
	setParent(a_Copy.m_Parent);

	setDirty();

	return *this;
}

void Transform::setPosition(const glm::vec3 a_NewPosition) {
	m_Position = a_NewPosition;
	setDirty();
}

void Transform::translate(const glm::vec3 a_Translation, const bool a_World) {
	if (a_World) {
		m_Position += a_Translation;
	} else {
		m_Position = glm::vec3(glm::translate(getLocalMatrix(), a_Translation)[3]);
	}
	setDirty();
}

void Transform::rotate(const glm::vec3 a_EulerRotation) {
#ifdef USE_QUATERNIONS
	m_Rotation *= glm::quat(glm::radians(a_EulerRotation));
#else
	m_Rotation += glm::radians(a_EulerRotation);
#endif // USE_QUATERNIONS
	setDirty();
}

void Transform::rotate(const glm::quat a_Quat) {
#ifdef USE_QUATERNIONS
	m_Rotation *= a_Quat;
#else
	m_Rotation += glm::eulerAngles(a_Quat);
#endif // USE_QUATERNIONS
	setDirty();
}

void Transform::setRotation(const glm::vec3 a_EulerRotation) {
#ifdef USE_QUATERNIONS
	m_Rotation = glm::quat(glm::radians(a_EulerRotation));
#else
	m_Rotation = glm::radians(a_EulerRotation);
#endif // USE_QUATERNIONS

	setDirty();
}

void Transform::setRotation(const glm::quat a_Quat) {
#ifdef USE_QUATERNIONS
	m_Rotation = a_Quat;
#else
	m_Rotation = glm::eulerAngles(a_Quat);
#endif // USE_QUATERNIONS

	setDirty();
}

void Transform::setScale(const glm::vec3 a_NewScale) {
	m_Scale = a_NewScale;
	setDirty();
}

void Transform::setScale(const float a_Scale) {
	m_Scale = glm::vec3(a_Scale);
	setDirty();
}

void Transform::setLookAt(const glm::vec3 a_Pos, const glm::vec3 a_LookAt, const glm::vec3 a_Up) {
	glm::mat4 lookAt = glm::lookAt(a_Pos, a_LookAt, a_Up);

	glm::quat rotation = glm::quat(glm::inverse(lookAt));

#ifdef USE_QUATERNIONS
	setRotation(rotation);
#else
	glm::vec3 quatRot = glm::degrees(glm::eulerAngles(rotation));
	if (abs(quatRot.z) > 90) {//if 180??!
		glm::vec3 euler;
		euler.x = quatRot.x - 180;
		euler.y = 180 - quatRot.y;
		euler.z = 0;
		setRotation(euler);
	} else {
		setRotation(rotation);
	}
#endif // USE_QUATERNIONS

	setPosition(a_Pos);
	setDirty();
}

void Transform::setLookAt(const glm::vec3 a_LookAt) {
	setLookAt(getGlobalPosition(), a_LookAt, glm::vec3(0, 1, 0));
}

void Transform::setLookAt(Transform * a_Transform) {
	setLookAt(getGlobalPosition(), a_Transform->getGlobalPosition(), glm::vec3(0, 1, 0));
}

glm::vec2 Transform::ToScreenSpace(Camera * a_Camera) {
	glm::vec4 screenSpace = a_Camera->getProjectionViewMatrix() * glm::vec4(m_Position, 1);
	glm::vec2 pos = glm::vec2(screenSpace.x / screenSpace.z, (screenSpace.y / screenSpace.w)) * 0.5f + 0.5f;
	return glm::vec2(pos.x, 1 - pos.y);
}

glm::vec3 Transform::getLocalPosition() const {
	return m_Position;
}

glm::quat Transform::getLocalRotation() const {
#ifdef USE_QUATERNIONS
	return m_Rotation;
#else
	return glm::quat(m_Rotation);
#endif // USE_QUATERNIONS
}

glm::vec3 Transform::getLocalRotationEulers() const {
#ifdef USE_QUATERNIONS
	return glm::degrees(glm::eulerAngles(m_Rotation));
#else
	return glm::degrees(m_Rotation);
#endif // USE_QUATERNIONS
}

glm::vec3 Transform::getLocalScale() const {
	return m_Scale;
}

glm::vec3 Transform::getGlobalPosition() {
	if (m_IsDirty) {
		updateTransform();
	}
	if (m_Parent == nullptr) {
		return m_Position;
	}
	return glm::vec3(m_Parent->getGlobalMatrix() * glm::vec4(m_Position, 1));
}

glm::quat Transform::getGlobalRotation() {
	if (m_IsDirty) {
		updateTransform();
	}
#ifdef USE_QUATERNIONS
	if (m_Parent == nullptr) {
		return m_Rotation;
	}
	return m_Parent->getGlobalRotation() * m_Rotation;
#else
	if (m_Parent == nullptr) {
		return glm::quat(m_Rotation);
	}
	return m_Parent->getGlobalRotation() * glm::quat(m_Rotation);
#endif // USE_QUATERNIONS
	}

glm::vec3 Transform::getGlobalRotationEulers() {
	return glm::degrees(glm::eulerAngles(getGlobalRotation()));
}

glm::vec3 Transform::getGlobalUnitVector() {
	return glm::normalize(getGlobalRotation() * glm::vec3(0, 0, -1));
	//glm::vec3 unitVector;
	//glm::vec3 rot = getGlobalRotationEulers();
	//glm::vec3 rad = glm::radians(rot);
	//
	////unitVector.x = -sin(rad.y);
	////unitVector.y = sin(rad.x);
	//unitVector.z = 1+( sin(rad.y) * cos(rad.x));
	//
	//if (abs(rot.z) >= 90) {
	//	unitVector.y *= -1;
	//	unitVector.z *= -1;
	//}
	//
	//return unitVector;
	////return glm::normalize(unitVector);
}

glm::vec3 Transform::getGlobalScale() {
	if (m_IsDirty) {
		updateTransform();
	}
	if (m_Parent == nullptr) {
		return m_Scale;
	}
	return glm::vec3(m_Parent->getGlobalScale() * m_Scale);
}

glm::mat4 Transform::getLocalMatrix() {
	if (m_IsDirty) {
		updateTransform();
	}
	return m_LocalTransform;
}

glm::mat4 Transform::getGlobalMatrix() {
	if (isParentDirty()) {
		updateTransform();
	}
	return m_GlobalTransform;
}

void Transform::updateTransform() {

	//update local
	m_LocalTransform = glm::translate(glm::mat4(1), m_Position);//reset and translate
#ifdef USE_QUATERNIONS
	m_LocalTransform *= glm::toMat4(m_Rotation);//rotation
#else
	glm::mat4 rotationMatrix;
	//weird issues with camera/other transforms if rotate goes x,y,z.
	//y,x,z seems to fix it
	rotationMatrix *= glm::rotate(m_Rotation.y, glm::vec3(0, 1, 0));
	rotationMatrix *= glm::rotate(m_Rotation.x, glm::vec3(1, 0, 0));
	rotationMatrix *= glm::rotate(m_Rotation.z, glm::vec3(0, 0, 1));

	m_LocalTransform *= rotationMatrix;

#endif // USE_QUATERNIONS
	m_LocalTransform *= glm::scale(m_Scale);//scale

	//update global
	//TODO update children
	if (m_Parent == nullptr) {
		m_GlobalTransform = m_LocalTransform;
	} else {
		m_GlobalTransform = m_Parent->getGlobalMatrix() * m_LocalTransform;
	}

	//setting the children as dirty means there will be less transforms being updated during this call
	//for (size_t i = 0; i < m_Children.size(); i++) {
	//	m_Children[i]->updateTransform();
	//}
	for (size_t i = 0; i < m_Children.size(); i++) {
		//this might need to be recursive
		m_Children[i]->setDirty();
	}

	m_LastUpdateFrame = TimeHandler::getCurrentFrameNumber();
	m_IsDirty = false;
}

void Transform::setDirty() {
	m_IsDirty = true;
}

void Transform::setParent(Transform* a_Parent) {
	if (a_Parent == m_Parent) {
		//cant set your parent to the same parent
		return;
	}
	removeParent();
	//TODO remove self from last parents child list
	if (a_Parent != nullptr) {
		m_Parent = a_Parent;
		m_Parent->m_Children.push_back(this);
	}
	setDirty();
}

Transform * Transform::getParent() {
	return m_Parent;
}

void Transform::removeParent() {
	if (m_Parent == nullptr) {
		//no parent to remove
		return;
	}
	for (size_t i = 0; i < m_Parent->m_Children.size(); i++) {
		//if this element is this transform
		if (m_Parent->m_Children[i] == this) {
			//erase this transform from the children array
			m_Parent->m_Children.erase(m_Parent->m_Children.begin() + i);
			//remove our parent
			m_Parent = nullptr;
			//global transform will be different so lets update that
			setDirty();
			return;
		}
	}
}

unsigned int Transform::getChildrenCount() const {
	return m_Children.size();
}

Transform * Transform::getChild(const unsigned int a_Index) const {
	if (a_Index > m_Children.size()) {
		return nullptr;
	}
	return m_Children[a_Index];
}

bool Transform::isChild(const Transform * a_Object) const {
	for (size_t i = 0; i < m_Children.size(); i++) {
		if (m_Children[i] == a_Object) {
			return true;
		}
	}
	return false;
}

void Transform::setRootTransform(Transform * a_RootTransform) {
	//TODO deal with letting another transform be set here after the first (as root parent)
	if (m_RootTransform != nullptr) {
		printf("CANT SET ANOTHER TRANSFORM AS ROOT\n");
		return;
	}
	m_RootTransform = a_RootTransform;
	a_RootTransform->m_IsRootTransform = true;

	for (size_t i = 0; i < m_TransformsBeforeRoot.size(); i++) {
		if (m_TransformsBeforeRoot[i] != m_RootTransform) {
			//only set
			if (m_TransformsBeforeRoot[i]->m_Parent == nullptr) {
				m_TransformsBeforeRoot[i]->setParent(m_RootTransform);
			}
		}
	}
	m_TransformsBeforeRoot.clear();
}

Transform * Transform::getRootTransform() {
	return m_RootTransform;
}

bool Transform::isParentDirty() const {
	//if this is dirty then return true
	if (m_IsDirty) {
		return true;
	}
	//if there is no parent then return false
	if (m_Parent == nullptr) {
		return false;
	}
	//go through each parent looking for a dirty transform
	return m_Parent->isParentDirty();
}

Transform * Transform::findTransform(const char * a_TransformName) {
	return findTransformChildren(m_RootTransform, a_TransformName);
}

Transform * Transform::findTransformChildren(Transform * a_Transform, const char * a_TransformName) {
	if (a_Transform->m_Name == a_TransformName) {
		return a_Transform;
	}
	int children = a_Transform->getChildrenCount();
	for (int i = 0; i < children; i++) {
		Transform* transform = findTransformChildren(a_Transform->getChild(i), a_TransformName);
		if (transform != nullptr) {
			return transform;
		}
	}
	return nullptr;
}
