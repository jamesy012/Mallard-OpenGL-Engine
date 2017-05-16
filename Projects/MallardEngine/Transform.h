#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>

#include <vector>
#include <string>

//SWAP BETWEEN USING QUATERNIONS OR A VEC3 IN RADIANS
#define USE_QUATERNIONS
#undef USE_QUATERNIONS

//mat4 position/rotation/scale of a object
//ability to parent which will offset this object by parents mat4
class Transform {
public:
	//sets up globalRoot parent gives default name
	Transform();
	//sets up globalRoot parent with a_Name
	Transform(std::string a_Name);
	~Transform();
	
	//TODO make these rule of 3/5 functions for Transform
	Transform(const Transform& a_Copy); // copy
	//Transform(Transform&& a_Move) = default; // move
	Transform& operator=(const Transform& a_Copy); // copy assignment
	//Transform& operator=(Transform&& a_Move)& = default; // move assignment

	//sets position of transform to a_NewPosition;
	void setPosition(const glm::vec3 a_NewPosition);
	//translates m_Position by a_Translation
	//if a_World is false then a_Translation will be multiplied by a_Rotation
	//causing it to move in that direction
	void translate(const glm::vec3 a_Translation,const bool a_World = true);
	//rotates transform with Euler rotation
	void rotate(const glm::vec3 a_EulerRotation);
	//rotates by a quaternion rotation
	void rotate(const glm::quat a_Quat);
	//sets rotation with Euler rotation
	void setRotation(const glm::vec3 a_EulerRotation);
	//sets rotation with a quaternion rotation
	void setRotation(const glm::quat a_Quat);
	//sets the scale to a_NewScale
	//be sure to keep default scales at 1 and not 0, since 0 will cause the model to have 0 width
	void setScale(const glm::vec3 a_NewScale);
	//sets the all 3 scale floats to a_Scale
	//be sure to keep default scales at 1 and not 0, since 0 will cause the model to have 0 width
	void setScale(const float a_Scale);

	//sets the position and rotation of this transform to be looking from a_Pos and looking at a_LookAt
	void setLookAt(const glm::vec3 a_Pos,const glm::vec3 a_LookAt,const glm::vec3 a_Up = glm::vec3(0, 1, 0));
	//sets the position and rotation of this transform to be looking at a_LookAt, up is 0,1,0
	void setLookAt(const glm::vec3 a_LookAt);
	//sets the position and rotation of this transform to be looking at the position of a_Transform, up is 0,1,0
	void setLookAt(Transform* a_Transform);
	
	//returns local Position vector
	glm::vec3 getLocalPosition() const;
	//returns local rotation quaternion
	glm::quat getLocalRotation() const;
	//returns local rotation converted into euler angles
	glm::vec3 getLocalRotationEulers() const;
	//returns local scale vector
	glm::vec3 getLocalScale() const;

	//returns position relative to parents
	glm::vec3 getGlobalPosition();
	//returns rotation relative to parents
	glm::quat getGlobalRotation();
	//returns the Euler version of rotation
	glm::vec3 getGlobalRotationEulers();
	//returns a unit vector from the rotation
	glm::vec3 getGlobalUnitVector();
	//returns global scale of object relative to parent
	glm::vec3 getGlobalScale();

	//gets the local mat4, contains position/scale and rotation
	glm::mat4 getLocalTransform();
	//gets the global mat4, which is a combination of localTransform and parents' globalTransform
	//if transform is dirty, it will update when calling this
	glm::mat4 getGlobalTransform();
	//updates local and global transforms
	//using data from position/scale/rotation
	//and for global, the parents global
	//called when getting data from Transforms Matrix' only if the transform is dirty
	virtual void updateTransform();
	//sets a flag for dirty to true
	//if dirty is true, next time a getTransform is called
	//they will update the transforms to new data
	void setDirty();
	//sets objects parent to a_Parent
	//this will move this object to be relative of that object instead of old parent
	//this overwrites old parent
	void setParent(Transform* a_Parent);
	//returns parent of this transform
	//can be nullptr
	Transform* getParent();
	//removes this transforms current parent
	//also removes this transform from parents child list
	//this will move the transform to be locally off 0,0,0
	void removeParent();

	//returns the amount of children
	unsigned int getChildrenCount() const ;
	//returns child at a_Index, if above child count will return nullptr
	Transform* getChild(const unsigned int a_Index) const;
	//returns true if a_Object is a child of this transform
	bool isChild(const Transform* a_Object) const;

	//sets the world root transform,
	static void setRootTransform(Transform* a_RootTransform);

	//returns RootTransform
	//can be nullptr if not set
	static Transform* getRootTransform();

	//finds transform based off the root transform
	//if no transform under then, will return nullptr
	static Transform* findTransform(const char* a_TransformName);

	//name of transform
	std::string m_Name;

	//a flag to turn off certain transforms from showing in the hierarchy
	bool m_ShowInHierarchy = true;

protected:
	//goes through each parent till it finds a dirty parent
	bool isParentDirty() const ;

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

