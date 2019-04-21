#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"
#include "osrm_wrap.hpp"

using namespace Rice;
using namespace osrm;


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
