require "rake/extensiontask"
require "rake/testtask"

Rake::ExtensionTask.new "libosrm" do |ext|
  ext.name = "ruby_libosrm"
  ext.lib_dir = "lib/libosrm"
end

Rake::TestTask.new(:test) do |t| 
  t.libs << "test"
  t.libs << "lib"
  t.test_files = FileList['test/**/*_test.rb']
end

task :default => [:compile]
