#pragma once
#include "../visualization/visualization_function.h"


class domain_setting
{
public:
    domain_setting(nana::form& main_pg)
    {
        nana::inputbox inbox(main_pg, "<bold>Domain Setting (Data Range 1000 ~ 3000)");
        nana::inputbox::integer w_input("W : ", 0, 0, 3000, 1);
        nana::inputbox::integer h_input("H : ", 0, 0, 3000, 1);

        inbox.verify([&w_input, &h_input](nana::window handle)
            {
                if ((w_input.value()) && (h_input.value()))
                {
                    std::cout << "Domain Set Complete. / Width : " << (w_input.value()) << " / Height : " << (h_input.value()) << std::endl;
                    return true; //verified successfully
                }
                nana::msgbox mb(handle, "Invalid input");
                mb << "Data should not be empty, Please input Domain location and size data.";
                mb.show();
                return false; //verification failed
            });

        if (inbox.show(w_input, h_input))
        {
            std::ofstream domain_output;
            domain_output.open("./data/domain_set/domain.txt");
            domain_output << w_input.value() << " " << h_input.value();
            domain_output.close();
        }
    }
};

class add_circle
{
public:
    add_circle(nana::form& main_pg)
    {
        nana::inputbox inbox(main_pg, "<bold>Domain Setting (X, Y Range : 0 ~ 3000, Radius Range : 0 ~ 1000)");
        nana::inputbox::integer x_input("X : ", 0, 0, 3000, 1);
        nana::inputbox::integer y_input("Y : ", 0, 0, 3000, 1);
        nana::inputbox::integer r_input("R : ", 0, 0, 1000, 1);

        inbox.verify([&x_input, &y_input, &r_input](nana::window handle)
            {
                if ((r_input.value()))
                {
                    std::cout << "Circle Set Complete. / Center location of X : " << x_input.value() << " / Center location of Y : "\
                        << y_input.value() << " / Radius : " << (r_input.value()) << std::endl;
                    return true; //verified successfully
                }
                nana::msgbox mb(handle, "Invalid input");
                mb << "Data should not be empty, Please input Domain location and size data.";
                mb.show();
                return false; //verification failed
            });

        if (inbox.show(x_input, y_input, r_input))
        {
            std::ofstream circle_output;
            circle_output.open("./data/obstacle_set/circle.txt", std::ios::app);
            circle_output << x_input.value() << " " << y_input.value() << " " << r_input.value() << std::endl;
            circle_output.close();
        }
    }
};

class add_rectangle
{
public:
    add_rectangle(nana::form& main_pg)
    {
        nana::inputbox inbox(main_pg, "<bold>Domain Setting (X, Y Range : 0 ~ 3000, W, H Range : 0 ~ 2000)");
        nana::inputbox::integer x_input("X : ", 0, 0, 3000, 1);
        nana::inputbox::integer y_input("Y : ", 0, 0, 3000, 1);
        nana::inputbox::integer w_input("W : ", 0, 0, 2000, 1);
        nana::inputbox::integer h_input("H : ", 0, 0, 2000, 1);

        inbox.verify([&x_input, &y_input, &w_input, &h_input](nana::window handle)
            {
                if ((w_input.value()) && (h_input.value()))
                {
                    std::cout << "Rectangle Set Complete. / Center location of X : " << x_input.value() << " / Center location of Y : "\
                        << y_input.value() << " / Width : " << (w_input.value()) << " / Height : " << (h_input.value()) << std::endl;
                    return true; //verified successfully
                }
                nana::msgbox mb(handle, "Invalid input");
                mb << "Data should not be empty, Please input Domain location and size data.";
                mb.show();
                return false; //verification failed
            });

        if (inbox.show(x_input, y_input, w_input, h_input))
        {
            std::ofstream domain_output;
            domain_output.open("./data/obstacle_set/rectangle.txt", std::ios::app);
            domain_output << x_input.value() << " " << y_input.value() << " " << w_input.value() << " " << h_input.value() << std::endl;
            domain_output.close();
        }
    }
};

class obstacle_tool
{
private:
    nana::form main_pg{ nana::API::make_center(800, 600), nana::appear::decorate<nana::appear::taskbar>() };    //Define a main program form.

public:
    obstacle_tool()
    {

        main_pg.caption(("2D Potential Flow Analysis Program"));
        main_pg.div("vert <height=32>\
            <height=56<width=40><width=720 header><width=40>>\
            <height=80>\
            <height=180<width=60><width=320 circle_set><width=40><width=320 rectangle_set><width=60>>\
            <height=80>\
            <height=140<width=140><width=140 check><width=50><width=140 reset><width=50><width=140 quit><width=140>>\
            <height=28>");


        //Define a title label.
        yh_lb header{ main_pg, "<bold white size=16>Obstacle Setting</>" };

        //Define a button for setting domain and obstatcle with label.
        yh_lb circle_btn{ main_pg, "<bold white size=16>Add Circle</>" };
        circle_btn.events().click([&] {add_circle{ main_pg }; });

        yh_lb rectangle_btn{ main_pg, "<bold white size=16>Add Rectangle</>" };
        rectangle_btn.events().click([&] {add_rectangle{ main_pg }; });




        //Define a button for Analysis with label
        yh_lb check_btn{ main_pg, "<bold white size=16>Check\nObstacle</>" };
        check_btn.events().click([&]
            {
                {
                std::cout << " \n########## Circle Obstacle Data  ##########\n" << std::endl;
                std::ifstream circle_output;
                char circle_inform_char[32];
                circle_output.open("./data/obstacle_set/circle.txt");
                int i = 1;
                while (!circle_output.eof())
                {
                    circle_output.getline(circle_inform_char, 32);
                    std::vector<std::string> circle_data = split(std::string{ circle_inform_char });
                    if (circle_data.size() == 3)
                    {
                        std::cout << "CIRCLE NO. " << i << " / X : " << circle_data[0] << " / Y : " << circle_data[1] << " / R : " << circle_data[2] << std::endl;
                        i++;
                    }
                }
                if (i == 1) { std::cout << "----------None----------" << std::endl; }
                circle_output.close();
                }
                {
                    std::cout << " \n########## Rectangle Obstacle Data  ##########\n" << std::endl;

                    std::ifstream rectangle_output;
                    char rectangle_inform_char[32];
                    rectangle_output.open("./data/obstacle_set/rectangle.txt");
                    int i = 1;
                    while (!rectangle_output.eof())
                    {
                        rectangle_output.getline(rectangle_inform_char, 32);
                        std::vector<std::string> rectangle_data = split(std::string{ rectangle_inform_char });
                        if (rectangle_data.size() == 4)
                        {
                            std::cout << "Rectangle NO. " << i << " / X : " << rectangle_data[0] << " / Y : " << rectangle_data[1] << " / W : " << rectangle_data[2] << " / H : " << rectangle_data[3] << std::endl;
                            i++;
                        }
                    }
                    if (i == 1) { std::cout << "----------None----------" << std::endl; }
                    rectangle_output.close();
                }
                std::cout << "================================================================" << std::endl;
            });

        yh_lb reset_btn{ main_pg, "<bold white size=16>Reset</>" };
        reset_btn.events().click([&]
            {
                reset_obstacle();
                nana::msgbox mb(main_pg, "Invalid input");
                mb << "Obstacles Reset Complete";
                mb.show();
            });

        yh_lb quit_btn{ main_pg, "<bold white size=16>Quit</>" };
        quit_btn.events().click([&] { main_pg.close(); });


        //Layout management
        main_pg["header"] << header;
        main_pg["circle_set"] << circle_btn;
        main_pg["rectangle_set"] << rectangle_btn;
        main_pg["check"] << check_btn;
        main_pg["reset"] << reset_btn;
        main_pg["quit"] << quit_btn;

        main_pg.collocate();


        //Show the form
        main_pg.show();

        //Start to event loop process, it blocks until the form is closed.
        nana::exec();
    }
};


class initial_setting
{
public:
    initial_setting(nana::form& main_pg)
    {
        nana::inputbox inbox(main_pg, "<bold>Initial Input Value Setting");
        nana::inputbox::integer v_input("V : ", 0, 0, 1000, 1);

        inbox.verify([&v_input](nana::window handle)
            {
                if (v_input.value())
                {
                    std::cout << "Initial Set Complete. / V : " << (v_input.value()) << std::endl;
                    return true; //verified successfully
                }
                nana::msgbox mb(handle, "Invalid input");
                mb << "Data should not be empty, Please input Value data.";
                mb.show();
                return false; //verification failed
            });

        if (inbox.show(v_input))
        {
            std::ofstream initial_output;
            initial_output.open("./data/initial_value_set/inlet_v.txt");
            initial_output << v_input.value();
            initial_output.close();
        }
    }
};