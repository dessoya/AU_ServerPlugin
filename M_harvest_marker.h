#pragma once

bool M_harvest_marker_init();
bool M_harvest_marker_done();

void __add_harvest_marker(unsigned long iid1, unsigned long iid2);
bool __get_harvest_marker(unsigned long iid1, unsigned long iid2);
