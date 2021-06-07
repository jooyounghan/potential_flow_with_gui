#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>

#include "../finite_element/finite_element.h"

class yh_lb // for reduce coding lines, got 
{
private:
    nana::label* btn;

public:
    yh_lb(const nana::form& pg, const std::string& title)
    {
        btn = new nana::label{ pg, title };
        btn->bgcolor(nana::colors::sky_blue);
        btn->events().mouse_enter([&] {btn->fgcolor(nana::colors::black); });
        btn->text_align(nana::align::center, nana::align_v::center);
        btn->format(true);
    }

    friend nana::place::field_reference operator << (nana::place::field_reference& out, yh_lb& yh_btn_in)
    {
        out << *(yh_btn_in.btn);
        return out;
    }

    nana::label& get() { return *btn; }

    nana::general_events& events() { return btn->events(); }

    nana::label& format(const bool& b) { return btn->format(b); }

    ~yh_lb() { if (btn != nullptr) { delete btn; } }
};

void reset_obstacle()
{
    std::ofstream circle_output;
    circle_output.open("./data/obstacle_set/circle.txt");
    circle_output.close();

    std::ofstream rectangle_output;
    rectangle_output.open("./data/obstacle_set/rectangle.txt");
    rectangle_output.close();
}

void reset_data()
{
    std::ofstream domain_data;
    domain_data.open("./data/domain_set/domain.txt");
    domain_data.close();

    reset_obstacle();
}