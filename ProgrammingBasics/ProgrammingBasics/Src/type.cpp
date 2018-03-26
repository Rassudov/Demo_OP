#include "Type.h"

Type::Type(const type_id _id) {
	switch (_id)
	{
	case point: {
		name = "Point";
		break;
	}
	case segment: {
		name = "Segment";
		break;
	}
	case arc: {
		name = "Arc";
		break;
	}
	default:
		/*throw unknown type_id*/
		break;
	}
	id = _id;
}

std::string Type::GetName() const {
	return name;
}

type_id Type::GetTypeId() const {
	return id;
}