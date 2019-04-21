#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"
#include "rice/Enum.hpp"
#include "rice/Struct.hpp"
#include "osrm_wrap.hpp"

using namespace Rice;
using namespace osrm;


// From globals.hpp
static Module rb_mLibOSRM = define_module("LibOSRM");

#define ATTR_ACCESSOR_DECL(klass, variable) \
    .define_method(#variable, &klass##_##variable##_get) \
    .define_method(#variable "=", &klass##_##variable##_set)

#endif


// Storage config forward declarations
void init_storage_config();

// Engine config forward declarations
Object EngineConfig_storage_config_get(Object self);
Object EngineConfig_storage_config_set(Object self, Object value);
Object EngineConfig_use_shared_memory_get(Object self);
Object EngineConfig_use_shared_memory_set(Object self, Object value);
char const * description(osrm::EngineConfig::Algorithm e);
void init_engine_config();


extern "C"
void Init_ruby_libosrm() {
    RUBY_TRY
    {
        Data_Type<OsrmWrap> rb_cOsrmWrap =
            define_class<OsrmWrap>("OSRM")
            .define_constructor(Constructor<OsrmWrap>())
            .define_method("route", &OsrmWrap::route)
            .define_method("match", &OsrmWrap::match)
            .define_method("nearest", &OsrmWrap::nearest)
            .define_method("table", &OsrmWrap::table)
            .define_method("trip", &OsrmWrap::trip)
            .define_method("tile", &OsrmWrap::tile)
            ;
    }
    RUBY_CATCH
}


// STORAGE CONFIG

Data_Type<osrm::storage::StorageConfig> rb_cStorageConfig;

template<>
Object to_ruby<boost::filesystem::path>(boost::filesystem::path const & x) {
    String s(x.c_str());
    return s;
}

template<>
boost::filesystem::path from_ruby<boost::filesystem::path>(Object x) {
    return x.to_s().c_str();
}

#define SC_ACC(variable) \
    Object StorageConfig_##variable##_get(Object self) { \
        Data_Object<osrm::storage::StorageConfig> c(self, rb_cStorageConfig); \
        Object o = to_ruby(c->variable); \
        return o; \
    }\
    \
    Object StorageConfig_##variable##_set(Object self, Object value) { \
        Data_Object<osrm::storage::StorageConfig> c(self, rb_cStorageConfig); \
        c->variable = value.to_s().c_str(); \
        return value; \
    }


void init_storage_config() {
    rb_cStorageConfig =
            define_class_under<osrm::storage::StorageConfig>(rb_mLibOSRM, "StorageConfig")
                .define_constructor(Constructor<osrm::storage::StorageConfig>())
                .define_constructor(Constructor<osrm::storage::StorageConfig, const boost::filesystem::path>(), Arg("base"))
                .define_method("valid?", &osrm::storage::StorageConfig::IsValid)
            ;
}



// ENGINE CONFIG

Data_Type<osrm::EngineConfig> rb_cEngineConfig;
Enum<osrm::EngineConfig::Algorithm> rb_eEngineConfigAlgorithm;

template<>
osrm::EngineConfig::Algorithm from_ruby<osrm::EngineConfig::Algorithm>(Object x) {
    Data_Object<osrm::EngineConfig::Algorithm> d(x, rb_eEngineConfigAlgorithm);
    return *d;
}

#define EC_ACC_INT(variable) \
    Object EngineConfig_##variable##_get(Object self) { \
        Data_Object<osrm::EngineConfig> c(self, rb_cEngineConfig); \
        return c->variable; \
    }\
    \
    Object EngineConfig_##variable##_set(Object self, Object value) { \
        Data_Object<osrm::EngineConfig> c(self, rb_cEngineConfig); \
        c->variable = value.value(); \
        return value; \
    }

Object EngineConfig_storage_config_get(Object self) {
    Data_Object<osrm::EngineConfig> c(self, rb_cEngineConfig);
    //return c->storage_config;
    return self;
}

Object EngineConfig_storage_config_set(Object self, Object value) {
    Data_Object<osrm::EngineConfig> c(self, rb_cEngineConfig);
    c->storage_config = from_ruby<osrm::storage::StorageConfig>(value.value());
    return value;
}

Object EngineConfig_use_shared_memory_get(Object self) {
    Data_Object<osrm::EngineConfig> c(self, rb_cEngineConfig);
    return c->use_shared_memory;
}

Object EngineConfig_use_shared_memory_set(Object self, Object value) {
    Data_Object<osrm::EngineConfig> c(self, rb_cEngineConfig);
    c->use_shared_memory = value.value();
    return value;
}

EC_ACC_INT(max_locations_trip)
EC_ACC_INT(max_locations_viaroute)
EC_ACC_INT(max_locations_distance_table)
EC_ACC_INT(max_locations_map_matching)
EC_ACC_INT(max_results_nearest)

char const * description(osrm::EngineConfig::Algorithm e) {
    switch(e) {
        case osrm::EngineConfig::Algorithm::CH:
            return "Contraction hierarchies";
        case osrm::EngineConfig::Algorithm::CoreCH:
            return "Partial contraction hierarchies";
        case osrm::EngineConfig::Algorithm::MLD:
            return "Multi level Dijkstra";
    }
    return "[invalid]";
}

void init_engine_config() {
    rb_cEngineConfig =
            define_class_under<osrm::EngineConfig>(rb_mLibOSRM, "EngineConfig")
                .define_constructor(Constructor<osrm::EngineConfig>())
                .define_method("valid?", &osrm::EngineConfig::IsValid)
                ATTR_ACCESSOR_DECL(EngineConfig, storage_config)
                ATTR_ACCESSOR_DECL(EngineConfig, max_locations_trip)
                ATTR_ACCESSOR_DECL(EngineConfig, max_locations_viaroute)
                ATTR_ACCESSOR_DECL(EngineConfig, max_locations_distance_table)
                ATTR_ACCESSOR_DECL(EngineConfig, max_locations_map_matching)
                ATTR_ACCESSOR_DECL(EngineConfig, max_results_nearest)
                ATTR_ACCESSOR_DECL(EngineConfig, use_shared_memory)
            ;

    rb_eEngineConfigAlgorithm =
      define_enum<osrm::EngineConfig::Algorithm>("Algorithm", rb_cEngineConfig)
      .define_value("CH", osrm::EngineConfig::Algorithm::CH)
      .define_value("CoreCH", osrm::EngineConfig::Algorithm::CoreCH)
      .define_value("MLD", osrm::EngineConfig::Algorithm::MLD);

    rb_eEngineConfigAlgorithm.define_method("description", description);

    init_storage_config();
}



// RUBY OSRM OBJECT


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

