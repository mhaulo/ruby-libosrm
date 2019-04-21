#include "ruby_osrm_object.hpp"

using namespace Rice;

Data_Type<osrm::OSRM> rb_cOsrm;

template<>
osrm::engine::EngineConfig from_ruby<osrm::engine::EngineConfig>(Object storage_config, Object algorithm) {
    if(storage_config.is_nil()) {
        // TODO: raise error
    }

    osrm::engine::EngineConfig config;
    config.storage_config = { storage_config.to_s().c_str() };
    config.use_shared_memory = false;

    switch(algorithm.to_s().c_str()) {
        case "CH":
            config.algorithm = osrm::engine::EngineConfig::Algorithm::CH;
            break;
        case "CoreCH":
            config.algorithm = osrm::engine::EngineConfig::Algorithm::CoreCH;
            break;
        case "MLD":
            config.algorithm = osrm::engine::EngineConfig::Algorithm::MLD;
            break:
        default:
            config.algorithm = osrm::engine::EngineConfig::Algorithm::CH;
            break;
    }

    return config;
}


