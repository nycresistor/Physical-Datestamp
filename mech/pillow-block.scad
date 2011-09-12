// All measurements in mm.

// Radius of rod
r_rod = 4.1;

// Radius of screw hole
r_screw = 2.3;

// Outer radius of top of block
r_block = 7.5;

// Width of block
w_block = 10;

// Height of rod center
h_rod = 25;

// Height of legs
h_legs = 8;

// Distance between holes
d_holes = 26;

// Total length of foot of block
l_foot = 36;

module rod_block() {
	difference() {
		union() {
			translate([0,0,h_rod/2]) cube(size=[r_block*2,w_block,h_rod],center=true);
			translate([0,0,h_rod]) rotate([90,0,0]) cylinder(h=w_block,r=r_block,center=true);
		}
		translate([0,0,h_rod]) rotate([90,0,0]) cylinder(h=2*w_block,r=r_rod,center=true);
	}
}

module foot() {
	difference() {
		translate([0,0,h_legs/2]) cube(size=[l_foot,w_block,h_legs],center=true);
		translate([d_holes/2,0,h_legs/2]) cylinder(h=h_legs*2,r=r_screw,center=true);
		translate([-d_holes/2,0,h_legs/2]) cylinder(h=h_legs*2,r=r_screw,center=true);
	}
}

union() {
	foot();
	rod_block();
}
