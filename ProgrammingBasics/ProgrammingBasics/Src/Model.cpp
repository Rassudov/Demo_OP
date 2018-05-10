﻿#include "Model.h"
#define cast dynamic_cast


bool Model::GetComponent(const ID& id, BinSearchTree<ID, ID>& component) {

	if (currentComponent != nullptr && currentComponent->Find(id).IsValid()) {
		component = *currentComponent;
		return true;
	}

	auto labels = new BinSearchTree<ID, ID>;
	Queue<ID> queue;
	if (!dataLink.Find(id).IsValid()) {
		return false;
	}

	queue.push(id);
	labels->Add(id, id);

	while (!queue.isEmpty()) {
		ID currentID = queue.pop();
		auto dataLinkMarker = dataLink.Find(currentID);
		if (dataLinkMarker.IsValid()) {
			for (auto l = dataLinkMarker.GetValue()->GetMarker(); l.IsValid(); ++l) {
				currentID = l.GetValue();
				if (!labels->Find(currentID).IsValid()) {
					labels->Add(currentID, currentID);
					queue.push(currentID);
				}
			}
		}
	}
	component = *labels;

	if (currentComponent != nullptr) {
		delete currentComponent;
	}
	currentComponent = labels;
	return true;
}

void Model::NewComponent(const ID& id, Array<ID>& Prims, Array<ID>& Reqs)
{
	delete currentComponent;
	currentComponent = new BinSearchTree<ID, ID>;
	Queue<ID> queuePrim;
	Queue<ID> queueReq;
	ID currentID;

	auto tempMarker = dataPrim.Find(id);
	if (tempMarker.IsValid()) {
		// get ID Requirement
		currentID = dataLink.Find(id).GetValue()->GetMarker().GetValue();
	}
	else {
		currentID = id;
	}

	queueReq.push(currentID);
	while (!queueReq.isEmpty())
	{
		while (!queueReq.isEmpty())
		{
			currentID = queueReq.pop();
			currentComponent->Add(currentID, currentID);
			Reqs.PushBack(currentID);
			
			auto marker = dataLink.Find(currentID);
			if (marker.IsValid()) {
				for (auto l = marker.GetValue()->GetMarker(); l.IsValid(); ++l) {
					currentID = l.GetValue();
					if (!currentComponent->Find(currentID).IsValid()) {
						queuePrim.push(currentID);
					}
				}
			}
		}
		while (!queuePrim.isEmpty())
		{
			currentID = queuePrim.pop();
			currentComponent->Add(currentID, currentID);
			Prims.PushBack(currentID);

			auto marker = dataLink.Find(currentID);
			if (marker.IsValid()) {
				for (auto l = marker.GetValue()->GetMarker(); l.IsValid(); ++l) {
					currentID = l.GetValue();
					if (!currentComponent->Find(currentID).IsValid()) {
						queueReq.push(currentID);
					}
				}
			}
		}
	}
}

bool Model::GetRequirements(Array<ID>& ids, Array<Requirement*>& req) {
	for (int i = 0; i < ids.GetSize; ++i) {
		
		req.PushBack(dataReq.Find(ids[i]).GetValue());
	}
	return (req.GetSize() != ids.GetSize());
}

bool Model::GetPrimitives(Array<ID>& ids, Array<Primitive*>& prim) {
	for (int i = 0; i < ids.GetSize; ++i) {

		prim.PushBack(dataPrim.Find(ids[i]).GetValue());
	}
	return (prim.GetSize() != ids.GetSize());
}


bool Model::GetRequirementsFromComponent(BinSearchTree<ID, ID>& component, Array<Requirement*>& reqs) {
	for (auto i = component.GetMarker(); i.IsValid(); ++i) {
		auto reqsMarker = dataReq.Find(i.GetValue());
		if (reqsMarker.IsValid()) {
			reqs.PushBack(reqsMarker.GetValue());
		}
	}
	return (reqs.GetSize() != 0);
}

bool Model::GetPrimitivesFromComponent(BinSearchTree<ID, ID>& component, Array<Primitive*>& prims) {
	for (auto i = component.GetMarker(); i.IsValid(); ++i) {
		auto primsMarker = dataPrim.Find(i.GetValue());
		if (primsMarker.IsValid()) {
			prims.PushBack(primsMarker.GetValue());
		}
	}
	return (prims.GetSize() != 0);
}


void Model::ConnectPrimitives(Primitive* point, Primitive* prim) {
	Requirement* _connection = new ConnectionReq;
	Array<Primitive*> prims(2);
	prims[0] = point;
	prims[1] = prim;
	CreateLink(_connection->GetID(), prims);
}

bool Model::CreateObject(const object_type type, Array<double>& params, ID& obj_id) {
	switch (type)
	{
	case point_t: {
		if (params.GetSize() != 2) {
			return false;
		}
		Point* _point;
		_point = new Point(params[0], params[1]);
		dataPrim.Add(_point->GetID(), _point);
		obj_id = _point->GetID();
		return true;
	}
	case segment_t: {
		if (params.GetSize() != 4) {
			return false;
		}
		Point* p1 = new Point(params[0], params[1]);
		Point* p2 = new Point(params[2], params[3]);

		Segment* _seg = new Segment(p1, p2);

		// points belong to segment
		p1->SetParent(_seg);
		p2->SetParent(_seg);

		dataPrim.Add(p1->GetID(), p1);
		dataPrim.Add(p2->GetID(), p2);
		dataPrim.Add(_seg->GetID(), _seg);

		ConnectPrimitives(p1, _seg);
		ConnectPrimitives(p2, _seg);

		obj_id = _seg->GetID();
		return true;
	}
	case arc_t: {
		if (params.GetSize() != 5) {
			return false;
		}
		Point* p1 = new Point(params[0], params[1]);
		Point* p2 = new Point(params[2], params[3]);

		Arc* _arc = new Arc(p1, p2, params[4]);

		// points belong to arc
		p1->SetParent(_arc);
		p2->SetParent(_arc);

		dataPrim.Add(p1->GetID(), p1);
		dataPrim.Add(p2->GetID(), p2);
		dataPrim.Add(_arc->GetID(), _arc);

		ConnectPrimitives(p1, _arc);
		ConnectPrimitives(p2, _arc);

		obj_id = _arc->GetID();
		return true;
	}
	default:
		return false;
	}
}

bool Model::DeleteRequirement(const ID& req_id) {
	auto dataReqMarker = dataReq.Find(req_id);
	if (!dataReqMarker.IsValid()) {
		return false;
	}

	auto dataLinkMarker = dataLink.Find(req_id);
	if (!dataLinkMarker.IsValid()) {
		throw std::exception("Invalid link requirement->primitive");
	}

	auto listPrim = dataLink.GetMarker().GetValue();
	dataLinkMarker.DeleteCurrent();

	for (auto i = listPrim->GetMarker(); i.IsValid(); ++i) {
		dataLinkMarker = dataLink.Find(i.GetValue());
		if (!dataLinkMarker.IsValid()) {
			throw std::exception("Invalid link primitive->requirement");
		}
		auto listReq = dataLinkMarker.GetValue();
		listReq->Find(req_id).DeleteCurrent();
	}

	Requirement* req = dataReqMarker.GetValue();
	dataReqMarker.DeleteCurrent();
	delete req;
	delete listPrim;


	return true;
}

bool Model::DeletePrimitive(const ID& prim_id) {
	auto dataPrimMarker = dataPrim.Find(prim_id);
	if (!dataPrimMarker.IsValid()) {
		return false;
	}

	Primitive* prim = dataPrimMarker.GetValue();
	auto dataLinkMarker = dataLink.Find(prim_id);
	if (!dataLinkMarker.IsValid()) {
		delete prim;
		dataPrimMarker.DeleteCurrent();
		return true;
	}

	auto list = dataLinkMarker.GetValue();

	while (list->GetSize() != 0) {
		DeleteRequirement(list->GetMarker().GetValue());
	}

	delete list;

	dataPrimMarker.DeleteCurrent();
	dataLinkMarker.DeleteCurrent();

	if (prim->GetType() == point_t) {
		Point* point = cast<Point*>(prim);
		if (point->GetParent() != nullptr) {
			bool result = DeletePrimitive(point->GetParent()->GetID());
			delete prim;
			return result;
		}
	}
	delete prim;
	return DeletePrimitive(true);
}

//bool Model::createSegment(ID& p1ID, ID& p2ID, ID& segID) {
//	Primitive* point1PR;
//	Primitive* point2PR;
//	bool error = false;
//	if (dataPrim.Find(p1ID)) {
//		point1PR = dataPrim.GetCurrent();
//		if (dataPrim.Find(p2ID)) {
//			point2PR = dataPrim.GetCurrent();
//			if ((point1PR->GetType() == point) && (point2PR->GetType() == point)) {
//				Point* point1 = dynamic_cast<Point*>(point1PR);
//				Point* point2 = dynamic_cast<Point*>(point2PR);
//				Segment* segment = new Segment(point1, point2);
//				segID = segment->GetID();
//				dataPrim.Add(segID, segment);
//				return true;
//			}
//		}
//	}
//	return false;
//}

bool Model::CreateRequirementByID(const object_type type, Array<ID>& id_arr, Array<double>& params) {
	Array<Primitive*> primitives;
	for (int i = 0; i < id_arr.GetSize(); ++i) {
		auto marker = dataPrim.Find(id_arr[i]);
		if (!marker.IsValid()) {
			return false;
		}
		primitives.PushBack(marker.GetValue());
	}
	return CreateRequirement(type, primitives, params);
}

bool Model::CreateRequirement(object_type type, Array<Primitive*>& primitives, Array<double>& params) {
	Requirement* requirement;
	switch (type)
	{
	case distBetPoints_t: {
		// verification parameters
		if ((primitives.GetSize() != 2)
			&& (params.GetSize() != 1)
			&& (primitives[0]->GetType() != point_t)
			&& (primitives[1]->GetType() != point_t)) {
			return false;
		}
		// creating
		requirement = new DistBetPointsReq(cast<Point*>(primitives[0]),
			cast<Point*>(primitives[1]),
			params[0]);
		break;
	}
	case equalSegmentLen_t: {
		// verification parameters
		if ((primitives.GetSize() != 2)
			&& (params.GetSize() != 0)
			&& (primitives[0]->GetType() != segment_t)
			&& (primitives[1]->GetType() != segment_t)) {
			return false;
		}
		// creating
		requirement = new EqualSegmentLenReq(*cast<Segment*>(primitives[0]),
			*cast<Segment*>(primitives[1]));
		break;
	}
	case pointsOnTheOneHand: {
		// verification parameters
		if ((primitives.GetSize() != 3)
			&& (params.GetSize() == 0)
			&& (primitives[0]->GetType() == segment_t)
			&& (primitives[1]->GetType() == point_t)
			&& (primitives[2]->GetType() == point_t))
		{
			return false;
		}
		// creating
		requirement = new PointsOnTheOneHand(*cast<Segment*>(primitives[0]),
			*cast<Point*>(primitives[1]),
			*cast<Point*>(primitives[2]));
		break;
	}
	case distBetPointSeg: {
		// verification parameters
		if ((primitives.GetSize() != 2)
			&& (params.GetSize() != 1)
			&& (primitives[0]->GetType() != segment_t)
			&& (primitives[1]->GetType() != point_t))
		{
			return false;
		}
		// creating
		requirement = new DistanceBetweenPointSegment(*cast<Segment*>(primitives[0]),
			*cast<Point*>(primitives[1]),
			params[0]);
		break;
	}
	case angleBetSeg: {
		// verification parameters
		if ((primitives.GetSize() != 2)
			&& (params.GetSize() == 1)
			&& (primitives[0]->GetType() == segment_t)
			&& (primitives[1]->GetType() == segment_t))
		{
			return false;
		}
		// creating
		requirement = new AngleBetweenSegments(*cast<Segment*>(primitives[0]),
			*cast<Segment*>(primitives[1]),
			params[0]);
		break;
	}
	case distBetPointArc: {
		// verification parameters
		if ((primitives.GetSize() != 2)
			&& (params.GetSize() != 1)
			&& (primitives[0]->GetType() != arc_t)
			&& (primitives[1]->GetType() != point_t))
		{
			return false;
		}
		// creating
		requirement = new DistanceBetweenPointArc(*cast<Arc*>(primitives[0]),
			*cast<Point*>(primitives[1]),
			params[0]);
		break;
	}
	case pointInArc: {
		// verification parameters
		if ((primitives.GetSize() != 2)
			&& (params.GetSize() != 0)
			&& (primitives[0]->GetType() != arc_t)
			&& (primitives[1]->GetType() != point_t))
		{
			return false;
		}
		// creating
		requirement = new PointInArc(*cast<Arc*>(primitives[0]),
			*cast<Point*>(primitives[1]));
		break;
		}
					 //case correctTriangle: {
					 //	CorrectTriangle* Requirement;
					 //	if ((primitives[0]->GetType() == segment)
					 //		&& (primitives[1]->GetType() == segment)
					 //		&& (primitives[2]->GetType() == segment)
					 //		&& (params.GetSize() > 0)) {
					 //		Requirement = new ÑorrectTriangle(dynamic_cast<Segment*>(primitives[0]),
					 //			dynamic_cast<Segment*>(primitives[1]),
					 //			dynamic_cast<Segment*>(primitives[2]),
					 //			params[0]);
					 //		dataPrimReq.PushBack(Requirement);
					 //		return true;
					 //	}
					 //	else {
					 //		return false;
					 //	}
					 //}
					 //case nsAngle: {
					 //	NsAngle* Requirement;
					 //	ListE<Segment*> list;
					 //	for (int i = 0; i < primitives.GetSize(); ++i) {
					 //		if (segment == primitives[i]->GetType()) {
					 //			list.PushTail(dynamic_cast<Segment*>(primitives[i]));
					 //		}
					 //		else {
					 //			return false;
					 //		}
					 //	}
					 //	Requirement = new NsAngle(list);
					 //	dataPrimReq.PushBack(Requirement);
					 //	return true;
					 //}
					 //case correctNsAngle: {
					 //	CorrectNsAngle* Requirement;
					 //	if (params.GetSize() > 0) {
					 //		ListE<Segment*> list;
					 //		for (int i = 0; i < primitives.GetSize(); ++i) {
					 //			if (segment == primitives[i]->GetType()) {
					 //				list.PushTail(dynamic_cast<Segment*>(primitives[i]));
					 //			}
					 //			else {
					 //				return false;
					 //			}
					 //		}
					 //		Requirement = new CorrectNsAngle(list, params[0]);
					 //		dataPrimReq.PushBack(Requirement);
					 //		return true;
					 //	}
					 //	else {
					 //		return false;
					 //	}
					 //}
					 //}
	default:
		return false;
	}

	dataReq.Add(requirement->GetID(), requirement);

	CreateLink(requirement->GetID(), primitives);

	OptimizeByID(requirement->GetID());

	return true;
}

void Model::CreateLink(const ID& IDreq, Array<Primitive*>& primitives) {
	// create link Prim on Req
	for (int i = 0; i < primitives.GetSize(); ++i) {
		auto markerLink = dataLink.Find(primitives[i]->GetID());
		if (markerLink.IsValid()) {
			markerLink.GetValue()->PushTail(IDreq);
		}
		else {
			auto newList = new List<ID>;
			newList->PushTail(IDreq);
			dataLink.Add(primitives[i]->GetID(), newList);
		}
	}
	// create link Req on Prim
	List<ID>* list = new List<ID>;
	for (int i = 0; i < primitives.GetSize(); ++i) {
		list->PushTail(primitives[i]->GetID());
	}
	dataLink.Add(IDreq, list);
}



bool Model::DischargeInfoObjects(Array<infoObject>& dataPrimInfoObjects)
{
	if (dataPrim.GetSize() == 0) {
		return false;
	}
	auto dataPrimMarker = dataPrim.GetMarker();
	do
	{
		infoObject temp;
		GetObjParam(dataPrimMarker.GetValue()->GetID(), temp.params);
		GetObjType(dataPrimMarker.GetValue()->GetID(), temp.type);
		dataPrimInfoObjects.PushBack(temp);
	} while (++dataPrimMarker);
	return true;
}

// optimize

double Model::GetError(const Array<Requirement*>& requirments) const {
	double error = 0.0;
	for (int i = 0; i < requirments.GetSize(); ++i) {
		error += requirments[i]->error();
	}
	return error / requirments.GetSize();
}

double Model::ErrorByAlpha(const Array<Requirement*>& req, const Array<double*>& params, const Array<double>& aGrad, double alpha) {
	for (int i = 0; i < params.GetSize(); ++i) {
		double delta = aGrad[i];
		*(params[i]) += delta * alpha;
	}
	double error = GetError(req);
	for (int i = 0; i < params.GetSize(); ++i) {
		*(params[i]) -= aGrad[i] * alpha;
	}
	return error;
}

void Model::OptimizeByGradient(const Array<Requirement*>& requirments, const Array<double*>& params, const Array<double>& aGradient) {

	const double gold_section = 1.6180339887498;
	int reqSize = requirments.GetSize();

	double error = GetError(requirments);

	double left = 0.0;
	double right = 0.1;

	double leftValue = ErrorByAlpha(requirments, params, aGradient, left);
	double rightValue = ErrorByAlpha(requirments, params, aGradient, right);

	double x1 = right - (right - left) / gold_section;
	double x2 = left + (right - left) / gold_section;

	double x1_Value = ErrorByAlpha(requirments, params, aGradient, x1);
	double x2_Value = ErrorByAlpha(requirments, params, aGradient, x2);

	while (abs(leftValue - rightValue) > OPTIM_GRAD_EPS * error) {

		if (x1_Value > x2_Value) {
			left = x1;
			leftValue = x1_Value;
			x1 = x2;
			x1_Value = x2_Value;
			x2 = left + (right - left) / gold_section;
			x2_Value = ErrorByAlpha(requirments, params, aGradient, x2);
		}
		else {
			right = x2;
			rightValue = x2_Value;
			x2 = x1;
			x1 = right - (right - left) / gold_section;
			x2_Value = x1_Value;
			x1_Value = ErrorByAlpha(requirments, params, aGradient, x1);
		}
	}

	double alpha = (leftValue < rightValue) ? left : right;

	for (int i = 0; i < params.GetSize(); ++i) {
		(*params[i]) += aGradient[i] * alpha;
	}
}

void Model::OptimizeRequirements(const Array<Requirement*>& requirments) {
	// get parameters number
	int params_number = 0;
	for (int i = 0; i < requirments.GetSize(); ++i) {
		Array<double*> params = requirments[i]->GetParams();
		params_number += params.GetSize();
	}

	class UniqueParam {
	private:
	public:
		double* param;
		int unique_number;
	};

	//ctreating match_array, which will establish a correspondence
	//between not-unique parameters and their unique numbers

	Array<double*> all_parameters(params_number);
	BinSearchTree<double*, UniqueParam> set;
	int* match_array = new int[params_number];

	// from 0 to params_number
	int all_parameters_iterator = 0;

	// from 0 to number of unique double*
	int uniq_numbers_iterator = 0;

	// filling match_array
	for (int i = 0; i < requirments.GetSize(); ++i) {

		Array<double*> currentRequirmentParams = requirments[i]->GetParams();

		for (int j = 0; j < currentRequirmentParams.GetSize(); ++j) {

			double* currentParameter = currentRequirmentParams[j];
			all_parameters[all_parameters_iterator] = currentParameter;

			UniqueParam unique_param;
			bool notUniqueParam = set.Find(currentParameter).IsValid();

			if (notUniqueParam) {
				match_array[all_parameters_iterator] = unique_param.unique_number;
			}
			else {
				unique_param = UniqueParam{ currentParameter, uniq_numbers_iterator };
				set.Add(currentParameter, unique_param);
				match_array[all_parameters_iterator] = uniq_numbers_iterator;
				uniq_numbers_iterator++;
			}
			all_parameters_iterator++;
		}
	}

	// array of unique_parameters
	Array<double*> unique_parameters(uniq_numbers_iterator, nullptr);

	for (int i = 0; i < params_number; ++i) {
		unique_parameters[match_array[i]] = all_parameters[i];
	}

	// anti gradient
	Array<double> aGradient(uniq_numbers_iterator, 0.0);

	//filling anti gradient
	double err = GetError(requirments);
	while (GetError(requirments) > OPTIM_EPS) {
		int match_array_iterator = 0;
		for (int i = 0; i < requirments.GetSize(); ++i) {
			Array<double> currentGradient = requirments[i]->gradient();
			for (int j = 0; j < currentGradient.GetSize(); ++j) {
				aGradient[match_array[match_array_iterator]] -= currentGradient[j] / requirments.GetSize();
				++match_array_iterator;
			}
		}

		OptimizeByGradient(requirments, unique_parameters, aGradient);
		aGradient.FillDefault(0.0);
		err = GetError(requirments);
	}
}

void Model::OptimizeByID(const ID& id) {
	BinSearchTree<ID, ID> component;
	if(!GetComponent(id, component)) {
		return;
	}
	Array<Requirement*> reqs;
	if (!GetRequirementsFromComponent(component, reqs)) {
		return;
	}
	OptimizeRequirements(reqs);
}

void Model::OptitmizeNewton(const ID& id) {
	Array<ID> primID;
	Array<ID> reqID;
	Array<Primitive*> prim;
	Array<Requirement*> req;
	Array<double*> params;
	NewComponent(id, primID, reqID);
	GetRequirements(reqID, req);
	GetPrimitives(primID, prim);
	GetDoublesForOptimize(prim, params);


}

void Model::GetDoublesForOptimize(Array<Primitive*>& prims, Array<double*>& params) {
	for (int i = 0; i < prims.GetSize(); ++i) {
		if (prims[i]->GetType() == point_t) {
			Point* point = cast<Point*>(prims[i]);
			params.PushBack(&point->position.x);
			params.PushBack(&point->position.y);
		}
		if (prims[i]->GetType() == arc_t) {
			Arc* arc = cast<Arc*>(prims[i]);
			params.PushBack(&arc->angle);
		}
	}
}

void Model::GetDifferential(const Array<Requirement*>& reqs, Array<double*>& params, Array<double>& diff) {
	double begin = GetError(reqs);
	for (int i = 0; i < params.GetSize(); ++i) {
		*params[i] += DELTA_X;
		diff[i] = (begin - GetError(reqs)) / DELTA_X;
		*params[i] -= DELTA_X;
	}
}

//

bool Model::GetObjType(const ID& obj_id, object_type& type) {
	Primitive* obj = nullptr;
	auto dataPrimMarker = dataPrim.Find(obj_id);

	if (dataPrimMarker.IsValid()) {
		obj = dataPrimMarker.GetValue();
		type = obj->GetType();
		return true;
	}
	else {
		return false;
	}
}

bool Model::GetObjParam(const ID& obj_id, Array<double>& result) {
	Primitive* obj = nullptr;
	auto dataPrimMarker = dataPrim.Find(obj_id);
	if (dataPrimMarker.IsValid()) {
		obj = dataPrimMarker.GetValue();
		switch (obj->GetType()) {
		case point_t: {
			Point* point = dynamic_cast<Point*>(obj);
			result.Clear();
			Vector2 pos = point->GetPosition();
			result.PushBack(pos.x);
			result.PushBack(pos.y);
			return true;
			break;
		}
		case segment_t: {
			Segment* segment = dynamic_cast<Segment*>(obj);
			result.Clear();
			Vector2 pos1 = segment->GetPoint1_pos();
			Vector2 pos2 = segment->GetPoint2_pos();
			result.PushBack(pos1.x);
			result.PushBack(pos1.y);
			result.PushBack(pos2.x);
			result.PushBack(pos2.y);
			return true;
			break;
		}
		case arc_t: {
			Arc* arc = dynamic_cast<Arc*>(obj);
			result.Clear();
			Vector2 pos1 = arc->GetPoint1_pos();
			Vector2 pos2 = arc->GetPoint2_pos();
			double angle = arc->GetAngle();
			result.PushBack(pos1.x);
			result.PushBack(pos1.y);
			result.PushBack(pos2.x);
			result.PushBack(pos2.y);
			result.PushBack(angle);
			return true;
			break;
		}
		default: {
			return false;
		}
		}
	}
	return false;

}

#define SEARCH_AREA 3.0
bool Model::GetObject(double x, double y, Array<ID>& obj_id, Array<double>& distances) {
	bool isFound = false;
	for (auto i = dataPrim.GetMarker(); i.IsValid(); ++i) {
		double dist = i.GetValue()->GetDistance(Vector2(x, y));
		if (dist < SEARCH_AREA) {
			isFound = true;
			distances.PushBack(dist);
			obj_id.PushBack(i.GetValue()->GetID());
		}
	}
	return isFound;
}
