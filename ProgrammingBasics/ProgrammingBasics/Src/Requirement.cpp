#include <Requirement.h>

#pragma region Requirement
Requirement::Requirement(object_type _type, const Array<double>& _params, const Array<ID>& _children) :
	Object(_type, _params, _children), args(0)
{
	primCtrl = PrimController::GetInstance();
}

Array<double*> Requirement::GetArgs() const {
	return args;
}

Array<double> Requirement::Gradient() {
	Array<double> grad(0);
	double err = error();
	for (int i = 0; i < objects.GetSize(); ++i) {
		Array<double*> activeArgs = primCtrl->GetPrimitiveDoubleParamsAsPointers(objects[i]);
		for (int j = 0; j < activeArgs.GetSize(); ++j) {
			double test = *activeArgs[j];
			(*activeArgs[j]) += DELTA_X;
			double delta_error_right = error();
			(*activeArgs[j]) -= DELTA_X;
			//(*arguments[i]) -= 2 * DELTA_X;
			//double delta_error_left = error();

			test = (delta_error_right - err) / (DELTA_X);
			//test = (delta_error_right - delta_error_left) / (DELTA_X * 2);
			grad = grad + test;
		}
	}
	return grad;
}
#pragma endregion

#pragma region DistBetPointsReq
DistBetPointsReq::DistBetPointsReq(const Array<ID>& _children, const Array<double>& _params) :
	Requirement(ot_distBetPoints, _params, _children)
{
	objects = _children;

	for (int i = 0; i < _children.GetSize(); ++i) {
		args += primCtrl->GetPrimitiveDoubleParamsAsPointers(objects[i]);
	}
}

double DistBetPointsReq::error() {
	Vector2 vectorAB(*(args[2]) - *(args[0]), *(args[3]) - *(args[1]));
	double modAB_inSquare = Vector2::Dot(vectorAB, vectorAB);
	return modAB_inSquare - params[0] * (2 * sqrt(modAB_inSquare) - params[0]);
}
#pragma endregion

#pragma region EqualSegment
EqualSegmentLenReq::EqualSegmentLenReq(const Array<ID>& _objects,
	const Array<double>& _params) :
	Requirement(ot_equalSegmentLen, _params, _objects)
{
	objects = primCtrl->GetChildren(_objects[0]) +
		primCtrl->GetChildren(_objects[1]);

	for (int i = 0; i < objects.GetSize(); ++i) {
		args += primCtrl->GetPrimitiveDoubleParamsAsPointers(objects[i]);
	}
}

double EqualSegmentLenReq::error() {
	Vector2 vec1(*(args[3]) - *(args[1]), *(args[2]) - *(args[0]));
	Vector2 vec2(*(args[7]) - *(args[5]), *(args[6]) - *(args[4]));

	double divergence = vec2.GetLength() - vec1.GetLength();
	return divergence * divergence;
}
#pragma endregion

#pragma region PointPosReq
PointPosReq::PointPosReq(const Array<ID>& _objects, const Array<double>& _params) :
	Requirement(ot_pointPosReq, _params, _objects) {

	for (int i = 0; i < _objects.GetSize(); ++i) {
		args += primCtrl->GetPrimitiveDoubleParamsAsPointers(_objects[i]);
	}
}

double PointPosReq::error() {
	Vector2 vector(*args[0] - params[0], *args[1] - params[1]);
	return Vector2::Dot(vector, vector);
}
#pragma endregion

#pragma region PointsOnTheOneHand
PointsOnTheOneHand::PointsOnTheOneHand(const Array<ID>& _objects, const Array<double>& _params) :
	Requirement(ot_pointsOnTheOneHand, _params, _objects)
{
	objects = primCtrl->GetChildren(_objects[0]);
	objects.PushBack(_objects[1]);
	objects.PushBack(_objects[2]);

	for (int i = 0; i < objects.GetSize(); ++i) {
		args += primCtrl->GetPrimitiveDoubleParamsAsPointers(objects[i]);
	}
}

double PointsOnTheOneHand::error() {
	Vector2 segment = Vector2(*(args[2]) - *(args[0]), *(args[3]) - *(args[1]));
	Vector2 point1 = Vector2(*(args[4]) - *(args[0]), *(args[5]) - *(args[1]));
	Vector2 point2 = Vector2(*(args[6]) - *(args[0]), *(args[7]) - *(args[1]));

	if (Vector2::Cross(segment, point1) * Vector2::Cross(segment, point2) >= 0) {
		return 0;
	}

	double length = segment.GetLength();

	double distance1 = abs(Vector2::Cross((point1 * -1), segment - point1)) / length;
	double distance2 = abs(Vector2::Cross((point2 * -1), segment - point2)) / length;
	if (distance1 > distance2) {
		return distance2 * distance2;
	}
	else {
		return distance1 * distance1;
	}
}
#pragma endregion

#pragma region DistanceBetweenPointSegment
// _objects = <segment, point>
DistanceBetweenPointSegment::DistanceBetweenPointSegment(const Array<ID>& _objects,
	const Array<double>& _params) :
	Requirement(ot_distBetPointSeg, _params, _objects)
{
	objects = primCtrl->GetChildren(_objects[0]);
	objects.PushBack(_objects[1]);

	for (int i = 0; i < objects.GetSize(); ++i) {
		args += primCtrl->GetPrimitiveDoubleParamsAsPointers(objects[i]);
	}
}

double DistanceBetweenPointSegment::error() {
	Vector2 segment = Vector2(*(args[2]) - *(args[0]), *(args[3]) - *(args[1]));
	Vector2 point = Vector2(*(args[4]) - *(args[0]), *(args[5]) - *(args[1]));
	Vector2 segToPoint = Vector2(*(args[4]) - *(args[2]), *(args[5]) - *(args[3]));


	
	double dotProduct1 = Vector2::Dot(point, segment);
	double dotProduct2 = Vector2::Dot(segToPoint, segment);
	dotProduct2 *= -1;

	if (dotProduct1 * dotProduct2 >= 0) {
		double distance = abs(Vector2::Cross((point * -1), segment - point)) / segment.GetLength();

		return distance * distance - 2 *params[0] * distance + params[0] * params[0];
	}
	else {
		double answer = Vector2::Dot(point, point);
		if (answer > Vector2::Dot(segToPoint, segToPoint)) {
			answer = Vector2::Dot(segToPoint, segToPoint);
		}
		return answer - 2 * sqrt(answer) * params[0] + params[0] * params[0];
	}
	
	
}

Array<double> DistanceBetweenPointSegment::gradient() {
	return Array<double>();
}
#pragma endregion

#pragma region AngleBetweenSegments
AngleBetweenSegments::AngleBetweenSegments(const Array<ID>& _objects,
	const Array<double>& _params) :
	Requirement(ot_angleBetSeg, _params, _objects)
{
	objects = primCtrl->GetChildren(_objects[0]) +
		primCtrl->GetChildren(_objects[1]);

	for (int i = 0; i < objects.GetSize(); ++i) {
		args += primCtrl->GetPrimitiveDoubleParamsAsPointers(objects[i]);
	}
}

double AngleBetweenSegments::error() {
	if (abs(params[0] - angle) - DELTA_X > 0) {
		ChangeParams();
	}
	Vector2 vec1(*args[2] - *args[0], *args[3] - *args[1]);
	Vector2 vec2(*args[6] - *args[4], *args[7] - *args[5]);

	double length1 = vec1.GetLength();
	double length2 = vec2.GetLength();

	// smaller side triangle:
	Vector2 vec3 = vec1 - vec2;
	if (Vector2::Dot(vec3, vec3) > length1 * length1 + length2 * length2 + DELTA_X) {
		vec3 = (vec1 * -1) - vec2;
	}
	//
	double length3 = sqrt(length1 * length1 + length2 * length2 - 2 * cosinus * length2 * length1);

	double different = length3 - vec3.GetLength();
	return different * different;
}

void AngleBetweenSegments::ChangeParams() {
	angle = params[0];
	if (angle  == 90) {
		cosinus = 0;
		sinus = 1;
	}
	else if (angle == 0) {
		cosinus = 1;
		sinus = 0;
	}
	else {
		cosinus = abs(cos((angle / 180) * PI));
		sinus = sin((angle / 180) * PI);
	}
}
#pragma endregion

#pragma region DistanceBetweenPointArc
DistanceBetweenPointArc::DistanceBetweenPointArc(const Array<ID>& _objects,
	const Array<double>& _params) :
	Requirement(ot_distBetPointArc, _params, _objects)
{}

double DistanceBetweenPointArc::error() {
	/*Vector2 vector(point->pos - arc->GetCenter());
	double modVector_inSquare = Vector2::Dot(vector, vector);
	return modVector_inSquare - params[0] * (2 * sqrt(modVector_inSquare) - params[0]);*/
	return 0;
}
#pragma endregion

#pragma region SegmentTouchCircle
SegmentTouchCircle::SegmentTouchCircle(const Array<ID>& _objects,
	const Array<double>& _params) :
	Requirement(ot_segmentTouchCircle, _params, _objects)
{
	Array<ID> objs =
		primCtrl->GetChildren(_objects[0]) +
		primCtrl->GetChildren(_objects[1]) +
		_objects[1];

	for (int i = 0; i < objs.GetSize(); ++i) {
		args = args + primCtrl->GetPrimitiveDoubleParamsAsPointers(objs[i]);
	}
}

double SegmentTouchCircle::error() {
	return 0.0;
}
#pragma endregion

#pragma region PointInArc
PointInArc::PointInArc(const Array<ID>& _objects,
	const Array<double>& _params) :
	Requirement(ot_pointInArc, _params, _objects)
{}

// return distance to arc and angle
double PointInArc::error() {
	/*Vector2 center = arc->GetCenter();
	Vector2 vec1 = arc->GetPointPos1() - center;
	Vector2 vecPoint = point->GetPos() - center;
	double angle = Vector2::Angle(vec1, vecPoint);
	double angleArc = arc->GetAngle();
	if (angle <= angleArc) {
		return 0;
	}
	if (angle <= PI + angleArc / 2) {
		if (angle < angleArc + PI / 2) {
			return point->GetDist(center) * sin(angle - angleArc);
		}
		else {
			return point->GetDist(center) + angle;
		}
	}
	else {
		if (angle < PI / 2) {
			return point->GetDist(center) * sin(angle);
		}
		else {
			return point->GetDist(center);
		}
	}*/
	return 0;
}
#pragma endregion