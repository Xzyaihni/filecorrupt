#include <iostream>
#include <functional>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/filectrl.h>

#include "corrupt.h"

class corrupt_app : public wxApp
{
public:
    virtual bool OnInit();
};

const std::string output_prefix = "corrupted_";

class main_frame : public wxFrame
{
public:
    main_frame();

    void random_checkbox(wxCommandEvent& event);
    void filepath_choice(wxFileCtrlEvent& event);
    void corrupt_button(wxCommandEvent& event);

private:
    void display_error(const std::string message) noexcept;

    template<typename T>
    T convert_number(const std::string number, const T invalid_value, std::function<T(std::string)> func) noexcept;

    wxTextCtrl* _start_pos_text;
    wxTextCtrl* _step_size_text;

    wxTextCtrl* _shift_value_text;
    wxTextCtrl* _random_chance_text;

    bool _random_mode = false;

    std::filesystem::path _corrupt_filepath;

    corruptor _corruptor;
};


wxIMPLEMENT_APP(corrupt_app);

bool corrupt_app::OnInit()
{
    main_frame* frame = new main_frame();

    frame->Show(true);

    return true;
}

main_frame::main_frame()
: wxFrame(NULL, wxID_ANY, "omg its like a corrupting thingy", wxDefaultPosition)
{
    wxBoxSizer* main_horizontal = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* file_vertical = new wxBoxSizer(wxVERTICAL);

    wxFileCtrl* file_ctl = new wxFileCtrl(this, wxID_ANY);
    file_ctl->SetSize(wxSize(400, 250));
    file_ctl->Bind(wxEVT_FILECTRL_SELECTIONCHANGED, &main_frame::filepath_choice, this);
    file_vertical->Add(file_ctl, wxSizerFlags(1).Top().FixedMinSize().Expand().Border(wxALL, 7));

    wxButton* corrupt_button = new wxButton(this, wxID_ANY, "corrupt", wxDefaultPosition, wxSize(100, 30));
    corrupt_button->Bind(wxEVT_BUTTON, &main_frame::corrupt_button, this);
    file_vertical->Add(corrupt_button, wxSizerFlags(0).Bottom().Shaped().Border(wxALL, 10));

    main_horizontal->Add(file_vertical, wxSizerFlags(1).Expand());

    wxBoxSizer* options_vertical = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* offsets_horizontal = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* start_pos_vertical = new wxBoxSizer(wxVERTICAL);
    _start_pos_text = new wxTextCtrl(this, wxID_ANY, "0");
    _start_pos_text->SetMaxSize(wxSize(-1, 30));

    start_pos_vertical->Add(new wxStaticText(this, wxID_ANY, "start position (in bytes)"));
    start_pos_vertical->Add(_start_pos_text, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 5));

    offsets_horizontal->Add(start_pos_vertical, wxSizerFlags(1).Expand());

    wxBoxSizer* step_size_vertical = new wxBoxSizer(wxVERTICAL);
    _step_size_text = new wxTextCtrl(this, wxID_ANY, "1");
    _step_size_text->SetMaxSize(wxSize(-1, 30));

    step_size_vertical->Add(new wxStaticText(this, wxID_ANY, "step size (in bytes)"));
    step_size_vertical->Add(_step_size_text, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 5));

    offsets_horizontal->Add(step_size_vertical, wxSizerFlags(1).Expand());

    options_vertical->Add(offsets_horizontal, wxSizerFlags(1).Expand());

    wxBoxSizer* manipulations_horizontal = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* shift_value_vertical = new wxBoxSizer(wxVERTICAL);
    _shift_value_text = new wxTextCtrl(this, wxID_ANY, "1");
    _shift_value_text->SetMaxSize(wxSize(-1, 30));

    shift_value_vertical->Add(new wxStaticText(this, wxID_ANY, "value shift"));
    shift_value_vertical->Add(_shift_value_text, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 5));

    manipulations_horizontal->Add(shift_value_vertical, wxSizerFlags(1).Expand());

    wxBoxSizer* random_chance_vertical = new wxBoxSizer(wxVERTICAL);
    _random_chance_text = new wxTextCtrl(this, wxID_ANY, "0.5");
    _random_chance_text->SetMaxSize(wxSize(-1, 30));
    _random_chance_text->Enable(false);

    random_chance_vertical->Add(new wxStaticText(this, wxID_ANY, "randomize chance"));
    random_chance_vertical->Add(_random_chance_text, wxSizerFlags(1).Expand().Border(wxLEFT | wxRIGHT, 5));

    manipulations_horizontal->Add(random_chance_vertical, wxSizerFlags(1).Expand());

    wxCheckBox* check_box = new wxCheckBox(this, wxID_ANY, "randomize");
    check_box->Bind(wxEVT_CHECKBOX, &main_frame::random_checkbox, this);
    manipulations_horizontal->Add(check_box, wxSizerFlags(0).Border(wxLEFT | wxRIGHT, 5));

    options_vertical->Add(manipulations_horizontal, wxSizerFlags(1).Expand());

    main_horizontal->Add(options_vertical, wxSizerFlags(0).Expand());

    SetSizerAndFit(main_horizontal);
}

void main_frame::random_checkbox(wxCommandEvent& event)
{
    _random_mode = event.IsChecked();

    _random_chance_text->Enable(_random_mode);
}

void main_frame::filepath_choice(wxFileCtrlEvent& event)
{
    const std::string dir = std::string(event.GetDirectory().utf8_str().data());
    const std::string file = std::string(event.GetFile().utf8_str().data());

    _corrupt_filepath = std::filesystem::path(dir)
        /std::filesystem::path(file);
}

void main_frame::corrupt_button(wxCommandEvent& event)
{
    const uint64_t start_pos = convert_number<uint64_t>
        (_start_pos_text->GetLineText(0).ToStdString(), -1, [](std::string s){return std::stoll(s);});

    const uint64_t step_size = convert_number<uint64_t>
        (_step_size_text->GetLineText(0).ToStdString(), -1, [](std::string s){return std::stoll(s);});

    const int shift_value = convert_number<int>
        (_shift_value_text->GetLineText(0).ToStdString(), INT_MAX, [](std::string s){return std::stoi(s);});

    const float random_chance = !_random_mode?0:convert_number<float>
        (_random_chance_text->GetLineText(0).ToStdString(), -1, [](std::string s){return std::stof(s);});

    if(start_pos==-1 || step_size==-1 || shift_value==INT_MAX || random_chance==-1)
        return;

    if(step_size<1)
        display_error("byte step size must be above 0");

    if(random_chance<0 || random_chance>1)
        display_error("random replace chance must be between 0.0 and 1.0");

    if(_corrupt_filepath=="")
    {
        display_error("select a file plz");
        return;
    }

    try
    {
        _corruptor.corrupt(_corrupt_filepath, output_prefix+_corrupt_filepath.filename().string(),
            corruptor::options{corruptor::positions{start_pos, step_size},
            corruptor::value_options{shift_value, _random_mode, random_chance}});
    } catch(std::runtime_error& e)
    {
        display_error(e.what());
        return;
    }
}

void main_frame::display_error(const std::string message) noexcept
{
    wxMessageBox(message, "error", wxOK, this);
}

template<typename T>
T main_frame::convert_number(const std::string number, const T invalid_value, std::function<T(std::string)> func) noexcept
{
    try
    {
        return func(number);
    } catch(std::invalid_argument& e)
    {
        display_error("invalid value: " + number);
        return invalid_value;
    }
}
