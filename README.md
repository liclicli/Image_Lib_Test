# Image_Lib_Test
###this project would write some simple codes for test the preformance among different imagelibs.

Four imagelibs would be tested in this project.
<table>
    <tr>
        <td></td>
        <td>raw</td>
        <td>ImageMagick</td>
        <td>GD</td>
        <td>GraphicsMagick</td>
    </tr>
    <tr>
        <td>time/s</td>
        <td>11.354s</td>
        <td>12.485</td>
        <td>12.279</td>
        <td>slow because use single core.</td>
    </tr>
    <tr>
        <td></td>
        <td>100%</td>
        <td>110.0%</td>
        <td>108.1%</td>
        <td>/</td>
    </tr>
</table>

compile command:<br>
gcc -o ../bin/imagemagick_test imagemagick_test.cc -lstdc++ -lMagickWand<br>
gcc -o ../bin/graphicsmagick_test graphicsmagick_test.cc -O \`GraphicsMagickWand-config --cppflags --ldflags --libs\` -lstdc++<br>
gcc -o jpeg_com_raw jpeg_com_raw_in_mem_v2_mt.cc jpeg_mem_process.cc -lstdc++ -ljpeg -lpthread
