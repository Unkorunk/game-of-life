#include "MapController.h"

namespace game_of_life {
	MapController::MapController(map_type& map) : map_(map), buff_map_(map.GetWidth(), map.GetHeight()) { }

	void MapController::Update() {
		for (size_t i = 0; i < map_.GetHeight(); i++) {
			for (size_t j = 0; j < map_.GetWidth(); j++) {
				size_t count = map_.MoorNeighborhood(i, j, 1, [](const bool& it) {
					return it;
				});

				buff_map_.Set(i, j, map_.Get(i, j));

				if (map_.Get(i, j)) {
					if (count < 2 || count > 3) {
						buff_map_.Set(i, j, false);
					}
				} else {
					if (count == 3) {
						buff_map_.Set(i, j, true);
					}
				}
			}
		}

		map_.Swap(buff_map_);
	}
}
