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

#include "crc16.h"

/**
 * @brief Calculate CRC16 checksum
 * 
 * @param data 
 * @param nrbytes 
 * @param crc 
 * @return uint16_t CRC-16 checksum
 */
uint16_t crc16(uint8_t *addr, uint16_t nrbytes, uint64_t crc) {

    for (uint16_t j=0; j<nrbytes; j++) {
        crc = crc ^ (*addr++ << 8);
        for (uint8_t i=0; i<8; i++) {
            crc = crc << 1;
            if (crc & 0x10000) {
                crc = (crc ^ POLY) & 0xFFFF;
            }
        }
    }

    return(crc);
}

/**
 * @brief Sign file with a closing checksum that yields a total CRC16 checksum
 * of 0x0000
 *
 * @param infile input file
 * @param outfile output file
 * @param mode signing mode (either substitute final two bytes or append)
 */
void sign_file(const std::string& infile, const std::string& outfile, SIGNMODE mode) {
    std::ifstream input(infile, std::ios::binary);
    if(input.is_open()) {
        std::cout << "Opening: " << infile << std::endl;
        std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(input), {});
        input.close();

        uint16_t startcrc = 0;

        switch(mode) {
            case SIGNMODE::MODE_APPEND:
                startcrc = crc16(buffer.data(), buffer.size(), 0x0000);
            break;
            case SIGNMODE::MODE_SUBSTITUTE:
                startcrc = crc16(buffer.data(), buffer.size() - 2, 0x0000);
            break;
            default:
                throw std::logic_error("Invalid mode");
            break;
        }

        uint16_t crc = find_signing_checksum(startcrc);
        std::cout << "Signing checksum: 0x" << (boost::format("%04X") % crc).str() << std::endl;

        switch(mode) {
            case SIGNMODE::MODE_APPEND:
                buffer.push_back(crc & 0xFF);
                buffer.push_back((crc >> 8) & 0xFF);
            break;
            case SIGNMODE::MODE_SUBSTITUTE:
                buffer[buffer.size() - 2] = crc & 0xFF;
                buffer[buffer.size() - 1] = (crc >> 8) & 0xFF;
            break;
            default:
                throw std::logic_error("Invalid mode");
            break;
        }

        // write to output file
        std::ofstream output(outfile, std::ios::binary);
        if(output.is_open()) {
            std::cout << "Signed file written to: " << outfile << std::endl;
            output.write((char*)buffer.data(), buffer.size());
            output.close();
        }

        // re-open file and verify its checksum
        std::ifstream check(outfile, std::ios::binary);
        if(check.is_open()) {
            std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(check), {});
            uint16_t crc_check = crc16(buffer.data(), buffer.size());

            if(crc_check == 0x0000) {
                std::cout << "Verifying checksum: " << "0x" << (boost::format("%04X") % crc_check).str() << std::endl;
                std::cout << "All done!" << std::endl;
            } else {
                throw std::runtime_error("Cannot verify final checksum");
            }
        } else {
            throw std::runtime_error("Cannot reopen file for final verification");
        }

    } else {
        throw std::runtime_error("Cannot open file " + infile);
    }
}

/**
 * @brief Find a checksum that yields a total CRC16 checksum that amounts to 0x0000
 * 
 * @param startcrc 
 * @return uint16_t signing checksum
 */
uint16_t find_signing_checksum(uint16_t startcrc) {
    std::vector<uint16_t> results;

    #pragma omp parallel for
    for(int i=0; i<=0xFFFF; i++) {
        uint8_t data[2];
        data[0] = i & 0xFF;
        data[1] = (i >> 8) & 0xFF;
        uint16_t crc = crc16(data, 2, startcrc);

        if(crc == 0x000) {
            #pragma omp critical
            {
                results.push_back(i);
            }
        }
    }

    if(results.size() > 0) {
        return results[0];
    } else {
        throw std::runtime_error("Did not find any suitable signing checksum");
    }
}