// Triangle side
side=130;
height = sqrt(3)*side/2;
led_margin = 0.9;
led_w = 5 + led_margin;
led_h = led_w;
thickness = 0.01;
inter_x = 1.92 - led_margin;
led_spacing_x = 6.89;
led_spacing_y = 8.6;
margin_x = 1; // mininum length between a LED and the edge
margin_y = 1.5;

module triangle(){
 difference(){   
     polygon(points=[[-side/2,0], [side/2,0], [0,height]]);
     polygon(points=[[(-side/2)+thickness,thickness], [(side/2)-thickness,thickness], [0,height-thickness]]);
 }
}

module led(){
    difference(){
        square([led_w,led_h], center=true);
        square([led_w-thickness,led_h-thickness], center=true);
    }
}

module row(start_x, start_y, stop_x){
    nb_leds = floor((stop_x - start_x +inter_x)/(led_spacing_x));
    echo(nb_leds);
    horizontal_centering = (stop_x - start_x - (nb_leds-1) * led_spacing_x)/2;
    for (i=[0:nb_leds-1]) {
        start_x_tmp = start_x + i*led_spacing_x  + horizontal_centering;
        translate([start_x_tmp, start_y])
        led();
    }
}

module leds(){
    margin_top = ((led_w+margin_x*2) * tan(60))/2;
    nb_lines = floor((height-margin_top)/(led_spacing_y));
    // Vertical centering 
    start_y_nb_lines = nb_lines*(led_spacing_y);
    vertical_centering = height - start_y_nb_lines;
    for (i=[0:nb_lines-1]){
        start_y = i*(led_spacing_y) + (vertical_centering / nb_lines) +margin_y+led_h/2;
        start_x = start_y/tan(60) + margin_x - side/2 + led_w/2;
        stop_x = -start_y/tan(60) - margin_x + side/2 - led_w/2;
        row(start_x, start_y, stop_x);
    }
}


module face() {
    triangle();
    leds();
    
}
face();