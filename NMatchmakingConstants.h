#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PROJECTQ_API NMatchmakingConstants
{
public:
	NMatchmakingConstants();
	~NMatchmakingConstants();

public:
	//making sure the first N matches of any player are easier
	static const int EasyModeMatchesLimit = 10;

	static const int BotNumberMultiplier = 3;

	static const int MinNumberOfPlayersInPool = 1000;

	static const int MinPairRating = 75;

	static const int MaxNumberOfFailedMatchmakingAttempts = 5;

	static float PlayerQuitingRatingChange;

	static float PlayerWinningRatingChangeMargin;
	static float PlayerWinningRatingChange;

	static float PlayerLosingRatingChangeMargin;
	static float PlayerLosingRatingChange;

	static TArray<int> RatingBracketMargins_Great;

	static TArray<int> RatingBracketMargins_Poor;
};
