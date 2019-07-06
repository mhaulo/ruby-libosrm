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

    Hash result = parseObject(osrm_output);

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
    Hash result = parseObject(osrm_output);

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

    Hash result = parseObject(osrm_output);

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
    Hash result = parseObject(osrm_output);

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
    Hash result = parseObject(osrm_output);

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
    Hash result = parseObject(osrm_output);

    return result;
}



// ------------- Private helper functions -------------------------------------

Hash OsrmWrap::parseObject(osrm::json::Object input) {
    Hash output;

    for(std::pair<std::string, osrm::json::Value> e : input.values) {
        int type_index = e.second.which();
        switch (type_index) {
            case 0: {
                output[String(e.first)] = e.second.get<osrm::json::String>().value;
                break;
            }
            case 1: {
                output[String(e.first)] = e.second.get<osrm::json::Number>().value;
                break;
            }
            case 2: {
                auto value = e.second.get<osrm::json::Object>();
                output[String(e.first)] = parseObject(value);
                break;
            }
            case 3: {
                auto array = e.second.get<osrm::json::Array>();
                output[String(e.first)] = parseArray(array);
                break;
            }
            case 4: {
                output[String(e.first)] = true;
                break;
            }
            case 5: {
                output[String(e.first)] = false;
                break;
            }
            case 6: {
                output[String(e.first)] = NULL;
                break;
            }
            default:
                break;
        }
    }

    return output;
}

Array OsrmWrap::parseArray(osrm::json::Array input) {
    Array output;

    for(auto const& array_item : input.values) {
        int type_index = array_item.which();

        switch (type_index) {
            case 0: {
                output.push(array_item.get<osrm::json::String>().value);
                break;
            }
            case 1: {
                output.push(array_item.get<osrm::json::Number>().value);
                break;
            }
            case 2: {
                auto value = array_item.get<osrm::json::Object>();
                output.push(parseObject(value));
                break;
            }
            case 3: {
                auto array = array_item.get<osrm::json::Array>();
                output.push(parseArray(array));
                break;
            }
            case 4: {
                output.push(true);
                break;
            }
            case 5: {
                output.push(false);
                break;
            }
            case 6: {
                output.push(NULL);
                break;
            }
            default:
                break;
        }
    }

    return output;
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
