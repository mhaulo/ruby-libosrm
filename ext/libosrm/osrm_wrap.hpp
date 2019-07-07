#ifndef OSRM_WRAP
#define OSRM_WRAP

#include <osrm/osrm.hpp>
#include <osrm/json_container.hpp>
#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"
#include "rice/Enum.hpp"
#include "rice/Struct.hpp"

using namespace Rice;
using namespace osrm;


/*
 * This class provides the main functionality of this gem. Rather than doing direct
 * Rice library mappings to the OSRM library, everything goes through this wrapper.
 * This keeps the interface simple and clean, and makes things easy to implement.
 */


class OsrmWrap {
public:
    // Standard constructors and destructors
    OsrmWrap(std::string database_path);
    ~OsrmWrap();

    // Main features
    Object route(Array coordinates, Hash options);
    Object match(Array coordinates, Hash options);
    Object nearest(double lat, double lon, Hash options);
    Object table(Array coordinates, Hash opts);
    Object trip(Array coordinates, Hash opts);

private:
    // Helper methods used by public feature functions
    Hash parseObject(osrm::json::Object input);
    Array parseArray(osrm::json::Array input);
    std::vector<std::size_t> table_array_conversion(Object o);
    template <typename T> void setCoordinates(Array &coordinates, T &params);
    void setRouteOptions(Hash &options, RouteParameters &params);

    // OSRM routing engine
    OSRM* osrm;
};


#endif
