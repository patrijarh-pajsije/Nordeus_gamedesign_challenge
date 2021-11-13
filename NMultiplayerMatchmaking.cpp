#include "Test/NMultiplayerMatchmaking.h"
#include "Test/NMatchmakingConstants.h"

void NMultiplayerMatchmaking::Tick()
{
	MatchPlayers();
}

void NMultiplayerMatchmaking::MatchPlayers()
{
	TArray<TPair< NPlayer*, NPlayer*>> Pairings;

	CreateMatchmakingPairings(Pairings, PlayerMatchmakingList);
}

void NMultiplayerMatchmaking::CreateMatchmakingPairings(TArray<TPair< NPlayer*, NPlayer*>>& Pairings, TArray<NPlayer*>& PlayerMatchmakingList)
{
	TArray<NPlayer*> AllPlayers = PlayerMatchmakingList;

	SetUpBeginnersBracket(AllPlayers);

	SetUpBrackets(AllPlayers);

	for (PlayerBracket Bracket : PlayerBrackets)
	{
		Bracket.PlayerList.Sort(
			[](const NPlayer* PlayerA, const NPlayer* PlayerB) {
				return PlayerA->MatchmakingData.SkillRating > PlayerB->MatchmakingData.SkillRating;
			});

		PlayerBracket PayingUsers;
		PlayerBracket NonPayingUsers;

		SplitPlayersBasedOnPayments(Bracket, PayingUsers, NonPayingUsers);

		CreatePairings(Pairings, PayingUsers, NonPayingUsers);

		FilterPairings(Pairings);
	}
}

void NMultiplayerMatchmaking::CalculateMatchmakingSkillRating(NPlayer* Player) const
{
	//hows players preparation for the match
	//is the player quiting
	//is the player winning or losing too much
	
	float SkillRating = Player->MatchmakingData.SkillRating;

	SkillRating += Player->MatchmakingData.PreparationForMatch;

	if (Player->MatchmakingData.IsPlayerQuiting)
	{
		SkillRating -= NMatchmakingConstants::PlayerQuitingRatingChange;
	}

	if (Player->MatchmakingData.WinPercentageLastTenGames <= NMatchmakingConstants::PlayerLosingRatingChangeMargin)
	{
		SkillRating -= NMatchmakingConstants::PlayerLosingRatingChange;
	}
	else if (Player->MatchmakingData.WinPercentageLastTenGames >= NMatchmakingConstants::PlayerWinningRatingChange)
	{
		SkillRating += NMatchmakingConstants::PlayerWinningRatingChange;
	}
}

void NMultiplayerMatchmaking::AddPlayerToMatchmakingPool(NPlayer* player)
{
	PlayerMatchmakingList.Add(player);
}

void NMultiplayerMatchmaking::SetUpBeginnersBracket(TArray<NPlayer*>& AllPlayers)
{
	PlayerBracket Bracket;
	//get all beginners into their own bracket
	for (int i = AllPlayers.Num(); i >= 0; --i)
	{
		if (AllPlayers[i]->TotalNumberOfMatchesPlayed < NMatchmakingConstants::EasyModeMatchesLimit)
		{
			//starting players bracket
			Bracket.PlayerList.Add(AllPlayers[i]);
			AllPlayers.RemoveAt(i);
		}
	}

	PlayerBrackets.Add(Bracket);
}

void NMultiplayerMatchmaking::SetUpBrackets(TArray<NPlayer*>& AllPlayers)
{
	TArray<int> RatingBracketMargins;

	if (AllPlayers.Num() >= NMatchmakingConstants::MinNumberOfPlayersInPool)
	{
		RatingBracketMargins = NMatchmakingConstants::RatingBracketMargins_Great;
	}
	else
	{
		RatingBracketMargins = NMatchmakingConstants::RatingBracketMargins_Poor;
	}

	for (int margin : RatingBracketMargins)
	{
		PlayerBracket NewBracket;

		for (int i = AllPlayers.Num() - 1; i >= 0; --i)
		{
			if (AllPlayers[i]->MatchmakingData.PlayerRating <= margin)
			{
				NewBracket.PlayerList.Add(AllPlayers[i]);
				AllPlayers.RemoveAt(i);
			}
		}

		PlayerBrackets.Add(NewBracket);
	}
}

void NMultiplayerMatchmaking::SplitPlayersBasedOnPayments(PlayerBracket& Bracket, PlayerBracket& PayingUsers, PlayerBracket& NonPayingUsers)
{
	for (NPlayer* Player : Bracket.PlayerList)
	{
		CalculateMatchmakingSkillRating(Player);

		//check for paying users
		if (Player->MatchmakingData.HasPlayerPayedRecently)
		{
			PayingUsers.PlayerList.Add(Player);
		}
		else
		{
			NonPayingUsers.PlayerList.Add(Player);
		}
	}
}

void NMultiplayerMatchmaking::CreatePairings(TArray<TPair< NPlayer*, NPlayer*>>& PairingsList, PlayerBracket& PayingUsers, PlayerBracket& NonPayingUsers)
{
	//pair the paying users with non paying
	//to help paying users win a couple of games, we pair them with lower skill rated players
	for (int i = PayingUsers.PlayerList.Num() - 1; i >= 0; --i)
	{
		if (NonPayingUsers.PlayerList.Num() > 0)
		{
			TPair< NPlayer*, NPlayer*> Pairing;

			int index = NonPayingUsers.PlayerList.Num() - 1;

			Pairing.Key = PayingUsers.PlayerList[i];
			Pairing.Value = NonPayingUsers.PlayerList[index];

			NonPayingUsers.PlayerList.RemoveAt(index);
			PayingUsers.PlayerList.RemoveAt(i);

			PairingsList.Add(Pairing);
		}
		else
		{
			break;
		}
	}

	//pair what is left
	PlayerBracket* Leftover = PayingUsers.PlayerList.Num() > 0 ? &PayingUsers : &NonPayingUsers;

	while (Leftover->PlayerList.Num() >= 2)
	{
		int size = Leftover->PlayerList.Num() - 1;

		for (int i = size; i >= 0; --i)
		{
			TPair< NPlayer*, NPlayer*> Pairing;
			Pairing.Key = Leftover->PlayerList[size];
			Pairing.Value = Leftover->PlayerList[size - 1];

			PairingsList.Add(Pairing);

			Leftover->PlayerList.RemoveAt(size);
			Leftover->PlayerList.RemoveAt(size - 1);
		}
	}

	for (NPlayer* Player : Leftover->PlayerList)
	{
		Player->MatchmakingData.NumberOfFailedMatchmakingAttempts++;
	}
}

void NMultiplayerMatchmaking::FilterPairings(TArray<TPair< NPlayer*, NPlayer*>>& Pairings)
{
	Pairings.RemoveAll(
		[this](const TPair< NPlayer*, NPlayer*>& Pairing)
		{
			float PairRating = RatePairing(Pairing);

			//if the rating is not good enough, and both players have been waiting less then allowed
			if (PairRating < NMatchmakingConstants::MinPairRating && 
				Pairing.Key->MatchmakingData.NumberOfFailedMatchmakingAttempts < NMatchmakingConstants::MaxNumberOfFailedMatchmakingAttempts &&
				Pairing.Value->MatchmakingData.NumberOfFailedMatchmakingAttempts < NMatchmakingConstants::MaxNumberOfFailedMatchmakingAttempts)
			{
				Pairing.Key->MatchmakingData.NumberOfFailedMatchmakingAttempts++;
				Pairing.Value->MatchmakingData.NumberOfFailedMatchmakingAttempts++;

				return true;
			}
			else
			{
				PlayerMatchmakingList.Remove(Pairing.Key);
				PlayerMatchmakingList.Remove(Pairing.Value);

				return false;
			}
		}
	);
}

float NMultiplayerMatchmaking::RatePairing(const TPair< NPlayer*, NPlayer*>& Pairing)
{
	float PlayerRatingA = Pairing.Key->MatchmakingData.PlayerRating;
	float PlayerRatingB = Pairing.Value->MatchmakingData.PlayerRating;
	float SkillRatingA = Pairing.Key->MatchmakingData.TempMatchmakingRating;
	float SkillRatingB = Pairing.Value->MatchmakingData.TempMatchmakingRating;

	float rating = 1/abs((PlayerRatingA - PlayerRatingB) * 0.3 + (SkillRatingA - SkillRatingB) * 0.7);

	return rating;
}
