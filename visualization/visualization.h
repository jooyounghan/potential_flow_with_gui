#pragma once
#include "../visualization/visualization_sub_program.h"
#include <filesystem>
class analyze_tool : public domain
{
private :
    nana::form main_pg{ nana::API::make_center(800, 600), nana::appear::decorate<nana::appear::taskbar>() };    //Define a main program form.

public : 

    analyze_tool() : domain()
    {
        reset_data();

        main_pg.caption(("2D Potential Flow Analysis Program"));

        main_pg.div("vert <height=32>\
            <height=56<width=40><width=720 header><width=40>>\
            <height=80>\
            <height=180<width=60><width=320 domain_set><width=40><width=320 obstacle_set><width=60>>\
            <height=80>\
            <height=140<width=50><width=160 initial><width=20><width=160 generate><width=20><width=160 result><width=20><width=160 quit><width=50>>\
            <height=28>");


        //Define a title label.
        yh_lb header{ main_pg, "<bold white size=16>2D Potential Flow Analysis Program(JYH)</>" };
        header.format(true);

        //Define a button for Analysis with label
        yh_lb quit_btn{ main_pg, "<bold white size=16>Quit</>" };
        quit_btn.events().click([&] { main_pg.close(); });

        yh_lb result_btn{ main_pg, "<bold white size=16>View Result</>" };
        result_btn.events().click([&]
            {
                std::cout << "Showing..." << std::endl;
                system(".\\dist\\Visualization\\Visualization.exe");
                std::cout << "Showing Complete" << std::endl;
            });


        yh_lb generate_btn{ main_pg, "<bold white size=16>Generate Analysis</>" };
        generate_btn.events().click([&]
        {
            std::cout << "Generating..." << std::endl;
            {
                std::ifstream domain_output;
                char domain_inform_char[32];
                domain_output.open("./data/domain_set/domain.txt");
                int i = 1;
                domain_output.getline(domain_inform_char, 32);
                std::vector<std::string> domain_data = split(std::string{ domain_inform_char });
                if (domain_data.size() == 2)
                {
                    this->setting(std::stoi(domain_data[0]), std::stoi(domain_data[1]));
                    std::cout << "Domain Setting Complete..." << std::endl;
                }
                else
                {
                    nana::msgbox mb(main_pg, "No Domain Setting");
                    mb << "Put Domain Setting Data Before Analyzing Flow ";
                    mb.show();
                    return;
                }
            }

            bool circle_exist = true;
            bool rectangle_exist = true;

            /*
                add the circle and rectangle with pointer
                if use reference_wrapper<obstacle_shape>, because circle and rectangle is local varialbes, so it is removed immediately
                so we can't use "is_inner" function of the obstacle whether it is circle or rectangle,
                to prevent it, create local obstacle value with pointer and copy the pointer and put it into the vector,
                and then use move semantic for saving data when scope is out of the local range.
            */

            {
                std::ifstream circle_output;
                char circle_inform_char[32];
                circle_output.open("./data/obstacle_set/circle.txt");
                int i = 1;
                while (!circle_output.eof())
                {

                    circle_output.getline(circle_inform_char, 32);
                    std::vector<std::string> circle_data = split(std::string { circle_inform_char });
                    if (circle_data.size() == 3)
                    {
                        circle* circle_temp = new circle{ std::stoi(circle_data[0]), std::stoi(circle_data[1]), std::stoi(circle_data[2]) };
                        this->insert(circle_temp);
                        circle_temp = nullptr;
                        i++;
                    }
                    circle_data.clear();
                }
                if (i == 1) { std::cout << "No Circle Obstacle" << std::endl; circle_exist = false; }
                circle_output.close();
            }

            {
                std::ifstream rectangle_output;
                char rectangle_inform_char[32];
                rectangle_output.open("./data/obstacle_set/rectangle.txt");
                int i = 1;
                while (!rectangle_output.eof())
                {
                    rectangle_output.getline(rectangle_inform_char, 32);
                    std::string rectangle_inform_str{ rectangle_inform_char };
                    std::vector<std::string> rectangle_data = split(rectangle_inform_str);
                    if (rectangle_data.size() == 4)
                    {
                        rectangle* rectangle_temp = new rectangle{ std::stod(rectangle_data[0]), std::stod(rectangle_data[1]), std::stoi(rectangle_data[2]), std::stoi(rectangle_data[3]) };
                        this->insert(rectangle_temp);
                        rectangle_temp = nullptr;
                        i++;
                    }
                    rectangle_data.clear();
                }
                if (i == 1) { std::cout << "No Rectangle Obstacle" << std::endl; rectangle_exist = false; }
                rectangle_output.close();
            }

            if (circle_exist || rectangle_exist) { /* delaunay triangulation */ }
            else
            {
                nana::msgbox mb(main_pg, "No Obstacle Input");
                mb << "No Obstacle Input. Add Obstacale For Analyzing Flow.";
                mb.show();
            }

            this->generate();

            std::cout << "Geometry Data Exporting..." << std::endl;
            std::ofstream vertex_output;
            vertex_output.open("./data/geometry_set/vertex.txt");
            for(auto & vt : this->vt_set)
            {
                vertex_output << vt.x_loc << " " << vt.y_loc << std::endl;
            }
            vertex_output.close();

            std::ofstream tri_output;
            tri_output.open("./data/geometry_set/triangle.txt");
            for (auto& tri : this->tri_set)
            {
                tri_output << tri.vt_1.x_loc << " " << tri.vt_1.y_loc << " " << tri.vt_2.x_loc << " " << tri.vt_2.y_loc << " " << tri.vt_3.x_loc << " " << tri.vt_3.y_loc << std::endl;
            }
            tri_output.close();
            std::cout << "Geometry Data Exporting Compelte" << std::endl;
            std::cout << "================================================================" << std::endl;         

            std::vector<vertex>* vt_set_ptr_in = &(this->vt_set);
            std::vector<triangle>* tri_set_ptr_in = &(this->tri_set);
            std::vector<obstacle_shape*>* obstacle_set_ptr_in = &(this->obstacle_set);
            finite_element fem(this->width, this->height, vt_set_ptr_in, tri_set_ptr_in, obstacle_set_ptr_in);
            fem.solve();
            fem.export_data();
        });


        yh_lb initial_btn{ main_pg, "<bold white size=16>Initial Value</>" };
        initial_btn.events().click([&] { initial_setting{ main_pg }; });

        //Define a button for setting domain and obstatcle with label.
        yh_lb obstacle_btn{ main_pg, "<bold white size=16>Setting Obsatcle</>" };
        obstacle_btn.events().click([&] {obstacle_tool obst_page{}; });
        yh_lb domain_btn{ main_pg, "<bold white size=16>Setting Domain</>" };
        domain_btn.events().click([&] {domain_setting{ main_pg }; });

        //Layout management
        main_pg["header"] << header;
        main_pg["domain_set"] << domain_btn;
        main_pg["obstacle_set"] << obstacle_btn;
        main_pg["initial"] << initial_btn;
        main_pg["generate"] << generate_btn;
        main_pg["result"] << result_btn;
        main_pg["quit"] << quit_btn;

        main_pg.collocate();


        //Show the form
        main_pg.show();

        //Start to event loop process, it blocks until the form is closed.
        nana::exec();
    }
};