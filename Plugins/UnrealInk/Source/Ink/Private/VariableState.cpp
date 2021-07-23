#include "VariablesState.h"

#include "Story.h"
#include "StoryState.h"
#include "MonoBaseClass.h"

#include <mono/jit/jit.h>
#include <mono/metadata/metadata.h>

UVariablesState::UVariablesState()
{

}

TArray<FString> UVariablesState::GetVariables()
{
	TArray<FString> variableNames;

	// Grab variable names
	MonoArray* result = MonoInvoke<MonoArray*>("GetVariables", nullptr);
	for (uintptr_t i = 0; i < mono_array_length(result); i++)
	{
		MonoString* variableName = mono_array_get(result, MonoString*, i);
		variableNames.Add(FString(mono_string_to_utf8(variableName)));
	}

	return variableNames;
}

FInkVar UVariablesState::GetVariable(const FString& variableName) 
{
	FInkVar result;
	ETryReturn returnBranch = ETryReturn::Fail;
	TryGetVariable(variableName, result, returnBranch);
	if (returnBranch == ETryReturn::Then)
		return result;

	UE_LOG(LogTemp, Error, TEXT("Variable not found or not of type int, float or string!"));
	return FInkVar();
}

FInkListVar UVariablesState::GetListVariable(const FString& variableName) 
{
	FInkListVar result;
	ETryReturn returnBranch = ETryReturn::Fail;
	TryGetListVariable(variableName, result, returnBranch);
	if (returnBranch == ETryReturn::Then)
		return result;
	
	UE_LOG(LogTemp, Error, TEXT("Variable not found or not a list variable!"));
	return FInkListVar();
}

TArray<FString> UVariablesState::GetListVariableEntries(const FString& variableName) {

	TArray<FString> result;
	for (const FInkListEntry& entry : GetListVariable(variableName).entries) {
		result.Add(entry.item);
	}
	return result;
}

TArray<FString> UVariablesState::GetListVariableOrigins(const FString& variableName) {
	TArray<FString> result;
	for (const FInkListEntry& entry : GetListVariable(variableName).entries) {
		result.AddUnique(entry.origin);
	}
	return result;
}

void UVariablesState::TryGetVariable(const FString& variableName, FInkVar& inkVar, ETryReturn& returnBranch)
{
	// Single parameter: variable name
	void* params[1];
	params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*variableName));

	// Get result as a mono object
	MonoObject* pObject = MonoInvoke<MonoObject*>("GetVariable", params);
	MonoClass* pClass = mono_object_get_class(pObject);

	returnBranch = ETryReturn::Then;
	if (pClass == mono_get_single_class()) {
		inkVar.type = EInkVarType::Float;
		inkVar.floatVar = *(float*) mono_object_unbox(pObject);
	}
	else if (pClass == mono_get_int32_class()) {
		inkVar.type = EInkVarType::Int;
		inkVar.intVar = *(int*) mono_object_unbox(pObject);
	}
	else if (pClass == mono_get_string_class()) {
		inkVar.type = EInkVarType::String;
		inkVar.stringVar = FString(mono_string_to_utf8((MonoString*) pObject));
	}
	else {
		returnBranch = ETryReturn::Fail;
		inkVar.type = EInkVarType::None;
		UE_LOG(LogTemp, Error, TEXT("Variable not found or not of type int, float or string!"));
	}
}

void UVariablesState::TryGetListVariable(const FString& variableName, FInkListVar& inkListVar, ETryReturn& returnBranch)
{
	// Single parameter: variable name
	void* params[1];
	params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*variableName));

	// Get result as a mono object
	MonoObject* pObject = MonoInvoke<MonoObject*>("GetListVariable", params);
	MonoClass* pClass = mono_object_get_class(pObject);
	if (pClass == mono_get_string_class()) {

		FString listAsString = FString(mono_string_to_utf8((MonoString*) pObject));
		//UE_LOG(LogTemp, Error, TEXT("List Variable found: %s!"), *listAsString);
		inkListVar.Init(listAsString);
		returnBranch = ETryReturn::Then;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Variable not found or not a list!"));
		returnBranch = ETryReturn::Fail;
	}
}

void UVariablesState::SetVariableFloat(const FString& variableName, float value)
{
	void* params[2];
	params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*variableName));
	params[1] = mono_value_box(mono_domain_get(), mono_get_single_class(), &value);

	MonoInvoke<void>("SetVariable", params);
}

void UVariablesState::SetVariableInt(const FString& variableName, int value)
{
	void* params[2];
	params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*variableName));
	params[1] = mono_value_box(mono_domain_get(), mono_get_int32_class(), &value);

	MonoInvoke<void>("SetVariable", params);
}

void UVariablesState::SetVariableString(const FString& variableName, const FString& value)
{
	void* params[2];
	params[0] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*variableName));
	params[1] = mono_string_new(mono_domain_get(), TCHAR_TO_UTF8(*value));

	MonoInvoke<void>("SetVariable", params);
}

void UVariablesState::SetVariable(const FString& variableName, const FInkVar& value)
{
	if (!ensure(value.type != EInkVarType::None))
		return;

	switch (value.type)
	{
	case EInkVarType::Float:
		SetVariableFloat(variableName, value.floatVar);
		break;
	case EInkVarType::Int:
		SetVariableInt(variableName, value.intVar);
		break;
	case EInkVarType::String:
		SetVariableString(variableName, value.stringVar);
		break;
	}
}

UVariablesState* UVariablesState::NewVariablesState(MonoObject* MonoVariableState)
{
	UVariablesState* NewVariableState = NewObject<UVariablesState>();
	NewVariableState->NewFromInstance(MonoVariableState);
	NewVariableState->ManualMethodBind("GetVariable", 1);
	return NewVariableState;
}