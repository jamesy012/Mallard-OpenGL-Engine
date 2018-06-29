#pragma once

#include "DLLBuild.h"

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>

#include <vector>
#include <string>

//SWAP BETWEEN USING QUATERNIONS OR A VEC3 IN RADIANS
#define USE_QUATERNIONS
//#undef USE_QUATERNIONS

//cant put DLL_BUILD after class due to it using glm

//mat4 position/rotation/scale of a object
//ability to parent which will offset this object by parents mat4
class Transform {
public:
	//sets up globalRoot parent gives default name
	//bool to check if we should add this transform as a child to the root transform
	DLL_BUILD Transform(bool a_AddToRoot = true);
	//sets up globalRoot parent with a_Name
	//bool to check if we should add this transform as a child to the root transform
	DLL_BUILD Transform(std::string a_Name, bool a_AddToRoot = true);
	DLL_BUILD ~Transform();
	
	//TODO make these rule of 3/5 functions for Transform
	DLL_BUILD Transform(const Transform& a_Copy); // copy
	//Transform(Transform&& a_Move) = default; // move
	DLL_BUILD Transform& operator=(const Transform& a_Copy); // copy assignment
	//Transform& operator=(Transform&& a_Move)& = default; // move assignment

	//sets position of transform to a_NewPosition;
	DLL_BUILD void setPosition(const glm::vec3 a_NewPosition);
	//translates m_Position by a_Translation
	//if a_World is false then a_Translation will be multiplied by a_Rotation
	//causing it to move in that direction
	DLL_BUILD void translate(const glm::vec3 a_Translation,const bool a_World = true);
	//rotates transform with Euler rotation
	DLL_BUILD void rotate(const glm::vec3 a_EulerRotation);
	//rotates by a quaternion rotation
	DLL_BUILD void rotate(const glm::quat a_Quat);
	//sets rotation with Euler rotation
	DLL_BUILD void setRotation(const glm::vec3 a_EulerRotation);
	//sets rotation with a quaternion rotation
	DLL_BUILD void setRotation(const glm::quat a_Quat);
	//sets the scale to a_NewScale
	//be sure to keep default scales at 1 and not 0, since 0 will cause the model to have 0 width
	DLL_BUILD void setScale(const glm::vec3 a_NewScale);
	//sets the all 3 scale floats to a_Scale
	//be sure to keep default scales at 1 and not 0, since 0 will cause the model to have 0 width
	DLL_BUILD void setScale(const float a_Scale);

	//sets the position and rotation of this transform to be looking from a_Pos and looking at a_LookAt
	DLL_BUILD void setLookAt(const glm::vec3 a_Pos,const glm::vec3 a_LookAt,const glm::vec3 a_Up = glm::vec3(0, 1, 0));
	//sets the position and rotation of this transform to be looking at a_LookAt, up is 0,1,0
	DLL_BUILD void setLookAt(const glm::vec3 a_LookAt);
	//sets the position and rotation of this transform to be looking at the position of a_Transform, up is 0,1,0
	DLL_BUILD void setLookAt(Transform* a_Transform);
	
	//returns local Position vector
	DLL_BUILD glm::vec3 getLocalPosition() const;
	//returns local rotation quaternion
	DLL_BUILD glm::quat getLocalRotation() const;
	//returns local rotation converted into euler angles
	DLL_BUILD glm::vec3 getLocalRotationEulers() const;
	//returns local scale vector
	DLL_BUILD glm::vec3 getLocalScale() const;

	//returns position relative to parents
	DLL_BUILD glm::vec3 getGlobalPosition();
	//returns rotation relative to parents
	DLL_BUILD glm::quat getGlobalRotation();
	//returns the Euler version of rotation
	DLL_BUILD glm::vec3 getGlobalRotationEulers();
	//returns a unit vector from the rotation
	DLL_BUILD glm::vec3 getGlobalUnitVector();
	//returns global scale of object relative to parent
	DLL_BUILD glm::vec3 getGlobalScale();

	//gets the local mat4, contains position/scale and rotation
	DLL_BUILD glm::mat4 getLocalMatrix();
	//gets the global mat4, which is a combination of localTransform and parents' globalTransform
	//if transform is dirty, it will update when calling this
	DLL_BUILD glm::mat4 getGlobalMatrix();
	//updates local and global transforms
	//using data from position/scale/rotation
	//and for global, the parents global
	//called when getting data from Transforms Matrix' only if the transform is dirty
	DLL_BUILD virtual void updateTransform();
	//sets a flag for dirty to true
	//if dirty is true, next time a getTransform is called
	//they will update the transforms to new data
	DLL_BUILD void setDirty();
	//sets objects parent to a_Parent
	//this will move this object to be relative of that object instead of old parent
	//this overwrites old parent
	DLL_BUILD void setParent(Transform* a_Parent);
	//returns parent of this transform
	//can be nullptr
	DLL_BUILD Transform* getParent();
	//removes this transforms current parent
	//also removes this transform from parents child list
	//this will move the transform to be locally off 0,0,0
	DLL_BUILD void removeParent();

	//returns the amount of children
	DLL_BUILD unsigned int getChildrenCount() const;
	//returns child at a_Index, if above child count will return nullptr
	DLL_BUILD Transform* getChild(const unsigned int a_Index) const;
	//returns true if a_Object is a child of this transform
	DLL_BUILD bool isChild(const Transform* a_Object) const;

	//sets the world root transform,
	DLL_BUILD static void setRootTransform(Transform* a_RootTransform);

	//returns RootTransform
	//can be nullptr if not set
	DLL_BUILD static Transform* getRootTransform();

	//finds transform based off the root transform
	//if no transform under then, will return nullptr
	DLL_BUILD static Transform* findTransform(const char* a_TransformName);

	//goes through each parent till it finds a dirty parent
	DLL_BUILD bool isParentDirty() const;

	//name of transform
	std::string m_Name;

	//a flag to turn off certain transforms from showing in the hierarchy
	bool m_ShowInHierarchy = true;

	//what frame did we last update this transforms matrixes
	unsigned int m_LastUpdateFrame = -1;
protected:
	//local position of transform
	glm::vec3 m_Position;
	//local scale of transform
	glm::vec3 m_Scale = glm::vec3(1);

#ifdef USE_QUATERNIONS
	//local rotation of transform in quaternion
	glm::quat m_Rotation;
#else
	//local rotation of transform in radians
	glm::vec3 m_Rotation;
#endif // USE_QUATERNIONS

	//combination of position/rotation/scale
	glm::mat4 m_LocalTransform;
	//combination of localTransform and parent
	glm::mat4 m_GlobalTransform;
	//has position/scale or rotation been changed?
	//if true the program will update local and global transforms
	bool m_IsDirty = true;
private:
	static Transform* findTransformChildren(Transform* a_Transform, const char* a_TransformName);

	//parent of this object
	//nullptr means it has no parent
	Transform* m_Parent = nullptr;

	////list of all the children for this transform
	std::vector<Transform*> m_Children;

	//a flag to check
	bool m_IsRootTransform = false;
};

