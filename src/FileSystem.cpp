
#include "FileSystem.h"

privPointLoc::FileSystem::FileSystem(const std::string &fileSystemPath)
    : filesystem_path(fileSystemPath)
    { }

void
privPointLoc::FileSystem::open_output_stream() {
    if(!output_file_stream.is_open())
        output_file_stream.open(filesystem_path);
}

void
privPointLoc::FileSystem::open_output_stream(const std::ios_base::openmode &mode) {
    if(!output_file_stream.is_open())
        output_file_stream.open(filesystem_path, mode);
}

std::ofstream &
privPointLoc::FileSystem::get_output_stream() {
    return this->output_file_stream;
}

void
privPointLoc::FileSystem::close_output_stream() {
    if(output_file_stream.is_open()) {
        output_file_stream.flush();
        output_file_stream.close();
    }
}

void
privPointLoc::FileSystem::open_input_stream() {
    if(!input_file_stream.is_open())
        input_file_stream.open(filesystem_path);
}

void
privPointLoc::FileSystem::open_input_stream(const std::ios_base::openmode &mode) {
    if(!input_file_stream.is_open())
        input_file_stream.open(filesystem_path, mode);
}

std::ifstream &
privPointLoc::FileSystem::get_input_stream() {
    return this->input_file_stream;
}

void
privPointLoc::FileSystem::close_input_stream() {
    if(input_file_stream.is_open())
        input_file_stream.close();
}