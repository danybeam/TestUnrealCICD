#pragma once

// Please note Macros are not documented by doxygen by default, and we don't intend to document them for now

#define DEBUGLOG(message) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT(message)), false);
#define DEBUGLOG_FMT(fmt, ...) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT(fmt), __VA_ARGS__), false);