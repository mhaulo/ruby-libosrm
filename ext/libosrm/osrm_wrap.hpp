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

class OsrmWrap {
public:
    OsrmWrap();
    OsrmWrap(std::string database_path);
    ~OsrmWrap();
    
    Object route(Array coordinates);
    Object match(Array coordinates);
    Object nearest(double lat, double lon);
    Object table(Array coordinates, Hash opts);
    Object trip(Array coordinates, Hash opts);
    Object tile(int x, int y, int zoom);
    
    Object distance_by_roads(Array coordinates);
    
private:
    Array parse_routes(osrm::json::Array routes);
    Hash parse_route(json::Object route);
    Hash parse_geometry(osrm::util::json::Value);
    Array parse_route_legs(osrm::util::json::Value value);
    Array parse_route_leg_steps(osrm::util::json::Value value);
    Hash parse_route_leg_annotations(osrm::util::json::Value value);
    Array parse_waypoints(json::Array waypoints);
    std::vector<std::size_t> table_array_conversion(Object o);
    
    OSRM* osrm;
};


#endif
