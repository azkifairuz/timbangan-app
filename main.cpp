#include <gtkmm.h>
#include <libserial/SerialStream.h>
#include <libserial/SerialStreamBuf.h>
#include <iostream>
#include <fstream>

using namespace LibSerial;
using namespace std;

class TimbanganApp : public Gtk::Window {
public:
    TimbanganApp();
    virtual ~TimbanganApp();

private:
    // Signal handlers
    void on_button_read();

    // Member widgets
    Gtk::Box m_VBox;
    Gtk::Label m_Label;
    Gtk::Button m_Button;

    // Serial stream
    SerialStream serial;
};

TimbanganApp::TimbanganApp()
    : m_VBox(Gtk::ORIENTATION_VERTICAL),
      m_Label("Berat:"),
      m_Button("Ambil Data") {
    set_title("Timbangan App");
    set_default_size(300, 100);

    // Setup serial port
    serial.Open("/dev/ttyUSB0");
    serial.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
    serial.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);

    // Configure layout
    m_VBox.pack_start(m_Label);
    m_VBox.pack_start(m_Button);
    add(m_VBox);

    m_Button.signal_clicked().connect(sigc::mem_fun(*this, &TimbanganApp::on_button_read));

    show_all_children();
}

TimbanganApp::~TimbanganApp() {
    if (serial.IsOpen())
        serial.Close();
}

void TimbanganApp::on_button_read() {
    if (serial.IsOpen()) {
        string data;
        getline(serial, data);
        m_Label.set_text("Berat: " + data);

        ofstream printer("/dev/usb/lp0");
        if (printer.is_open()) {
            printer << "Berat: " << data << endl;
            printer.close();
        } else {
            cout << "Gagal mengakses printer." << endl;
        }
    } else {
        m_Label.set_text("Gagal membaca dari timbangan.");
    }
}

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
    TimbanganApp window;
    return app->run(window);
}
