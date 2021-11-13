#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
struct PlayerMatchmakingData
{
	//data used to find a matching opponent

	//rating based on player rating average
	int PlayerRating;

	//hidden rating based on how good a player actually is
	int SkillRating;

	//Is the player in danger of quiting the game 
	bool IsPlayerQuiting;

	//has the player spent real money recently
	bool HasPlayerPayedRecently;

	//Range 0 -1. Represents the win rate in recent games
	float WinPercentageLastTenGames;

	//Incremented each time an ideal opponent is not found 
	int NumberOfFailedMatchmakingAttempts;

	//Rating used for the current matchmaking
	int TempMatchmakingRating;
};


class PROJECTQ_API NPlayer
{
public:
	NPlayer();
	~NPlayer();

	FString PlayerName;

	int TotalNumberOfMatchesPlayed;

	PlayerMatchmakingData MatchmakingData;

	float TotalWinPercentage;
};
