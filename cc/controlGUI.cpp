#include "controlGUI.hpp"

bool ControlGUI::get_PID_test_flag() {
    return PID_test_flag;
}

void ControlGUI::set_PID_test_flag(bool input) {
    PID_test_flag = input;
}

ControlGUI::ControlGUI() {
    Serial_Enabled=false;
    // Adicionar o frame do Serial e sua VBOX
    pack_start(Top_hbox, false, true, 5);
    Top_hbox.pack_start(Serial_fm, false, true, 5);
    Serial_fm.set_label("Serial");
    Serial_fm.add(Serial_vbox);

    Top_hbox.pack_start(Test_fm, false, true, 5);

    Test_fm.set_label("Test");
    Test_fm.add(Test_vbox);
    button_PID_Test.set_label("PID Test on Click");
    Test_vbox.pack_start(button_PID_Test, false, true, 5);

    Serial_hbox[0].pack_start(cb_serial, false, true, 5);
    Serial_hbox[0].pack_start(bt_Serial_Start, false, true, 5);
    Serial_hbox[0].pack_start(bt_Serial_Refresh, false, true, 5);
    Serial_vbox.pack_start(Serial_hbox[0], false, true, 5);

    Serial_hbox[1].pack_start(cb_test, false, true, 5);
    Serial_hbox[1].pack_start(Tbox_V1, false, true, 5);
    Serial_hbox[1].pack_start(Tbox_V2, false, true, 5);
    Serial_hbox[1].pack_start(bt_Serial_test, false, true, 5);
    Serial_vbox.pack_start(Serial_hbox[1], false, true, 5);

    Serial_hbox[2].pack_start(send_cmd_box, false, true, 5);
    Serial_hbox[2].pack_start(bt_send_cmd, false, true, 5);
    send_cmd_box.set_width_chars(25);
    bt_send_cmd.set_label("Send Command");
    Serial_vbox.pack_start(Serial_hbox[2], false, true, 5);

    Tbox_V1.set_max_length(6);
    Tbox_V2.set_max_length(6);
    Tbox_V1.set_width_chars(6);
    Tbox_V2.set_width_chars(6);
    cb_test.append("Robot A");
    cb_test.append("Robot B");
    cb_test.append("Robot C");
    cb_test.append("Robot D");
    cb_test.append("Robot E");
    cb_test.append("Robot F");
    cb_test.append("All");

    cb_test.set_active(6); // ALL
    Tbox_V1.set_text("0.8");
    Tbox_V2.set_text("0.8");


    bt_Serial_Start.set_state(Gtk::STATE_NORMAL);
    cb_serial.set_state(Gtk::STATE_NORMAL);

    Tbox_V1.set_state(Gtk::STATE_INSENSITIVE);
    Tbox_V2.set_state(Gtk::STATE_INSENSITIVE);
    bt_Serial_Refresh.set_state(Gtk::STATE_INSENSITIVE);
    bt_Serial_test.set_state(Gtk::STATE_INSENSITIVE);
    cb_test.set_state(Gtk::STATE_INSENSITIVE);
    bt_Robot_Status.set_state(Gtk::STATE_INSENSITIVE);

    bt_Serial_Start.set_label("Start");
    bt_Serial_Refresh.set_label("Refresh");

    bt_Serial_test.set_label("Send");

    _create_status_frame();

    pack_start(testFrame, false, true, 5);
    configureTestFrame();

    _update_cb_serial();
    // Conectar os sinais para o acontecimento dos eventos
    button_PID_Test.signal_pressed().connect(sigc::mem_fun(*this, &ControlGUI::_PID_Test));
    bt_Serial_test.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_send_test));
    bt_Serial_Refresh.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_update_cb_serial));
    bt_Serial_Start.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_start_serial));
    bt_Robot_Status.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_robot_status));
    bt_send_cmd.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_send_command));
}

void ControlGUI::configureTestFrame() {
    std::string labels[5] = {"Name 1", "Name 2", "Name 3", "Name 4", "Name 5"};
    double min[5] = {0, 0, 0, 0, 0};
    double max[5] = {100, 100, 100, 100, 100};
    double currentValue[5] = {0, 10, 20, 30, 40};
    double digits[5] = {0, 0, 0, 0, 0};
    double steps[5] = {1, 1, 1, 1, 1};

    for (int i = 0; i < 5; i++) {
        testFrame.setLabel(i, labels[i]);
        testFrame.configureHScale(i, currentValue[i],  min[i], max[i], digits[i], steps[i]);
    }
}

void ControlGUI::_send_command(){
    std::string cmd;
    cmd.append(send_cmd_box.get_text());
    s.sendAPISerialText(cmd);
}

void ControlGUI::_PID_Test(){
    PID_test_flag = !PID_test_flag;
}

// translate battery message
void ControlGUI::handleBatteryMsg(char buf[12], int id) {
    double battery;
    std::string cmd(buf);
    // check if first element is an ID
    if (cmd[0] != 'A' && cmd[0] != 'B' && cmd[0] != 'C' && cmd[0] != 'D' && cmd[0] != 'E' && cmd[0] != 'F') {
        std::cout << "ControlGUI::updateBattery: failed to update battery (WRONG ID). " << cmd << std::endl;
        return;
    }

    // check if the message's type is correct
    if (cmd.find("VBAT;") == std::string::npos) {
        std::cout << "ControlGUI::updateBattery: failed to update battery (WRONG MSG TYPE). " << cmd << std::endl;
        return;
    }

    // get battery
    battery = atof(cmd.substr(6,4).c_str());
    battery = ((battery - 6.4)/2.0)*100; // % of battery

    // update battery
    updateInterfaceStatus(battery, id);
}

// Gets battery % and robot id to update a single robot's battery status
void ControlGUI::updateInterfaceStatus(double battery, int id) {
    if (battery > 20) {
        status_img[id].set("img/online.png");
        battery_bar[id].set_fraction(battery/100);
        status_lb[id].set_text(std::to_string(battery).substr(0,5)+"%");
    }
    else if (battery > 0) {
        status_img[id].set("img/critical.png");
        battery_bar[id].set_fraction(battery/100);
        status_lb[id].set_text(std::to_string(battery).substr(0,5)+"%");
    }
    else {
        status_img[id].set("img/zombie.png");
        battery_bar[id].set_fraction(0.0);
        battery_bar[id].set_text("0%");
        status_lb[id].set_text("DEAD");
    }
}

// update the battery status of all robots
void ControlGUI::_robot_status(){
    std::string cmd[TOTAL_ROBOTS] = {"A@BAT#", "B@BAT#", "C@BAT#", "D@BAT#", "E@BAT#", "F@BAT#"};
    std::string dateString;
    time_t tt;
    char buf[TOTAL_ROBOTS][12];

    // define last update time
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    tt = std::chrono::system_clock::to_time_t ( now );
    dateString.append("Last Update: ");
    dateString.append(std::string(ctime(&tt)).substr(0,24));
    lastUpdate_lb.set_text(dateString);

    // update robot status
    for (int i = 0; i < TOTAL_ROBOTS; i++) {
        // s.sendSerial(cmd[i]);
        usleep(100000);
        if(s.readSerial(buf[i],sizeof buf[i]) == 1) {
            handleBatteryMsg(buf[i], i);
        } else {
            status_img[i].set("img/offline.png");
            battery_bar[i].set_fraction(0.0);
            battery_bar[i].set_text("0%");
            status_lb[i].set_text("Offline");
        }
    }

}

void ControlGUI::_start_serial(){
    int fd;
    Glib::ustring serial = cb_serial.get_active_text();

    if (serial.size() < 1) return;
    fd = s.start(serial);

     if(fd != -1)
    {
           std::cout<<serial<<" - Connected"<<std::endl;
    } else{
        std::cout<<serial<<" - Error"<<std::endl;
    }



    bt_Serial_Start.set_state(Gtk::STATE_INSENSITIVE);
    cb_serial.set_state(Gtk::STATE_INSENSITIVE);

    Tbox_V1.set_state(Gtk::STATE_NORMAL);
    Tbox_V2.set_state(Gtk::STATE_NORMAL);
    pid_edit_bt.set_state(Gtk::STATE_NORMAL);
    bt_Serial_Refresh.set_state(Gtk::STATE_NORMAL);
    bt_Serial_test.set_state(Gtk::STATE_NORMAL);
    cb_test.set_state(Gtk::STATE_NORMAL);
    pid_edit_bt.set_state(Gtk::STATE_NORMAL);
    bt_Robot_Status.set_state(Gtk::STATE_NORMAL);
}

bool ControlGUI::isFloat(std::string value){
    int counter = 0, i = 0;

    if (value.size() < 1 || value.front() == '.' || value.back() == '.')
        return false;

    if(!isdigit(value[0])) {
        if(value[0] != '-')
            return false;
        else
            i = 1;
    }

    for (; i < value.size(); i++) {
        if (value[i] == '.') counter++;
        else if (!isdigit(value[i])) return false;
    }
    // só pode ter um ponto
    if (counter > 1) return false;

    return true;
}

void ControlGUI::_send_test(){
    std::string cmd;

    // verifica se os valores inseridos nos campos são válidos (são números entre -1.4 e 1.4)
    if(!isFloat(Tbox_V1.get_text()))
        Tbox_V1.set_text("0");
    if(!isFloat(Tbox_V2.get_text()))
        Tbox_V2.set_text("0");

    float v1 = std::stof(Tbox_V1.get_text());
    float v2 = std::stof(Tbox_V2.get_text());

    if(abs(v1) > 1.4) {
        if(v1 < 0)
            Tbox_V1.set_text("-1.4");
        else
            Tbox_V1.set_text("1.4");
    }
    if(abs(v2) > 1.4) {
        if(v2 < 0)
            Tbox_V2.set_text("-1.4");
        else
            Tbox_V2.set_text("1.4");
    }

    switch(cb_test.get_active_row_number()){
        case -1:
        return;
        break;

        case 0:
        cmd.append("A@");
        cmd.append(Tbox_V1.get_text());
        cmd.append(";");
        cmd.append(Tbox_V2.get_text());
        cmd.append("#");
        break;

        case 1:
        cmd.append("B@");
        cmd.append(Tbox_V1.get_text());
        cmd.append(";");
        cmd.append(Tbox_V2.get_text());
        cmd.append("#");
        break;

        case 2:
        cmd.append("C@");
        cmd.append(Tbox_V1.get_text());
        cmd.append(";");
        cmd.append(Tbox_V2.get_text());
        cmd.append("#");
        break;

        case 3:
        cmd.append("D@");
        cmd.append(Tbox_V1.get_text());
        cmd.append(";");
        cmd.append(Tbox_V2.get_text());
        cmd.append("#");
        break;

        case 4:
        cmd.append("E@");
        cmd.append(Tbox_V1.get_text());
        cmd.append(";");
        cmd.append(Tbox_V2.get_text());
        cmd.append("#");
        break;

        case 5:
        cmd.append("F@");
        cmd.append(Tbox_V1.get_text());
        cmd.append(";");
        cmd.append(Tbox_V2.get_text());
        cmd.append("#");
        break;

        case 6:
        cmd.append("A@");
        cmd.append(Tbox_V1.get_text());
        cmd.append(";");
        cmd.append(Tbox_V2.get_text());
        cmd.append("#");

        cmd.append("B@");
        cmd.append(Tbox_V1.get_text());
        cmd.append(";");
        cmd.append(Tbox_V2.get_text());
        cmd.append("#");

        cmd.append("C@");
        cmd.append(Tbox_V1.get_text());
        cmd.append(";");
        cmd.append(Tbox_V2.get_text());
        cmd.append("#");

        cmd.append("D@");
        cmd.append(Tbox_V1.get_text());
        cmd.append(";");
        cmd.append(Tbox_V2.get_text());
        cmd.append("#");

        cmd.append("E@");
        cmd.append(Tbox_V1.get_text());
        cmd.append(";");
        cmd.append(Tbox_V2.get_text());
        cmd.append("#");

        cmd.append("F@");
        cmd.append(Tbox_V1.get_text());
        cmd.append(";");
        cmd.append(Tbox_V2.get_text());
        cmd.append("#");
        break;

        break;
    }
    // s.sendSerial(cmd);
}

void ControlGUI::_update_cb_serial(){

    std::string port;
    int fd;
    cb_serial.remove_all();
    for(int i = 0; i < 256; ++i) {
        port.clear();
        port.append("/dev/ttyUSB");
        port.append(std::to_string(i));

        fd = open(port.c_str(), O_RDWR);
        if(fd != -1) {
            std::cout<<port<<std::endl;
            cb_serial.append(port);
        }
    }
    bt_Serial_Start.set_state(Gtk::STATE_NORMAL);
    cb_serial.set_state(Gtk::STATE_NORMAL);
    bt_Serial_Refresh.set_state(Gtk::STATE_NORMAL);

    pid_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
    Tbox_V1.set_state(Gtk::STATE_INSENSITIVE);
    Tbox_V2.set_state(Gtk::STATE_INSENSITIVE);
    bt_Serial_test.set_state(Gtk::STATE_INSENSITIVE);
    cb_test.set_state(Gtk::STATE_INSENSITIVE);
    bt_Robot_Status.set_state(Gtk::STATE_INSENSITIVE);
    s.Serial_Enabled = false;

}

void ControlGUI::_create_status_frame(){

    pack_start(status_fm, false, true, 5);
    status_fm.set_label("Robot Status");
    status_fm.add(status_grid);

    status_grid.set_border_width(10);
    status_grid.set_column_spacing(10);
    status_grid.set_halign(Gtk::ALIGN_CENTER);

    bt_Robot_Status.set_label("Update");
    bt_Robot_Status.set_state(Gtk::STATE_NORMAL);
    status_grid.attach(bt_Robot_Status, 0, 0, 1, 1);
    lastUpdate_lb.set_text("Last Update: -");
    lastUpdate_lb.set_valign(Gtk::ALIGN_BASELINE);
    status_grid.attach(lastUpdate_lb, 1, 0, 3, 1);

    std::vector<std::string> name;
    name.push_back("Robot A");
    name.push_back("Robot B");
    name.push_back("Robot C");
    name.push_back("Robot D");
    name.push_back("Robot E");
    name.push_back("Robot F");

    for (int i = 0; i < TOTAL_ROBOTS; i++) {
        status_img[i].set("img/offline.png");
        status_grid.attach(status_img[i], 0, i+1, 1, 1);
        robots_lb[i].set_text(name.at(i));
        status_grid.attach(robots_lb[i], 1, i+1, 1, 1);
        battery_bar[i].set_valign(Gtk::ALIGN_CENTER);
        status_grid.attach(battery_bar[i], 2, i+1, 1, 1);
        status_lb[i].set_text("Offline");
        status_grid.attach(status_lb[i], 3, i+1, 1, 1);
    }
}

// Função para verificar se os valores digitados nos campos
// de PID são válidos: apenas números e um único ponto
bool ControlGUI::checkPIDvalues(){
    std::string value;
    int counter;
    for (int i = 0; i < 3; i++) {
        counter = 0;
        value.clear();
        value.append(pid_box[i].get_text());
        std::cout << i << ": " << value << std::endl;

        if (value.front() == '.' || value.back() == '.')
            return false;
        for (int j = 0; j < value.size(); j++) {
            if (value[j] == '.') counter++;
            if (!isdigit(value[j]) && value[j] != '.') return false;
        }
        if (counter > 1) return false;
    }
    return true;
}
