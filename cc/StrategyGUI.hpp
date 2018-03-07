/*
 * strategyGUI.hpp
 *
 *  Created on: May 8th, 2016
 *      Author: Daniel
 */

#ifndef STRATEGYGUI_HPP_
#define STRATEGYGUI_HPP_

#include <gtkmm.h>
#include <boost/algorithm/string.hpp>
#include "Strategy.hpp"
#include "filechooser.hpp"

class StrategyGUI : public Gtk::VBox {
	private:
		bool deslocamentoZagaAtaque_flag = false;
		bool goalSize_flag = false;
		bool defenseLine_flag = false;
		bool goalMax_flag = false;
		bool goalMin_flag = false;
		bool goalCenter_flag = false;
		bool areasDivision_flag = false;
		bool banheira_flag = false;
		bool areaLimitX_flag = false;
		bool ballRadius_flag = false;
		bool goalArea_flag = false;
		bool sideRectangles_flag = false;

		char *unconstant_char(const char *c_str);
		void _event_loadFormation_bt_clicked();
		void _event_deleteFormation_bt_clicked();
		void _event_saveFormation_bt_clicked();
		void _event_createFormation_bt_clicked();
		void _event_formation_box_changed();
		void _event_set_penalty_behavior();
		void loadSavedFormations();

	public:
		Strategy strategy;

		//Tree model columns:
		class ModelColumns : public Gtk::TreeModel::ColumnRecord {
			public:
				ModelColumns() {
					add(strategy_name);
				}

				Gtk::TreeModelColumn<Glib::ustring> strategy_name;
		};

		ModelColumns strategyColumns;

		Gtk::Frame menu_fm;
		Gtk::Frame info_text_fm;
		Gtk::Frame info_img_fm;
		Gtk::TreeView m_TreeView;
		Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

		Gtk::HBox selection_hbox;

		Gtk::VBox menu_vbox;
		Gtk::HBox menu_hbox[2];
		Gtk::VBox info_text_hbox;
		Gtk::Image strategy_img;
		Gtk::Button select_button;

		Gtk::ScrolledWindow m_ScrolledWindow;
		Gtk::TextView strategy_description_view;
		Glib::RefPtr<Gtk::TextBuffer> strategy_description_text;

		bool updating_formation_flag = false;

		Gtk::Frame formation_fm;
		bool formation_flag = false;
		bool update_interface_flag = false;
		Gtk::Grid formation_grid;
		Gtk::Button bt_loadFormation;
		Gtk::Button bt_deleteFormation;
		Gtk::ToggleButton bt_createFormation;
		Gtk::Button bt_saveFormation;
		Gtk::ComboBoxText formation_box;
		Gtk::Entry formation_name_entry;
		cv::Point formation_positions[3]; // é atualizado só se a flag estiver ativada
		float formation_orientations[3]; // é atualizado só se a flag estiver ativada

		Gtk::Frame behavior_fm;
		Gtk::Grid behavior_grid;
		Gtk::ToggleButton bt_penaltyBehavior;

		void createBehaviorsFrame();
		void configureTestFrame();
		void createFormationFrame();
		void createMenuFrame();
		void createInfoTextFrame();
		void createInfoImageFrame();
		StrategyGUI();
		~StrategyGUI(){};
};

#endif /* STRATEGYGUI_HPP_ */
