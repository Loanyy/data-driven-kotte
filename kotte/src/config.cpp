#include "kotte/config.hpp"

#include <nlohmann/json-schema.hpp>
#include <raylib.h>

#include <fstream>
#include <iterator>
#include <stdexcept>

namespace kotte
{
    void from_json(const nlohmann::json& json, KeyBindings& bindings){
        json.at("place_bomb").get_to(bindings.place_bomb);

        // TODO: read move_up, move_down, move_left, move_right and quit the same way.


        json.at("move_up").get_to(bindings.move_up);
        json.at("move_down").get_to(bindings.move_down);
        json.at("move_left").get_to(bindings.move_left);
        json.at("move_right").get_to(bindings.move_right);
        json.at("quit").get_to(bindings.quit);

        //done, added 5 missing keys from json
        // from_json reads values from json to c++, so we can use them in game
    }

    void to_json(nlohmann::json& json, const KeyBindings& bindings){
        json = nlohmann::json{
            {"place_bomb", bindings.place_bomb},

            // TODO: write move_up, move_down, move_left, move_right and quit the same way.

            { "move_up", bindings.move_up },
            { "move_down", bindings.move_down },
            { "move_left", bindings.move_left },
            { "move_right", bindings.move_right },
            { "quit", bindings.quit }

        };

        //done, again added 5 missing keys
        // to_json converts the struct to json again, in case if config is broken or smth, this is creating json just in case.
        // however write_config() writes into json, not to_json().
    }

    void from_json(const nlohmann::json& json, Config& config){
        json.at("tile_map_width").get_to(config.tile_map_width);
        json.at("tile_map_height").get_to(config.tile_map_height);
        json.at("seed").get_to(config.seed);
        json.at("input").get_to(config.input);
    }

    void to_json(nlohmann::json& json, const Config& config){
        json = nlohmann::json{
            {"tile_map_width", config.tile_map_width},
            {"tile_map_height", config.tile_map_height},
            {"seed", config.seed},
            {"input", config.input},
        };
    }

    Config default_config(){
        return Config{
            .tile_map_width = 17,
            .tile_map_height = 13,
            .seed = 323453473861,
            .input = KeyBindings{
                .move_up = {"W", "UP"},
            .move_down = {"S", "DOWN"},
            .move_left = {"A", "LEFT"},
            .move_right = {"D", "RIGHT"},
            .place_bomb = {"SPACE", "ENTER"},
            .quit = {"Q", "ESCAPE"}
            }
        };
    }

    namespace {
        void write_config(const std::string& config_path, const Config& config){
            std::ofstream config_file{config_path, std::ios::trunc};
            config_file << nlohmann::json(config).dump(4);
        }
    }

    Config load_config(const std::string& config_path, const std::string& schema_path){
        std::ifstream schema_file{schema_path};
        if(!schema_file.is_open()){
            throw std::runtime_error{"Could not open " + schema_path};
        }

        nlohmann::json_schema::json_validator validator;
        validator.set_root_schema(nlohmann::json::parse(schema_file));

        std::ifstream config_file{config_path};
        if(!config_file.is_open()){
            TraceLog(LOG_WARNING, "%s not found; writing defaults", config_path.c_str());
            const Config config = default_config();
            write_config(config_path, config);
            return config;
        }

        const std::string raw_content{
            std::istreambuf_iterator<char>{config_file},
            std::istreambuf_iterator<char>{}};
        config_file.close();

        try{
            nlohmann::json config_json = nlohmann::json::parse(raw_content);
            validator.validate(config_json);
            return config_json.get<Config>();
        } catch(const std::exception& error){
            TraceLog(
                LOG_WARNING,
                "%s is invalid (%s); restoring defaults",
                config_path.c_str(), error.what());

            const Config config = default_config();
            write_config(config_path, config);
            return config;
        }
    }
}
