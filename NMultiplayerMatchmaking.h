#pragma once

#include "CoreMinimal.h"
#include "NPlayer.h";
/**
 * 
 */
struct PlayerBracket
{
	TArray<NPlayer*> PlayerList;
};

class PROJECTQ_API NMultiplayerMatchmaking
{
public:
	NMultiplayerMatchmaking();
	~NMultiplayerMatchmaking();

private:
	TArray<NPlayer*> PlayerMatchmakingList;

	TArray<PlayerBracket> PlayerBrackets;

	//called every X seconds
	void Tick();

	void CreateMatchmakingPairings(TArray<TPair< NPlayer*, NPlayer*>>& Pairings, TArray<NPlayer*>& PlayerMatchmakingList);

	void CalculateMatchmakingSkillRating(NPlayer* player) const;

	void MatchPlayers();

	void FilterPairings(TArray<TPair< NPlayer*, NPlayer*>>& Pairings);
	float RatePairing(const TPair< NPlayer*, NPlayer*>& Pairing);

	void SetUpBeginnersBracket(TArray<NPlayer*>& AllPlayers);

	void SetUpBrackets(TArray<NPlayer*>& AllPlayers);

	void SplitPlayersBasedOnPayments(PlayerBracket& Bracket, PlayerBracket& PayingUsers, PlayerBracket& NonPayingUsers);

	void CreatePairings(TArray<TPair< NPlayer*, NPlayer*>>& PairingsList, PlayerBracket& PayingUsers, PlayerBracket& NonPayingUsers);
public:
	//Called when a player starts matchmaking
	void AddPlayerToMatchmakingPool(NPlayer* player);

};
