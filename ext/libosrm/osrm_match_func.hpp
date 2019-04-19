#ifndef OSRM_MATCH_FUNC
#define OSRM_MATCH_FUNC

#include "globals.hpp"
#include "osrm_action.hpp"

#include <osrm/osrm.hpp>
#include <osrm/json_container.hpp>

using namespace Rice;

Rice::Object wrap_match(Rice::Object self, Rice::Array coordinates, Rice::Hash opts);

class MatchFunc : private OSRMAction {

public:
    Object wrap_match(Object self, Array coordinates, Hash opts);

private:
    Hash parse_match_result(osrm::json::Object match);
};

//Array parse_route_legs(osrm::util::json::Value value);
//Array parse_route_leg_steps(osrm::util::json::Value value);
//Hash parse_route_leg_annotations(osrm::util::json::Value value);



#endif
