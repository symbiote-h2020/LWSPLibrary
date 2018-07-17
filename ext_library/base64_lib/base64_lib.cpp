/**
 * base64.cpp
 *
 * Created on: 09.12.2015
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the ESP8266 core for Arduino.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

extern "C" {
    //#include "libb64/cencode.h"
    #include "cencode.h"
    #include "cdecode.h"
}
#include "base64_lib.h"

/**
 * convert input data to base64
 * @param data uint8_t *
 * @param length size_t
 * @return std::string
 */
std::string base64::encode(uint8_t * data, size_t length, bool doNewLines) {
    // base64 needs more size then the source data
    size_t size = ((length * 1.6f) + 1);
    char * buffer = (char *) malloc(size);
    if(buffer) {
        base64_encodestate _state;
        if(doNewLines)
        {
            base64_init_encodestate(&_state);
        }
        else 
        {
            base64_init_encodestate_nonewlines(&_state);
        }
        int len = base64_encode_block((const char *) &data[0], length, &buffer[0], &_state);
        len = base64_encode_blockend((buffer + len), &_state);

        std::string base64 = std::string(buffer);
        free(buffer);
        return base64;
    }
    return std::string("-FAIL-");
}

/**
 * convert input data to base64
 * @param text std::string
 * @return std::string
 */
std::string base64::encode(std::string text, bool doNewLines) {
    return base64::encode((uint8_t *) text.c_str(), text.length(), doNewLines);
}

