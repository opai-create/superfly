#pragma once

#include <Windows.h>

#include "modules.h"

namespace mem_utils
{

    inline void FillShiftTable(const uint8_t* pattern, size_t pattern_size, uint8_t wildcard, size_t* bad_char_skip)
    {
        size_t idx = 0;
        size_t last = pattern_size - 1;

        // Get last wildcard position
        for (idx = last; idx > 0 && pattern[idx] != wildcard; --idx);
        size_t diff = last - idx;
        if (diff == 0)
            diff = 1;

        // Prepare shift table
        for (idx = 0; idx <= UCHAR_MAX; ++idx)
            bad_char_skip[idx] = diff;
        for (idx = last - diff; idx < last; ++idx)
            bad_char_skip[pattern[idx]] = last - idx;
    }

    static Address find_pattern(Address base_addr, size_t scan_size, const std::string& pattern) {

        size_t bad_char_skip[UCHAR_MAX + 1];
        size_t pattern_size = strlen(pattern.c_str());
        intptr_t last = static_cast<intptr_t>(pattern_size) - 1;

        const uint8_t* scan_start = base_addr.as< const uint8_t* >();
        const uint8_t* scan_end = scan_start + scan_size;
        const uint8_t* pattern_converted = reinterpret_cast<const uint8_t*>(pattern.c_str());
        //TODO: condition operator if true wilcard = 0xCC else 0x00
        //This will prevent skipping CC opcode if you want to find something near or even get this opcode
        uint8_t wildcard = 0xCC;


        FillShiftTable(pattern_converted, pattern_size, wildcard, bad_char_skip);

        // Search
        for (; scan_start <= scan_end; scan_start += bad_char_skip[scan_start[last]])
        {
            for (intptr_t idx = last; idx >= 0; --idx)
                if (pattern_converted[idx] != wildcard && scan_start[idx] != pattern_converted[idx])
                    goto skip;
                else if (idx == 0)
                    return scan_start;
        skip:;
        }

        return nullptr;
    }

	static Address find_pattern(const PE::Module& module, const std::string& pat) {
		return find_pattern(module.GetBase(), module.GetImageSize(), pat);
	}
}