#!/usr/bin/ruby

#First argument -> path to the css file containing the icon codes
#Second argument (optional) -> name of the generated enum

f= File.open(ARGV[0].to_s)
str = f.read
reg = /\.icon-([a-zA-Z0-9-]+):before\s{\scontent:\s'\\(e[0-9a-z]+)';/

name = "custom_icons"
if(ARGV.length > 1) then
  namespace = ARGV[1]
end

fmt =  "enum class #{name} {\n"
str.scan(reg) {|match|
  fmt += "\t#{match[0]} = 0x#{match[1]}, \n"
}
fmt  += "}\n"
puts fmt


