#ifndef VSSS_CONFIGFILEMANAGER_H
#define VSSS_CONFIGFILEMANAGER_H

#include <lib/json.hpp>
#include <V4LInterface.hpp>

using json = nlohmann::json;

class jsonSaveManager {
	private:
		json configs;

		Game& game;
		capture::V4LInterface *interface;
		
		bool exists(json &config, std::string name);

		void save_team(Team &team, std::string name);
		void load_params(Team &team, std::string name);
		void load_team(Team &team, std::string name);
		void save_camera();
		void load_camera();
		void save_warp_matrix(json &mat_config, warp::PointArray& mat);
		void load_warp_matrix(const json &mat_config, bool isAdjust = false);
		void config_dynamic_matrix(json &mat_config, cv::Mat &mat, bool save);
		int read_configs_from_file(std::string file_path = "quicksave.json");
		void write_configs_to_file(std::string file_path = "quicksave.json");

	public:
		explicit jsonSaveManager(Game& game, capture::V4LInterface *interf) : game(game), interface(interf) {};
		void load(std::string file_path = "quicksave.json");
		void load_json_text(const std::string& json_text);
		void save(std::string file_path = "quicksave.json");
		VSSRef::Frame* load_replacement(std::string team, std::string fault, std::string file_path = "replacement.json");

		template<typename TYPE>
		void set(json &config, TYPE& variable, std::string name) {
			if (exists(config, name)) {
				variable = config[name];
			}
		}
};		

#endif //VSSS_CONFIGFILEMANAGER_H
