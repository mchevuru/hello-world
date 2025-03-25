#include "ppmio.h"
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

// void returnRGBValues(const std::vector<std::vector<RGB>>& input);
// void colorValues(const std::vector<int>& colors, const std::vector<std::vector<RGB>>& input);
void grayscale(std::vector<std::vector<RGB>>& input);
void inversion(std::vector<std::vector<RGB>>& input);
void contrast(std::vector<std::vector<RGB>>& input);
int within_bounds(int& color);
void blur(std::vector<std::vector<RGB>>& input);
void mirror(std::vector<std::vector<RGB>>& input);
void compress(std::vector<std::vector<RGB>>& input);

int main(int argc, char* argv[]){
    if (argc == 1){
        throw std::invalid_argument("There is no input file linked. Please attach a .ppm file.");
    }

    std::vector<std::string> files{};
    std::vector<std::string> commands{};
    for (int i{1}; i < argc; ++i){
        std::string current_string = argv[i];
        if (current_string.substr(0, 1) == "-"){
            commands.push_back(current_string);
        } else {
            files.push_back(current_string);
        }
    }
    
    if (files.size() > 2){
        throw std::invalid_argument("There are more than 2 files. Please input atmost two files, one for the input and one for the output.");
    }

    int incorrect_commands{0};
    std::vector<std::string> invalid_commands;
    for (std::string& c : commands){
        if (!(c == "-g" || c == "-i" || c == "-x" || c == "-b" || c == "-m" || c == "-c")){
            incorrect_commands++;
            invalid_commands.push_back(c);
        }
    }

    if (incorrect_commands > 0){
        std::string error_message{""};
        if (incorrect_commands == 1){
            error_message = invalid_commands[0] + " is an invalid command. Please input valid commands only.";
        } else if (incorrect_commands == 2){
            error_message = invalid_commands[0] + " and " + invalid_commands[1] + " are invalid commands. Please input valid commands only.";
        } else {
            for (int i{0}; i < incorrect_commands - 1; ++i){
                error_message += (invalid_commands[i] + ", ");
            }
            error_message += ("and " + invalid_commands[incorrect_commands-1] + " are invalid commands. Please input valid commands only.");
        }
        throw std::invalid_argument(error_message);
    }

    std::string input_file{files.at(0)};
    std::string output_file{};
    if (files.size() > 1){
        output_file = files.at(1);
    } else {
        output_file = "output.ppm";
    }
    
    if (output_file.find(".ppm") < 0){
        throw std::invalid_argument("Output file name " + output_file + " is incorrect. Please type in a .ppm file for output.");
    }
    std::vector<std::vector<RGB>> user_input;

    try{
        user_input = readPPM(input_file);
    } catch (const std::runtime_error& error){
        std::cerr << "Error with input file! " << error.what() << std::endl;
        return 1;
    }

    if (user_input.empty()){
        throw std::invalid_argument("Input file is empty. Please input a valid ppm file for input.");
    }
    // returnRGBValues(user_input);
    // std::cout << std::endl;

    for (std::string& c : commands){
        if (c == "-g"){
            grayscale(user_input);
        } else if (c == "-i"){
            inversion(user_input);
        } else if (c == "-x"){
            contrast(user_input);
        } else if (c == "-b"){
            blur(user_input);
        } else if (c == "-m"){
            mirror(user_input);
        } else if (c == "-c"){
            compress(user_input);
        }
    }

    // returnRGBValues(user_input);

    try{
        writePPM(output_file, user_input);
    } catch (const std::runtime_error& error){
        std::cerr << "Error with output file! " << error.what() << std::endl;
        return 1;
    }
    return 0;
}

// void returnRGBValues(const std::vector<std::vector<RGB>>& input){
//     std::vector<int> red;
//     std::vector<int> green;
//     std::vector<int> blue;
//     for (const std::vector<RGB>& vec : input){
//         for (const RGB& rgb: vec){
//             red.push_back(rgb.r);
//             green.push_back(rgb.g);
//             blue.push_back(rgb.b);
//         }
//     }
//     std::cout << "Red" << std::endl;
//     colorValues(red, input);
//     std::cout << "\nGreen" << std::endl;
//     colorValues(green, input);
//     std::cout << "\nBlue" << std::endl;
//     colorValues(blue, input);
// }

// void colorValues(const std::vector<int>& colors, const std::vector<std::vector<RGB>>& input){
//     for (size_t i{0}; i < input.size(); ++i){
//         for (size_t j{0}; j < input[0].size(); ++j){
//             std::cout << colors.at((i*(input.size()))+j) << " ";
//         }
//         std::cout << std::endl;
//     }
// }

void grayscale(std::vector<std::vector<RGB>>& input){
    for (std::vector<RGB>& vec : input){
        for (RGB& rgb: vec){
            int sum = rgb.r + rgb.g + rgb.b;
            int gray_value = sum/3;
            rgb.r = gray_value;
            rgb.g = gray_value;
            rgb.b = gray_value;
        }
    }
}

void inversion(std::vector<std::vector<RGB>>& input){
    for (std::vector<RGB>& vec : input){
        for (RGB& rgb: vec){
            rgb.r = 255 - rgb.r;
            rgb.g = 255 - rgb.g;
            rgb.b = 255 - rgb.b;
        }
    }
}

void contrast(std::vector<std::vector<RGB>>& input){
    for (std::vector<RGB>& vec : input){
        for (RGB& rgb: vec){
            int new_red = (rgb.r - 128) * 1.2 + 128;
            int new_green = (rgb.g - 128) * 1.2 + 128;
            int new_blue = (rgb.b - 128) * 1.2 + 128;
            rgb.r = within_bounds(new_red);
            rgb.g = within_bounds(new_green);
            rgb.b = within_bounds(new_blue);
        }
    }
}

int within_bounds(int& color){
    if (color > 255){
        color = 255;
    } else if (color < 0){
        color = 0;
    }
    return color;
}

void blur(std::vector<std::vector<RGB>>& input){
    std::vector<std::vector<RGB>> output = input;
    for (size_t i{1}; i < input.size()-1; ++i){
        for(size_t j{1}; j < input[0].size() - 1; ++j){
            int red_sum = input[i][j].r + input[i-1][j].r + input[i-1][j-1].r + input[i][j-1].r + input[i+1][j-1].r + input[i+1][j].r 
            + input[i+1][j+1].r + input[i][j+1].r + input[i-1][j+1].r;

            int green_sum = input[i][j].g + input[i-1][j].g + input[i-1][j-1].g + input[i][j-1].g + input[i+1][j-1].g + input[i+1][j].g 
            + input[i+1][j+1].g + input[i][j+1].g + input[i-1][j+1].g;

            int blue_sum = input[i][j].b + input[i-1][j].b + input[i-1][j-1].b + input[i][j-1].b + input[i+1][j-1].b + input[i+1][j].b 
            + input[i+1][j+1].b + input[i][j+1].b + input[i-1][j+1].b;
            
            output[i][j].r = red_sum/9;
            output[i][j].g = green_sum/9;
            output[i][j].b = blue_sum/9;
            }
    }
    input = output;
}

void mirror(std::vector<std::vector<RGB>>& input){
    std::vector<std::vector<RGB>> output = input;
    int row_size = static_cast<int>(input.size());
    int col_size = static_cast<int>(input[0].size());
    for (int i{0}; i < row_size; ++i){
        for (int j{0}; j < col_size; ++j){
            output[i][j] = input[i][col_size-1-j];
        }
    }
    input = output;
}

void compress(std::vector<std::vector<RGB>>& input){
    std::vector<std::vector<RGB>> output(input.size()/2, std::vector<RGB>(input[0].size()/2));
    for (size_t i{1}; i < input.size(); ++i){
        for (size_t j{0}; j < input[0].size(); ++j){
            if ((i+1)%2 == 0){
                if ((j+1)%2 == 0){
                    output[i/2][j/2] = input[i][j];
                }
            }
        }
    }
    input = output;
}