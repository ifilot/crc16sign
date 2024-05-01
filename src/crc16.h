#ifndef _CRC16_H
#define _CRC16_H

#include <stdint.h>
#include <fstream>
#include <iterator>
#include <vector>
#include <stdio.h>
#include <cstring>
#include <omp.h>
#include <iostream>
#include <boost/format.hpp>

#define POLY 0x1021

enum class SIGNMODE {
    MODE_APPEND,
    MODE_SUBSTITUTE
};

/**
 * @brief Calculate CRC16 checksum
 * 
 * @param data 
 * @param nrbytes 
 * @param crc 
 * @return uint16_t CRC-16 checksum
 */
uint16_t crc16(uint8_t *data, uint16_t nrbytes, uint64_t crc = 0);

/**
 * @brief Sign file with a closing checksum that yields a total CRC16 checksum
 * of 0x0000
 *
 * @param infile input file
 * @param outfile output file
 * @param mode signing mode (either substitute final two bytes or append)
 */
void sign_file(const std::string& infile, const std::string& outfile, SIGNMODE mode);

/**
 * @brief Find a checksum that yields a total CRC16 checksum that amounts to 0x0000
 * 
 * @param startcrc 
 * @return uint16_t signing checksum
 */
uint16_t find_signing_checksum(uint16_t startcrc);

#endif