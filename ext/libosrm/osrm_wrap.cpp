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

Object OsrmWrap::route(Array coordinates, Hash options) {
    RouteParameters params;

    setRouteOptions(options, params);
    setCoordinates(coordinates, params);

    if (!options.is_nil()) {
        Object alternatives_option = options[Symbol("alternatives")];
        if (!alternatives_option.is_nil()) {
            bool alternatives = from_ruby<bool>(alternatives_option);
            params.alternatives = alternatives;
        }

        Object continue_straight_option = options[Symbol("continue_straight")];
        if (!continue_straight_option.is_nil()) {
            bool continue_straight = from_ruby<bool>(continue_straight_option);
            params.alternatives = continue_straight;
        }

        // TODO support for the following options:
        // - waypoints
    }

    json::Object osrm_output;
    osrm->Route(params, osrm_output);
    Hash result = parseObject(osrm_output);

    return result;
}


Object OsrmWrap::match(Array coordinates, Hash options) {
    MatchParameters params;

    setRouteOptions(options, params);
    setCoordinates(coordinates, params);

    if (!options.is_nil()) {
        Object gaps_option = options[Symbol("gaps")];
        if (!gaps_option.is_nil()) {
            const char *type = ((Symbol)gaps_option).c_str();

            if (strcmp(type, "split") == 0) {
                params.gaps = MatchParameters::GapsType::Split;
            }
            else if (strcmp(type, "ignore") == 0) {
                params.gaps = MatchParameters::GapsType::Ignore;
            }
        }

        Object tidy_option = options[Symbol("tidy")];
        if (!tidy_option.is_nil()) {
            bool tidy = from_ruby<bool>(tidy_option);
            params.tidy = tidy;
        }

        // TODO support for the following options:
        // - timestamps
        // - radiuses
        // - waypoints
    }

    json::Object osrm_output;
    osrm->Match(params, osrm_output);
    Hash result = parseObject(osrm_output);

    return result;
}


Object OsrmWrap::nearest(double lat, double lon, Hash options) {
    NearestParameters params;

    // Set default parameters
    params.number_of_results = 1;

    if (!options.is_nil()) {
        Object number = options[Symbol("number")];

        if (!number.is_nil()) {
            int number_of_results = from_ruby<int>(options[Symbol("number")]);
            params.number_of_results = number_of_results;
        }
    }

    params.coordinates.push_back(
        {util::FloatLongitude{lon}, util::FloatLatitude{lat}}
    );

    json::Object osrm_output;
    osrm->Nearest(params, osrm_output);
    Hash result = parseObject(osrm_output);

    return result;
}


Object OsrmWrap::table(Array coordinates, Hash options) {
    osrm::TableParameters params;

    setCoordinates(coordinates, params);

    if (!options.is_nil()) {
        Object sources = options[Symbol("sources")];
        if (!sources.is_nil()) {
            params.sources = table_array_conversion(sources);
        }

        Object destinations = options[Symbol("destinations")];
        if (!destinations.is_nil()) {
            params.destinations = table_array_conversion(destinations);
        }

        Object fallback_speed_setting = options[Symbol("fallback_speed")];
        if (!fallback_speed_setting.is_nil()) {
            double fallback_speed = from_ruby<double>(fallback_speed_setting);

            if (fallback_speed >= 0) {
                params.fallback_speed = fallback_speed;
            }
        }

        Object fallback_coordinate_setting = options[Symbol("fallback_coordinate")];
        if (!fallback_coordinate_setting.is_nil()) {
            const char *type = ((Symbol)fallback_coordinate_setting).c_str();

            if (strcmp(type, "input") == 0) {
                params.fallback_coordinate_type = TableParameters::FallbackCoordinateType::Input;
            }
            else if (strcmp(type, "snapped") == 0) {
                params.fallback_coordinate_type = TableParameters::FallbackCoordinateType::Snapped;
            }
        }

        // According to OSRM documentation, there should be a setting called
        // scale_factor. However, can't find it in code (OSRM backend v. 5.20).
        /*Object scale_factor_setting = options[Symbol("scale_factor")];
        if (!scale_factor_setting.is_nil()) {
            double scale_factor = from_ruby<double>(scale_factor_setting);

            if (scale_factor >= 0) {
                params.scale_factor = scale_factor;
            }
        }*/
    }

    osrm::json::Object osrm_output;
    osrm->Table(params, osrm_output);
    Hash result = parseObject(osrm_output);

    return result;
}


Object OsrmWrap::trip(Array coordinates, Hash options) {
    osrm::TripParameters params;

    setCoordinates(coordinates, params);
    setRouteOptions(options, params);

    if (!options.is_nil()) {
        Object roundtrip_options = options[Symbol("roundtrip")];
        if (!roundtrip_options.is_nil()) {
            bool roundtrip = from_ruby<bool>(roundtrip_options);
            params.roundtrip = roundtrip;
        }

        Object source = options[Symbol("source")];
        if (!source.is_nil()) {
            Symbol source_symbol = (Symbol) source;
            const char *source_string = source_symbol.c_str();

            if (strcmp(source_string, "any") == 0) {
                params.source = osrm::TripParameters::SourceType::Any;
            }
            else if (strcmp(source_string, "first") == 0) {
                params.source = osrm::TripParameters::SourceType::First;
            }
            else {
                throw Exception(rb_eRuntimeError, "libosrm.so#wrap_trip(): failed to recognize given source symbol: %s", source_string);
            }
        }

        Object destination = options[Symbol("destination")];
        if (!destination.is_nil()) {
            Symbol destination_symbol = (Symbol) destination;
            const char *destination_string = destination_symbol.c_str();

            if (strcmp(destination_string, "any") == 0) {
                params.destination = osrm::TripParameters::DestinationType::Any;
            }
            else if (strcmp(destination_string, "last") == 0) {
                params.destination = osrm::TripParameters::DestinationType::Last;
            }
            else {
                throw Exception(rb_eRuntimeError, "libosrm.so#wrap_trip(): failed to recognize given destination symbol: %s", destination_string);
            }
        }
    }

    osrm::json::Object osrm_output;
    osrm->Trip(params, osrm_output);
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

template <typename T>
void OsrmWrap::setCoordinates(Array &coordinates, T &params) {
    Array::iterator it = coordinates.begin();
    Array::iterator end = coordinates.end();

    for(; it != end; ++it) {
        Hash latlon = (Hash)*it;

        double lat = from_ruby<double>(latlon[Symbol("lat")]);
        double lon = from_ruby<double>(latlon[Symbol("lon")]);

        params.coordinates.push_back({
            util::FloatLongitude{lon}, util::FloatLatitude{lat}
        });
    }
}

void OsrmWrap::setRouteOptions(Hash &options, RouteParameters &params) {
    if (options.is_nil()) {
        return;
    }

    Object geometry_type = options[Symbol("geometries")];
    if (!geometry_type.is_nil()) {
        Symbol g_type = (Symbol) geometry_type;
        const char *type = g_type.c_str();
        if (strcmp(type, "polyline") == 0) {
            params.geometries = osrm::RouteParameters::GeometriesType::Polyline;
        }
        if (strcmp(type, "polyline6") == 0) {
            params.geometries = osrm::RouteParameters::GeometriesType::Polyline6;
        }
        if (strcmp(type, "geojson") == 0) {
            params.geometries = osrm::RouteParameters::GeometriesType::GeoJSON;
        }
    }

    Object overview_option = options[Symbol("overview")];
    if (!overview_option.is_nil()) {
        const char *type = ((Symbol)overview_option).c_str();

        if (strcmp(type, "simplified") == 0) {
            params.overview = RouteParameters::OverviewType::Simplified;
        }
        else if (strcmp(type, "full") == 0) {
            params.overview = RouteParameters::OverviewType::Full;
        }
        else if (strcmp(type, "false") == 0) {
            params.overview = RouteParameters::OverviewType::False;
        }
    }

    Object steps = options[Symbol("steps")];
    if (!steps.is_nil()) {
        params.steps = true;
    }

    Object annotations = options[Symbol("annotations")];
    if (!annotations.is_nil()) {
        params.annotations = true;
    }
}
