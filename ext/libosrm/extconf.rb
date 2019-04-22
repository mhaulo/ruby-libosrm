require "mkmf-rice"

require "fileutils"
require "yaml"

def do_help
  print <<HELP
usage: ruby #{$0} [options]

    --use-system-libraries
        Use system libraries instead of building and using the bundled
        libraries.
HELP
  exit! 0
end

case
when arg_config("--help")
    do_help
end

message "Building ruby-libosrm using system libraries.\n"

abort "Unable to find necessary libraries" unless
    have_library("boost_system") &&
    have_library("boost_filesystem") &&
    have_library("boost_iostreams") &&
    have_library("boost_thread") &&
    have_library("osrm")

append_cflags("-DHAVE_CXX11")
append_cflags("-I/usr/local/include/osrm")
append_cflags("-I/usr/include/boost")

create_makefile "libosrm/ruby_libosrm"
