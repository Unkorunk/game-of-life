#pragma once

#include <vector>
#include <functional>

namespace cellular_automaton {
	template <typename T>
	class Map {
	public:
		Map(size_t width, size_t height)
			: width_(width), height_(height), map_(height, std::vector<T>(width)) { }

		size_t GetWidth() const {
			return width_;
		}

		size_t GetHeight() const {
			return height_;
		}

		T Get(size_t row, size_t col) const {
			return map_.at(row).at(col);
		}

		void Set(size_t row, size_t col, T val) {
			map_.at(row).at(col) = val;
		}

		size_t MoorNeighborhood(size_t row, size_t col, size_t dimension, const std::function<bool(const T&)>& selector) {
			size_t result = 0;

			const int dim = static_cast<int>(dimension);

			for (int dy = -dim; dy <= dim; dy++) {
				size_t current_row = (row + dy + height_) % height_;
				for (int dx = -dim; dx <= dim; dx++) {
					size_t current_col = (col + dx + width_) % width_;
					if (row == current_row && col == current_col) {
						continue;
					}

					result += selector(map_[current_row][current_col]);
				}
			}

			return result;
		}

		void Swap(Map& other) {
			std::swap(width_, other.width_);
			std::swap(height_, other.height_);
			map_.swap(other.map_);
		}

		void Clear() {
			map_ = std::vector<std::vector<T>>(height_, std::vector<T>(width_));
		}

	private:
		size_t width_, height_;
		std::vector<std::vector<T>> map_;

	};
}
