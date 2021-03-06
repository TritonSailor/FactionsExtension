// Copyright 2015-2018 Piperift. All Rights Reserved.

#include "FactionsLibrary.h"
#include <EngineUtils.h>
#include <Engine/Engine.h>

#include "FactionsModule.h"


FFaction UFactionsLibrary::RegistryFaction(const FName& Name, const FFactionInfo& FactionInfo)
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	Settings->Internal_RegistryFaction(Name, FactionInfo);

	// Return the faction, won't be valid if its not registered
	return { Name };
}

bool UFactionsLibrary::UnregistryFaction(FFaction Faction)
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	return Settings->Internal_UnregistryFaction(Faction);
}

void UFactionsLibrary::GetAllFactions(TArray<FFaction>& Factions)
{
	const UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	Factions.Reserve(Factions.Num() + Settings->Factions.Num());

	for (const auto& Entry : Settings->Factions)
	{
		Factions.Add({Entry.Key});
	}
}

bool UFactionsLibrary::RegistryRelation(const FFactionRelation& NewRelation)
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	return Settings->Internal_RegistryRelation(NewRelation);
}

bool UFactionsLibrary::UnregistryRelation(const FFactionRelation& Relation)
{
	UFactionsSettings* Settings = FFactionsModule::GetFactionManager();
	check(Settings);

	return Settings->Internal_UnregistryRelation(Relation);
}

bool UFactionsLibrary::GetAllActorsWithFaction(const UObject* ContextObject, const FFaction Faction, EFactionTestMode Comparison, TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors)
{
	QUICK_SCOPE_CYCLE_COUNTER(UGameplayStatics_GetAllActorsOfClass);
	OutActors.Reset();

	if (Faction.IsNone() || !ActorClass)
		return false;

	UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::ReturnNull);
	if (World)
	{
		switch (Comparison) {
		case EFactionTestMode::IsTheSame:
			for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
			{
				AActor* Actor = *It;
				if (!Actor->IsPendingKill() && UFactionsLibrary::GetFaction(Actor) == Faction)
					OutActors.Add(Actor);
			}
			break;
		case EFactionTestMode::IsFriendly:
			for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
			{
				AActor* Actor = *It;
				if (!Actor->IsPendingKill() && UFactionsLibrary::GetFaction(Actor).IsFriendlyTowards(Faction))
					OutActors.Add(Actor);
			}
			break;
		case EFactionTestMode::IsNeutral:
			for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
			{
				AActor* Actor = *It;
				if (!Actor->IsPendingKill() && UFactionsLibrary::GetFaction(Actor).IsNeutralTowards(Faction))
					OutActors.Add(Actor);
			}
			break;
		case EFactionTestMode::IsHostile:
			for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
			{
				AActor* Actor = *It;
				if (!Actor->IsPendingKill() && UFactionsLibrary::GetFaction(Actor).IsHostileTowards(Faction))
					OutActors.Add(Actor);
			}
			break;
		}

		return OutActors.Num() > 0;
	}

	return false;
}
