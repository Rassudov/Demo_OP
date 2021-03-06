#include "Presenter.h"
#include "Mode.h"

Array<ID> CreatePoint(const Array<ID>& obj, const Array<double>& params) {
	if (params.GetSize() != 2) {
		return Array<ID>(0);
	}
	ID id = Model::GetInstance()->CreatePrimitive(ot_point, params);
	return CreateArr(id);
}
Array<ID> CreateSegment(const Array<ID>& obj, const Array<double>& params) {
	if (params.GetSize() != 4) {
		return Array<ID>(0);
	}
	ID id = Model::GetInstance()->CreatePrimitive(ot_segment, params);
	auto res = CreateArr(id);
	res += PrimController::GetInstance()->GetChildren(id);
	return res;
}
Array<ID> CreateArc(const Array<ID>& obj, const Array<double>& params) {
	if (params.GetSize() != 6) {
		return Array<ID>(0);
	}
	Vector2 center(params[0], params[1]);
	Vector2 start(params[2], params[3]);
	Vector2 end(params[4], params[5]);
	double radius1 = (start - center).GetLength();
	double radius2 = (end - center).GetLength();
	double angle;
	if (abs(radius2) > EPS) {
		end = center + (end - center) / radius2 * radius1;
		angle = Vector2::Angle(start - center, end - center);
	}
	else {
		end = start;
		angle = 2 * PI;
		radius2 = radius1;
	}
	Array<double> arcParams(5);
	arcParams[0] = start.x;
	arcParams[1] = start.y;
	arcParams[2] = end.x;
	arcParams[3] = end.y;
	arcParams[4] = angle;
	ID id = Model::GetInstance()->CreatePrimitive(ot_arc, arcParams);
	auto res = CreateArr(id);
	res += PrimController::GetInstance()->GetChildren(id);
	return res;
}
Array<ID> CreateCircle(const Array<ID>& obj, const Array<double>& params) {
	if (params.GetSize() != 3) {
		return Array<ID>(0);
	}
	ID id = Model::GetInstance()->CreatePrimitive(ot_circle, params);
	auto res = CreateArr(id);
	res += PrimController::GetInstance()->GetChildren(id);
	return res;
}
Array<ID> Move(const Array<ID>& obj, const Array<double>& params) {
	if (params.GetSize() != 2) {
		throw std::exception("invalid arguments");
	}
	Model::GetInstance()->Move(obj, Vector2(params[0], params[1]));
	return Array<ID>(0);

}
Array<ID> Scale(const Array<ID>& obj, const Array<double>& params) {
	if (params.GetSize() != 1) {
		throw std::exception("invalid arguments");
	}
	Model::GetInstance()->Scale(obj, params[0]);
	Model::GetInstance()->Scale(obj, params[0]);
	return Array<ID>(0);
}
Array<ID> DistBetPoints(const Array<ID>& obj, const Array<double>& params) {
	ID id = Model::GetInstance()->CreateRequirement(ot_distBetPoints, obj, params);
	return CreateArr(id);
}
Array<ID> EqualSegment(const Array<ID>& obj, const Array<double>& params) {
	ID id = Model::GetInstance()->CreateRequirement(ot_equalSegmentLen, obj, params);
	return CreateArr(id);
}
Array<ID> DistanceBetPointSegment(const Array<ID>& obj, const Array<double>& params) {
	ID id = Model::GetInstance()->CreateRequirement(ot_distBetPointSeg, obj, params);
	return CreateArr(id);
}
Array<ID> Delete(const Array<ID>& obj, const Array<double>& params) {
	for (int i = 0; i < obj.GetSize(); ++i) {
		DataController::GetInstance()->DeleteObject(obj[i]);
	}
	return Array<ID>(0);
}

Mode* Presenter::mode(nullptr);
IView* Presenter::view(nullptr);
Model* Presenter::model(nullptr);
//temp

IView* Presenter::GetView()
{
	return view;
}

void Presenter::Initializer(IView* _view)
{
	LOG("Initializing presenter", LEVEL_3);
	view = _view;
	mode = new Selection();
	model = Model::GetInstance();
	auto tree = new treeFunc;
	tree->Add("Point", CreatePoint);
	tree->Add("Segment", CreateSegment);
	tree->Add("Arc", CreateArc);
	tree->Add("Circle", CreateCircle);
	tree->Add("Move", Move);
	tree->Add("Scale", Scale);
	tree->Add("Dist_bet_points", DistBetPoints);
	tree->Add("Equal_segment", EqualSegment);
	tree->Add("Distance_bet_point_segment", DistanceBetPointSegment);
	tree->Add("Delete", Delete);
	Compiler::Initializer(tree);
}

void Presenter::CleareScene() {
	auto temp = mode->HandleEvent(ev_escape, Array<double>(0));
	if (temp != nullptr) {
		delete mode;
		mode = temp;
	}
	Undo_Redo::GetInstance()->Clear();
	model->Clear();
	view->Update();
}

void Presenter::MoveObject(const Array<ID>& primitiveID,const Vector2& vector) {
	model->Move(primitiveID, vector);
}

//void Presenter::DrawSelectedObjects(const Array<ID>& selectedObjects)
//{
//	for (int i = 0; i < selectedObjects.GetSize(); i++)
//	{
//		Array<double> params = model->GetPrimParamsForDrawing(selectedObjects[i]);
//		object_type type = model->GetObjType(selectedObjects[i]);
//
//		switch (type)
//		{
//		case ot_point:
//			GetView()->DrawPoint(Vector2(params[0], params[1]));
//			break;
//		case ot_segment:
//			GetView()->DrawLine(Vector2(params[0], params[1]),
//				Vector2(params[2], params[3]), line);
//			break;
//		case ot_arc:
//			GetView()->DrawArc(Vector2(params[5], params[6]),
//				Vector2(params[0], params[1]),
//				Vector2(params[2], params[3]), line);
//			break;
//		case ot_circle:
//			GetView()->DrawCircle(Vector2(params[0], params[1]),
//				Vector2(params[0] + params[2], params[1]), line);
//			break;
//		}
//	}
//}

void Presenter::DrawSelectedObjects(const Array<ID>& selectedObjects)
{
	for (int i = 0; i < selectedObjects.GetSize(); i++)
	{
		ID obj = selectedObjects[i];
		object_type type = model->GetObjType(selectedObjects[i]);
		switch (type)
		{
		case ot_point: {
			auto params = model->GetVariableObjParam(obj, VERTEX);
			view->DrawPoint(params);
			break;
		}
		case ot_segment: {
			auto params = model->GetVariableObjParam(obj, VERTEX);
			view->DrawLine(params);
			break;
		}
		case ot_arc: {
			auto params = model->GetVariableObjParam(obj, VERTEX, CENTER, RADIUS, ANGLE);
			view->DrawArc(params);
			break;
		}
		case ot_circle: {
			auto params = model->GetVariableObjParam(obj, CENTER, RADIUS);
			view->DrawCircle(params);
			break;
		}
		case ot_curve: {
			auto params = model->GetVariableObjParam(obj, VERTEX);
			view->DrawCurve(params);
			break;
		}
		default:
			break;
		}
	}
}

//void Presenter::DrawScene()
//{
//	auto objCtrl = ObjectController::GetInstance();
//	auto primCtrl = PrimController::GetInstance();
//	auto iter = model->GetPrimIterator();
//	while (iter.IsValid()) {
//		ID obj = *iter;
//		++iter;
//		if (!objCtrl->IsValid(obj)) {
//			continue;
//		}
//		Array<double> params = model->GetPrimParamsForDrawing(obj);
//
//		if (model->GetObjType(obj) == ot_point) {
//			view->SetColor(col_Red);
//			view->DrawPoint(Vector2(params[0], params[1]));
//		}
//		if (model->GetObjType(obj) == ot_segment) {
//			view->SetColor(col_White);
//			view->DrawLine(Vector2(params[0], params[1]),
//				Vector2(params[2], params[3]), line);
//		}
//		if (model->GetObjType(obj) == ot_arc) {
//			view->SetColor(col_White);
//			view->DrawArc(Vector2(params[5], params[6]),
//				Vector2(params[0], params[1]),
//				Vector2(params[2], params[3]), line);
//		}
//		if (model->GetObjType(obj) == ot_circle) {
//			view->SetColor(col_White);
//			view->DrawCircle(Vector2(params[0], params[1]),
//				Vector2(params[0] + params[2], params[1]), line);
//		}
//		if (model->GetObjType(obj) == ot_curve) {
//			view->SetColor(col_White);
//			view->DrawCurve(params, line);
//		}
//
//	}
//
//	mode->DrawMode();
//}

void Presenter::DrawScene()
{
	auto color = Color::GetInstance();
	auto objCtrl = ObjectController::GetInstance();
	auto iter = model->GetPrimIterator();
	view->SetStyleDrawing(color->Primitives(), solid);
	while (iter.IsValid()) {
		ID obj = *iter;
		++iter;
		if (!objCtrl->IsValid(obj)) {
			continue;
		}
		if (model->GetObjType(obj) == ot_point) {
			auto params = model->GetVariableObjParam(obj, VERTEX);
			view->DrawPoint(params);
		}
		if (model->GetObjType(obj) == ot_segment) {
			auto params = model->GetVariableObjParam(obj, VERTEX);
			view->DrawLine(params);
		}
		if (model->GetObjType(obj) == ot_arc) {
			auto params = model->GetVariableObjParam(obj, VERTEX, CENTER, RADIUS, ANGLE);
			view->DrawArc(params);
		}
		if (model->GetObjType(obj) == ot_circle) {
			auto params = model->GetVariableObjParam(obj, CENTER, RADIUS);
			view->DrawCircle(params);
		}
		if (model->GetObjType(obj) == ot_curve) {
			auto params = model->GetVariableObjParam(obj, VERTEX);
			view->DrawCurve(params);
		}
	}

	mode->DrawMode();
	view->Update();
}

void Presenter::Set_event(Event _ev, Array<double>& _params, const std::string& str)
{
	Mode* temp = mode->HandleEvent(_ev, _params);
	if (temp != nullptr)
	{
		delete mode;
		mode = temp;
	}

	switch (_ev)
	{
	case ev_save:
	{
		model->Save(str);
		break;
	}
	case ev_download:
	{
		model->Download(str);
		break;
	}	
	case ev_compile:
	{
		Presenter::Compile();
		break;
	}
	case ex_set_theme:
	{
		Color::GetInstance()->SetTheme(int(_params[0]));
		break;
	}
	}
	view->Update();
}

void Presenter::Compile() {
	Compiler* compiler = Compiler::GetInstance();
	std::ifstream file;
	file.open("script.txt");
	if (file.is_open()) {
		if (!file.eof()) {
			compiler->Parse(file);
		}
		file.close();
	}
	view->Update();
}
