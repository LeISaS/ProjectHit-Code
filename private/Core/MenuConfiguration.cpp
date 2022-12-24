// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MenuConfiguration.h"
#include "Components/InputKeySelector.h"
#include "Components/Button.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"
bool UMenuConfiguration::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	return true;
}

void UMenuConfiguration::NativeConstruct()
{
	Super::NativeConstruct();

	TArray<FInputActionKeyMapping> MappingKeys;

	class UInputSettings* InputSettings = UInputSettings::GetInputSettings();
	InputSettings->GetActionMappingByName("JumpButton", MappingKeys);
	InputSettings->GetActionMappingByName("Dash", MappingKeys);
	InputSettings->GetActionMappingByName("Shield", MappingKeys);

	InputSettings->GetActionMappingByName("JumpButtonPad", MappingKeys);
	InputSettings->GetActionMappingByName("DashPad", MappingKeys);
	InputSettings->GetActionMappingByName("ShieldPad", MappingKeys);
	
	


	TArray<FInputActionKeyMapping> KeyMapped;
	KeyMapped.Add(MappingKeys[0]);
	KeyMapped.Add(MappingKeys[1]);
	KeyMapped.Add(MappingKeys[2]);

	KeyMapped.Add(MappingKeys[3]);
	KeyMapped.Add(MappingKeys[4]);
	KeyMapped.Add(MappingKeys[5]);

	if (JumpKeyButton)
	{
		JumpKeyButton->SetSelectedKey(KeyMapped[0].Key);
		JumpKeyButton->OnKeySelected.AddUniqueDynamic(this, &ThisClass::JumpKeyButtonCliked);
	}
	if (DashkeyButton)
	{
		DashkeyButton->SetSelectedKey(KeyMapped[1].Key);
		DashkeyButton->OnKeySelected.AddUniqueDynamic(this, &ThisClass::DashkeyButtonCliked);
	}
	if (ShieldKeyButton)
	{
		ShieldKeyButton->SetSelectedKey(KeyMapped[2].Key);
		ShieldKeyButton->OnKeySelected.AddUniqueDynamic(this, &ThisClass::ShieldKeyButtonCliked);
	}

	if (JumpKeyButtonPad)
	{
		JumpKeyButtonPad->SetSelectedKey(KeyMapped[3].Key);
		JumpKeyButtonPad->OnKeySelected.AddUniqueDynamic(this, &ThisClass::JumpKeyButtonPadCliked);
	}
	if (DashkeyButtonPad)
	{
		DashkeyButtonPad->SetSelectedKey(KeyMapped[4].Key);
		DashkeyButtonPad->OnKeySelected.AddUniqueDynamic(this, &ThisClass::DashkeyButtonPadCliked);
	}
	if (ShieldKeyButtonPad)
	{
		ShieldKeyButtonPad->SetSelectedKey(KeyMapped[5].Key);
		ShieldKeyButtonPad->OnKeySelected.AddUniqueDynamic(this, &ThisClass::ShieldKeyButtonPadCliked);
	}
}



void UMenuConfiguration::JumpKeyButtonCliked(FInputChord InputChord)
{
	UpdatekeyMappingButton(InputChord, "JumpButton");
}

void UMenuConfiguration::DashkeyButtonCliked(FInputChord InputChord)
{
	UpdatekeyMappingButton(InputChord, "Dash");
}

void UMenuConfiguration::ShieldKeyButtonCliked(FInputChord InputChord)
{
	UpdatekeyMappingButton(InputChord, "Shield");
}

void UMenuConfiguration::JumpKeyButtonPadCliked(FInputChord InputChord)
{
	UpdatekeyMappingButton(InputChord, "JumpButtonPad");
}

void UMenuConfiguration::DashkeyButtonPadCliked(FInputChord InputChord)
{
	UpdatekeyMappingButton(InputChord, "DashPad");
}

void UMenuConfiguration::ShieldKeyButtonPadCliked(FInputChord InputChord)
{
	UpdatekeyMappingButton(InputChord, "ShieldPad");
}

void UMenuConfiguration::UpdatekeyMappingButton(FInputChord InputChord, FName MappingName)
{
	TArray<FInputActionKeyMapping> Mapping;

	class UInputSettings* InputSettings = UInputSettings::GetInputSettings();

	InputSettings->GetActionMappingByName(MappingName, Mapping);
	FInputActionKeyMapping Mapped = Mapping[0];
	
	InputSettings->RemoveActionMapping(Mapped);
	Mapped = FInputActionKeyMapping(MappingName, InputChord.Key);
	InputSettings->AddActionMapping(Mapped);
	InputSettings->ForceRebuildKeymaps();
}

