#ifndef RUBY_OSRM_OBJECT_H_
#define RUBY_OSRM_OBJECT_H_

#include "globals.hpp"

using namespace Rice;

#include <osrm/osrm.hpp>

#include <osrm/engine_config.hpp>

#include <osrm/route_parameters.hpp>
#include <osrm/table_parameters.hpp>
#include <osrm/nearest_parameters.hpp>
#include <osrm/trip_parameters.hpp>
#include <osrm/match_parameters.hpp>
#include <osrm/tile_parameters.hpp>
#include <osrm/json_container.hpp>

// Hackery for getting rendered JSON
//#include "util/json_renderer.hpp"

Object wrap_distance_by_roads(Object self, Object o);

Object wrap_route(Object self, Object o);

void init_osrm_object();

#endif