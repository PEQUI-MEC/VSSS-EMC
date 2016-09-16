#ifndef INFOFM_HPP_
#define INFOFM_HPP_

#include <gtkmm.h>


class InfoFm: public Gtk::VBox
{
public:
	Gtk::Label *label;
	Gtk::VBox main_vbox;
	Gtk::HBox robots_pos_hbox;

	vector<Tag> tag_list;
	
	void setTagList(vector<Tag> list)
	{
		tag_list = list;
	}
	
	InfoFm()
	{
		pack_start(main_vbox, false, true, 5);
		
	}
	
	
	



};



#endif /* INFOFM_HPP_ */
