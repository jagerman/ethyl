#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ios>

namespace utils
{

    enum class PaddingDirection {
        LEFT,
        RIGHT
    };

    template <typename Container>
    std::string toHexString(const Container& bytes) {
        std::ostringstream oss;
        for(const auto byte : bytes) {
            oss << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(static_cast<unsigned char>(byte));
        }                                                                                                                     
        return oss.str();                                                                                                     
    }

    template <typename Container>
    std::string toHexStringBigEndian(const Container& bytes) {
        std::ostringstream oss;
        for(auto it = bytes.rbegin(); it != bytes.rend(); ++it) {
            oss << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(static_cast<unsigned char>(*it));
        }
        return oss.str();
    }

    std::string      decimalToHex(uint64_t decimal);
    std::string_view trimPrefix(std::string_view src, std::string_view prefix);
    std::string_view trimLeadingZeros(std::string_view src);

    std::vector<unsigned char>    fromHexString(std::string_view hexStr);
    uint64_t                      fromHexStringToUint64(std::string_view hexStr);
    std::array<unsigned char, 32> fromHexString32Byte(std::string_view hexStr);

    std::array<unsigned char, 32> hash(std::string in);

    std::string getFunctionSignature(const std::string& function);

    std::string padToNBytes(const std::string& input, size_t byteCount, PaddingDirection direction = PaddingDirection::LEFT);
    std::string padTo8Bytes(const std::string& input, PaddingDirection direction = PaddingDirection::LEFT);
    std::string padTo32Bytes(const std::string& input, PaddingDirection direction = PaddingDirection::LEFT);

    std::vector<unsigned char> intToBytes(uint64_t num);

    std::vector<unsigned char> removeLeadingZeros(std::vector<unsigned char> vec);

    std::string generateRandomString(size_t length);

    std::string trimAddress(const std::string& address);

// END
}
