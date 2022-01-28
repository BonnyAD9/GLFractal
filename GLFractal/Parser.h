#pragma once

namespace Parser
{
    bool tryParse(char* str, double* num);
    bool tryParse(char* str, int* num);
    bool tryParse(char* str, float* num);

    bool tryParseHex(char* str, unsigned int* num);
}
