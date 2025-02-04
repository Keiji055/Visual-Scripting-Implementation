#include "RegisterExternalNodes.h"

void MVNode_TestNode::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);
	CreateDataPin<float>("Value", PinDirection::Input);
}

size_t MVNode_TestNode::DoOperation()
{
	return ExitViaPin("Out");
}



void SGNode_CreateGameObject::Init()
{
	CreateExecPin("In", PinDirection::Input, true);

	CreateExecPin("Out", PinDirection::Output, true);
	CreateDataPin<int>("ID", PinDirection::Output);
	CreateDataPin<std::string>("SpriteName", PinDirection::Input);

}

size_t SGNode_CreateGameObject::DoOperation()
{
	myGameObject = std::make_shared<GameObject>();
	GameObjectRegistery::Get().AddGameObject(myGameObject);
	std::string spriteName;
	if (GameObjectRegistery::Get().GetGameObject(myGlobalGameObjectID) != nullptr)
	{

		SetPinData("ID", GameObjectRegistery::Get().GetGameObject(myGlobalGameObjectID)->GetID());


		GameObjectRegistery::Get().GetGameObject(myGlobalGameObjectID)->SetID();
		if (GetPinData("SpriteName", spriteName))
		{
			GameObjectRegistery::Get().GetGameObject(myGlobalGameObjectID)->Init(spriteName);
		}
		else
		{
			std::cout << "no inputed sprite defualt is used" << std::endl;
		}
		return ExitViaPin("Out");
	}

	return -1;
}


void SGNode_SetGOPos::Init()
{

	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<int>("GameObjectID", PinDirection::Input);

	CreateDataPin<float>("X", PinDirection::Input);
	CreateDataPin<float>("Y", PinDirection::Input);
	CreateDataPin<float>("Z", PinDirection::Input);
}

size_t SGNode_SetGOPos::DoOperation()
{

	float x;
	float y;
	float z;
	int ID;
	GetPinData("GameObjectID", ID);
	if (GameObjectRegistery::Get().GetGameObject(ID) != nullptr)
	{
		GetPinData("X", x);
		GetPinData("Y", y);
		GetPinData("Z", z);

		GameObjectRegistery::Get().GetGameObject(ID)->SetPosition({ x,y,z });
		return ExitViaPin("Out");

	}
	return -1;
}


void SGNode_GetGOPos::Init()
{

	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<int>("GameObjectID", PinDirection::Input);

	CreateDataPin<float>("Result X", PinDirection::Output);
	CreateDataPin<float>("Result Y", PinDirection::Output);
	CreateDataPin<float>("Result Z", PinDirection::Output);
}

size_t SGNode_GetGOPos::DoOperation()
{
	float x;
	float y;
	float z;

	int ID;
	GetPinData("GameObjectID", ID);
	if (GameObjectRegistery::Get().GetGameObject(ID) != nullptr)
	{


		x = GameObjectRegistery::Get().GetGameObject(ID)->GetPosition().x;
		y = GameObjectRegistery::Get().GetGameObject(ID)->GetPosition().y;
		z = GameObjectRegistery::Get().GetGameObject(ID)->GetPosition().z;
		SetPinData("Result X", x);
		SetPinData("Result Y", y);
		SetPinData("Result Z", z);
		return ExitViaPin("Out");
	}
	return -1;
}

void SGNode_UpdateGOPos::Init()
{

	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<float>("DeltaTime", PinDirection::Input);
}

size_t SGNode_UpdateGOPos::DoOperation()
{
	int ID;
	float DeltaTime;
	if (GetPinData("DeltaTime", DeltaTime))
	{
		for (size_t i = 0; i < GameObjectRegistery::Get().GetAllGameObject().size(); i++)
		{
			GameObjectRegistery::Get().GetAllGameObject()[i]->Update(DeltaTime);
		}
		return ExitViaPin("Out");
		
	}
	else
	{
		std::cout << "Gameobject does not exist" << std::endl;
	}
	return -1;
}
void SGNode_UpdateGOScale::Init()
{

	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);

	CreateDataPin<float>("Scale", PinDirection::Input);
	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t SGNode_UpdateGOScale::DoOperation()
{
	float Scale;

	if (GetPinData("Scale", Scale))
	{
		Scale /= 100;
		for (size_t i = 0; i < GameObjectRegistery::Get().GetAllGameObject().size(); i++)
		{
			GameObjectRegistery::Get().GetAllGameObject()[i]->SetScale(Scale);
		}
		SetPinData("Result", Scale);
		return ExitViaPin("Out");
	}
	else
	{
		std::cout << "Gameobject does not exist" << std::endl;
	}
	return -1;
}

void SGNode_GetMousePos::Init()
{
	CreateDataPin<float>("MousePosX", PinDirection::Output);
	CreateDataPin<float>("MousePosY", PinDirection::Output);
}

size_t SGNode_GetMousePos::DoOperation()
{
	float windowHeight = 900;
	float newY = windowHeight - CommonUtilities::InputHandler::GetMousePosition().y;
	SetPinData<float>("MousePosX",CommonUtilities::InputHandler::GetMousePosition().x);
	SetPinData<float>("MousePosY", newY);

	return Exit();
}

void SGNode_OnTriggerEntry::Init()
{
	CreateExecPin("Out", PinDirection::Output,true);
	CreateDataPin<int>("Source ID", PinDirection::Output);
	CreateDataPin<int>("Target ID", PinDirection::Output);
}
void SGNode_OnTriggerExit::Init()
{
	CreateExecPin("Out", PinDirection::Output, true);
	CreateDataPin<int>("Source ID", PinDirection::Output);
	CreateDataPin<int>("Target ID", PinDirection::Output);
}
