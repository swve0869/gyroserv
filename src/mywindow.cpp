#include "mywindow.h"

MyWindow::MyWindow():
    m_adjustment_digits( Gtk::Adjustment::create(1.0, 0.0, 5.0, 1.0, 2.0) ),
    m_button("mysilly button", 0)
{
  set_title("My PID tuner");
  set_default_size(200, 200);


  m_adjustment_digits->signal_value_changed().connect(sigc::mem_fun(*this,
    &MyWindow::on_adjustment1_value_changed));
  
}


void MyWindow::on_adjustment1_value_changed()
{
  const double val = m_adjustment_digits->get_value();
  kp = (double)val;

}