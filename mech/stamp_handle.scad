h_handle = 55;
r_handle = 16;
r_neck =8;
r_bot=12;
h_neck = 18;

// 3 in ~= 76 mm
// 2 in ~= 50 mm
w_stamp = 50;
h_stamp = 25;
t_stamp = 4;

module handle() {
	union() {
		translate([0,0,h_handle]) sphere(r=r_handle,center=true);
		cylinder(h=h_handle,r1=r_neck,r2=r_handle);
		cylinder(h=h_neck,r1=r_bot,r2=r_neck);

	}
}

module base() {
	cube([w_stamp,h_stamp,t_stamp],center=true);
}

union() {
	base();
	handle();
}

