function GradientCircle(x, y, radius, color1, color2, aa){
	if(aa)
		GradientEllipse(x, y, radius, color1, color2, aa);
	else
		GradientEllipse(x, y, radius, color1, color2);
}