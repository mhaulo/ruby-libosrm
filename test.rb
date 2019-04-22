require "libosrm"


puts "Initializing OSRM"
osrm = OSRM.new "/home/xcube/tmp/osrm-tmp/car/finland-latest.osrm"


puts "Running distance_by_roads"
distance = osrm.distance_by_roads [{ latitude: 60.1681473, longitude: 24.9417190 }, { latitude: 60.1694561, longitude: 24.9385663 }]
puts distance

puts "Running route"
routing = osrm.route [{ lat: 60.1681473, lon: 24.9417190 }, { lat: 60.1694561, lon: 24.9385663 }]
puts routing

puts "Running match (1)"
matching = osrm.match [{ lat: 60.1681473, lon: 24.9417190 }, { lat: 60.1694561, lon: 24.9385663 }]
puts matching

puts "Running match (2)"
matching = osrm.match [{lat: 61.4445, lon: 23.8383}, {lat: 61.4423, lon: 23.8426}, {lat: 61.4369, lon: 23.8474}]
puts matching


puts "Running nearest"
nearest = osrm.nearest 60.1681473, 24.9417190
puts nearest

puts "All tests done"
