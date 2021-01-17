#pragma once

#include <vector>
#include "world.pb.h"
#include "Player.h"

namespace cellular_automaton {
	class PlayerCollection {
	public:
		PlayerCollection() = default;

		void Update();

		void Add(const Player& player);
		void Delete(const Player& player);
		void Delete(unsigned int player_id);
		
		Player& At(unsigned int player_id);

		void UpdateProto(generated_files::World& world);

		const std::vector<Player>& GetPlayers() const;

	private:
		std::vector<Player> players_;
		
	};
}