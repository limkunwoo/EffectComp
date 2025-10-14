// Copyright 2022-2023 Metaverse World, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace EnumHelper
{
	//////////////////////////////////////////////////////////////////////////
	//> Enum to Name
	template<typename TEnum>
	static FORCEINLINE FName EnumToName(TEnum InEnumValue)
	{
		static_assert(TIsEnum<TEnum>::Value);
		if (auto _FindEnum = StaticEnum<TEnum>())
		{
			auto _EnumIndex = _FindEnum->GetIndexByValue(static_cast<int64>(InEnumValue));
			if (_EnumIndex != INDEX_NONE)
			{
				return FName(*_FindEnum->GetNameStringByIndex(_EnumIndex));
			}
		}
		return NAME_None;
	};

	template<typename TEnum>
	static FORCEINLINE bool EnumToNameCheck(FName& OutEnumName, TEnum InEnumValue)
	{
		static_assert(TIsEnum<TEnum>::Value);
		if (auto _FindEnum = StaticEnum<TEnum>())
		{
			auto _EnumIndex = _FindEnum->GetIndexByValue(static_cast<int64>(InEnumValue));
			if (_EnumIndex != INDEX_NONE)
			{
				OutEnumName = FName(*_FindEnum->GetNameStringByIndex(_EnumIndex));
				return true;
			}
		}
		return false;
	};

	


	//////////////////////////////////////////////////////////////////////////
	//> Enum To String
	template<typename TEnum>
	static FORCEINLINE FString EnumToString(TEnum InEnumValue)
	{
		static_assert(TIsEnum<TEnum>::Value);
		if (auto _FindEnum = StaticEnum<TEnum>())
		{
			auto _EnumIndex = _FindEnum->GetIndexByValue(static_cast<int64>(InEnumValue));
			if (_EnumIndex != INDEX_NONE)
			{
				return _FindEnum->GetNameStringByIndex(_EnumIndex);
			}
		}
		return FString();
	};

	template<typename TEnum>
	static FORCEINLINE bool EnumToStringCheck(FString& OutEnumString, TEnum InEnumValue)
	{
		static_assert(TIsEnum<TEnum>::Value);
		if (auto _FindEnum = StaticEnum<TEnum>())
		{
			auto _EnumIndex = _FindEnum->GetIndexByValue(static_cast<int64>(InEnumValue));
			if (_EnumIndex != INDEX_NONE)
			{
				OutEnumString = _FindEnum->GetNameStringByIndex(_EnumIndex);
				return true;
			}
		}
		return false;
	};

	template<typename TEnum>
	static FORCEINLINE TArray<FString> EnumToStringArr()
	{
		static_assert(TIsEnum<TEnum>::Value);
		TArray<FString> ReturnArr;
		if (UEnum* _FindEnum = StaticEnum<TEnum>())
		{
			for (int i = 0; i < _FindEnum->NumEnums(); i++)
			{
				ReturnArr.Add(_FindEnum->GetNameStringByIndex(i));
			}
			ReturnArr.Pop();
		}
		else
		{
			UE_DEBUG_BREAK();
		}
		return ReturnArr;
	}
	//////////////////////////////////////////////////////////////////////////
	//> Enum To Name
	template<typename TEnum>
	static FORCEINLINE TArray<FName> EnumToNameArr()
	{
		static_assert(TIsEnum<TEnum>::Value);
		TArray<FName> ReturnArr;
		if (UEnum* _FindEnum = StaticEnum<TEnum>())
		{
			for (int i = 0; i < _FindEnum->NumEnums(); i++)
			{
				ReturnArr.Add(*(_FindEnum->GetNameStringByIndex(i)));
			}
			ReturnArr.Pop();
		}
		else
		{
			UE_DEBUG_BREAK();
		}
		return ReturnArr;
	}
	//////////////////////////////////////////////////////////////////////////
	//> Enum To DisplayString
	template <typename TEnum>
	static FORCEINLINE FString EnumToDisplayString(TEnum InEnumValue)
	{
		static_assert(TIsEnum<TEnum>::Value);
		if (UEnum* _FindEnum = StaticEnum<TEnum>())
		{
			return _FindEnum->GetDisplayNameTextByValue(static_cast<int64>(InEnumValue)).ToString();
		}
		else
		{
			UE_DEBUG_BREAK();
		}
		return FString();
	}

	template <typename TEnum>
	static FORCEINLINE TArray<FString> EnumToDisplayStringArr()
	{
		static_assert(TIsEnum<TEnum>::Value);
		TArray<FString> ReturnArr;
		if (UEnum* _FindEnum = StaticEnum<TEnum>())
		{
			for (int i = 0; i < _FindEnum->NumEnums() - 1; i++)
			{
				ReturnArr.Add(_FindEnum->GetDisplayNameTextByIndex(i).ToString());
			}
		}
		else
		{
			UE_DEBUG_BREAK();
		}
		return ReturnArr;
	}


	//////////////////////////////////////////////////////////////////////////
	//> Name To Enum
	template<typename TEnum>
	static FORCEINLINE TEnum NameToEnum(const FName& InEnumValueName)
	{
		static_assert(TIsEnum<TEnum>::Value);
		if (auto _FindEnum = StaticEnum<TEnum>())
		{
			auto _EnumIndex = _FindEnum->GetIndexByName(InEnumValueName);
			if (_EnumIndex != INDEX_NONE)
			{
				return static_cast<TEnum>(_FindEnum->GetValueByIndex(_EnumIndex));
			}
		}
		return static_cast<TEnum>(0);
	};

	template<typename TEnum>
	static FORCEINLINE bool NameToEnumCheck(TEnum& OutEnumValue, const FName& InEnumValueName)
	{
		static_assert(TIsEnum<TEnum>::Value);
		if (auto _FindEnum = StaticEnum<TEnum>())
		{
			auto _EnumIndex = _FindEnum->GetIndexByName(InEnumValueName);
			if (_EnumIndex != INDEX_NONE)
			{
				OutEnumValue = static_cast<TEnum>(_FindEnum->GetValueByIndex(_EnumIndex));
				return true;
			}
		}
		return false;
	};



	//////////////////////////////////////////////////////////////////////////
	//> Name To String
	template<typename TEnum>
	static FORCEINLINE TEnum StringToEnum(const FString& InEnumValueString)
	{
		static_assert(TIsEnum<TEnum>::Value);
		if (auto _FindEnum = StaticEnum<TEnum>())
		{
			auto _EnumIndex = _FindEnum->GetIndexByName(*InEnumValueString);
			if (_EnumIndex != INDEX_NONE)
			{
				return static_cast<TEnum>(_FindEnum->GetValueByIndex(_EnumIndex));
			}
		}
		return static_cast<TEnum>(0);
	};

	template<typename TEnum>
	static FORCEINLINE bool StringToEnumCheck(TEnum& OutEnumValue, const FString& InEnumValueString)
	{
		static_assert(TIsEnum<TEnum>::Value);
		if (auto _FindEnum = StaticEnum<TEnum>())
		{
			auto _EnumIndex = _FindEnum->GetIndexByName(*InEnumValueString);
			if (_EnumIndex != INDEX_NONE)
			{
				OutEnumValue = static_cast<TEnum>(_FindEnum->GetValueByIndex(_EnumIndex));
				return true;
			}
		}
		return false;
	};

	template<typename TEnum>
	static FORCEINLINE bool UintToEnumCheck(TEnum& OutEnumValue, const uint8 InEnumValueUint)
	{
		static_assert(TIsEnum<TEnum>::Value);
		if (auto _FindEnum = StaticEnum<TEnum>())
		{
			auto _EnumIndex = _FindEnum->GetIndexByValue(InEnumValueUint);
			if (_EnumIndex != INDEX_NONE)
			{
				OutEnumValue = static_cast<TEnum>(_FindEnum->GetValueByIndex(_EnumIndex));
				return true;
			}
		}
		return false;
	}
};


#define DISPLAY_STR(Value) EnumHelper::EnumToDisplayString(Value)
#define NAME_STR(EnumType, Value) EnumHelper::EnumToString<EnumType>(Value)
#define DISPLAY_STR_ARR(EnumType) EnumHelper::EnumToDisplayStringArr<EnumType>()
#define NAME_STR_ARR(EnumType) EnumHelper::EnumToNameArr<EnumType>()
#define NAME_TO_VALUE(EnumType, Name) EnumHelper::NameToEnum<EnumType>(Name)