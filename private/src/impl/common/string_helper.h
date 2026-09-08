#pragma once

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <string>

namespace comm
{
  namespace axisprofile
  {

#if defined(_MSC_VER)
#define STATE_STRING_COPY_COUNT _TRUNCATE
    //! @brief save string cat (will not overwrite the destination)
    //! @param[in]   dest 	pointer to the null-terminated byte string to append to
    //! @param[in]   src    pointer to the character array to copy from
    //! @param[in]   copyCount  maximum number of characters to copy
    //! @param[in]   destSize the size of the destination buffer
    //! @return returns zero on success, returns non-zero on error. Also, on error, writes zero to dest[0] (unless dest is a null pointer or destsz is zero or greater than RSIZE_MAX).
    inline std::int32_t stringCatS(char* dest, std::int32_t destSize, const char* src, rsize_t copyCount)
    {
      return strncat_s(dest, destSize, src, copyCount);
    }

    //! @brief save string copy (will not overwrite the destination)
    //! @param[in]   dest 	pointer to the character array to copy to
    //! @param[in]   src    pointer to the character array to copy from
    //! @param[in]   copyCount  maximum number of characters to copy
    //! @param[in]   destSize the size of the destination buffer 
    //! @return returns zero on success, returns non-zero on error. Also, on error, writes zero to dest[0] (unless dest is a null pointer or destsz is zero or greater than RSIZE_MAX) and may clobber the rest of the destination array with unspecified values.
    inline std::int32_t stringCpyS(char* dest, std::int32_t destSize, const char* src, rsize_t copyCount)
    {
      return strncpy_s(dest, destSize, src, copyCount);
    }
#else
#define STATE_STRING_COPY_COUNT 0
    //! @brief save string cat (will not overwrite the destination)
    //! @param[in]   dest 	pointer to the null-terminated byte string to append to
    //! @param[in]   destSize the size of the destination buffer
    //! @param[in]   src    pointer to the character array to copy from
    //! @param[in]   copyCount  maximum number of characters to copy
    //! @return returns zero on success, returns non-zero on error. Also, on error, writes zero to dest[0] (unless dest is a null pointer or destsz is zero or greater than RSIZE_MAX).
    inline std::int32_t stringCatS(char* dest, std::int32_t destSize, const char* src, std::int32_t copyCount)
    {
      auto maxCount = destSize - static_cast<std::int32_t>(strlen(dest)) - 1;
      strncat(dest, src, ((copyCount > STATE_STRING_COPY_COUNT) && (copyCount < maxCount)) ? copyCount : maxCount);
      dest[destSize - 1] = '\0';  // Make sure the destination string is alway zero terminated!
      return 0;
    }

    //! @brief save string copy (will not overwrite the destination)
    //! @param[in]   dest 	pointer to the character array to copy to
    //! @param[in]   src    pointer to the character array to copy from
    //! @param[in]   copyCount  maximum number of characters to copy
    //! @param[in]   destSize the size of the destination buffer 
    //! @return returns zero on success, returns non-zero on error. Also, on error, writes zero to dest[0] (unless dest is a null pointer or destsz is zero or greater than RSIZE_MAX) and may clobber the rest of the destination array with unspecified values.
    inline std::int32_t stringCpyS(char* dest, std::int32_t destSize, const char* src, std::int32_t copyCount)
    {
      auto maxCount = destSize - 1;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
      strncpy(dest, src, ((copyCount > STATE_STRING_COPY_COUNT) && (copyCount < destSize)) ? copyCount : maxCount);
#pragma GCC diagnostic pop
      dest[destSize - 1] = '\0';  // Make sure the destination string is alway zero terminated!
      return 0;
    }
#endif // _MSC_VER
        
    /// @brief Convert string to int value in specific radix
    /// @param[in]     source string
    /// @param[in]     target value radix(pass by decimal)
    /// @return        the result int value
    inline std::int32_t safeConvertToInt(const std::string& str, std::int32_t resultRadix)
    {
      char* end {};
      errno = 0; // Reset errno before conversion
      long value = std::strtol(str.c_str(), &end, resultRadix);

      // Check for errors
      if (errno == ERANGE || value > INT_MAX || value < INT_MIN)
      {
        return INT_MIN; // Default minimum value
      }
      if (*end != '\0')
      {
        return INT_MIN; // Default minimum value
      }
      return static_cast<int>(value);
    }
  }
}
