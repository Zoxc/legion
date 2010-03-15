require 'reno'
require 'reno/languages/c'
require 'reno/languages/c++'
require 'reno/toolchains/gnu'

package = Reno::Application.new do
	name 'Legion'
	version '0.1'
	sources '*.cpp', 'lexer/*.cpp', 'parser/*.cpp'
	default :lang => 'C++'
end

task :default do
	package.build
end
