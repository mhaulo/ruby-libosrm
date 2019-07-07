# Ruby bindings for libosrm

ruby-libosrm is a Ruby bindings for OSRM’s libosrm, the C++ bindings for OSRM, open source routing machine,
which provides fast and customizable interface compared to OSRM’s old HTTP API.

## Installation

After you have required dependencies, you can install libosrm gem:

```shell
$ gem install libosrm
```

or with Bundler

```ruby
# In Gemfile
gem "libosrm"
```

and then install the bundle:

```shell
$ bundle install
```

### Prepare map data

For libosrm to be able to calculate anything, it requires map data. You can use either data of whole world
or a portion it, for example your country’s or specific city’s data.

One place to get the data from is {http://download.geofabrik.de/index.html Geofabrik}.

Currently (04/2019) ruby-libosrm only supports MLD routing algorithm. See preparation instructions from the OSRM project.


You can find more information at [OSRM’s README](https://github.com/Project-OSRM/osrm-backend/tree/5.12).

## Requirements

- Ruby 2.3 or greater

Since libosrm is bindings for OSRM’s C++ library, you need to install same dependencies as you would do when compiling only OSRM.

From [OSRM’s README](https://github.com/Project-OSRM/osrm-backend/tree/5.12), following dependencies are required:

- gcc-4.9 or greater or clang-3.4 or greater
- cmake-2.8.11 or greater
- boost-1.54 or greater
- libtbb
- threading support
- expat
- bzip2
- lua-5.2 (lua-5.1 works for now but is deprecated)
- osmium
- zlib

For Ubuntu 16.04 and 18.10, the packages to be installed would be

```shell
$ sudo apt install build-essential git cmake pkg-config \
libbz2-dev libstxxl-dev libstxxl1v5 libxml2-dev \
libzip-dev libboost-all-dev lua5.2 liblua5.2-dev libtbb-dev
```


## Features

This gem provides the following OSRM features. Please note that the options hash in required. If you don't want to set any custom options, provide an empty hash.

### Match

```ruby
require "libosrm"

osrm = OSRM.new "map.osrm"
options = {geometries: :geojson, overview: :full, gaps: :ignore, annotations: true}
matchings = osrm.match([{ lat: 60.1681473, lon: 24.9417190 }, { lat: 60.1694561, lon: 24.9385663 }], options)

```

### Route

```ruby
require "libosrm"

osrm = OSRM.new "map.osrm"
options = {geometries: :geojson, overview: :full, gaps: :ignore, annotations: true}
routings = osrm.route([{ lat: 60.1681473, lon: 24.9417190 }, { lat: 60.1694561, lon: 24.9385663 }], options)

```

### Nearest

```ruby
require "libosrm"

osrm = OSRM.new "map.osrm"

nearest = osrm.nearest(60.1681473, 24.9417190)

```

### Table

```ruby
require "libosrm"

osrm = OSRM.new "map.osrm"
options = {geometries: :geojson, overview: :full, gaps: :ignore, annotations: true}
routings = osrm.table([{ lat: 60.1681473, lon: 24.9417190 }, { lat: 60.1694561, lon: 24.9385663 }], options)

```

### Trip

```ruby
require "libosrm"

osrm = OSRM.new "map.osrm"
options = {geometries: :geojson, overview: :full, gaps: :ignore, annotations: true}
routings = osrm.trip([{ lat: 60.1681473, lon: 24.9417190 }, { lat: 60.1694561, lon: 24.9385663 }], options)

```

### Tile

This OSRM feature is not yet supported.

## Known issues & TODO

- Routing algorithm is fixed to MLD (should be configurable)
- some requests are not fully supported (TODO)
- Build files after gem installation are not cleaned
- Linking OSRM statically instead as a library would further reduce disk space usage
- Moving the gem to another location breaks the linkage, requiring reinstallation of the gem
- Maybe we should by default bundle all libraries? That would make it easier to get everything compiled. Or at least check if system has version and only then use bundled one.

## For developers

Clone the repository and check that everything works:

```shell
$ git clone https://github.com/mhaulo/ruby-libosrm.git
$ cd ruby-libosrm
$ # Install bundler if required: gem install bundler
$ bundle install
$ # Compile the library.
$ bundle exec rake
$ # Run tests to ensure the library works
$ bundle exec rake test
```

Now you should have working instance of ruby-libosrm. To use it, you most likely want to build a gem and use it through
the gem in your project:

```shell
$ # Build the gem
$ gem build libosrm.gemspec
$ # prepend with sudo if you don’t use user specific gem data
$ gem install libosrm-1.0.0.gem
```

### Contributing

You can do pull requests at Github using the standard pattern :-)

1. Fork it
2. Create your feature branch (git checkout -b my-new-feature)
3. Commit your changes (git commit -am 'Add some feature')
4. Push to the branch (git push origin my-new-feature)
5. Create new Pull Request
