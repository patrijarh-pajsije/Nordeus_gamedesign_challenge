#include "Test/NMatchmakingConstants.h"

NMatchmakingConstants::NMatchmakingConstants()
{
	RatingBracketMargins_Great = { 15, 30, 45, 60, 75, 90, 100 };
	RatingBracketMargins_Poor = { 20, 40, 60, 80, 100 };

	PlayerQuitingRatingChange = 0.85;
	
	PlayerWinningRatingChangeMargin = 0.75;
	PlayerWinningRatingChange = 1.2;

	PlayerLosingRatingChangeMargin = 0.3;
	PlayerLosingRatingChange = 0.85;
}

NMatchmakingConstants::~NMatchmakingConstants()
{
}
