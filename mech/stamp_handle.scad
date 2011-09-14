h_handle = 70;
r_handle = 20;
r_neck =10;
h_neck = 20;

// 3 in ~= 76 mm
// 2 in ~= 50 mm
w_stamp = 76;
h_stamp = 50;
t_stamp = 4;

module handle() {
	union() {
		translate([0,0,h_handle]) sphere(r=r_handle,center=true);
		cylinder(h=h_handle,r1=r_neck,r2=r_handle);
		cylinder(h=h_neck,r1=r_handle,r2=r_neck);

	}
}

module base() {
	cube([w_stamp,h_stamp,t_stamp],center=true);
}

union() {
	base();
	handle();
}

