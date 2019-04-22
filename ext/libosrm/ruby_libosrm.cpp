#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"
#include "rice/Enum.hpp"
#include "rice/Struct.hpp"
#include "osrm_wrap.hpp"

using namespace Rice;
using namespace osrm;

/*
 * This is the entry point for this gem. The methods listed below are the ones
 * that can be called from ruby code.
 */

extern "C"
void Init_ruby_libosrm() {
    RUBY_TRY
    {
        Data_Type<OsrmWrap> rb_cLibOSRM =
            define_class<OsrmWrap>("OSRM")
            .define_constructor(Constructor<OsrmWrap>())
            .define_constructor(Constructor<OsrmWrap, const std::string>(), Arg("database"))
            .define_method("route", &OsrmWrap::route)
            .define_method("match", &OsrmWrap::match)
            .define_method("nearest", &OsrmWrap::nearest)
            .define_method("table", &OsrmWrap::table)
            .define_method("trip", &OsrmWrap::trip)
            .define_method("tile", &OsrmWrap::tile)
            .define_method("distance_by_roads", &OsrmWrap::distance_by_roads)
            ;
    }
    RUBY_CATCH
}
