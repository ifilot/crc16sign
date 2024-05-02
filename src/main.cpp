/**************************************************************************
 *                                                                        *
 *   Author: Ivo Filot <i.a.w.filot@tue.nl>                               *
 *                                                                        *
 *   CRC16SIGN is free software:                                          *
 *   you can redistribute it and/or modify it under the terms of the      *
 *   GNU General Public License as published by the Free Software         *
 *   Foundation, either version 3 of the License, or (at your option)     *
 *   any later version.                                                   *
 *                                                                        *
 *   CRC16SIGN is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#include <iostream>
#include <tclap/CmdLine.h>

#include "crc16.h"
#include "config.h"

int main(int argc, char* argv[]) {

    try {
        TCLAP::CmdLine cmd("Signs a file by appending two bytes that will yield 0x0000 for the CRC16 checksum.", ' ', PROGRAM_VERSION);

        // input filename
        TCLAP::ValueArg<std::string> arg_input_filename("i", "input", "Input file", false, "FILE.BIN", "filename");
        cmd.add(arg_input_filename);

        // whether to substitute the final two bytes
        TCLAP::SwitchArg arg_s("s","substitute", "Substitute final two bytes", cmd, false);

        // whether to substitute the final two bytes
        TCLAP::SwitchArg arg_a("a","append", "Append final two bytes", cmd, false);

        // output filename
        TCLAP::ValueArg<std::string> arg_output_filename("o", "filename", "Filename to output to", true, "FILE.BIN", "string");
        cmd.add(arg_output_filename);

        cmd.parse(argc, argv);

        //**************************************
        // Inform user about execution
        //**************************************
        std::cout << "--------------------------------------------------------------" << std::endl;
        std::cout << "Executing "<< PROGRAM_NAME << " v." << PROGRAM_VERSION << std::endl;
        std::cout << "Author:  Ivo Filot <ivo@ivofilot.nl>" << std::endl;
        std::cout << "Github:  https://github.com/ifilot/crc16sign" << std::endl;
        std::cout << "--------------------------------------------------------------" << std::endl;
        std::cout << "Compilation time: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "Git Hash: " << PROGRAM_GIT_HASH << std::endl;
        std::cout << "--------------------------------------------------------------" << std::endl;

        if(arg_s.getValue() && arg_a.getValue() || !(arg_s.getValue() || arg_a.getValue())) {
            throw std::runtime_error("You have to select either append or substitute mode");
        }

        if(arg_s.getValue()) {
            sign_file(arg_input_filename.getValue(), arg_output_filename.getValue(), SIGNMODE::MODE_SUBSTITUTE);
        } else if(arg_a.getValue()) {
            sign_file(arg_input_filename.getValue(), arg_output_filename.getValue(), SIGNMODE::MODE_APPEND);
        }
        

    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() <<
                     " for arg " << e.argId() << std::endl;
        return -1;
    }

    return 0;
}