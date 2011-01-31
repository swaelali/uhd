//
// Copyright 2011 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <uhd/utils/safe_main.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <iostream>

#include "fpga_downloader.cpp"
#include "clkgen_config.hpp"

namespace po = boost::program_options;

static const std::string default_passthrough_path = "/usr/share/uhd/images/usrp_e1xx_passthrough.bin";

int UHD_SAFE_MAIN(int argc, char *argv[]){

    //variables to be set by po
    std::string fpga_path;

    //setup the program options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
        ("fpga", po::value<std::string>(&fpga_path)->default_value(""), "loads the specified FPGA file")
        ("reclk",                                                       "runs the clock recovery")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    //print the help message
    if (vm.count("help")){
        std::cout << boost::format("UHD USRP-E Utility %s") % desc << std::endl;
        return ~0;
    }

    bool loaded_fpga_image = false;
    if (vm.count("fpga")){
        std::cout << "USRP-E Utility loading the FPGA..." << std::endl << std::endl;
        usrp_e100_load_fpga(fpga_path);
        loaded_fpga_image = true;
        sleep(1);
    }

    if (vm.count("reclk")){
        std::cout << "USRP-E Utility running the clock recovery..." << std::endl << std::endl;
        //if an image was not loaded or specified, we load pass-through
        if (fpga_path.empty()) fpga_path = default_passthrough_path;
        if (not loaded_fpga_image) usrp_e100_load_fpga(fpga_path);
        clock_genconfig_main();
        std::system("rm /tmp/usrp*hash"); //clear hash so driver must reload
    }

    std::cout << "Done!" << std::endl;

    return 0;
}
