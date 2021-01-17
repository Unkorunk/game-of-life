#include "PlayerCollection.h"

namespace cellular_automaton {
	void PlayerCollection::Update() {
		for (Player& player : players_) {
			if (player.IsPlayer()) {
				player.Update();
			}
		}
	}

	void PlayerCollection::Add(const Player& player) {
		for (const Player& it : players_) {
			if (it.GetId() == player.GetId()) {
				return;
			}
		}

		players_.push_back(player);
	}

	void PlayerCollection::Delete(const Player& player) {
		Delete(player.GetId());
	}

	void PlayerCollection::Delete(unsigned int player_id) {
		for (size_t i = 0; i < players_.size(); i++) {
			if (players_[i].GetId() == player_id) {
				std::swap(players_[i], players_.back());
				players_.pop_back();
				return;
			}
		}
	}

	Player& PlayerCollection::At(unsigned int player_id) {
		for (auto& player : players_) {
			if (player.GetId() == player_id) {
				return player;
			}
		}

		throw std::exception();
	}

	void PlayerCollection::UpdateProto(generated_files::World& world) {
		for (size_t i = 0; i < world.players_size(); i++) {
			const generated_files::Player& player = world.players(i);
			for (Player& it : players_) {
				if (it.GetId() == player.id()) {
					it.UpdateProto(player);
				}
			}
		}
	}

	const std::vector<Player>& PlayerCollection::GetPlayers() const {
		return players_;
	}
}
