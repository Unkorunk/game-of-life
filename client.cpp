#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <boost/asio.hpp>
#include "world.pb.h"

#include "Map.h"
#include "MapController.h"
#include "MapRenderer.h"
#include "MapUtilities.h"
#include "Camera.h"
#include "Player.h"

using boost::asio::ip::tcp;

const size_t width = 100;
const size_t height = 100;
const float fps = 1.f/8.f;
const sf::Color builder_color(255, 0, 0, 100);
const sf::Color other_builder_color(0, 255, 0, 100);
const char* default_port = "5000";
const char* default_host = "localhost";

using namespace std;

std::string buff(1024,' ');
std::vector<sf::Vector2i> other_players;

void do_read(tcp::socket &socket)
{
    socket.async_read_some(boost::asio::buffer(buff),
        [&](boost::system::error_code ec, std::size_t length)
        {        	
            if (!ec)
            {
                other_players.clear();
                proto::World world;
                world.ParseFromArray(buff.data(), length);
                for (auto player : world.players()) {
                    if (!player.is_player()) other_players.emplace_back(player.x(), player.y());
                }
                do_read(socket);
            }
            else {
                exit(0);
            }
        });
}

class GameOfLifeMapController final : game_of_life::MapController<bool> {
public:
	explicit GameOfLifeMapController(game_of_life::Map<bool>& map)
		: MapController<bool>(map), buff_map_(map.GetWidth(), map.GetHeight()) {}

	void Update() override {
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

private:
	game_of_life::Map<bool> buff_map_;

};

class GameOfLifeMapUtilities final : game_of_life::MapUtilities<bool> {
public:
	GameOfLifeMapUtilities(game_of_life::Map<bool>& map, game_of_life::MapRenderer<bool>& map_renderer)
		: MapUtilities<bool>(map, map_renderer), dist_(0, 1) {}

	void Update() override {
		MapUtilities<bool>::Update();

		if (key_r_is_released_) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
				map_.Clear();
				for (size_t i = 0; i < map_.GetHeight(); i++) {
					for (size_t j = 0; j < map_.GetWidth(); j++) {
						map_.Set(i, j, dist_(rd_));
					}
				}
				key_r_is_released_ = false;
			}
		}
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
			key_r_is_released_ = true;
		}
	}

private:
	bool key_r_is_released_ = true;
    random_device rd_;
    uniform_int_distribution<size_t> dist_;

};

int main()
{
    game_of_life::Map<bool> map(width, height);
	GameOfLifeMapController map_controller(map);

    boost::asio::io_context io_context;

    tcp::socket s(io_context);
    tcp::resolver resolver(io_context);
    boost::asio::connect(s, resolver.resolve(default_host, default_port));

    do_read(s);
    
    sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Game Of Life");

    game_of_life::MapRenderer<bool> map_renderer(window, map);
	map_renderer.MapValueToExistence([](const bool& it) {
		return it;
	});
	map_renderer.MapValueToColor([](const bool& it) {
		return sf::Color(255, 255, 255);
	});
	
    game_of_life::Camera camera(window);

	auto cell_size = game_of_life::MapRenderer<bool>::kCellSize;
	auto cell_offset = game_of_life::MapRenderer<bool>::kCellOffset;
	
    camera.GetViewEditable().setCenter(sf::Vector2f(width * (cell_size.x + cell_offset.x) + cell_offset.x, height * (cell_size.y + cell_offset.y) + cell_offset.y) / 2.f);

    sf::Clock clock;
    sf::Clock fps_clock;
	
	game_of_life::Player player(map);

    GameOfLifeMapUtilities map_utilities(map, map_renderer);
	
    bool game_paused = true;
    bool key_p_is_released = true;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        
        io_context.poll();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

    	camera.Update(dt);

        sf::Vector2i old_builder_position = player.GetPosition();

        player.Update();

        if (old_builder_position != player.GetPosition()) {
            boost::asio::write(s, boost::asio::buffer(player.GetProto().SerializeAsString()));
        }

        if (key_p_is_released) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                game_paused = !game_paused;
                key_p_is_released = false;
            }
        }
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
            key_p_is_released = true;
        }

        map_utilities.Update();
        map_renderer.Update();

        sf::RectangleShape builder_rect(cell_size);
        builder_rect.setFillColor(builder_color);
        sf::RectangleShape other_builder_rect(cell_size);
        other_builder_rect.setFillColor(other_builder_color);
    	
        for (auto builder_position : other_players) {
            other_builder_rect.setPosition(builder_position.x * (cell_size.x + cell_offset.x) + cell_offset.x,
                builder_position.y * (cell_size.y + cell_offset.y) + cell_offset.y);
            window.draw(other_builder_rect);
        }
    	
        builder_rect.setPosition(player.GetX() * (cell_size.x + cell_offset.x) + cell_offset.x,
            player.GetY() * (cell_size.y + cell_offset.y) + cell_offset.y);
        window.draw(builder_rect);

        if (fps_clock.getElapsedTime().asSeconds() < fps) {
            window.display();
            continue;
        }
        fps_clock.restart();

        if (!game_paused) {
        	map_controller.Update();
        }

        window.display();
    }

    return 0;
}