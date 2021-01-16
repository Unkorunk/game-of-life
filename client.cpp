#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <boost/asio.hpp>
#include "world.pb.h"

#include "Map.h"
#include "MapController.h"
#include "Camera.h"
#include "Player.h"

using boost::asio::ip::tcp;

const sf::Vector2f cell_offset(0.5f, 0.5f);
const sf::Vector2f cell_size (4.f,4.f);
const size_t width = 100;
const size_t height = 100;
const float fps = 1.f/8.f;
const sf::Color background_color(50, 50, 50);
const sf::Color cell_color(255, 255, 255);
const sf::Color grid_color(150, 150, 150);
const sf::Color builder_color(255, 0, 0, 100);
const sf::Color other_builder_color(0, 255, 0, 100);
const char* default_port = "5000";
const char* default_host = "localhost";

using namespace std;

uniform_int_distribution<size_t> dist(0,1);
random_device rd;

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


int main()
{
    game_of_life::Map<bool> map(width, height);
	game_of_life::MapController map_controller(map);

    boost::asio::io_context io_context;

    tcp::socket s(io_context);
    tcp::resolver resolver(io_context);
    boost::asio::connect(s, resolver.resolve(default_host, default_port));

    do_read(s);
    
    sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Game Of Life");

    game_of_life::Camera camera(window);
	
    camera.GetViewEditable().setCenter(sf::Vector2f(width * (cell_size.x + cell_offset.x) + cell_offset.x, height * (cell_size.y + cell_offset.y) + cell_offset.y) / 2.f);

    sf::Clock clock;
    sf::Clock fps_clock;
	
	game_of_life::Player player(map);
    
    bool grid_visible = true;
    bool game_paused = true;
    bool key_g_is_released = true;
    bool key_p_is_released = true;
    bool key_c_is_released = true;
    bool key_r_is_released = true;

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

        if (key_g_is_released) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
                grid_visible = !grid_visible;
                key_g_is_released = false;
            }
        }
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
            key_g_is_released = true;
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

        if (key_c_is_released) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
                map.Clear();
                key_c_is_released = false;
            }
        }
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
            key_c_is_released = true;
        }

        if (key_r_is_released) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                map.Clear();
                for (size_t i = 0; i < map.GetHeight(); i++) {
                    for (size_t j = 0; j < map.GetWidth(); j++) {
                    	map.Set(i, j, dist(rd));
                    }
                }
                key_r_is_released = false;
            }
        }
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            key_r_is_released = true;
        }

        window.clear(background_color);

        sf::RectangleShape builder_rect(cell_size);
        builder_rect.setFillColor(builder_color);
        sf::RectangleShape other_builder_rect(cell_size);
        other_builder_rect.setFillColor(other_builder_color);
        sf::RectangleShape cell_rect(cell_size);
        cell_rect.setFillColor(cell_color);
        sf::RectangleShape v_rect(sf::Vector2f(cell_offset.x, height * (cell_size.y + cell_offset.y) + cell_offset.y));
        v_rect.setFillColor(grid_color);
        sf::RectangleShape h_rect(sf::Vector2f(width * (cell_size.x + cell_offset.x) + cell_offset.x, cell_offset.y));
        h_rect.setFillColor(grid_color);

        if (grid_visible) {
            for (size_t i = 0; i < width + 1; i++) {
                v_rect.setPosition(i * (cell_size.x + cell_offset.x), 0.f);
                window.draw(v_rect);
            }

            for (size_t i = 0; i < height + 1; i++) {
                h_rect.setPosition(0.f, i * (cell_size.y + cell_offset.y));
                window.draw(h_rect);
            }
        }
        

        for (size_t i = 0; i < map.GetHeight(); i++) {
            for (size_t j = 0; j < map.GetWidth(); j++) {
                if (map.Get(i, j)){
                    cell_rect.setPosition(j * (cell_size.x + cell_offset.x) + cell_offset.x, i * (cell_size.y + cell_offset.y) + cell_offset.y);
                    window.draw(cell_rect);
                }
            }
        }
        
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