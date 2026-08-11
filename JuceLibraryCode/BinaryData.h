/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   label_png;
    const int            label_pngSize = 87481;

    extern const char*   doomsday_2jpeg;
    const int            doomsday_2jpegSize = 2452276;

    extern const char*   doomsday_jpeg;
    const int            doomsday_jpegSize = 2097271;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 3;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
