#include "jsonSaveManager.h"

using std::string;
using std::cout;
using std::endl;
using vision::Vision;

void jsonSaveManager::save_robots() {
  auto team = static_cast<int>(Teams::Real);
	for (Robot2* robot : interface->teams[team].robots) {
		json &robot_config = configs["Robots"][robot->get_role_name()];

		robot_config["ID"] = string(1, robot->ID);
		robot_config["tag"] = robot->tag;
		robot_config["speed"] = round(robot->default_target_velocity * 1000) / 1000;
	}
}

void jsonSaveManager::load_robots() {
	if (!exists(configs, "Robots")) return;

  auto team = static_cast<int>(Teams::Real);
	for (Robot2* robot : interface->teams[team].robots) {
		json &robot_config = configs["Robots"][robot->get_role_name()];

		if (exists(robot_config, "ID")) {
			string id = robot_config["ID"];
			robot->ID = id[0];
		}

		if (exists(robot_config, "tag")) robot->tag = robot_config["tag"];
		if (exists(robot_config, "speed"))
			robot->default_target_velocity = robot_config["speed"];
	}
}

void jsonSaveManager::save_camera() {
	json &camera_config = configs["Cameras"][interface->camera_card];

	Vision &vision = *(interface->visionGUI.vision);
	for (unsigned int i = 0; i < vision::Vision::MAX_COLORS; ++i) {
		json &lab = camera_config["CIELAB Calibration"][cielab_calib[i]];

		lab["L_min"] = vision.getCIE_L(i, 0);
		lab["L_max"] = vision.getCIE_L(i, 1);
		lab["A_min"] = vision.getCIE_A(i, 0);
		lab["A_max"] = vision.getCIE_A(i, 1);
		lab["B_min"] = vision.getCIE_B(i, 0);
		lab["B_max"] = vision.getCIE_B(i, 1);
		lab["dilate"] = vision.getDilate(i);
		lab["erode"] = vision.getErode(i);
		lab["blur"] = vision.getBlur(i);
		lab["amin"] = vision.getAmin(i);
	}

	if (interface->imageView.imageWarp.is_warp_ready()) {
		json &warp_mat = camera_config["warp_mat"];
		save_warp_matrix(warp_mat, interface->imageView.imageWarp.get_warp());
	} else {
		camera_config.erase("warp_mat");
	}

	if (interface->visionGUI.vision->flag_cam_calibrated) {
		//parâmetros intrínsecos e extrínsecos da câmera
		json &camera_matrix = camera_config["cam_mat"];
		config_dynamic_matrix(camera_matrix, interface->visionGUI.vision->cameraMatrix, true);

		json &cam_dst_coefficients = camera_config["dst_coefficients"];
		config_dynamic_matrix(cam_dst_coefficients, interface->visionGUI.vision->distanceCoeficents, true);
	}

	if (interface->imageView.imageWarp.is_adjust_ready()) {
		json &adjust_mat = camera_config["adjust_mat"];
		save_warp_matrix(adjust_mat, interface->imageView.imageWarp.get_adjust());
	} else {
		camera_config.erase("adjust_mat");
	}

	camera_config["offsetL"] = interface->imageView.imageWarp.get_offset_L();
	camera_config["offsetR"] = interface->imageView.imageWarp.get_offset_R();

	json &properties_config = camera_config["Camera Properties"];
	for (ControlHolder ctrl : interface->ctrl_list_default) {
		v4l2_control control{};
		interface->vcap.get_control(&control, ctrl.qctrl.id);
		string name = string((char *) ctrl.qctrl.name);
		properties_config[name] = control.value;
	}
}

void jsonSaveManager::load_camera() {
	if (!exists(configs["Cameras"], interface->camera_card)) return;
	json &camera_config = configs["Cameras"][interface->camera_card];
	
	Vision &vision = *(interface->visionGUI.vision);
	for (unsigned int i = 0; i < vision::Vision::MAX_COLORS; ++i) {
		json &lab = camera_config["CIELAB Calibration"][cielab_calib[i]];

		if (exists(lab, "L_min")) vision.setCIE_L(i, 0, lab["L_min"]);
		if (exists(lab, "L_max")) vision.setCIE_L(i, 1, lab["L_max"]);
		if (exists(lab, "A_min")) vision.setCIE_A(i, 0, lab["A_min"]);
		if (exists(lab, "A_max")) vision.setCIE_A(i, 1, lab["A_max"]);
		if (exists(lab, "B_min")) vision.setCIE_B(i, 0, lab["B_min"]);
		if (exists(lab, "B_max")) vision.setCIE_B(i, 1, lab["B_max"]);
		if (exists(lab, "dilate")) vision.setDilate(i, lab["dilate"]);
		if (exists(lab, "erode")) vision.setErode(i, lab["erode"]);
		if (exists(lab, "blur")) vision.setBlur(i, lab["blur"]);
		if (exists(lab, "amin")) vision.setAmin(i, lab["amin"]);
	}

	if (exists(camera_config, "warp_mat")) {
		json &warp_mat = camera_config["warp_mat"];
		interface->imageView.imageWarp.clear_warp_points();
		load_warp_matrix(warp_mat);
		interface->imageView.imageWarp.set_warp_ready();
	} else {
		interface->imageView.imageWarp.set_warp_ready(false);
	}

	if (exists(camera_config, "cam_mat") && exists(camera_config, "dst_coefficients")) {
		json &camera_matrix = camera_config["cam_mat"];
		json &cam_dst_coefficients = camera_config["dst_coefficients"];
		if (exists(camera_matrix, "rows") && exists(camera_matrix, "cols") &&
			exists(cam_dst_coefficients, "rows") && exists(cam_dst_coefficients, "cols")) {
			config_dynamic_matrix(camera_matrix, interface->visionGUI.vision->cameraMatrix, false);
			config_dynamic_matrix(cam_dst_coefficients, interface->visionGUI.vision->distanceCoeficents, false);
			interface->visionGUI.vision->flag_cam_calibrated = true;
		}
	} else {
		interface->visionGUI.vision->flag_cam_calibrated = false;
	}

	if (exists(camera_config, "adjust_mat")) {
		json &adjust_mat = camera_config["adjust_mat"];
		interface->imageView.imageWarp.clear_adjust_points();
		load_warp_matrix(adjust_mat, true);
		interface->imageView.imageWarp.set_adjust_ready();
	} else {
		interface->imageView.imageWarp.set_adjust_ready(false);
	}

	if (exists(camera_config, "offsetL")) interface->imageView.imageWarp.set_offset_L(camera_config["offsetL"]);
	if (exists(camera_config, "offsetR")) interface->imageView.imageWarp.set_offset_R(camera_config["offsetR"]);

	if (exists(camera_config, "Camera Properties")) {
		json &properties_config = camera_config["Camera Properties"];
		for (ControlHolder &ctrl : interface->ctrl_list_default) {
			string name = string((char *) ctrl.qctrl.name);
			if (exists(properties_config, name)) {
				int value = properties_config[name];
				interface->vcap.set_control(ctrl.qctrl.id, value);
			}
		}
	}
}

void jsonSaveManager::save_warp_matrix(json &mat_config, warp::PointArray& mat) {
	int count = 0;
	for (unsigned short index = 0; index < warp::MAX_POINTS; index++) {
		std::pair<int, int> point = {mat->at(index).x, mat->at(index).y};
		mat_config[std::to_string(count++)] = point;
	}
}

void jsonSaveManager::load_warp_matrix(const json &mat_config, const bool isAdjust) {
	for (const auto &point : mat_config) {
		interface->imageView.imageWarp.add_mat_point({point[0], point[1]}, isAdjust);
	}
}

//método pode ser usado para matrizes do opencv (tipo double) que não possuem tamanhos fixos
void jsonSaveManager::config_dynamic_matrix(json &mat_config, cv::Mat &mat, bool save) {
	if (save) {
		mat_config["rows"] = mat.rows;
		mat_config["cols"] = mat.cols;
		for (int i = 0; i < mat.rows; ++i) {
			for (int j = 0; j < mat.cols; ++j) {
				string pos_string = "[" + std::to_string(i) + "][" + std::to_string(j) + "]";
				mat_config[pos_string] = mat.at<double>(i, j);
			}
		}
	} else {
		mat = cv::Mat::zeros(mat_config["rows"], mat_config["cols"], cv::DataType<double>::type);
		for (int i = 0; i < mat.rows; ++i) {
			for (int j = 0; j < mat.cols; ++j) {
				string pos_string = "[" + std::to_string(i) + "][" + std::to_string(j) + "]";
				double value = mat_config[pos_string];
				mat.at<double>(i, j) = value;
			}
		}
	}
}

int jsonSaveManager::read_configs_from_file(string file_path) {
	try {
		std::ifstream file(file_path);
		if (file.is_open()) {
			file >> configs;
			file.close();
		} else return 1;
	} catch (std::exception &e) {
		string error = e.what();
		if (error.find("parse_error") != string::npos) return 2;
	}
	return 0;
}

void jsonSaveManager::write_configs_to_file(string file_path) {
	std::ofstream file(file_path);
	file << std::setw(4) << configs << endl;
	file.close();
}

void jsonSaveManager::load(const string file_path) {
	int error = read_configs_from_file(file_path);

	if (error == 1) cout << "File " << file_path << " not found" << endl;
	else if (error == 2) cout << file_path << " is not a valid JSON file" << endl;

	if (!error) {
		load_camera();
		load_robots();
	}
}

void jsonSaveManager::save(const string file_path) {
	int error = read_configs_from_file(file_path);
	if (error == 1) cout << "Creating " << file_path << endl;
	else if (error == 2) cout << file_path << " is not a valid JSON file, will be overwritten" << endl;

	if(interface->vcap.isCameraON) {
		save_camera();
	}else{
		std::cout << "CAN'T SAVE, CAMERA DISCONNECTED" << std::endl;
	}
	save_robots();
	write_configs_to_file(file_path);
}

bool jsonSaveManager::exists(json &config, string name) {
	return (config.find(name) != config.end());
}
