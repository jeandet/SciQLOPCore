// TODO copyright
/**
 * @file Version.h
 */
#ifndef SCIQLOP_VERSION_H
#define SCIQLOP_VERSION_H

/***************************************************
 * DON'T CHANGE THIS FILE. AUTOGENERATED BY CMAKE. *
 ***************************************************/

//#include "SciqlopExport.h"
#include <QtCore/QString>

class QDateTime;

namespace sciqlop {

/**
 * Holds the version of Sciqlop.
 *
 * @attention Don't update this class directly, it is generated from the
 * `resources/Version.h.in` and `resources/Version.cpp.in` files, along with
 * the cmake variables defined in the `cmake/sciqlop_version.cmake` file.
 *
 * To change the Sciqlop version number, update the `cmake/sciqlop_version.cmake`
 * file, and to change this class other than to change the version number,
 * update the `resources/Version.h.in` and `resources/Version.cpp.in` files.
 * @ingroup Utils
 */
class /*SCIQLOP_API*/ Version {
public:
    /**
     * Retrieve the version of Sciqlop.
     *
     * The version is of the form MAJOR.MINOR.PATCH. If a suffix has been
     * provided to the version, it is appended after the PATCH.
     *
     * The version can be modified by updating the cmake/sciqlop_version.cmake
     * file.
     *
     * @return The version of Sciqlop as a QString.
     */
    static QString version()
    {
        static const QString v("0.1.0");
        return v;
    }

    /**
     * @return The datetime of the build.
     */
    static QDateTime buildDateTime();

    /**
     * Major version.
     */
    static const int VERSION_MAJOR = 0;
    /**
     * Minor version.
     */
    static const int VERSION_MINOR = 1;
    /**
     * Patch version.
     */
    static const int VERSION_PATCH = 0;
    /**
     * Suffix version.
     */
    static const char *VERSION_SUFFIX;

    /**
     * Compile date computed with the __DATE__ macro.
     *
     * From the C99 standard:
     * __DATE__: The date of translation of the preprocessing translation unit:
     * a character string literal of the form "Mmm dd yyyy", where the names of
     * the months are the same as those generated by the asctime function, and
     * the first character of dd is a space character if the value is less than
     * 10. If the date of translation is not available, an
     * implementation-defined valid date shall be supplied.
     */
    static const char *BUILD_DATE;
    /**
     * Compile time computed with the __TIME__ macro.
     *
     * From the C99 standard:
     * __TIME__: The time of translation of the preprocessing translation unit:
     * a character string literal of the form "hh:mm:ss" as in the time
     * generated by the asctime function. If the time of translation is not
     * available, an implementation-defined valid time shall be supplied.
     */
    static const char *BUILD_TIME;
};

} // namespace sciqlop

#endif // SCIQLOP_VERSION_H
