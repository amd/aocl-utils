#include "alci/cxx/alci.hh"
#include "alci/cxx/enum.hh"

namespace alci {
// clang-format off
/**
 * \brief   Overloading the ostream output (<<) operator with cache level.
 *
 * L1/L2/L3 etc cache levels will be returned.
 *
 * \param[in] os    Pointer to output stream.
 * \param[in] level Pointer to cache level.
 *
 * \return  Returns ostream output with cache level.
 */
std::ostream&
operator<<(std::ostream& os, const CacheLevel& level)
{
    switch (level) {
        case CacheLevel::e_Level_1:
            os << "L1";
            break;
        case CacheLevel::e_Level_2:
            os << "L2";
            break;
        case CacheLevel::e_Level_3:
            os << "L3";
            break;
        case CacheLevel::e_Level_4:
            os << "L4";
            break;
        case CacheLevel::e_Level_5:
            os << "L5";
            break;
        case CacheLevel::e_Level_Unknown:
        default:
            os << "Unknown";
            break;
    }
    return os;
}

/**
 * \brief   Overloading the ostream output (<<) operator with cache type.
 *
 * Instruction/data/unified cache types will be returned.
 *
 * \param[in] os    Pointer to output stream.
 * \param[in] type  Pointer to cache type.
 *
 * \return  Returns ostream output with cache type.
 */
std::ostream&
operator<<(std::ostream& os, const CacheType& type)
{
    switch (type) {
        case CacheType::eInstruction:
            os << "Instruction Cache";
            break;
        case CacheType::eData:
            os << "Data Cache";
            break;
        case CacheType::eUnified:
            os << "Unified Cache";
            break;
        default:
            os << "Unknown";
            break;
    }
    return os;
}

/**
 * \brief   Overloading the ostream output (<<) operator with vendor name.
 *
 * \param[in] os  Pointer to output stream.
 * \param[in] av  Pointer to Vendor.
 *
 * \return  Returns ostream output with Vendor name.
 */
std::ostream&
operator<<(std::ostream& os, const Vendor& av)
{
    switch (av) {
        case Vendor::eIntel:
            os << "Intel";
            break;
        case Vendor::eAmd:
            os << "AMD";
            break;
        default:
            os << "Unknown";
            break;
    }
    return os;
}

/**
 * \brief   Overloading the ostream output operator (<<) with architecture name.
 *
 * \param[in] os  Pointer to output stream.
 * \param[in] av  Pointer to architecture name.
 *
 * \return  Returns ostream output with architecture name.
 */
std::ostream&
operator<<(std::ostream& os, const Uarch& av)
{
    switch (av) {
        case Uarch::eZen:
            os << "Zen";
            break;
        case Uarch::eZen2:
            os << "Zen2";
            break;
        case Uarch::eZen3:
            os << "Zen3";
            break;
        case Uarch::eZen4:
            os << "Zen4";
            break;
        default:
            os << "Unknown";
            break;
    }
    return os;
}
// clang-format on

} // namespace alci
