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
    OsrmWrap();
    OsrmWrap(std::string database_path);
    ~OsrmWrap();

    // Main features
    Object route(Array coordinates);
    Object match(Array coordinates);
    Object nearest(double lat, double lon);
    Object table(Array coordinates, Hash opts);
    Object trip(Array coordinates, Hash opts);
    Object tile(int x, int y, int zoom);
    Object distance_by_roads(Array coordinates);

private:
    // Helper methods used by public feature functions
    Array parse_routes(osrm::json::Array routes);
    Hash parse_route(json::Object route);
    Hash parse_geometry(osrm::util::json::Value);
    Array parse_route_legs(osrm::util::json::Value value);
    Array parse_route_leg_steps(osrm::util::json::Value value);
    Hash parse_route_leg_annotations(osrm::util::json::Value value);
    Array parse_waypoints(json::Array waypoints);
    Hash parse_waypoint(json::Object waypoint);
    std::vector<std::size_t> table_array_conversion(Object o);

    // OSRM routing engine
    OSRM* osrm;
};


#endif
