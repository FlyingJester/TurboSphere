function OutlinedCircle(x, y, radius, color, aa){
	if(aa)
		OutlinedEllipse(x, y, radius, color, aa);
	else
		OutlinedEllipse(x, y, radius, color);
}