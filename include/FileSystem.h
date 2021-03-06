/**
* Authored by Peizhao Hu.
* Copyright SpiRITlab - Computations on Encrypted Data
* Publicly available at: https://gitlab.com/SpiRITlab/coed
*/

#ifndef PRIVPOINTLOC_FILESYSTEM_H
#define PRIVPOINTLOC_FILESYSTEM_H

#include <iostream>
#include <fstream>

class FileSystem {
public:
    FileSystem(const std::string &);

    void open_output_stream();
    void open_output_stream(const std::ios_base::openmode &);
    std::ofstream &get_output_stream();
    void close_output_stream();
    void open_input_stream();
    void open_input_stream(const std::ios_base::openmode &);
    std::ifstream &get_input_stream();
    void close_input_stream();

private:
    std::string filesystem_path;
    std::ifstream input_file_stream;
    std::ofstream output_file_stream;
};

#endif //PRIVPOINTLOC_FILESYSTEM_H
