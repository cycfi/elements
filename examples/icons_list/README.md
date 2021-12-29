# Notes 

This example shows Elements icons next to their names. 

# Add custom icons

You can add custom to Elements icons using [Fontello](https://fontello.com/). 
If you want to add custom icons to elements : 

- Go to the resource/fonts directory. 
- Drag/drop config.json file to fontello web page. 
- You can then add any fontello icon to the icons list. 
- Download the webfont.
- In resource/fonts, replace elements_basic.ttf and config.json with the equivalents you just downloaded. 
- If needed, you can use generate_enum.rb script to generate an C++ enum to use the icons. You can use it as follow : `ruby generate_enum.rb path/to/elements_basic_codes.css name_of_enum`. It will print the formatted enum with all icons from your font.

