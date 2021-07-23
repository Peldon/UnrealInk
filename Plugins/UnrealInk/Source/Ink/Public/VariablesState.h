#pragma once

#include "MonoBaseClass.h"

#include "VariablesState.generated.h"

struct FInkVar;
struct FInkListVar;

UENUM(BlueprintType)
enum class ETryReturn : uint8 {
	Then,
	Fail
};

UCLASS(BlueprintType)
class INK_API UVariablesState : public UMonoBaseClass
{
	GENERATED_BODY()
public:
	UVariablesState();

	UFUNCTION(BlueprintPure, Category = Ink)
	TArray<FString> GetVariables();

	UFUNCTION(BlueprintPure, Category = Ink)
	FInkVar GetVariable(const FString& variableName);

	UFUNCTION(BlueprintPure, Category = Ink)
	FInkListVar GetListVariable(const FString& variableName);

	UFUNCTION(BlueprintPure, Category = Ink)
	TArray<FString> GetListVariableEntries(const FString& variableName);

	UFUNCTION(BlueprintPure, Category = Ink)
	TArray<FString> GetListVariableOrigins(const FString& variableName);

	UFUNCTION(BlueprintCallable, Category = Ink, Meta = (ExpandEnumAsExecs = "returnBranch"))
	void TryGetVariable(const FString& variableName, FInkVar& inkVar, ETryReturn& returnBranch);

	UFUNCTION(BlueprintCallable, Category = Ink, Meta = (ExpandEnumAsExecs = "returnBranch"))
	void TryGetListVariable(const FString& variableName, FInkListVar& inkListVar, ETryReturn& returnBranch);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void SetVariableFloat(const FString& variableName, float value);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void SetVariableInt(const FString& variableName, int value);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void SetVariableString(const FString& variableName, const FString& value);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void SetVariable(const FString& variableName, const FInkVar& value);

private:
	friend class UStory;
	static UVariablesState* NewVariablesState(MonoObject* MonoVariableState);
};