function FilledCircle(x, y, radius, color, aa){
	if(aa)
		FilledEllipse(x, y, radius, color, aa);
	else
		FilledEllipse(x, y, radius, color);
}