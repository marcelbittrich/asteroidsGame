// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// The correct tolerance would be different for each
// opperation since we calculate with the errror of 
// EPSILON in each number and step so the tolerance
// depends on the actual function.
// Since it is not critical to have the correct 
// tolerance a value which is high enough but still 
// catches big errors is chosen.
const float FLT_TOLERANCE = 0.0001f;

// add headers that you want to pre-compile here

#endif //PCH_H
