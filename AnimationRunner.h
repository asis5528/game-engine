#pragma once
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
/*
class AnimationRunner{
public:
	std::vector<glm::mat4> mats;
	 void readAnimation(float time, AnimationData& data, glm::mat4& matrix) {
		//	float TicksPerSecond = (float)(ticksperSec != 0 ? ticksperSec : 25.0f);
		//	float TimeInTicks = time * TicksPerSecond;
		//	float AnimationTime = fmod(TimeInTicks, duration);


		glm::vec3 pos = data.animationTransformation[0].position;
		glm::vec3 sc = data.animationTransformation[0].scale;
		glm::mat4 trans(1.0);
		glm::mat4 scale(1.0);
		glm::quat q;
		CalcInterpolatedRotation(q, 0.1);
		glm::mat4 rotmat = glm::toMat4(q);

		trans = glm::translate(trans, glm::vec3(pos.x, pos.y, pos.z));
		scale = glm::scale(scale, glm::vec3(sc.x, sc.y, sc.z));
		rotmat = trans * rotmat * scale;
		rotmat = rotmat * data.boneOffset;
		matrix = rotmat;
		outMatrices.push_back(rotmat);

		for (int i = 0; i < data.childAnimationData.size(); i++) {

			readAnimation(time, data.childAnimationData[i], matrix);
		}
		//	std::cout << "Time  " << AnimationTime << "\n";
	}
	 void CalcInterpolatedRotation(glm::quat& Out, float AnimationTime)
	 {
		 // we need at least two values to interpolate...
		 float size = animationTransformation.size();
		 if (animationTransformation.size() == 1) {
			 Out = animationTransformation[0].rotation;
			 return;
		 }

		 unsigned int RotationIndex = FindRotation(AnimationTime);
		 unsigned int NextRotationIndex = (RotationIndex + 1);
		 assert(NextRotationIndex < size);
		 float DeltaTime = (float)(animationTransformation[NextRotationIndex].time - animationTransformation[RotationIndex].time);
		 float Factor = (AnimationTime - (float)animationTransformation[RotationIndex].time) / DeltaTime;
		 assert(Factor >= 0.0f && Factor <= 1.0f);
		 const glm::quat& StartRotationQ = animationTransformation[RotationIndex].rotation;
		 const  glm::quat& EndRotationQ = animationTransformation[NextRotationIndex].rotation;
		 glm::quat q1;
		 q1.w = StartRotationQ.w; q1.x = StartRotationQ.x; q1.y = StartRotationQ.y; q1.z = StartRotationQ.z;
		 glm::quat q2;
		 q2.w = EndRotationQ.w; q2.x = EndRotationQ.x; q2.y = EndRotationQ.y; q2.z = EndRotationQ.z;

		 glm::quat fina = glm::mix(q1, q2, Factor);
		 //aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		 //Out = Out.Normalize();
		 Out.w = fina.w;
		 Out.x = fina.x;
		 Out.y = fina.y;
		 Out.z = fina.z;
	 }
	 int FindRotation(float AnimationTime)
	 {
		 assert(animationTransformation.size() > 0);

		 for (unsigned int i = 0; i < animationTransformation.size() - 1; i++) {
			 if (AnimationTime < (float)animationTransformation[i + 1].time) {
				 return i;
			 }
		 }

		 assert(0);

		 return 0;
	 }

};*/