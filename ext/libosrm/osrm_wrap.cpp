#include "osrm/match_parameters.hpp"
#include "osrm/nearest_parameters.hpp"
#include "osrm/route_parameters.hpp"
#include "osrm/table_parameters.hpp"
#include "osrm/trip_parameters.hpp"
#include "osrm/tile_parameters.hpp"

#include "osrm/coordinate.hpp"
#include "osrm/engine_config.hpp"
#include "osrm/json_container.hpp"

#include "osrm/osrm.hpp"
#include "osrm/status.hpp"

#include <string>

#include "osrm_wrap.hpp"


// ------------- Standard constructors and destructors ------------------------

OsrmWrap::OsrmWrap() {
    EngineConfig config;
    config.use_shared_memory = false;
    config.algorithm = EngineConfig::Algorithm::MLD;
    osrm = new OSRM({config});
}


OsrmWrap::OsrmWrap(std::string database_path) {
    EngineConfig config;
    config.storage_config = {database_path.c_str()};
    config.use_shared_memory = false;
    config.algorithm = EngineConfig::Algorithm::MLD;
    osrm = new OSRM({config});
}


OsrmWrap::~OsrmWrap() {
    delete osrm;
}

// ------------- Main features ------------------------------------------------

Object OsrmWrap::route(Array coordinates) {
    RouteParameters params;
    params.geometries = RouteParameters::GeometriesType::GeoJSON;
    params.overview = RouteParameters::OverviewType::Full;
    params.annotations = true;
    params.steps = false;
    
// TODO: use this
//     Object geometry_type = opts[Symbol("geometry_type")];
//     if(!geometry_type.is_nil()) {
//         Symbol g_type = (Symbol) geometry_type;
//         const char *type = g_type.c_str();
//         if(strcmp(type, "polyline") == 0) {
//             params.geometries = osrm::RouteParameters::GeometriesType::Polyline;
//         }
//         if(strcmp(type, "polyline6") == 0) {
//             params.geometries = osrm::RouteParameters::GeometriesType::Polyline6;
//         }
//         if(strcmp(type, "geojson") == 0) {
//             params.geometries = osrm::RouteParameters::GeometriesType::GeoJSON;
//         }
//     }
// 
//     Object steps = opts[Symbol("steps")];
//     if(steps) {
//         params.steps = true;
//     }
// 
//     Object annotations = opts[Symbol("annotations")];
//     if(annotations) {
//         params.annotations = true;
//     }
    
    Array::iterator it = coordinates.begin();
    Array::iterator end = coordinates.end();
    for(; it != end; ++it) {
        Hash latlon = (Hash)*it;
        double lat = from_ruby<double>(latlon[Symbol("lat")]);
        double lon = from_ruby<double>(latlon[Symbol("lon")]);
        params.coordinates.push_back({
            util::FloatLongitude{lon},
            util::FloatLatitude{lat}
        });
    }

    json::Object osrm_output;
    const auto status = osrm->Route(params, osrm_output);

    Hash result;
    result[String("code")] = osrm_output.values["code"].get<json::String>().value;

    if (status == Status::Ok) {
        Array routes_array;
        try {
            auto &routeValues = osrm_output.values["routes"].get<osrm::json::Array>();
            for(auto const& routeValue : routeValues.values) {
                routes_array.push(parse_route(routeValue.get<osrm::json::Object>()));
            }
            result[String("routes")] = routes_array;
        }
        catch(...) {}

    }
    else {
        const auto message = osrm_output.values["message"].get<json::String>().value;
        result[String("message")] = message;
    }

    return result;
}


Object OsrmWrap::match(Array coordinates) {
    MatchParameters params;
    params.geometries = RouteParameters::GeometriesType::GeoJSON;
    params.overview = RouteParameters::OverviewType::Full;
    params.gaps = MatchParameters::GapsType::Ignore;
    params.annotations = true;

// TODO: use this
//     if(!opts.is_nil()) {
//         Object geometry_type = opts[Symbol("geometry_type")];
//         if(!geometry_type.is_nil()) {
//             Symbol g_type = (Symbol) geometry_type;
//             const char *type = g_type.c_str();
//             if(strcmp(type, "polyline") == 0) {
//                 params.geometries = osrm::RouteParameters::GeometriesType::Polyline;
//             }
//             if(strcmp(type, "polyline6") == 0) {
//                 params.geometries = osrm::RouteParameters::GeometriesType::Polyline6;
//             }
//             if(strcmp(type, "geojson") == 0) {
//                 params.geometries = osrm::RouteParameters::GeometriesType::GeoJSON;
//             }
//         }
// 
//         Object steps = opts[Symbol("steps")];
//         if(steps) {
//             params.steps = true;
//         }
// 
//         Object annotations = opts[Symbol("annotations")];
//         if(annotations) {
//             params.annotations = true;
//         }
//     }
    
    Array::iterator it = coordinates.begin();
    Array::iterator end = coordinates.end();
    for(; it != end; ++it) {
        Hash latlon = (Hash)*it;
        double lat = from_ruby<double>(latlon[Symbol("lat")]);
        double lon = from_ruby<double>(latlon[Symbol("lon")]);
        
        params.coordinates.push_back({
            util::FloatLongitude{lon},
            util::FloatLatitude{lat}
        });
    }
    
    json::Object osrm_output;
    const auto status = osrm->Match(params, osrm_output);
    Hash result;
    result[String("code")] = osrm_output.values["code"].get<json::String>().value;
    
    if (status == Status::Ok) {
        for(std::pair<std::string, osrm::util::json::Value> e : osrm_output.values) {
            if(e.first == "code") {
                try {
                    result[String("code")] = e.second.get<osrm::json::String>().value;
                }
                catch(...) {}
            }
            else if(e.first == "tracepoints") {
                try {
                    result[String("tracepoints")] = parse_waypoints(e.second.get<osrm::json::Array>());
                }
                catch(...) {}
            }
            else if(e.first == "matchings") {
                try {
                    result[String("matchings")] = parse_routes(e.second.get<osrm::json::Array>());
                }
                catch(...) {}
            }
            else {
            }
        }
    }
    else {
        const auto message = osrm_output.values["message"].get<json::String>().value;
        result[String("message")] = message;
    }
    
    return result;
}


Object OsrmWrap::nearest(double lat, double lon) {
    NearestParameters params;
    params.number_of_results = 1; // TODO add argument for this
    
// TODO: use this
//     if(!opts.is_nil()) {
//         Object geometry_type = opts[Symbol("geometry_type")];
//         if(!geometry_type.is_nil()) {
//             Symbol g_type = (Symbol) geometry_type;
//             const char *type = g_type.c_str();
//             if(strcmp(type, "polyline") == 0) {
//                 params.geometries = osrm::RouteParameters::GeometriesType::Polyline;
//             }
//             if(strcmp(type, "polyline6") == 0) {
//                 params.geometries = osrm::RouteParameters::GeometriesType::Polyline6;
//             }
//             if(strcmp(type, "geojson") == 0) {
//                 params.geometries = osrm::RouteParameters::GeometriesType::GeoJSON;
//             }
//         }
// 
//         Object steps = opts[Symbol("steps")];
//         if(steps) {
//             params.steps = true;
//         }
// 
//         Object annotations = opts[Symbol("annotations")];
//         if(annotations) {
//             params.annotations = true;
//         }
//     }

    params.coordinates.push_back(
        {util::FloatLongitude{lon}, util::FloatLatitude{lat}}
    );
    
    json::Object osrm_output;
    const auto status = osrm->Nearest(params, osrm_output);
    
    Hash result;
    result[String("code")] = osrm_output.values["code"].get<json::String>().value;
    
    if (status == Status::Ok) {
        for(std::pair<std::string, osrm::util::json::Value> e : osrm_output.values) {
            if(e.first == "code") {
                try {
                    result[String("code")] = e.second.get<osrm::json::String>().value;
                }
                catch(...) {}
            }
            else if(e.first == "waypoints") {
                try {
                    result[String("waypoints")] = parse_waypoints(e.second.get<osrm::json::Array>());
                }
                catch(...) {}
            }
            else {
            }
        }
    }
    else {
        const auto message = osrm_output.values["message"].get<json::String>().value;
        result[String("message")] = message;
    }
    
    return result;
}


Object OsrmWrap::table(Array coordinates, Hash opts) {
    osrm::TableParameters params;

    Array::iterator it = coordinates.begin();
    Array::iterator end = coordinates.end();
    for(; it != end; ++it) {
        Hash latlon = (Hash)*it;
        double lat = from_ruby<double>(latlon[Symbol("latitude")]);
        double lon = from_ruby<double>(latlon[Symbol("longitude")]);
        params.coordinates.push_back({osrm::util::FloatLongitude{lon}, osrm::util::FloatLatitude{lat}});
    }

    if(!opts.is_nil()) {
        Object sources = opts[Symbol("sources")];
        if(sources) {
            params.sources = table_array_conversion(sources);
        }

        Object destinations = opts[Symbol("destinations")];
        if(sources) {
            params.destinations = table_array_conversion(destinations);
        }
    }

    osrm::json::Object osrm_output;

    // Execute routing request, this does the heavy lifting
    const auto status = osrm->Table(params, osrm_output);
    Hash result;
    result[String("code")] = osrm_output.values["code"].get<json::String>().value;

    if (status == osrm::Status::Ok) {
        for(std::pair<std::string, osrm::util::json::Value> e : osrm_output.values) {
            if(e.first == "code") {
                try {
                    result[String("code")] = e.second.get<osrm::json::String>().value;
                }
                catch(...) {}
            }
            else if(e.first == "durations") {
                try {
                    Array durations_result;
                    for(auto const& durationArrayValue : e.second.get<osrm::json::Array>().values) {
                        auto durationArray = durationArrayValue.get<osrm::json::Array>();
                        Array duration_result;

                        for(auto const& durationValue : durationArray.values) {
                            duration_result.push(durationValue.get<osrm::json::Number>().value);
                        }
                        durations_result.push(duration_result);
                    }
                    result[String("durations")] = durations_result;
                }
                catch(...) {}
            }
            else if(e.first == "sources") {
                try {
                    result[String("sources")] = parse_waypoints(e.second.get<osrm::json::Array>());
                }
                catch(...) {}
            }
            else if(e.first == "destinations") {
                try {
                    result[String("destinations")] = parse_waypoints(e.second.get<osrm::json::Array>());
                }
                catch(...) {}
            }
            else {
            }
        }
    }
    else {
        const auto message = osrm_output.values["message"].get<osrm::json::String>().value;
        result[String("message")] = message;
    }

    return result;
}


Object OsrmWrap::trip(Array coordinates, Hash opts) {
    osrm::TripParameters params;

    Array::iterator it = coordinates.begin();
    Array::iterator end = coordinates.end();
    for(; it != end; ++it) {
        Hash latlon = (Hash)*it;
        double lat = from_ruby<double>(latlon[Symbol("latitude")]);
        double lon = from_ruby<double>(latlon[Symbol("longitude")]);
        params.coordinates.push_back({osrm::util::FloatLongitude{lon}, osrm::util::FloatLatitude{lat}});
    }

    Object roundtrip = opts[Symbol("roundtrip")];
    if(roundtrip) {
        params.roundtrip = true;
    }

    Object source = opts[Symbol("source")];
    if(!source.is_nil()) {
        Symbol source_symbol = (Symbol) source;
        const char *source_string = source_symbol.c_str();
        if(strcmp(source_string, "any") == 0) {
            params.source = osrm::TripParameters::SourceType::Any;
        } else if(strcmp(source_string, "first") == 0) {
            params.source = osrm::TripParameters::SourceType::First;
        } else {
            throw Exception(rb_eRuntimeError, "libosrm.so#wrap_trip(): failed to recognize given source symbol: %s", source_string);
        }
    }

    Object destination = opts[Symbol("destination")];
    if(!destination.is_nil()) {
        Symbol destination_symbol = (Symbol) destination;
        const char *destination_string = destination_symbol.c_str();
        if(strcmp(destination_string, "any") == 0) {
            params.destination = osrm::TripParameters::DestinationType::Any;
        } else if(strcmp(destination_string, "last") == 0) {
            params.destination = osrm::TripParameters::DestinationType::Last;
        } else {
            throw Exception(rb_eRuntimeError, "libosrm.so#wrap_trip(): failed to recognize given destination symbol: %s", destination_string);
        }
    }

    // TODO: since this option is same as in route thing, maybe we could have some kind of abstraction?
    Object geometry_type = opts[Symbol("geometry_type")];
    if(!geometry_type.is_nil()) {
        Symbol g_type = (Symbol) geometry_type;
        const char *type = g_type.c_str();
        if(strcmp(type, "polyline") == 0) {
            params.geometries = osrm::RouteParameters::GeometriesType::Polyline;
        }
        if(strcmp(type, "polyline6") == 0) {
            params.geometries = osrm::RouteParameters::GeometriesType::Polyline6;
        }
        if(strcmp(type, "geojson") == 0) {
            params.geometries = osrm::RouteParameters::GeometriesType::GeoJSON;
        }
    }

    Object steps = opts[Symbol("steps")];
    if(steps) {
        params.steps = true;
    }

    Object annotations = opts[Symbol("annotations")];
    if(annotations) {
        params.annotations = true;
    }

    osrm::json::Object osrm_output;
    const auto status = osrm->Trip(params, osrm_output);
    Hash result;
    result[String("code")] = osrm_output.values["code"].get<json::String>().value;

    if (status == osrm::Status::Ok) {
        result[String("waypoints")] = parse_waypoints(osrm_output.values["waypoints"].get<osrm::json::Array>());

        Array trips_array;
        auto &tripValues = osrm_output.values["trips"].get<osrm::json::Array>();
        for(auto const& tripValue : tripValues.values) {
            trips_array.push(parse_route(tripValue.get<osrm::json::Object>()));
        }
        result[String("trips")] = trips_array;
    }
    else {
        const auto message = osrm_output.values["message"].get<osrm::json::String>().value;
        result[String("message")] = message;
    }

    return result;
}


Object OsrmWrap::tile(int x, int y, int zoom) {
    osrm::TileParameters params;

    params.x = x;
    params.y = y;
    params.z = zoom;

    // Response is a std::string, instead of JSON stuff that is elsewhere
    std::string result;

    const auto status = osrm->Tile(params, result);

    if (status != osrm::Status::Ok) {
        throw Exception(rb_eRuntimeError, "Failed to get tile data with given input.");
    }

    return to_ruby(result);
}



Object OsrmWrap::distance_by_roads(Array coordinates) {
    osrm::RouteParameters params;

    Array::iterator it = coordinates.begin();
    Array::iterator end = coordinates.end();
    for(; it != end; ++it) {
        Hash latlon = (Hash)*it;
        double lat = from_ruby<double>(latlon[Symbol("latitude")]);
        double lon = from_ruby<double>(latlon[Symbol("longitude")]);
        params.coordinates.push_back({osrm::util::FloatLongitude{lon}, osrm::util::FloatLatitude{lat}});
    }

    params.overview = osrm::RouteParameters::OverviewType::False;

    osrm::json::Object osrm_output;

    // Execute routing request, this does the heavy lifting
    const auto status = osrm->Route(params, osrm_output);
    const auto code = osrm_output.values["code"].get<osrm::json::String>().value;

    auto distance = 0.0;

    if (status == osrm::Status::Ok) {
        // We can take first route since we only want the distance.
        auto &routes = osrm_output.values["routes"].get<osrm::json::Array>();
        auto &route = routes.values.at(0).get<osrm::json::Object>();
        distance = route.values["distance"].get<osrm::json::Number>().value;
    }
    else {
        const auto message = osrm_output.values["message"].get<osrm::json::String>().value;
        distance = 0;
    }

    return to_ruby(distance);
}



// ------------- Private helper functions -------------------------------------


Array OsrmWrap::parse_routes(osrm::json::Array routes) {
    Array result;

    for (auto const& routeValue : routes.values) {
        auto route = routeValue.get<osrm::json::Object>();
        Hash route_result = parse_route(route);
        result.push(route_result);
    }

    return result;
}


Hash OsrmWrap::parse_route(osrm::json::Object route) {
    Hash route_result;

    for(std::pair<std::string, osrm::util::json::Value> e : route.values) {
        if(e.first == "distance") {
            route_result[String("distance")] = e.second.get<osrm::json::Number>().value;
        }
        else if(e.first == "duration") {
            route_result[String("duration")] = e.second.get<osrm::json::Number>().value;
        }
        else if(e.first == "weight") {
            route_result[String("weight")] = e.second.get<osrm::json::Number>().value;
        }
        else if(e.first == "weight_name") {
            route_result[String("weight_name")] = e.second.get<osrm::json::String>().value;
        }
        else if(e.first == "geometry") {
            route_result[String("geometry")] = parse_geometry(e.second);
        }
        else if(e.first == "legs") {
            route_result[String("legs")] = parse_route_legs(e.second);
        }
        else {

        }
    }

    return route_result;
}

Hash OsrmWrap::parse_geometry(osrm::util::json::Value value) {
    auto geometry = value.get<osrm::json::Object>();
    Hash result;

    for(std::pair<std::string, osrm::util::json::Value> e : geometry.values) {
        if (e.first == "type") {
            result[String("type")] = e.second.get<osrm::json::String>().value;
        }
        else if (e.first == "coordinates") {
            Array coordinates;
            for(auto const &value : e.second.get<osrm::json::Array>().values) {
                Array coordinate;

                for (auto const &c : value.get<osrm::json::Array>().values) {
                    coordinate.push(c.get<osrm::json::Number>().value);
                }

                coordinates.push(coordinate);
            }
            result[String("coordinates")] = coordinates;
        }
        else {
        }
    }

    return result;
}

Array OsrmWrap::parse_route_legs(osrm::util::json::Value value) {
    Array legs_array;
    auto &legsValues = value.get<osrm::json::Array>();

    for(auto const& legValue : legsValues.values) {
        auto leg = legValue.get<osrm::json::Object>();
        Hash leg_result;

        for(std::pair<std::string, osrm::util::json::Value> e : leg.values) {
            if(e.first == "distance") {
                leg_result[String("distance")] = e.second.get<osrm::json::Number>().value;
            }
            else if(e.first == "duration") {
                leg_result[String("duration")] = e.second.get<osrm::json::Number>().value;
            }
            else if(e.first == "weight") {
                leg_result[String("weight")] = e.second.get<osrm::json::Number>().value;
            }
            else if(e.first == "summary") {
                leg_result[String("summary")] = e.second.get<osrm::json::String>().value;
            }
            else if(e.first == "steps") {
                leg_result[String("steps")] = parse_route_leg_steps(e.second);
            }
            else if(e.first == "annotation") {
                leg_result[String("annotation")] = parse_route_leg_annotations(e.second);
            }
            else {
            }
        }

        legs_array.push(leg_result);
    }

    return legs_array;
}

Array OsrmWrap::parse_route_leg_steps(osrm::util::json::Value value) {
    Array steps_array;
    auto &stepsValues = value.get<osrm::json::Array>();
    
    for(auto const& stepValue : stepsValues.values) {
        auto step = stepValue.get<osrm::json::Object>();
        Hash step_result;
        
        for(std::pair<std::string, osrm::util::json::Value> e : step.values) {
            if(e.first == "distance") {
                step_result[String("distance")] = e.second.get<osrm::json::Number>().value;
            }
            else if(e.first == "duration") {
                step_result[String("duration")] = e.second.get<osrm::json::Number>().value;
            }
            else if(e.first == "weight") {
                step_result[String("weight")] = e.second.get<osrm::json::Number>().value;
            }
            else if(e.first == "geometry") {
                step_result[String("geometry")] = e.second.get<osrm::json::String>().value;
            }
            else if(e.first == "name") {
                step_result[String("name")] = e.second.get<osrm::json::String>().value;
            }
            else if(e.first == "ref") {
                step_result[String("ref")] = e.second.get<osrm::json::String>().value;
            }
            else if(e.first == "pronunciation") {
                step_result[String("pronunciation")] = e.second.get<osrm::json::String>().value;
            }
            else if(e.first == "destinations") {
                step_result[String("destinations")] = e.second.get<osrm::json::String>().value;
            }
            else if(e.first == "exits") {
                step_result[String("exits")] = e.second.get<osrm::json::String>().value;
            }
            else if(e.first == "mode") {
                step_result[String("mode")] = e.second.get<osrm::json::String>().value;
            }
            else if(e.first == "maneuver") {
                auto values = e.second.get<osrm::json::Object>().values;
                Hash maneuver;
                maneuver[String("modifier")] = values.at("modifier").get<osrm::json::String>().value;
                maneuver[String("type")] = values.at("type").get<osrm::json::String>().value;
                step_result[String("maneuver")] = maneuver;
            }
            else if(e.first == "intersections") {
                //step_result[String("intersections")] = e.second.get<osrm::json::String>().value; // TODO
            }
            else if(e.first == "rotary_name") {
                step_result[String("rotary_name")] = e.second.get<osrm::json::String>().value;
            }
            else if(e.first == "rotary_pronunciation") {
                step_result[String("rotary_pronunciation")] = e.second.get<osrm::json::String>().value;
            }
            else {
            }
        }

        steps_array.push(step_result);
    }

    return steps_array;
}

Hash OsrmWrap::parse_route_leg_annotations(osrm::util::json::Value value) {
    auto annotations = value.get<osrm::json::Object>();
    Hash result;

    for(std::pair<std::string, osrm::util::json::Value> e : annotations.values) {
        if(e.first == "distance") {
            Array values;
            for(auto const &value : e.second.get<osrm::json::Array>().values) {
                values.push(value.get<osrm::json::Number>().value);
            }
            result[String("distance")] = values;
        }
        else if(e.first == "duration") {
            Array values;
            for(auto const &value : e.second.get<osrm::json::Array>().values) {
                values.push(value.get<osrm::json::Number>().value);
            }
            result[String("duration")] = values;
        }
        else if(e.first == "datasources") {
            Array values;
            for(auto const &value : e.second.get<osrm::json::Array>().values) {
                values.push(value.get<osrm::json::Number>().value);
            }
            result[String("datasources")] = values;
        }
        else if(e.first == "nodes") {
            Array values;
            for(auto const &value : e.second.get<osrm::json::Array>().values) {
                values.push(value.get<osrm::json::Number>().value);
            }
            result[String("nodes")] = values;
        }
        else if(e.first == "weight") {
            Array values;
            for(auto const &value : e.second.get<osrm::json::Array>().values) {
                values.push(value.get<osrm::json::Number>().value);
            }
            result[String("weight")] = values;
        }
        else if (e.first == "metadata") {
            // TODO
        }
        else if(e.first == "speed") {
            Array values;
            for(auto const &value : e.second.get<osrm::json::Array>().values) {
                values.push(value.get<osrm::json::Number>().value);
            }
            result[String("speed")] = values;
        }
        else  {
        }
    }

    return result;
}

Array OsrmWrap::parse_waypoints(osrm::json::Array waypoints) {
    Array waypoints_result;

    for(auto const& waypointValue : waypoints.values) {
        auto waypoint = waypointValue.get<osrm::json::Object>();
        Hash waypoint_result;

        for(std::pair<std::string, osrm::util::json::Value> e : waypoint.values) {
            if(e.first == "name") {
                waypoint_result[String("name")] = e.second.get<osrm::json::String>().value;
            }
            else if(e.first == "location") {
                Hash location;
                auto const& values = e.second.get<osrm::json::Array>().values;
                location[Symbol("latitude")] = values[1].get<osrm::json::Number>().value;
                location[Symbol("longitude")] = values[0].get<osrm::json::Number>().value;
                waypoint_result[String("location")] = location;
            }
            else if(e.first == "hint") {
                waypoint_result[String("hint")] = e.second.get<osrm::json::String>().value;
            }
            else if(e.first == "distance") {
                waypoint_result[String("distance")] = e.second.get<osrm::json::Number>().value;
            }
            else if(e.first == "waypoint_index") { // Present in trip action
                waypoint_result[String("waypoint_index")] = e.second.get<osrm::json::Number>().value;
            }
            else if(e.first == "trips_index") { // Present in trip action
                waypoint_result[String("trips_index")] = e.second.get<osrm::json::Number>().value;
            }
            else {
            }
        }

        waypoints_result.push(waypoint_result);
    }

    return waypoints_result;
}

std::vector<std::size_t> OsrmWrap::table_array_conversion(Object o) {
    std::vector<std::size_t> out;
    if(o.is_a(rb_cArray)) {
        Array a = (Array) o;
        Array::iterator it = a.begin();
        Array::iterator end = a.end();
        for(; it != end; ++it) {
            int index = (int)(Object)*it;
            out.push_back(index);
        }
    } else if(o.is_a(rb_cString)) {
        out.push_back(from_ruby<int>(o));
    } else if(o.is_a(rb_cNumeric)) {
        out.push_back(from_ruby<int>(o));
    }

    return out;
}
