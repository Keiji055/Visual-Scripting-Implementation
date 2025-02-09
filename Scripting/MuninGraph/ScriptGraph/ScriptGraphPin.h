﻿#pragma once
#include <any>
#include <typeindex>
#include <utility>
#include "Graph/GlobalUID.h"
#include "ScriptGraphDataObject.h"
#include "ScriptGraphTypes.h"

class ScriptGraphSchema;
class ScriptGraphNode;
class ScriptGraphPin;

enum class PinIcon
{
	Exec,
	Circle,
	Square,
	Array,
	RoundedSquare,
	Diamond
};

enum class ScriptGraphPinType
{
	Data,
	Exec,
	Variable
};

class ScriptGraphPin : public NodeGraphPin<ScriptGraphNode, ScriptGraphSchema>
{
	friend ScriptGraphSchema;
	ScriptGraphPinType myType;
	PinIcon myIcon = PinIcon::Circle;
	bool myLabelVisible = true;

	ScriptGraphDataObject myData;

	ScriptGraphPin(const std::shared_ptr<ScriptGraphNode>& anOwner, const std::string& aLabel, PinDirection aDirection, PinIcon anIcon, ScriptGraphPinType aType)
		: NodeGraphPin(anOwner, aLabel, aDirection), myType(aType), myIcon(anIcon)
	{  }

protected:

	void InitVariableBlock(const std::type_index& aType);

	/**
	 * \brief Initializes the PinData container.
	 * \tparam DataType The type that the pin should hold.
	 */
	template<typename DataType>
	void InitDataBlock()
	{
		//myData = std::move(ScriptGraphDataObject::Create<DataType>());
		ScriptGraphDataObject::Create<DataType>(myData);
	}

	void SetDataObject(ScriptGraphDataObject&& aDataObject)
	{
		myData = std::move(aDataObject);
	}

public:

	template<typename DataType>
	static ScriptGraphPin CreateDataPin(const std::shared_ptr<ScriptGraphNode>& aOwner, const std::string& aLabel, ScriptGraphPinType aType, PinIcon anIcon, PinDirection aDirection, bool hideLabelOnNode = false)
	{
		ScriptGraphPin Pin(aOwner, aLabel, aDirection, anIcon, aType);
		Pin.InitDataBlock<DataType>();
		Pin.myLabelVisible = !hideLabelOnNode;
		return std::move(Pin);
	}

	static ScriptGraphPin CreatePin(const std::shared_ptr<ScriptGraphNode>& aOwner, const std::string& aLabel, ScriptGraphPinType aType, PinIcon anIcon, PinDirection aDirection, bool hideLabelOnNode = false)
	{
		ScriptGraphPin Pin(aOwner, aLabel, aDirection, anIcon, aType);
		Pin.myLabelVisible = !hideLabelOnNode;
		return std::move(Pin);
	}

	template<typename DataType>
	bool GetData(DataType& outData) const
	{
#if _DEBUG
		const std::type_index& RequestedType = typeid(DataType);
		assert(RequestedType == myData.TypeData->GetType() && "Cannot GetData of another type than the one that is stored!");
#endif
		DataType* ptr = static_cast<DataType*>(myData.Ptr);
		if (ptr)
		{
			outData = *ptr;
			return true;
		}

		return false;
	}
	size_t GetDataSize() const { return myData.TypeData->GetTypeSize(); }

	// USE AT YOUR OWN RISK!
	bool GetRawData(void* outData, size_t outDataSize) const;

	// USE AT YOUR OWN RISK!
	void SetRawData(const void* inData, size_t inDataSize);

	template<typename DataType>
	void SetData(DataType data)
	{
#if _DEBUG
		const std::type_index& RequestedType = typeid(DataType);
		assert(RequestedType == myData.TypeData->GetType() && "Cannot SetData of another type than the one that is stored!");
#endif

		if(RequestedType == typeid(std::string))
		{
			std::string* localStrPtr = static_cast<std::string*>(myData.Ptr);
			std::string* dataStrPtr = reinterpret_cast<std::string*>(&data);
			(*localStrPtr) = *dataStrPtr;
			//int a = 1;
		}
		else
		{
			// TODO: This mangles strings because strings in c++ are terrible things.
			// Meanwhile we have the if statement above to handle strings.
			// TODO: really the RegisterScriptGraphTypes.h file should contain operators
			// for this that the user can override. Something to be done when there's time.
			memset(myData.Ptr, 0, myData.TypeData->GetTypeSize());
			memcpy_s(myData.Ptr, myData.TypeData->GetTypeSize(), &data, sizeof(DataType));			
		}
	}

	[[nodiscard]] void* GetPtr() const { return myData.Ptr; }

	FORCEINLINE ScriptGraphPinType GetType() const { return myType; }
	FORCEINLINE bool IsLabelVisible() const { return myLabelVisible; }
	virtual FORCEINLINE std::type_index GetDataType() const { if (myData.TypeData) return myData.TypeData->GetType(); return typeid(std::nullptr_t); }
};