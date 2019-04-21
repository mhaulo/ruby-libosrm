#ifndef OSRM_WRAP
#define OSRM_WRAP

#include <osrm/osrm.hpp>
#include <osrm/json_container.hpp>

using namespace Rice;
using namespace osrm;

class OsrmWrap {
public:
    OsrmWrap();
    
    Object route(Array coordinates);
    Object match(Object input);
    Object nearest(double lat, double lon);
    Object table(Object table);
    Object trip(Object trip);
    Object tile(Object input);
    
private:
    Hash parse_route(json::Object route);
    Array parse_route_legs(osrm::util::json::Value value);
    Array parse_route_leg_steps(osrm::util::json::Value value);
    Hash parse_route_leg_annotations(osrm::util::json::Value value);
    Array parse_waypoints(json::Array waypoints);
    
    OSRM* osrm;
}


#endif OSRM_WRAP
