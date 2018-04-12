#include "TestFrame.hpp"

void TestFrame::setLabel(int index, std::string lb) {
  if (index < 0 || index > 4) {
    std::cout << "TestFrame::setLabel: invalid index error." << std::endl;
    return;
  }
  label[index].set_text(lb);
}

double TestFrame::getValue(int index) {
  if (index < 0 || index > 4) {
    std::cout << "TestFrame::getValue: invalid index error." << std::endl;
    return -1;
  }

  return hscale[index].get_value();
}

void TestFrame::configureHScale(int index, double currentValue, double min, double max, double digits, double steps) {
  if (index < 0 || index > 4) {
    std::cout << "TestFrame::configureHScale: invalid index error." << std::endl;
    return;
  }
  hscale[index].set_digits(digits);
  hscale[index].set_increments(steps,steps*5);
  hscale[index].set_range(min,max);
  hscale[index].set_value(currentValue);
}

void TestFrame::init() {

  add(grid);
  set_label("Test Parameters");

  grid.set_column_spacing(5);
  grid.set_halign(Gtk::ALIGN_CENTER);
  grid.set_column_homogeneous(true);
  grid.set_size_request(300,-1);

  for (int i = 0; i < 5; i++) {
    grid.attach(label[i], 0, i, 1, 1);
    grid.attach(hscale[i], 1, i, 4, 1);

    label[i].set_text("HScale");
    hscale[i].set_digits(0);
    hscale[i].set_increments(1,1);
    hscale[i].set_range(0,100);
    hscale[i].set_valign(Gtk::ALIGN_CENTER);
  }
}

TestFrame::TestFrame() {
  init();
}

TestFrame::~TestFrame() {
}
