#ifndef TESTFRAME_HPP_
#define TESTFRAME_HPP_

#include <gtkmm.h>
#include <string>
#include <iostream>

class TestFrame: public Gtk::Frame
{
private:
  Gtk::Grid grid;

  Gtk::HScale hscale[5];
  Gtk::Label label[5];

private:
  void init();

public:
  TestFrame();
  ~TestFrame();

  double getValue(int index);
  void configureHScale(int index, double currentValue, double min, double max, double digits=1, double steps=1);
  void setLabel(int index, std::string label);
  void setName(std::string name);
};


#endif /* TESTFRAME_HPP_ */
