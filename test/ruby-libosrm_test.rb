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
        response = @osrm.match [{lat: 61.4445, lon: 23.8383}, {lat: 61.4423, lon: 23.8426}, {lat: 61.4369, lon: 23.8474}]
        assert_equal "Ok", response["code"]
        matched_coordinate = response["matchings"][0]["geometry"]["coordinates"][0]
        assert_equal matched_coordinate[0].is_a?(Float), true
        assert_equal matched_coordinate[1].is_a?(Float), true
    end

    def test_match_fail
        response = @osrm.match [{ lat: 60.1681473, lon: 24.9417190 }, { lat: 60.1694561, lon: 24.9385663 }]
        assert_equal "NoMatch", response["code"]
    end

    #################
    # Nearest       #
    #################

    def test_nearest
        response = @osrm.nearest 60.1681473, 24.9417190
        assert_equal "Ok", response["code"]
    end

    def test_nearest_with_integer_arguments
        response = @osrm.nearest 60, 24
        assert_equal "Ok", response["code"]
    end

    def test_nearest_with_invalid_arguments
        assert_raises TypeError do
            response = @osrm.nearest "60.1681473", "24.9417190"
        end

        assert_raises ArgumentError do
           response = @osrm.nearest
        end

        assert_raises ArgumentError do
           response = @osrm.nearest 60
        end
    end

    #################
    # Route         #
    #################

    def test_route_between_two_coordinates
        response = @osrm.route [{lat: 61.4445, lon: 23.8383}, {lat: 61.4423, lon: 23.8426}, {lat: 61.4369, lon: 23.8474}]
        assert_equal "Ok", response["code"]
    end
    
    #################
    # Table         #
    #################
    
    #################
    # Tile          #
    #################
    
    #################
    # Trip          #
    #################
end
