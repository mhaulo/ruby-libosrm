require 'test_helper'
require 'libosrm'

class RubyLibOsrmTest < Minitest::Test
    def setup
        # NOTE! In order to make tests run, you need to provide this file by
        # preparing OSM data manually. See README for details.
        @osrm = OSRM.new "test/osrm-profiles/car/finland-latest.osrm"
    end

    #################
    # Match         #
    #################

    def test_match_coordinate
        options = {
          geometries: :geojson,
          overview: :full,
          gaps: :ignore,
          annotations: true
        }
        response = @osrm.match [{lat: 61.4445, lon: 23.8383}, {lat: 61.4423, lon: 23.8426}, {lat: 61.4369, lon: 23.8474}], options
        assert_equal "Ok", response["code"]
        matched_coordinate = response["matchings"][0]["geometry"]["coordinates"][0]
        waypoints = response["tracepoints"]
        assert_equal matched_coordinate[0].is_a?(Float), true
        assert_equal matched_coordinate[1].is_a?(Float), true
        assert (waypoints and waypoints.length > 0)
    end

    def test_match_fail
        options = {
          geometries: :geojson,
          overview: :full,
          gaps: :ignore,
          annotations: true
        }
        response = @osrm.match [{ lat: 60.1681473, lon: 24.9417190 }, { lat: 60.1694561, lon: 24.9385663 }], options
        assert_equal "NoMatch", response["code"]
    end

    #################
    # Nearest       #
    #################

    def test_nearest
        response = @osrm.nearest 60.1681473, 24.9417190, {}
        assert_equal "Ok", response["code"]
    end

    def test_nearest_with_integer_arguments
        response = @osrm.nearest 60, 24, {}
        assert_equal "Ok", response["code"]
    end

    def test_nearest_with_invalid_arguments
        assert_raises TypeError do
            @osrm.nearest "60.1681473", "24.9417190", {}
        end

        assert_raises ArgumentError do
           @osrm.nearest
        end

        assert_raises ArgumentError do
           @osrm.nearest 60
        end

        assert_raises ArgumentError do
           @osrm.nearest 60, 24
        end
    end

    #################
    # Route         #
    #################

    def test_route_between_two_coordinates
      options = {
        geometries: :geojson,
        overview: :full,
        gaps: :ignore,
        annotations: true
      }
      response = @osrm.route [{lat: 61.4445, lon: 23.8383}, {lat: 61.4423, lon: 23.8426}, {lat: 61.4369, lon: 23.8474}], options
      assert_equal "Ok", response["code"]
    end
    
    # TODO check why this succeeds even though it should fail with given coordinates
    #def test_route_fail
    #  options = {
    #    geometries: :geojson,
    #    overview: :full,
    #    gaps: :ignore,
    #    annotations: true
    #  }
    #  response = @osrm.route [{lon: 61.4445, lat: 23.8383}, {lon: 61.4423, lat: 23.8426}], options
    #  assert_equal "NoRoute", response["code"]
    #end

    def test_route_with_invalid_arguments
      options = {
          geometries: :geojson,
          overview: :full,
          gaps: :ignore,
          annotations: true
      }

      assert_raises ArgumentError do
        @osrm.route [{lat: 61.4445, lon: 23.8383}, {lat: 61.4423, lon: 23.8426}, {lat: 61.4369, lon: 23.8474}]
      end

      # TODO check why this causes segfault
      #assert_raises ArgumentError do
      #  @osrm.route [lat: 61.4445, lon: 23.8383], options
      #end

      assert_raises TypeError do
        @osrm.route [{lat: "61.4445", lon: "23.8383"}, {lat: "61.4423", lon: "23.8426"}], options
      end
    end

    #################
    # Table         #
    #################

    def test_table_with_valid_coordinates
      options = {
          geometries: :geojson,
          overview: :full,
          gaps: :ignore,
          annotations: true
      }

      response = @osrm.table [{lat: 61.4445, lon: 23.8383}, {lat: 61.4423, lon: 23.8426}, {lat: 61.4369, lon: 23.8474}], options
      assert_equal "Ok", response["code"]
    end

    def test_table_with_invalid_arguments
      options = {
          geometries: :geojson,
          overview: :full,
          gaps: :ignore,
          annotations: true
      }

      assert_raises ArgumentError do
        @osrm.table [{lat: 61.4445, lon: 23.8383}, {lat: 61.4423, lon: 23.8426}, {lat: 61.4369, lon: 23.8474}]
      end

      # TODO check why this does not raise any errors
      #assert_raises ArgumentError do
      #  @osrm.table [lat: 61.4445, lon: 23.8383], options
      #end

      assert_raises TypeError do
        @osrm.table [{lat: "61.4445", lon: "23.8383"}, {lat: "61.4423", lon: "23.8426"}], options
      end
    end

    #################
    # Trip          #
    #################

    def test_trip_with_valid_coordinates
      options = {
          geometries: :geojson,
          overview: :full,
          gaps: :ignore,
          annotations: true
      }

      response = @osrm.trip [{lat: 61.4445, lon: 23.8383}, {lat: 61.4423, lon: 23.8426}, {lat: 61.4369, lon: 23.8474}], options
      assert_equal "Ok", response["code"]
    end

    def test_tript_with_invalid_arguments
      options = {
          geometries: :geojson,
          overview: :full,
          gaps: :ignore,
          annotations: true
      }

      assert_raises ArgumentError do
        @osrm.trip [{lat: 61.4445, lon: 23.8383}, {lat: 61.4423, lon: 23.8426}, {lat: 61.4369, lon: 23.8474}]
      end

      # TODO check why this does not raise any errors
      #assert_raises ArgumentError do
      #  @osrm.trip [lat: 61.4445, lon: 23.8383], options
      #end

      assert_raises TypeError do
        @osrm.trip [{lat: "61.4445", lon: "23.8383"}, {lat: "61.4423", lon: "23.8426"}], options
      end
    end
end
