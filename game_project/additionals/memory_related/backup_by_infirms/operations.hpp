#pragma once

#include <Windows.h>

#define FIELD_CONCAT( x, y ) x ## y
#define FIELD_CONCATIATE( x, y ) FIELD_CONCAT( x, y )
#define FIELD_PAD( size ) private: std::uint8_t FIELD_CONCATIATE( __pad, __COUNTER__ )[ size ] = { }; public: