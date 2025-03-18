
#ifndef GTKMM_MY_WINDOW
#define GTKMM_MY_WINDOW

#include <iostream>
using namespace Gtk;

class MyWindow : public Gtk::Window
{
    public:
         MyWindow();
    protected:
        //Signal handlers
        void on_quit_button_clicked();

        //Member widgets
        Gtk::Button m_quit_button;
        Gtk::Scale kp_scale;
        Glib::RefPtr<Gtk::Adjustment> kp_adjustment;



        // member data
        double kp = 0;
};




MyWindow::MyWindow():       m_quit_button("Quit"),
                            kp_adjustment( Gtk::Adjustment::create(0.0, 0.0, 101.0, 0.1, 1.0, 1.0) ),
                            kp_scale(kp_adjustment)
{
    set_title("My PID tuner");
    set_default_size(400, 400);

    //m_quit_button.set_margin(10);
    m_quit_button.set_size_request(10,10);
    m_quit_button.signal_clicked().connect(sigc::mem_fun(*this,
    &MyWindow::on_quit_button_clicked));
    set_child(m_quit_button);

    // kp scale
    
  
}



void MyWindow::on_quit_button_clicked()
{
  std::cout << "the  quit button was clicked" <<std::endl;
  set_visible(false);// Close the window
} 
#endif