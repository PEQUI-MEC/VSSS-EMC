#include "jsonSaveManager.h"

using std::string;
using std::cout;
using std::endl;

void jsonSaveManager::save_robots() {
	for (int i = 0; i < 3; ++i) {
		json& robot_config = configs["Robots"]["Robot "+std::to_string(i+1)];
		Robot& robot = interface->robot_list.at((unsigned long) i);

		robot_config["ID"] = string(1, robot.ID);
		robot_config["role"] = robot.role;
		robot_config["speed"] = round(robot.vdefault*1000)/1000;
	}
}

void jsonSaveManager::load_robots() {
	if(!exists(configs, "Robots")) return;

	for (int i = 0; i < 3; ++i) {
		json& robot_config = configs["Robots"]["Robot "+std::to_string(i+1)];
		Robot& robot = interface->robot_list.at((unsigned long) i);

		if(exists(robot_config, "ID")){
			string id = robot_config["ID"];
			robot.ID = id[0];
		}
		if(exists(robot_config, "role")) robot.role = robot_config["role"];
		if(exists(robot_config, "speed")){
			robot.vdefault = robot_config["speed"];
			robot.vmax = robot.vdefault;
		}
	}
}

void jsonSaveManager::save_camera() {
	json& camera_config = configs["Cameras"][interface->camera_card];
	string hsv_calibs[4] = {"Main", "Green", "Ball", "Opp."};

	for (int i = 0; i < 4; ++i) {
		json& hsv = camera_config["HSV Calibration"][hsv_calibs[i]];
		Vision& vision = *(interface->visionGUI.vision);

		hsv["hue_min"] = vision.getHue(i, 0);
		hsv["hue_max"] = vision.getHue(i, 1);
		hsv["saturation_min"] = vision.getSaturation(i, 0);
		hsv["saturation_max"] = vision.getSaturation(i, 1);
		hsv["value_min"] = vision.getValue(i, 0);
		hsv["value_max"] = vision.getValue(i, 1);
		hsv["dilate"] = vision.getDilate(vision.HSV, i);
		hsv["erode"] = vision.getErode(vision.HSV, i);
		hsv["blur"] = vision.getBlur(vision.HSV, i);
		hsv["amin"] = vision.getAmin(vision.HSV, i);

		json& lab = camera_config["CIELAB Calibration"][hsv_calibs[i]];

		lab["L_min"] = vision.getCIE_L(i, 0);
		lab["L_max"] = vision.getCIE_L(i, 1);
		lab["A_min"] = vision.getCIE_A(i, 0);
		lab["A_max"] = vision.getCIE_A(i, 1);
		lab["B_min"] = vision.getCIE_B(i, 0);
		lab["B_max"] = vision.getCIE_B(i, 1);
		lab["dilate"] = vision.getDilate(vision.CIELAB, i);
		lab["erode"] = vision.getErode(vision.CIELAB, i);
		lab["blur"] = vision.getBlur(vision.CIELAB, i);
		lab["amin"] = vision.getAmin(vision.CIELAB, i);
	}

	if(interface->warped) {
		json &warp_mat = camera_config["warp_mat"];
		config_matrix(warp_mat, interface->imageView.warp_mat, true);
	}

	if(interface->imageView.adjust_rdy) {
		json &adjust_mat = camera_config["adjust_mat"];
		config_matrix(adjust_mat, interface->imageView.adjust_mat, true);
	}

	camera_config["offsetL"] = interface->offsetL;
	camera_config["offsetR"] = interface->offsetR;

	json& properties_config = camera_config["Camera Properties"];
	for(capture::ControlHolder ctrl : interface->ctrl_list_default) {
		v4l2_control control{};
		interface->vcap.get_control(&control, ctrl.qctrl.id);
		string name = string((char *) ctrl.qctrl.name);
		properties_config[name] = control.value;
	}
}

void jsonSaveManager::load_camera() {
	if(!exists(configs["Cameras"], interface->camera_card)) return;
	json& camera_config = configs["Cameras"][interface->camera_card];

	string hsv_calibs[4] = {"Main", "Green", "Ball", "Opp."};
	for (int i = 0; i < 4; ++i) {
		json& hsv = camera_config["HSV Calibration"][hsv_calibs[i]];
		json& lab = camera_config["CIELAB Calibration"][hsv_calibs[i]];
		Vision& vision = *(interface->visionGUI.vision);

		if(exists(hsv, "hue_min")) vision.setHue(i, 0, hsv["hue_min"]);
		if(exists(hsv, "hue_max")) vision.setHue(i, 1, hsv["hue_max"]);
		if(exists(hsv, "saturation_min")) vision.setSaturation(i, 0, hsv["saturation_min"]);
		if(exists(hsv, "saturation_max")) vision.setSaturation(i, 1, hsv["saturation_max"]);
		if(exists(hsv, "value_min")) vision.setValue(i, 0, hsv["value_min"]);
		if(exists(hsv, "value_max")) vision.setValue(i, 1, hsv["value_max"]);
		if(exists(hsv, "dilate")) vision.setDilate(vision.HSV, i, hsv["dilate"]);
		if(exists(hsv, "erode")) vision.setErode(vision.HSV, i, hsv["erode"]);
		if(exists(hsv, "blur")) vision.setBlur(vision.HSV, i, hsv["blur"]);
		if(exists(hsv, "amin")) vision.setAmin(vision.HSV, i, hsv["amin"]);

		if(exists(lab, "L_min")) vision.setCIE_L(i, 0, lab["L_min"]);
		if(exists(lab, "L_max")) vision.setCIE_L(i, 1, lab["L_max"]);
		if(exists(lab, "A_min")) vision.setCIE_A(i, 0, lab["A_min"]);
		if(exists(lab, "A_max")) vision.setCIE_A(i, 1, lab["A_max"]);
		if(exists(lab, "B_min")) vision.setCIE_B(i, 0, lab["B_min"]);
		if(exists(lab, "B_max")) vision.setCIE_B(i, 1, lab["B_max"]);
		if(exists(lab, "dilate")) vision.setDilate(vision.CIELAB, i, lab["dilate"]);
		if(exists(lab, "erode")) vision.setErode(vision.CIELAB, i, lab["erode"]);
		if(exists(lab, "blur")) vision.setBlur(vision.CIELAB, i, lab["blur"]);
		if(exists(lab, "amin")) vision.setAmin(vision.CIELAB, i, lab["amin"]);
	}

	if(exists(camera_config, "warp_mat")){
		json& warp_mat = camera_config["warp_mat"];
		config_matrix(warp_mat, interface->imageView.warp_mat, false);
		interface->warped = true;
	} else {
		interface->warped = false;
	}

	if(exists(camera_config, "adjust_mat")) {
		json &adjust_mat = camera_config["adjust_mat"];
		config_matrix(adjust_mat, interface->imageView.adjust_mat, false);
		interface->imageView.adjust_rdy = true;
	} else {
		interface->imageView.adjust_rdy= false;
	}

	if(exists(camera_config, "offsetL")) interface->offsetL = camera_config["offsetL"];
	if(exists(camera_config, "offsetR")) interface->offsetR = camera_config["offsetR"];

	if(exists(camera_config, "Camera Properties")){
		json& properties_config = camera_config["Camera Properties"];
		for(capture::ControlHolder& ctrl : interface->ctrl_list_default) {
			string name = string((char *) ctrl.qctrl.name);
			if(exists(properties_config, name)){
				int value = properties_config[name];
				interface->vcap.set_control(ctrl.qctrl.id, value);
			}
		}
	}
}

void jsonSaveManager::config_matrix(json& mat_config, int (&mat)[4][2], bool save){
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 2; ++j) {
			string pos_string = "["+std::to_string(i)+"]["+std::to_string(j)+"]";
			if(save) mat_config[pos_string] = mat[i][j];
			else mat[i][j] = mat_config[pos_string];
		}
	}
}

int jsonSaveManager::read_configs_from_file(string file_path) {
	try {
		std::ifstream file(file_path);
		if(file.is_open()) {
			file >> configs;
			file.close();
		} else return 1;
	} catch(std::exception& e) {
		string error = e.what();
		if(error.find("parse_error") != string::npos) return 2;
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

	if(error == 1) cout << "File " << file_path << " not found" << endl;
	else if(error == 2) cout << file_path << " is not a valid JSON file" << endl;

	if(!error){
		load_camera();
		load_robots();
	}
}

void jsonSaveManager::save(const string file_path) {
	int error = read_configs_from_file(file_path);
	if(error == 1) cout << "Creating " << file_path << endl;
	else if(error == 2) cout << file_path << " is not a valid JSON file, will be overwritten" << endl;

	save_camera();
	save_robots();
	write_configs_to_file(file_path);
}

bool jsonSaveManager::exists(json &config, string name) {
	return (config.find(name) != config.end());
}