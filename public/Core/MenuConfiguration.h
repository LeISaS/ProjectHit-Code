// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuConfiguration.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHIT_API UMenuConfiguration : public UUserWidget
{
	GENERATED_BODY()
	
public:

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void JumpKeyButtonCliked(FInputChord InputChord);
	UFUNCTION()
	void DashkeyButtonCliked(FInputChord InputChord);
	UFUNCTION()
	void ShieldKeyButtonCliked(FInputChord InputChord);

	UFUNCTION()
	void JumpKeyButtonPadCliked(FInputChord InputChord);
	UFUNCTION()
	void DashkeyButtonPadCliked(FInputChord InputChord);
	UFUNCTION()
	void ShieldKeyButtonPadCliked(FInputChord InputChord);
	
	void UpdatekeyMappingButton(FInputChord InputChord, FName MappingName);
	
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UInputKeySelector* JumpKeyButton;

	UPROPERTY(meta = (BindWidget))
	UInputKeySelector* DashkeyButton;

	UPROPERTY(meta = (BindWidget))
	UInputKeySelector* ShieldKeyButton;	

	UPROPERTY(meta = (BindWidget))
	UInputKeySelector* JumpKeyButtonPad;

	UPROPERTY(meta = (BindWidget))
	UInputKeySelector* DashkeyButtonPad;

	UPROPERTY(meta = (BindWidget))
	UInputKeySelector* ShieldKeyButtonPad;
};
