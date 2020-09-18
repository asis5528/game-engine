#pragma once
struct BoneProcessedMat {
	string name;
	glm::mat4 transformedBone;
};
struct AnimationAction {
	string name;
	glm::vec2 range;
	float speed = 1.0;
};
class Animation {
public:

	AnimationData adata;
	float ticksperSec;
	float duration;
	std::vector<BoneProcessedMat> info;
	std::vector<AnimationAction> actions;
	unsigned int actionIndex;
	void initAction() {
		AnimationAction defaultAction;
		defaultAction.name = "defaultAction";
		defaultAction.range = glm::vec2(0.0, duration*24);
		actions.push_back(defaultAction);
		defaultAction.name = "idle";
		defaultAction.range = glm::vec2(0.0, 35);
		actions.push_back(defaultAction);
		defaultAction.name = "walk";
		defaultAction.range = glm::vec2(263, 287.5);
		actions.push_back(defaultAction);
		defaultAction.name = "run";
		defaultAction.range = glm::vec2(289, 298.0);
		actions.push_back(defaultAction);
		defaultAction.name = "idle2";
		defaultAction.range = glm::vec2(81, 149.0);
		actions.push_back(defaultAction);

	}

	void BoneTransform(float time, AnimationData& data) {
		glm::mat4 identity(1.0);
		//readAnimation(time, data, identity);
	}

	void readAnimation(float time, AnimationData& data, const glm::mat4& Parentmatrix) {
		glm::mat4 globalMatrix(1.0);
		bool n1 = data.name == "";
		bool n2 = data.name == "Armature";
		if (!(n2||n1)) {


			float TicksPerSecond = (float)(ticksperSec != 0 ? ticksperSec : 25.0f);
			//std::cout << "Time0 :" << TicksPerSecond << "\n";
			float TimeInTicks = time * TicksPerSecond;
			//	std::cout << "Time1 :" << TimeInTicks << "\n";
				//std::cout << "dura :" << data.duration << "\n";
				//std::cout << "dura2 :" << data.animationTransformation.at(data.animationTransformation.size() - 1).time << "\n";
			float n =duration - data.animationTransformation.at(0).time;
			//std::cout << "n : " << n << "\n";
			float AnimationTime = fmod(TimeInTicks, n);

			AnimationTime += data.animationTransformation.at(0).time;
			//std::cout << "AnimationTime : " << AnimationTime << "\n";
			//AnimationTime = 0.;
			//std::cout << "Timea :" << AnimationTime << "\n";
			//std::cout << "Time1 :" << data.animationTransformation.at(1).time << "\n";

			//std::cout << "Time :" << AnimationTime << "\n";
			glm::vec3 pos;
			CalcInterpolatedPosition(pos, AnimationTime, data);
			//	pos = data.animationTransformation[0].position;
			glm::vec3 sc = data.animationTransformation[0].scale;
			glm::mat4 trans(1.0);
			glm::mat4 scale(1.0);
			glm::quat q;
			CalcInterpolatedRotation(q, AnimationTime, data);
			glm::mat4 rotmat = glm::toMat4(q);

			trans = glm::translate(trans, glm::vec3(pos.x, pos.y, pos.z));
			scale = glm::scale(scale, glm::vec3(sc.x, sc.y, sc.z));
			rotmat = trans * rotmat * scale;
			globalMatrix = Parentmatrix * rotmat;
			//	Parentmatrix = rotmat;
			BoneProcessedMat inf;
			inf.name = data.name;
			inf.transformedBone = globalMatrix;
			info.push_back(inf);
			

		}
		for (int i = 0; i < data.childAnimationData.size(); i++) {

			readAnimation(time, data.childAnimationData[i], globalMatrix);
		}
		//	std::cout << "Time  " << AnimationTime << "\n";
	}

	void CalcInterpolatedRotation(glm::quat& Out, float AnimationTime, AnimationData& data)
	{
		// we need at least two values to interpolate...
		float size = data.animationTransformation.size();
		if (data.animationTransformation.size() == 1) {
			Out = data.animationTransformation[0].rotation;
			return;
		}

		unsigned int RotationIndex = FindRotation(AnimationTime, data);

		unsigned int NextRotationIndex = (RotationIndex + 1);
		assert(NextRotationIndex < size);
		float DeltaTime = (float)(data.animationTransformation[NextRotationIndex].time - data.animationTransformation[RotationIndex].time);
		float Factor = (AnimationTime - (float)data.animationTransformation[RotationIndex].time) / DeltaTime;

		assert(Factor >= 0.0f && Factor <= 1.0f);
		const glm::quat& StartRotationQ = data.animationTransformation[RotationIndex].rotation;
		const  glm::quat& EndRotationQ = data.animationTransformation[NextRotationIndex].rotation;
		//std::cout << "start: " << RotationIndex << "\n";
		//std::cout << "next: " << NextRotationIndex << "\n";
		glm::quat q1;
		q1.w = StartRotationQ.w; q1.x = StartRotationQ.x; q1.y = StartRotationQ.y; q1.z = StartRotationQ.z;
		glm::quat q2;
		q2.w = EndRotationQ.w; q2.x = EndRotationQ.x; q2.y = EndRotationQ.y; q2.z = EndRotationQ.z;
		//std::cout << "k  " << q1.w << "\n";

		aiQuaternion qa = aiQuaternion();
		aiQuaternion qb = aiQuaternion();

		qa.w = q1.w;
		qa.x = q1.x;
		qa.y = q1.y;
		qa.z = q1.z;

		qb.w = q2.w;
		qb.x = q2.x;
		qb.y = q2.y;
		qb.z = q2.z;

		aiQuaternion ou = aiQuaternion();
		aiQuaternion::Interpolate(ou, qa, qb, Factor);
		glm::quat fina = glm::slerp(q1, q2, Factor);

		fina = glm::normalize(fina);
		//aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		//Out = Out.Normalize();
		Out.w = ou.w;
		Out.x = ou.x;
		Out.y = ou.y;
		Out.z = ou.z;
		Out = fina;
	}

	void CalcInterpolatedPosition(glm::vec3& Out, float AnimationTime, AnimationData& data)
	{
		// we need at least two values to interpolate...
		int size = data.animationTransformation.size();
		if (data.animationTransformation.size() == 1) {
			Out = data.animationTransformation[0].position;
			return;
		}

		unsigned int PositionIndex = FindPosition(AnimationTime, data);

		unsigned int NextPositionIndex = (PositionIndex + 1);
		assert(NextPositionIndex < size);
		float DeltaTime = (float)(data.animationTransformation[NextPositionIndex].time - data.animationTransformation[PositionIndex].time);
		float Factor = (AnimationTime - (float)data.animationTransformation[PositionIndex].time) / DeltaTime;

		assert(Factor >= 0.0f && Factor <= 1.0f);
		const glm::vec3 start = data.animationTransformation[PositionIndex].position;
		const  glm::vec3 End = data.animationTransformation[NextPositionIndex].position;
		glm::vec3 Delta = End - start;
		Out = start + Factor * Delta;
	}
	unsigned int FindRotation(float AnimationTime, AnimationData& data)
	{
		assert(data.animationTransformation.size() > 0);
		//std::cout << "animationData: " << AnimationTime << "\n";
		for (unsigned int i = 0; i < data.animationTransformation.size() - 1; i++) {
			if (AnimationTime < (float)data.animationTransformation[i + 1].time) {

				return i;
			}
		}
		//std::cout << "animationData: " << AnimationTime << "\n";
		assert(0);

		return 0;
	}

	unsigned int FindPosition(float AnimationTime, AnimationData& data)
	{
		assert(data.animationTransformation.size() > 0);

		for (unsigned int i = 0; i < data.animationTransformation.size() - 1; i++) {
			if (AnimationTime < (float)data.animationTransformation[i + 1].time) {

				return i;
			}
		}
		//std::cout << "animationData: " << AnimationTime << "\n";
		assert(0);

		return 0;
	}

	unsigned int FindScale(float AnimationTime, AnimationData& data)
	{
		assert(data.animationTransformation.size() > 0);

		for (unsigned int i = 0; i < data.animationTransformation.size() - 1; i++) {
			if (AnimationTime < (float)data.animationTransformation[i + 1].time) {

				return i;
			}
		}
		//std::cout << "animationData: " << AnimationTime << "\n";
		assert(0);

		return 0;
	}

};
